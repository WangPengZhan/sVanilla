#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class PluginsInfo;
}
QT_END_NAMESPACE

class PluginsInfo : public QWidget
{
    Q_OBJECT

public:
    explicit PluginsInfo(QWidget* parent = nullptr);
    ~PluginsInfo();

protected:
    void showEvent(QShowEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

    void initPluginInfos();
    void setPluginConfig();

private:
    Ui::PluginsInfo* ui;
    bool m_showFirst = true;
    bool m_isTip = true;
};
