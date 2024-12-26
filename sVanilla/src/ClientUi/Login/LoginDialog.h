#pragma once
#include <QDialog>
#include <QImage>
#include <QMovie>
#include <QBuffer>

#include <memory>

#include <Login.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class LoginDialog;
}
QT_END_NAMESPACE

class LoginMonitor;
class LoginProxy;
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(std::shared_ptr<LoginProxy> loginer, QDialog* parent = nullptr);
    ~LoginDialog();

    void slotStatusChanged(AbstractLogin::LoginSatus status);
    void slotBtnRefreshClicked();

    static QPixmap binToImage(const std::vector<uint8_t>& bin, QSize size);

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject*, QEvent*) override;

private:
    void setUi();
    void signalsAndSlots();

    void loadOrc();

private:
    Ui::LoginDialog* ui;
    LoginMonitor* m_monitor;
    std::shared_ptr<LoginProxy> m_loginer;
    AbstractLogin::LoginSatus m_status;
    std::string m_context;
    QMovie m_movie;
    QBuffer m_buffer;
};
