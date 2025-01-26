#include "WebSetting.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QWebEngineCookieStore>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QWebEngineUrlRequestJob>
#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>
#include <QWebEngineView>

class QtSchemeHandler : public QWebEngineUrlSchemeHandler
{
public:
    QtSchemeHandler(QObject* parent = nullptr)
        : QWebEngineUrlSchemeHandler(parent)
    {
    }

    void requestStarted(QWebEngineUrlRequestJob* request) override
    {
        QByteArray request_method = request->requestMethod();
        if (request_method != "GET")
        {
            request->fail(QWebEngineUrlRequestJob::RequestDenied);
            return;
        }

        QUrl request_url = request->requestUrl();
        QString request_path = request_url.toString();
        request_path = request_path.right(request_path.size() - QString("apptemp:/").size());
        //        qDebug() << request_url << "request_path" <<request_path <<
        //        request->initiator();
        QString path = request_path_;
        qDebug() << path;
        if (!QFile::exists(path))
        {
            request->fail(QWebEngineUrlRequestJob::UrlNotFound);
            return;
        }
        QFileInfo file_info = QFileInfo(path);
        QFile* file = new QFile(path, request);
        connect(request, &QObject::destroyed, file, &QFile::deleteLater);
        QMimeDatabase mime_database;
        QMimeType mime_type = mime_database.mimeTypeForFile(file_info);
        request->reply(QUrl(mime_type.name()).toEncoded(), file);
    }

private:
    QString request_path_;
};

void initializeWebEngine(const QString& cache_path, const QString& storage_path)
{
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9876");
    // QWebEngineUrlScheme scheme = QWebEngineUrlScheme("apptemp");
    // scheme.setFlags(QWebEngineUrlScheme::CorsEnabled |
    //                 QWebEngineUrlScheme::LocalAccessAllowed |
    //                 QWebEngineUrlScheme::ViewSourceAllowed);
    // QWebEngineUrlScheme::registerScheme(scheme);
    //// 设置全局WebProfile
    QWebEngineProfile* profile = QWebEngineProfile::defaultProfile();
    // QtSchemeHandler* scheme_handler = new QtSchemeHandler(qApp);
    // profile->installUrlSchemeHandler("apptemp", scheme_handler);
    //  QObject::connect(profile, &QWebEngineProfile::downloadRequested, this,
    //                   &Application::onDownloadRequested);
    //  设置缓存目录
    // profile->setCachePath(cache_path);
    //  设置持久存储的目录
    // profile->setPersistentStoragePath(storage_path);
    //  设置缓存类型
    profile->setHttpCacheType(QWebEngineProfile::DiskHttpCache);

    // 设置请求的UserAgent
    QString request_user_agent = "";
    // 设置HTTP UserAgent
    QString user_agent = profile->httpUserAgent();
    profile->setHttpUserAgent(user_agent);
    // 设置Cookies
    //    if (QWebEngineCookieStore* store = profile->cookieStore()) {
    //        QNetworkCookie cookie("", request_user_agent.toUtf8());
    //        store->setCookie(cookie);
    //    }
}

void initDefaultWebSettings(QWebEngineView* view)
{
    QWebEngineSettings* settings = view->settings();
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanPaste, true);
    settings->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    settings->setAttribute(QWebEngineSettings::AllowWindowActivationFromJavaScript, true);
#ifdef Q_OS_WIN
    static constexpr const char* kMSYaHeiFontFamily = "\345\276\256\350\275\257\351\233\205\351\273\221";
    settings->setFontFamily(QWebEngineSettings::StandardFont, kMSYaHeiFontFamily);
#endif  // Q_OS_WIN
    return;
}
