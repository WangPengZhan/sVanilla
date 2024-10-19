#include "CollapsePanel.h"

#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QStyle>
#include <QToolButton>

EditableLabel::EditableLabel(const QString& text, bool editable, QWidget* parent)
    : QStackedWidget(parent)
    , m_pLabel(new QLabel(text, this))
    , m_pEdit(new QLineEdit(text, this))
    , m_isReadOnly(!editable)
{
    setUi();
}

EditableLabel::EditableLabel(QWidget* parent)
    : QStackedWidget(parent)
    , m_pLabel(new QLabel(this))
    , m_pEdit(new QLineEdit(this))
    , isEdited(false)
    , m_isReadOnly(false)
{
    setUi();
}

void EditableLabel::setUi()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    addWidget(m_pLabel);
    addWidget(m_pEdit);

    m_pLabel->installEventFilter(this);

    connect(m_pEdit, &QLineEdit::editingFinished, this, &EditableLabel::slotEditfinished);
}

void EditableLabel::setText(const QString& text)
{
    m_pLabel->setText(text);
    m_pEdit->setText(text);
}

QString EditableLabel::text() const
{
    QString ret;
    if (m_pLabel != nullptr)
    {
        ret = m_pLabel->text();
    }
    return ret;
}

void EditableLabel::setReadOnly(bool isReadOnly)
{
    m_isReadOnly = isReadOnly;
}

bool EditableLabel::readOnly() const
{
    return m_isReadOnly;
}

void EditableLabel::setValidator(const QValidator* pValidator)
{
    m_pEdit->setValidator(pValidator);
}

const QValidator* EditableLabel::validator() const
{
    return m_pEdit->validator();
}

void EditableLabel::setMaxLength(int nMax)
{
    m_maxLength = nMax;
}

int EditableLabel::maxLength() const
{
    return m_maxLength;
}

void EditableLabel::setPrefix(const QString& strPrefix)
{
    m_prefix = strPrefix;
}

const QString& EditableLabel::prefix() const
{
    return m_prefix;
}

void EditableLabel::setSuffix(const QString& strSuffix)
{
    m_suffix = strSuffix;
}

const QString& EditableLabel::suffix() const
{
    return m_suffix;
}

void EditableLabel::setCheckFunc(const std::function<bool(QLineEdit*)>& checkFuc)
{
    m_checkFunc = checkFuc;
}

const std::function<bool(QLineEdit*)>& EditableLabel::checkFunc() const
{
    return m_checkFunc;
}

bool EditableLabel::eventFilter(QObject* obj, QEvent* ev)
{
    if (m_isReadOnly)
    {
        return QStackedWidget::eventFilter(obj, ev);
    }

    if (obj == m_pLabel)
    {
        if (ev->type() == QEvent::MouseButtonDblClick)
        {
            isEdited = true;
            auto txt = m_pLabel->text();
            if (txt.startsWith(m_prefix, Qt::CaseInsensitive))
            {
                txt = txt.right(txt.length() - m_prefix.length());
            }
            if (txt.endsWith(m_suffix, Qt::CaseInsensitive))
            {
                txt = txt.left(txt.length() - m_suffix.length());
            }
            m_pEdit->setText(txt);
            m_pEdit->setText(txt);
            m_pEdit->selectAll();
            m_pEdit->setFocus();
            setCurrentIndex(1);
        }
    }

    return QStackedWidget::eventFilter(obj, ev);
}

void EditableLabel::slotEditfinished()
{
    if (m_checkFunc)
    {
        if (m_checkFunc(m_pEdit))
        {
            auto preText = m_pLabel->text();
            auto newText = m_pEdit->text() + m_suffix;
            m_pLabel->setText(newText);
            if (preText != newText)
            {
                emit sigRenamed(newText);
            }
        }
    }

    setCurrentIndex(0);
}

CollapsePanel::CollapsePanel(QString title, QWidget* pContentWidget, bool displayCheckbox, bool displayCloseBtn, bool titleEditable, QWidget* parent)
    : QWidget(parent)
    , m_pContentWidget(pContentWidget)
{
    m_pContentWidget->setParent(this);
    m_pCheckbox = new QCheckBox(this);
    m_pCheckbox->setVisible(displayCheckbox);
    connect(m_pCheckbox, &QCheckBox::stateChanged, this, &CollapsePanel::sigCheckBoxStateChanged);

    m_pTextLabel = new EditableLabel(title, titleEditable, this);

    m_pCloseBtn = new QToolButton(this);
    m_pCloseBtn->setIcon(QIcon(":/images/filterpanel/close.svg"));
    m_pCloseBtn->setVisible(displayCloseBtn);

    m_pCollapseBtn = new QToolButton(this);
    m_pCollapseBtn->setCheckable(true);
    m_pCollapseBtn->setChecked(true);

    connect(m_pCloseBtn, &QToolButton::clicked, this, &CollapsePanel::sigCloseBtnClicked);
    connect(m_pCollapseBtn, &QToolButton::clicked, [this](bool isChecked) {
        hide();  // avoid flash
        emit sigCollapseBtnClicked(isChecked);
        m_pContentWidget->setVisible(isChecked);
        QIcon icon;
        if (isChecked)
        {
            icon.addFile(":/images/filterpanel/arrow_up.svg");
        }
        else
        {
            icon.addFile(":/images/filterpanel/arrow_down.svg");
        }
        m_pCollapseBtn->setIcon(icon);
        show();
    });
    m_pCollapseBtn->clicked(true);
    QHBoxLayout* headLayout = new QHBoxLayout(this);
    headLayout->addWidget(m_pCheckbox);
    headLayout->addWidget(m_pTextLabel);
    headLayout->addStretch();
    headLayout->addWidget(m_pCloseBtn);
    headLayout->addWidget(m_pCollapseBtn);
    headLayout->setContentsMargins(16, 1, 1, 1);
    headLayout->setSpacing(1);
    QWidget* pHeadbar = new QWidget;
    pHeadbar->setLayout(headLayout);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(pHeadbar);
    mainLayout->addWidget(m_pContentWidget);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->setSpacing(1);
    setLayout(mainLayout);
}

void CollapsePanel::setCheckboxState(bool isChecked)
{
    m_pCheckbox->setChecked(isChecked);
}

bool CollapsePanel::isCheckboxState() const
{
    return m_pCheckbox->isChecked();
}

void CollapsePanel::setExpanded(bool bExpand)
{
    m_pCollapseBtn->setChecked(bExpand);
    m_pCollapseBtn->clicked(bExpand);
}

bool CollapsePanel::isExpanded() const
{
    return m_pCollapseBtn->isChecked();
}

void CollapsePanel::setCheckBoxStyle(const QString& qss)
{
    if (!qss.isEmpty())
    {
        m_pCheckbox->setStyleSheet(qss);
    }
}
