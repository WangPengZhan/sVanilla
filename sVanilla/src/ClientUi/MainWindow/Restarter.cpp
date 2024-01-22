#include <QDir>
#include <QProcess>
#include <QApplication>

#include "Restarter.h"

Restarter::Restarter(int argc, char* argv[])
{
    Q_UNUSED(argc)
    m_executable = QString::fromLocal8Bit(argv[0]);
    m_workingPath = QDir::currentPath();
}

void Restarter::setArguments(const QStringList& args)
{
    m_args = args;
}

QString Restarter::executable() const
{
    return m_executable;
}

QStringList Restarter::arguments() const
{
    return m_args;
}

QString Restarter::workingPath() const
{
    return m_workingPath;
}

int Restarter::restartOrExit(int exitCode)
{
    return qApp->property("restart").toBool() ? restart(exitCode) : exitCode;
}

int Restarter::restart(int exitCode)
{
    QProcess::startDetached(m_executable, m_args, m_workingPath);
    return exitCode;
}
