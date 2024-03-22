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

    auto* timer = new QTimer(this);
    timer->setInterval(5000);
    connect(timer, &QTimer::timeout, this, [this] {
        ui->ErrorMsgLabel->clear();
    });
    timer->start();

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

HomePage::~HomePage()
{
    delete ui;
}
void HomePage::setMsgLabel(const std::string& msg)
{
    ui->ErrorMsgLabel->setText(QString::fromStdString(msg));
}
void HomePage::updateMsg(const std::string& msg)
{
    setMsgLabel(msg);
}

void HomePage::signalsAndSlots()
{
    // ClipBoardBtn clicked signal -> Event::AddUri (ui -> core)
    connect(ui->ClipBoardBtn, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        if (const QString originalText = clipboard->text(); !util::UrlProcess::IsUrl(originalText))
        {
            setMsgLabel("Not a valid url");
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
        ui->HomeLineEdit->clear();
    });
}

void HomePage::parseUri(const std::string& uri)
{
    // if bili
    emit loadBiliViewView(uri);
}

