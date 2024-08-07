#pragma once

#include <QStackedWidget>
#include <QWidget>
#include <functional>

class QLabel;
class QLineEdit;
class QCheckBox;
class QToolButton;
class QValidator;

class EditableLabel : public QStackedWidget
{
    Q_OBJECT
public:
    EditableLabel(const QString& text, bool editable, QWidget* parent = nullptr);
    EditableLabel(QWidget* parent = nullptr);

    void setUi();

    void setText(const QString& text);
    QString text() const;

    bool getEditedStatus() const
    {
        return isEdited;
    };

    void setReadOnly(bool isReadOnly);
    bool readOnly() const;

    void setValidator(const QValidator* pValidator);
    const QValidator* validator() const;

    void setMaxLength(int nMax);
    int maxLength() const;

    void setPrefix(const QString& strPrefix);
    const QString& prefix() const;

    void setSuffix(const QString& strSuffix);
    const QString& suffix() const;

    void setCheckFunc(const std::function<bool(QLineEdit*)>& checkFuc);
    const std::function<bool(QLineEdit*)>& checkFunc() const;

signals:
    void sigRenamed(QString newName);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    void slotEditfinished();

private:
    QLabel* m_pLabel;
    QLineEdit* m_pEdit;
    QString m_suffix;
    QString m_prefix;
    bool isEdited;
    bool m_isReadOnly;
    int m_maxLength = 50;
    std::function<bool(QLineEdit*)> m_checkFunc;
};

class CollapsePanel : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsePanel(QString title, QWidget* pContentWidget, bool displayCheckbox = true, bool displayCloseBtn = true, bool titleEditable = true,
                           QWidget* parent = nullptr);
    void setCheckboxState(bool isChecked);
    bool isCheckboxState() const;

    void setExpanded(bool bExpand);
    bool isExpanded() const;

    EditableLabel* getTitleLabel()
    {
        return m_pTextLabel;
    };

    void setCheckBoxStyle(const QString& qss);

signals:
    void sigCollapseBtnClicked(bool);
    void sigCloseBtnClicked(bool);
    void sigCheckBoxStateChanged(int);

private:
    QCheckBox* m_pCheckbox;
    EditableLabel* m_pTextLabel;
    QToolButton* m_pCloseBtn;
    QToolButton* m_pCollapseBtn;
    QWidget* m_pContentWidget;
};
