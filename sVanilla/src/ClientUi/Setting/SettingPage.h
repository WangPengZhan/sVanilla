#pragma once

#include <QWidget>

#include "Aria2Net/Protocol/Protocol.h"

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
    void connectAria2Server();

    Q_SIGNAL void UpdateTheme(int theme);

public:
    bool isConnect = false;

private:
    Ui::SettingPage* ui;
};
