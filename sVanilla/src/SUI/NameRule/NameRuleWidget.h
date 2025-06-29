#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class NameRuleWidget;
}
QT_END_NAMESPACE

class NameRuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NameRuleWidget(QWidget* parent = nullptr);
    ~NameRuleWidget() override;

    void init(const std::unordered_map<std::string, std::string>& rules);

    void updateLineEdit(const QString& rule) const;

    void hidePreviewLabel(bool hide) const;
    bool isHidePreviewLabel() const;

    void setParseNameRulesFunction(const std::function<std::string(const std::string&)>& parseNameRulesFunction);

    Q_SIGNAL void editingFinished(const QString& rule);

private:
    void signalsAndSlots();
    void initListWidget(const std::vector<QString>& nameRules) const;
    void updatePreview(const QString& preview);

private:
    Ui::NameRuleWidget* ui;
    std::function<std::string(const std::string&)> m_parseNameRulesFunction;
};
