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

    void updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version);

private:
    Ui::SettingPage* ui;
};
