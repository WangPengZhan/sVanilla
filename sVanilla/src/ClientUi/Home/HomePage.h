//
// Created by Alanus Meminius on 2024/1/5.
//

#ifndef _HOMEPAGE_H_
#define _HOMEPAGE_H_

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

private:
    Ui::HomePage* ui;
};

#endif  //_HOMEPAGE_H_
