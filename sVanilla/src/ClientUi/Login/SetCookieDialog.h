#pragma once
#include <QDialog>

#include <memory>
#include <vector>

#include "ILogin.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class SetCookieDialog;
}
QT_END_NAMESPACE

namespace plugin
{
class IPlugin;
}

class LoginProxy;
class SetCookieDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SetCookieDialog(std::vector<std::shared_ptr<plugin::IPlugin>> plugins, QWidget* parent = nullptr);
    ~SetCookieDialog();

    std::shared_ptr<LoginProxy> loginer() const;

private:
    void setUi();
    void signalsAndSlots();

    void pluginChanged();
    void setPluginInfo(std::shared_ptr<plugin::IPlugin> plugin);
    static void writeCookieToDb(const AbstractLogin& login);

    void slotAccept();

private:
    Ui::SetCookieDialog* ui;
    std::vector<std::shared_ptr<plugin::IPlugin>> m_plugins;
    std::shared_ptr<LoginProxy> m_loginer;
};
