#pragma once

#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui
{
class FileSelector;
}
QT_END_NAMESPACE

class FileSelector : public QLineEdit
{
    Q_OBJECT

public:
    enum Mode
    {
        FileOpen,
        Directory
    };
    explicit FileSelector(QWidget* parent = nullptr);
    explicit FileSelector(Mode mode, QWidget* parent = nullptr);
    ~FileSelector();

    Mode mode() const;
    void setMode(Mode mode);
    void chooseFile();

signals:
    void modeChanged(Mode mode);

private:
    Mode m_mode = Directory;
    QAction* m_action = nullptr;
    QValidator* m_validator;
    Ui::FileSelector* ui;
};
