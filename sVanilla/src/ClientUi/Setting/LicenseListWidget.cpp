#include "LicenseInfoWidget.h"
#include "LicenseListWidget.h"
#include "ui_LicenseListWidget.h"

LicenseItemWidget::LicenseItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LicenseItemWidget)
    , m_item(nullptr)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

LicenseItemWidget::LicenseItemWidget(const LicenseInfo& license, QListWidgetItem* item, QWidget* parent)
    : LicenseItemWidget(parent)
{
    setLicense(license);
    setListWidgetItem(item);
}

LicenseItemWidget::~LicenseItemWidget()
{
    delete ui;
}

void LicenseItemWidget::setLicense(const LicenseInfo& license)
{
    ui->labelLib->setText(QString::fromStdString(license.name));
    ui->labelVersion->setText(QString::fromStdString(license.version));
    ui->labelGithub->setText(QString("<a href=\"%1\">%1</a>").arg(QString::fromStdString(license.github)));
    ui->labelLicenseContext->setText(QString::fromStdString(license.license));
}

void LicenseItemWidget::setListWidgetItem(QListWidgetItem* item)
{
    m_item = item;
}

void LicenseItemWidget::setUi()
{
    ui->labelLicenseContext->hide();
}

void LicenseItemWidget::signalsAndSlots()
{
    connect(ui->btnExpended, &QPushButton::clicked, this, [this]() {
        const auto upIcon = QIcon(QString(":/icon/setting/up.svg"));
        const auto downIcon = QIcon(QString(":/icon/setting/down.svg"));
        ui->btnExpended->setIcon(ui->btnExpended->isChecked() ? upIcon : downIcon);
        ui->labelLicenseContext->setHidden(!ui->labelLicenseContext->isHidden());
        adjustSize();
        m_item->setSizeHint(QSize(0, sizeHint().height()));
    });
}

LicenseListWidget::LicenseListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

void LicenseListWidget::addLicense(const LicenseInfo& license)
{
    auto* pItem = new QListWidgetItem(this);
    auto* pWidget = new LicenseItemWidget(license, pItem, this);
    pItem->setSizeHint(QSize(0, pWidget->sizeHint().height()));
    setItemWidget(pItem, pWidget);
}

void LicenseListWidget::addLicenses(const std::vector<LicenseInfo>& licenses)
{
    for (const auto& license : licenses)
    {
        addLicense(license);
    }
}
