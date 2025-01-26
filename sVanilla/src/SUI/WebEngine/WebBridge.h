
#pragma once
#include <QMetaMethod>
#include <QMetaObject>
#include <QPair>
#include <QTimer>
#include <QWebEnginePage>

QT_BEGIN_NAMESPACE
class QWebEnginePage;
class QJsonValue;
QT_END_NAMESPACE

class WebBridge : public QObject
{
    Q_OBJECT
public:
    WebBridge(QWebEnginePage* page, QObject* parent);
    ~WebBridge();

    bool inject(const QString& objectName, const QString& scriptName, const QString& prefix = "QtBridge") const;
    virtual void invokeMethod(const QString& method_name, const QString& args, const QString& callback_id);
    void invokeJSCallback(const QString& callback_id, const QString& return_args);

    template <typename Func>
    inline void asynCall(Func f)
    {
        QTimer::singleShot(0, this, std::move(f));
    }

    bool injectWebChannelScript(QWebEnginePage& page, const QString& js_file, const QString& objectName);

    static bool injectScript(QWebEnginePage& page, const QString& sourceCode, const QString& scriptName);
    static QString scriptSourceCode(const QMetaMethod& method, const QString& objectName, const QString& prefix);
    static QMetaMethod metaMethod(const QMetaObject* object, const QString& name);
    static QList<QMetaMethod> metaMethods(const QMetaObject* object, const QList<QString>& names);

    QString genScriptName(const QString& objectName);
    QString toWebString(const QJsonValue& data, int code = 0, const QString& msg = "");
    QString toJsonString(const QJsonObject& data);
    QJsonObject toJsonObject(const QString& json);

protected slots:
    void runJavaScript(const QString& code);

protected:
    QWebEnginePage* m_page;
};
