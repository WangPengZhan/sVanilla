#pragma once
#include <QStringList>
#include <QString>

class Restarter
{
public:
    Restarter(int argc, char *argv[]);

    void setArguments(const QStringList &args);

    QString executable() const { return m_executable; }
    QStringList arguments() const { return m_args; }
    QString workingPath() const { return m_workingPath; }

    int restartOrExit(int exitCode);
    int restart(int exitCode);

private:
    QString m_executable;
    QStringList m_args;
    QString m_workingPath;
};