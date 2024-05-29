#include "About.h"
#include "ui_About.h"
#include "version.h"

About::About(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::About)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

About::~About()
{
    delete ui;
}

void About::setUi()
{
    const QStringList textList = {tr("About"), tr("Authors"), tr("License")};
    ui->verticalNavigationWidget->setItemList(textList);
    const QStringList iconList(
        {QStringLiteral(":/icon/setting/about.svg"), QStringLiteral(":/icon/setting/author.svg"), QStringLiteral(":/icon/setting/license.svg")});
    ui->verticalNavigationWidget->setIconList(iconList);
    ui->verticalNavigationWidget->setVertical();
    constexpr int navigationWidth = 100;
    ui->verticalNavigationWidget->setColumnWidth(navigationWidth);
    constexpr int navigationRowHeight = 35;
    ui->verticalNavigationWidget->setRowHeight(navigationRowHeight);

    ui->verticalLayoutAppInfo->setAlignment(ui->labelIcon, Qt::AlignCenter);
    ui->labelVerContext->setText(SVNLA_VERSION_STR);
    ui->labelGitHashContext->setText(GIT_HASH);
    ui->labelBranchName->setText(GIT_BRANCH);
    ui->labelBuildTimeContext->setText(SVNLA_BUILD_STR);
}

void About::signalsAndSlots()
{
    connect(ui->verticalNavigationWidget, &Vanilla::ToggleButton::currentItemChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}
