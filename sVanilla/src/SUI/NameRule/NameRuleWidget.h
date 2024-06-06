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

    void updateLineEdit(const QString& rule);

    void hidePreviewLabel(bool hide);
    bool isHidePreviewLabel() const;

    Q_SIGNAL void editingFinished(const QString& rule);

private:
    void signalsAndSlots();
    void initListWidget(const std::vector<QString>& nameRules) const;
    std::string parseNameRules(const std::string& rules);
    void updatePreview(const QString& preview);

private:
    Ui::NameRuleWidget* ui;
    std::unordered_map<std::string, std::string> m_rules;
};
