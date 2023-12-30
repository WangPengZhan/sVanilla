#pragma once

#include "windowbutton.h"

namespace Ui {
class WindowButtonPrivate {
    Q_DECLARE_PUBLIC(WindowButton)
public:
    WindowButtonPrivate();
    virtual ~WindowButtonPrivate();

    void init();

    WindowButton *q_ptr;

    QIcon iconNormal;
    QIcon iconChecked;
    QIcon iconDisabled;

    void reloadIcon();
};
}