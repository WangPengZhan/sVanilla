#include <QClipboard>
#include <QTimer>
#include "HomePage.h"
#include "ui_HomePage.h"
#include "ClientUi/Event.h"
#include "Util/UrlProcess.h"

HomePage::HomePage(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::HomePage)
{
    ui->setupUi(this);
    SignalsAndSlots();

    auto* timer = new QTimer(this);
    timer->setInterval(5000);
    connect(timer, &QTimer::timeout, this, [this] {
        ui->ErrorMsgLabel->clear();
    });
    timer->start();
}

HomePage::~HomePage()
{
    delete ui;
}
void HomePage::SignalsAndSlots()
{
    // ClipBoardBtn clicked signal -> Event::AddUri (ui -> core)
    connect(ui->ClipBoardBtn, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        if (const QString originalText = clipboard->text(); !util::UrlProcess::IsUrl(originalText))
        {
            ui->ErrorMsgLabel->setText(QStringLiteral("Not a valid url"));
        } else {
            emit AddUri({originalText.toStdString()});
        }
    });

    // HomeLineEdit start signal -> Event::AddUri (ui -> core)
    connect(ui->HomeLineEdit, &SearchLineEdit::Complete, this, [this] {
        qDebug() << ui->HomeLineEdit->text();
        emit Event::getInstance() -> AddUri({ui->HomeLineEdit->text().toStdString()});
        ui->HomeLineEdit->clear();
    });

    // Event::updateMsg -> update MsgLabel (core -> ui)
    // connect(Event::getInstance(), &Event::updateMsg, this, [this](const std::string& msg) {
    //     ui->ErrorMsgLabel->setText(QString::fromStdString(msg));
    // });

}
