#include <QClipboard>

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
    const QStringList iconList({QStringLiteral(":/icon/about.svg"), QStringLiteral(":/icon/author.svg"), QStringLiteral(":/icon/license.svg")});
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
    connect(ui->btnCopyInfo, &QPushButton::clicked, this, [&]() {
        QString split = ": ";
        QString text = ui->labelVersion->text() + split + ui->labelVerContext->text() + "\r\n" + ui->labelGitHash->text() + split +
                       ui->labelGitHashContext->text() + "\r\n" + ui->labelBransh->text() + split + ui->labelBranchName->text() + "\r\n" +
                       ui->labelBuildTime->text() + split + ui->labelBuildTimeContext->text() + "\r\n";
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(text);
    });
}
