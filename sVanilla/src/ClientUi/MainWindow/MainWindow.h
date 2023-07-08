#pragma once

#include <QtWidgets/QMainWindow>

namespace Ui{ class MainWindow; };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void SearchUrl();

private:
    void SetUi();
    void SignalsAndSlots();

private:
    Ui::MainWindow* ui;
};
