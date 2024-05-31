#pragma once

#include <QButtonGroup>
#include <QWidget>

#include "Aria2Net/Protocol/Protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class DefaultSettings;
}
QT_END_NAMESPACE

class DefaultSettings : public QWidget
{
    Q_OBJECT

public:
    explicit DefaultSettings(QWidget* parent = nullptr);
    ~DefaultSettings() override;

    void updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version);
    void updateStatus(const std::string& status);
    void updateVersion(const std::string& version);
    void updateFeatures(const std::string& features);
    [[nodiscard]] Qt::CheckState getTrayState() const;
    [[nodiscard]] Qt::CheckState isEnableMinimizeTray() const;
    [[nodiscard]] Qt::CheckState isSaveMainWindow() const;

signals:
    void updateTheme(int theme);
    void enableTray(int);

private:
    void setUi();
    void signalsAndSlots();
    inline void setRedStatus();
    inline void setGreenStatus();
    void autoStartRun(bool isRun);

private:
    Ui::DefaultSettings* ui;
    QButtonGroup* m_themeGroup;
};
