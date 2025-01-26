#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui
{
class LoginWebDialog;
}
class QUrl;
QT_END_NAMESPACE

class LoginMonitor;
class LoginProxy;
class QNetworkCookie;

class LoginWebDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginWebDialog(std::shared_ptr<LoginProxy> loginer, QWidget* parent = nullptr);
    ~LoginWebDialog();

    void setUrl(const QUrl& url);

protected:
    void cookiesChanged(const QNetworkCookie& cookie);

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::LoginWebDialog* ui;
    std::shared_ptr<LoginProxy> m_loginer;
};
