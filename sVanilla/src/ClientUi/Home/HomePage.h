#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class HomePage;
}
QT_END_NAMESPACE

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget* parent = nullptr);
    ~HomePage() override;
    void setMsgLabel(const std::string& msg);
    void updateMsg(const std::string& msg);

signals:
    void HasAdded(bool hasAdded);
    void loadBiliViewView(const std::string& uri);

private:
    void signalsAndSlots();
    void parseUri(const std::string& uri);

private:
    Ui::HomePage* ui;
    bool hasAdded = false;
};
