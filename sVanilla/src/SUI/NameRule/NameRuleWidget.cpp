#include "NameRuleWidget.h"
#include "ui_NameRuleWidget.h"

NameRuleWidget::NameRuleWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::NameRuleWidget)
{
    ui->setupUi(this);
    signalsAndSlots();
}

NameRuleWidget::~NameRuleWidget()
{
    delete ui;
}

void NameRuleWidget::init(const std::unordered_map<std::string, std::string>& rules)
{
    m_rules = rules;

    std::vector<QString> nameRules;
    nameRules.reserve(m_rules.size());
    for (const auto& [fst, snd] : m_rules)
    {
        nameRules.push_back(QString::fromStdString(fst));
    }
    initListWidget(nameRules);
}

void NameRuleWidget::updateLineEdit(const QString& rule)
{
    ui->lineEditInput->setText(rule);
    emit ui->lineEditInput->textChanged(rule);
}

void NameRuleWidget::hidePreviewLabel(bool hide)
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

void NameRuleWidget::signalsAndSlots()
{
    connect(ui->lineEditInput, &NameRuleEditWidget::textChanged, this, [this](const QString& newText) {
        updatePreview(newText);
        emit editingFinished(newText);
    });
}

std::string NameRuleWidget::parseNameRules(const std::string& rules)
{
    std::string replacedRules = rules;
    constexpr auto ruleStart = '$';
    size_t startPos = 0;

    while ((startPos = replacedRules.find(ruleStart, startPos)) != std::string::npos)
    {
        const size_t endPos = replacedRules.find(ruleStart, startPos + 1);
        if (endPos == std::string::npos)
        {
            break;
        }

        std::string foundRule = replacedRules.substr(startPos + 1, endPos - startPos - 1);
        if (auto iter = m_rules.find(foundRule); iter != m_rules.end())
        {
            replacedRules.replace(startPos, endPos - startPos + 1, iter->second);
            startPos += iter->second.length();
        }
        else
        {
            startPos = endPos + 1;
        }
    }
    return replacedRules;
}

void NameRuleWidget::updatePreview(const QString& preview)
{
    const auto previewStr = parseNameRules(preview.toStdString());
    ui->labelPreview->setText(QString::fromStdString(previewStr));
}
