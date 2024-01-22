#pragma once

#include "ClientUi/Event.h"
#include <QButtonGroup>
#include <QWidget>

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
signals:
    void UpdateTheme(int theme);

private:
    Ui::DefaultSetting* ui;
    void signalsAndSlots();
    QButtonGroup* m_themeGroup;

    void updateStatus(const std::string& status);
    void updateVersion(const std::string& version);
    void updateFeatures(const std::string& features);
    inline void setRedStatus();
    inline void setGreenStatus();
};

