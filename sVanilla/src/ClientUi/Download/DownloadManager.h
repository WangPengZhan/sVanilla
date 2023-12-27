#include <QObject>

class AbstractDownloader : public QObject
{
    Q_OBJECT
public:
    enum Status
    {
        Ready,
        Downloading,
        Paused,
        Stopped,
        Finished,
        Error
    };

    using QObject::QObject;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void downloadStatus() = 0;

    Status status() const;

signals:
    void progressChanged(int progress);
    void finished();
    void error(QString error);

protected:
    Status m_status;
};
