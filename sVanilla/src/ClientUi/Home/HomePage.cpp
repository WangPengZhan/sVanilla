#include "HomePage.h"
#include "ui_HomePage.h"
#include "ClientUi/Event.h"

HomePage::HomePage(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::HomePage)
{
    ui->setupUi(this);
//    connect(ui->HomeLineEdit, &SearchLineEdit::searchCliecked, Event::getInstance(), &Event::SearchBtnClick);
    connect(ui->ClipBoardBtn, &QPushButton::clicked, Event::getInstance(), &Event::ClipboardBtnClick);
    connect(Event::getInstance(), &Event::updateMsg,this,[this](const std::string& msg){
        ui->ErrorMsgLabel->setText(QString::fromStdString(msg));
    });
}

HomePage::~HomePage()
{
    delete ui;
}

