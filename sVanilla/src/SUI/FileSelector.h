//
// Created by Alanus Meminius on 2024/1/8.
//

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
    class QAction* m_action = nullptr;
    class QValidator* m_validator;
    Ui::FileSelector* ui;
};
