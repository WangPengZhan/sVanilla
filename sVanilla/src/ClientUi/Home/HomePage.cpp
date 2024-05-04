#include <QClipboard>
#include <QTimer>
#include <QDebug>
#include <QPushButton>
#include <QStandardPaths>

#include "HomePage.h"
#include "ui_HomePage.h"
#include "Util/UrlProcess.h"
#include "VanillaStyle/Style.h"

HomePage::HomePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    signalsAndSlots();
    setUi();
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::signalsAndSlots()
{
    // ClipBoardBtn clicked signal -> Event::AddUri (ui -> core)
    connect(ui->ClipBoardBtn, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        if (const QString originalText = clipboard->text(); !util::UrlProcess::IsUrl(originalText))
        {
        }
        else
        {
            parseUri({originalText.toStdString()});
        }
    });

    // HomeLineEdit start signal -> Event::AddUri (ui -> core)
    connect(ui->HomeLineEdit, &SearchLineEdit::Complete, this, [this] {
        qDebug() << ui->HomeLineEdit->text();
        parseUri({ui->HomeLineEdit->text().toStdString()});
    });
}

void HomePage::setUi()
{
    // ui->IconBtn->setIcon(QIcon(":icon/home/whiteBg.svg"));
    constexpr int iconSize = 40;
    ui->IconBtn->setIconSize({iconSize, iconSize});

    constexpr int homeLineEditHeight = 30;
    ui->HomeLineEdit->setFixedHeight(homeLineEditHeight);
}

void HomePage::parseUri(const std::string& uri)
{
    // if bili
    emit loadBiliViewView(uri);
}

