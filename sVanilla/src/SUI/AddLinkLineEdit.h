#pragma once
#include <QLineEdit>

namespace Ui
{
class AddLinkLineEdit;
};

class AddLinkLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit AddLinkLineEdit(QWidget* parent = nullptr);
    ~AddLinkLineEdit();

    void SetEditFinishedSearch(bool enabled);
    void setWebsiteIcon(const QString& iconPath) const;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

signals:
    void Complete();

private:
    void setUi();
    void signalsAndSlots();
    void resizeIcons(const QSize& size);

private:
    Ui::AddLinkLineEdit* ui;
};
