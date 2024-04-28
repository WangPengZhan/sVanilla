#include <QClipboard>
#include <QTimer>
#include <QDebug>
#include <QStandardPaths>

#include "Util/UrlProcess.h"
#include "HomePage.h"
#include "ui_HomePage.h"

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
    ui->HistoryBtn->setIcon(QIcon(":icon/home/history.svg"));
    ui->ClipBoardBtn->setIcon(QIcon(":icon/home/clipboard.svg"));
    ui->HomePageSubmit->setIcon(QIcon(":icon/home/enter.svg"));
    ui->HomePageSubmit->setLabel(QStringLiteral("Submit"));
    ui->HomePageSubmit->setIconFirst(false);
    ui->IconBtn->setIcon(QIcon(":icon/home/icon.svg"));
    ui->IconBtn->setIconSize({40, 40});
    ui->LearnBtn->setIcon(QIcon(":icon/home/openurl.svg"));
    ui->HomeLineEdit->setFixedHeight(30);
}

void HomePage::parseUri(const std::string& uri)
{
    // if bili
    emit loadBiliViewView(uri);
}

