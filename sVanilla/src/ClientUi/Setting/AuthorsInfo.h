#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class AuthorsInfo;
}
QT_END_NAMESPACE

class AuthorsInfo : public QWidget
{
    Q_OBJECT

public:
    explicit AuthorsInfo(QWidget* parent = nullptr);
    ~AuthorsInfo();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setUi();
    void signalsAndSlots();

    void loadAuthors();

private:
    Ui::AuthorsInfo* ui;
    bool m_resized = false;
};
