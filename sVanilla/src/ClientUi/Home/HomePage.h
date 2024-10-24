#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class HomePage;
}
QT_END_NAMESPACE

class AbstractLogin;

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget* parent = nullptr);
    ~HomePage();

    void setWebsiteIcon(const QString& iconPath);

signals:
    void hasAdded(bool hasAdded);
    void updateWebsiteIcon(const std::string& uri);
    void parseUri(const QString& uri);
    void loginSucceed(std::shared_ptr<AbstractLogin> loginer);

private:
    void signalsAndSlots();
    void setUi();
    void createHistoryMenu();

private:
    Ui::HomePage* ui;
    QMenu* m_historyMenu = nullptr;
};
