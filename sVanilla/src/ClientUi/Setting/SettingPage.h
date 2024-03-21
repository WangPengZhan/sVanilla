#pragma once
#include "Aria2Net/Protocol/Protocol.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class SettingPage;
}
QT_END_NAMESPACE

class SettingPage final : public QWidget
{
    Q_OBJECT
public:
    explicit SettingPage(QWidget* parent = nullptr);

    void updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version) const;

    bool isConnect = false;
    Q_SLOT void connectAria2Server();
    Q_SIGNAL void UpdateTheme(int theme);
private:
    Ui::SettingPage* ui;
};
