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
    Q_SIGNAL void HasAdded(bool hasAdded);
    Q_SIGNAL void loadBiliViewView(const std::string& uri);
    void updateMsg(const std::string& msg);

private:
    Ui::HomePage* ui;
    void signalsAndSlots();
    void parseUri(const std::string& uri);
    bool hasAdded = false;
};
