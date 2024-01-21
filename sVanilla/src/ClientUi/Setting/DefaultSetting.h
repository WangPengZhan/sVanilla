//
// Created by Alanus Meminius on 2024/1/6.
//

#ifndef _DEFAULTSETTING_H_
#define _DEFAULTSETTING_H_

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

#endif  //_DEFAULTSETTING_H_
