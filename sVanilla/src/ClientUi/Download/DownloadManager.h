#include <QObject>

class AbstractDownloader : public QObject
{
    Q_OBJECT
public:
    enum status
    {
        Ready,
        Downloading,
        Paused,
        Error
    };
    using QObject::QObject;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;

signals:
    void progressChanged(int progress);
    void finished();
    void error(QString error);
};
