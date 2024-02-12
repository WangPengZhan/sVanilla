
#include "VideoDetailWidget.h"
#include <utility>
#include "ui_VideoDetailWidget.h"

VideoDetailWidget::VideoDetailWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoDetailWidget)
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

VideoDetailWidget::~VideoDetailWidget()
{
    delete ui;
}
void VideoDetailWidget::updateUi(const std::string& data)
{
    ui->DetailTitle->setText(QString::fromStdString(data));
}

void VideoDetailWidget::setUi()
{
    this->setMaximumWidth(150);
}

void VideoDetailWidget::signalsAndSlots()
{
    connect(ui->DetailPanelCloseBtn, &QPushButton::clicked,this, [this]() {
        this->hide();
    });
}
