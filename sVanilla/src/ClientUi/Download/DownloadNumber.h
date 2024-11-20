#pragma once

#include <QWidget>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DownloadNumber;
}  //  namespace Ui
QT_END_NAMESPACE

class DownloadNumber : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadNumber(QWidget* parent = nullptr);
    ~DownloadNumber();

    void setDownloadingNumber(int downloading);
    void setDownloadErrorNumber(int dowloadError);
    int downloadingNumber() const;
    int downloadErrorNumber() const;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::DownloadNumber* ui;
    QMovie* m_movie;
};