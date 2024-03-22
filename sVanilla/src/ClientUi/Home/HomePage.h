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
signals:
    void HasAdded(bool hasAdded);
public slots:
    void updateMsg(const std::string& msg);

private:
    Ui::HomePage* ui;
    void signalsAndSlots();
    void parseUri(const std::string& uri);
    void addUri(const std::list<std::string>& uris);
    bool hasAdded = false;

};
