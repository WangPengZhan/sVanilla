#pragma once

#include <QListWidget>
#include <QWidget>

struct LicenseInfo;
namespace Ui
{
class LicenseItemWidget;
}  // namespace Ui

class LicenseItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LicenseItemWidget(QWidget* parent = nullptr);
    explicit LicenseItemWidget(const LicenseInfo& license, QListWidgetItem* item, QWidget* parent = nullptr);
    ~LicenseItemWidget();

    void setLicense(const LicenseInfo& license);
    void setListWidgetItem(QListWidgetItem* item);

private:
    void setUi();
    void signalsAndSlots();

private:
    Ui::LicenseItemWidget* ui;
    QListWidgetItem* m_item;
};

class LicenseListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit LicenseListWidget(QWidget* parent = nullptr);

    void addLicense(const LicenseInfo& license);
    void addLicenses(const std::vector<LicenseInfo>& licenses);
};
