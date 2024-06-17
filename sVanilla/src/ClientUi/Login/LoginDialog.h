#pragma once
#include <QDialog>
#include <QImage>
#include <QMovie>
#include <QBuffer>

#include <memory>

#include "Login/login.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class LoginDialog;
}
QT_END_NAMESPACE

class LoginMonitor;
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(std::shared_ptr<AbstractLogin> loginer, QDialog* parent = nullptr);
    ~LoginDialog();

    void slotStatusChanged(AbstractLogin::LoginSatus status);
    void slotBtnRefreshClicked();

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject*, QEvent*) override;

private:
    void setUi();
    void signalsAndSlots();

    void loadOrc();

    static QPixmap binToImage(const std::vector<uint8_t>& bin, QSize size);

private:
    Ui::LoginDialog* ui;
    LoginMonitor* m_monitor;
    std::shared_ptr<AbstractLogin> m_loginer;
    AbstractLogin::LoginSatus m_status;
    std::string m_context;
    QMovie m_movie;
    QBuffer m_buffer;
};
