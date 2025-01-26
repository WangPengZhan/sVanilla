#include "WebBridge.h"

#include <QEventLoop>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaMethod>
#include <QTimer>
#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QtDebug>

QString kInjectJSCode = R"(
function QtBridge(dev = false) {
    var bridge = this;
    this.nextCallbackId = 0;
    this.callbacks = {};
    this.dev = dev

    this.qDebug = function (msg) {
        if (this.dev) {
            console.log(msg);
        }
    }

    this.callHandler = function (method_name, args, callback) {
        bridge.qDebug('method_name:' + method_name)
        if(args && typeof(args) == 'object'){
            args = JSON.stringify(args)
            bridge.qDebug('args:' + JSON.stringify(args))
        }

        if (!callback) {
            window.%1.invokeMethod(method_name, args, "");
            return;
        }

        if (bridge.nextCallbackId === Number.MAX_VALUE) {
            bridge.nextCallbackId = Number.MIN_VALUE;
        }

        var id = "callback_" + bridge.nextCallbackId++;
        bridge.callbacks[id] = callback;
        window.%1.invokeMethod(method_name, args, id);
    }

    this.registerHandler= function(method_name, callback){
        bridge.qDebug('regesiter method_name:' + method_name)
        if(bridge.callbacks.hasOwnProperty(method_name)){
            bridge.qDebug('method_name:' + method_name + 'has registered!')
            return;
        }
        bridge.callbacks[method_name] = callback;
    }

    this.invokeCallback = function (callback_id, args) {
        bridge.qDebug({
            'callback args': args
        })
        var f = bridge.callbacks[callback_id];
        if (f) {
            f(args);
        }
    }
}

//required for use with nodejs
if (typeof module === 'object') {
    module.exports = {
        QtBridge: QtBridge
    };
}
)";

QString& formatJavaScriptString(QString& s)
{
    s.remove("\r\n");
    s.remove("\n");
    s.replace("\\", "\\\\");
    s.replace("'", "\\'");
    return s;
}

WebBridge::WebBridge(QWebEnginePage* page, QObject* parent)
    : QObject(parent)
    , m_page(page)
{
}

WebBridge::~WebBridge()
{
}

void WebBridge::invokeMethod(const QString& method_name, const QString& args, const QString& callback_id)
{
    qDebug() << method_name;

    QMetaMethod method = WebBridge::metaMethod(this->metaObject(), method_name);
    if (method.isValid())
    {
        method.invoke(this, Qt::AutoConnection, Q_ARG(QString, args), Q_ARG(QString, callback_id));
        qInfo() << method_name << "args:" << args << "cb:" << callback_id;
    }
    else
    {
        qWarning() << method_name << "undefined!";
    }
}

void WebBridge::invokeJSCallback(const QString& callback_id, const QString& return_args)
{
    qInfo() << "invokeJSCallback cb:" << callback_id << "args" << return_args;
    QString js = QString("window.qtBridge.invokeCallback('%1', '%2')").arg(callback_id).arg(return_args);
    this->metaObject()->invokeMethod(this, "runJavaScript", Qt::QueuedConnection, Q_ARG(QString, js));
}

bool WebBridge::inject(const QString& objectName, const QString& scriptName, const QString& prefix) const
{
    if (m_page)
    {
        QList<QString> names;
        names << "invokeMethod";
        QList<QMetaMethod> methods = metaMethods(metaObject(), names);
        if (!methods.empty())
        {
            QString sourceCode;
            for (const QMetaMethod& method : methods)
            {
                sourceCode += scriptSourceCode(method, objectName, prefix);
                sourceCode += '\n';
            }
            QString file_code(kInjectJSCode);
            file_code.replace("%1", objectName);
            sourceCode += file_code;
            return injectScript(*m_page, sourceCode, scriptName);
        }
    }
    return false;
}

void WebBridge::runJavaScript(const QString& code)
{
    qDebug() << code;
    m_page->runJavaScript(code);
}

bool WebBridge::injectWebChannelScript(QWebEnginePage& page, const QString& js_file, const QString& object_name)
{
    QFile file(js_file);
    if (file.open(QFile::ReadOnly))
    {
        QByteArray ba = file.readAll();
        file.close();
        QString code = QString::fromLatin1(ba);
        return injectScript(page, code, genScriptName(object_name));
    }
    return false;
}

bool WebBridge::injectScript(QWebEnginePage& page, const QString& source_code, const QString& script_name)
{
    QWebEngineScriptCollection& sl = page.scripts();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (sl.find(script_name).isEmpty())
#else
    if (sl.findScripts(script_name).isEmpty())
#endif
    {
        QWebEngineScript s;
        s.setName(script_name);
        s.setWorldId(QWebEngineScript::MainWorld);
        s.setInjectionPoint(QWebEngineScript::DocumentCreation);
        s.setRunsOnSubFrames(true);
        s.setSourceCode(source_code);

        sl.insert(s);
        return true;
    }

    return false;
}

QString WebBridge::scriptSourceCode(const QMetaMethod& method, const QString& objectName, const QString& prefix)
{
    QString args;
    QString args_stats;
    QList<QByteArray> names = method.parameterNames();
    for (const QByteArray& s : names)
    {
        const QString t(s);
        args += s;
        args += ',';
        args_stats += QString("if (typeof(%1) == 'undefined') { %1 = ''};\n").arg(t);
    }
    args.chop(1);

    QString method_index = QString::number(method.methodIndex());
    QString method_name = QString::fromLatin1(method.name());
    QString func_name(method_name);
    QString code = QString("function %1(%2){\ntry{\n%3"
                           "qt.webChannelTransport.send(JSON.stringify({type:6,object:'%4',id:"
                           "'%4',method:"
                           "%5,args:[%2]}));\n"
                           "}catch(e){\nconsole.warn('invokeQT call failed', e)\n}}\n"
                           "window.external = window.external || {}\n"
                           "window.external.%6 = window.external.%6 || %1\n"
                           "window.%4 = window.%4 || {}\n"
                           "window.%4.%6 = %1\n")
                       .arg(func_name, args, args_stats, objectName, method_index, method_name);
    return code;
}

QMetaMethod WebBridge::metaMethod(const QMetaObject* object, const QString& name)
{
    QMetaMethod method;
    Q_ASSERT(object);
    int count = object->methodCount();
    for (int i = 0; i < count; ++i)
    {
        QMetaMethod m = object->method(i);
        if (QString::fromLatin1(m.name()).compare(name) == 0)
        {
            method = m;
            break;
        }
    }
    return method;
}

QList<QMetaMethod> WebBridge::metaMethods(const QMetaObject* object, const QList<QString>& names)
{
    QList<QMetaMethod> methods;
    Q_ASSERT(object);
    for (const QString& s : names)
    {
        QMetaMethod m = metaMethod(object, s);
        if (m.isValid())
        {
            methods.push_back(m);
        }
        qDebug() << m.isValid() << m.name() << m.methodIndex() << m.methodType();
    }
    return methods;
}

QString WebBridge::genScriptName(const QString& object_name)
{
    static int id = 1;
    QString name = QString("%1.injectionscript%1").arg(object_name, id++);
    return name;
}

QString WebBridge::toWebString(const QJsonValue& data, int code, const QString& msg)
{
    if (code == 0)
    {
        if (data.isBool())
        {
            code = data.toBool() ? 0 : 1;
        }
    }
    else if (code != 2)
    {
        code = 1;
    }
    QJsonObject jo;
    jo.insert("data", data);
    jo.insert("code", code);
    jo.insert("msg", msg);
    QByteArray ba = QJsonDocument(jo).toJson(QJsonDocument::Compact);
    QString str = QString::fromUtf8(ba);
    return formatJavaScriptString(str);
}

QString WebBridge::toJsonString(const QJsonObject& data)
{
    QByteArray ba = QJsonDocument(data).toJson(QJsonDocument::Compact);
    QString str = QString::fromUtf8(ba);
    return formatJavaScriptString(str);
}

QJsonObject WebBridge::toJsonObject(const QString& json)
{
    QJsonObject jo;
    if (!json.isEmpty())
    {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);
        if (error.error != QJsonParseError::NoError)
        {
            qWarning() << error.errorString() << json;
        }
        jo = doc.object();
    }
    return jo;
}
