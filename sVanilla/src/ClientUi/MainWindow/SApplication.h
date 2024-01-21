#pragma once
#include <QApplication>

class SApplication : public QApplication
{
    Q_OBJECT
public:
    SApplication(int& argc, char** argv);
    ~SApplication();
};

#define sApp static_cast<SApplication*>(qApp)
