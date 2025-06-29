#include "NameRuleWidget.h"
#include "ui_NameRuleWidget.h"

#include <QMap>

NameRuleWidget::NameRuleWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NameRuleWidget)
{
    ui->setupUi(this);
    signalsAndSlots();
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    adjustSize();
}

NameRuleWidget::~NameRuleWidget()
{
    delete ui;
}

void NameRuleWidget::init(const std::unordered_map<std::string, std::string>& rules)
{
    std::vector<QString> nameRules;
    QMap<QString, QString> nameRulesMap;
    nameRules.reserve(rules.size());
    for (const auto& [fst, snd] : rules)
    {
        nameRules.push_back(QString::fromStdString(fst));
        nameRulesMap[QString::fromStdString(fst)] = QString::fromStdString(snd);
    }
    initListWidget(nameRules);
    ui->lineEditInput->setRules(nameRulesMap);
}

void NameRuleWidget::updateLineEdit(const QString& rule) const
{
    ui->lineEditInput->setText(rule);
    emit ui->lineEditInput->textChanged(rule);
}

void NameRuleWidget::hidePreviewLabel(bool hide) const
{
    if (hide)
    {
        ui->labelPreview->hide();
        ui->pushButton->hide();
    }
    else
    {
        ui->labelPreview->show();
        ui->pushButton->show();
    }
}

bool NameRuleWidget::isHidePreviewLabel() const
{
    return ui->labelPreview->isHidden();
}

void NameRuleWidget::initListWidget(const std::vector<QString>& nameRules) const
{
    ui->listNameRule->setNameRules(nameRules);
}

void NameRuleWidget::setParseNameRulesFunction(const std::function<std::string(const std::string&)>& parseNameRulesFunction)
{
    m_parseNameRulesFunction = parseNameRulesFunction;
}

void NameRuleWidget::signalsAndSlots()
{
    connect(ui->lineEditInput, &NameRuleEditWidget::textChanged, this, [this](const QString& newText) {
        updatePreview(newText);
        emit editingFinished(newText);
    });
}

void NameRuleWidget::updatePreview(const QString& preview)
{
    std::string previewStr = preview.toStdString();
    if (m_parseNameRulesFunction)
    {
        previewStr = m_parseNameRulesFunction(previewStr);
    }
    ui->labelPreview->setText(QString::fromStdString(previewStr));
}
