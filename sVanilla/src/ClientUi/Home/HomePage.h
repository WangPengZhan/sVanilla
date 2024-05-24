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
    ~HomePage();

    void setWebsiteIcon(const QString& iconPath);

signals:
    void HasAdded(bool hasAdded);
    void updateWebsiteIcon(const std::string& uri);
    void parseUri(const std::string& uri);

private:
    void signalsAndSlots();
    void setUi();
    void createHistoryMenu();

private:
    Ui::HomePage* ui;
    std::list<std::string> m_history;
    QMenu* m_historyMenu = nullptr;
};
