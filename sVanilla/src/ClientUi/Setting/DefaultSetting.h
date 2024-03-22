#pragma once

#include <QButtonGroup>
#include <QWidget>

#include "Aria2Net/Protocol/Protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class DefaultSetting;
}
QT_END_NAMESPACE

class DefaultSetting : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultSetting(QWidget* parent = nullptr);
    ~DefaultSetting() override;

    void updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version);
    void updateStatus(const std::string& status);
    void updateVersion(const std::string& version);
    void updateFeatures(const std::string& features);

    Q_SIGNAL void UpdateTheme(int theme);

private:
    void signalsAndSlots();
    inline void setRedStatus();
    inline void setGreenStatus();

private:
    Ui::DefaultSetting* ui;
    QButtonGroup* m_themeGroup;
};
