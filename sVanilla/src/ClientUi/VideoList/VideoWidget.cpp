
// You may need to build the project (run Qt uic code generator) to get "ui_VideoWidget.h" resolved

#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "VideoGridWidget.h"

#include <QtWidgets/QLabel>

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::VideoPage) {
    ui->setupUi(this);
    // ui->VideoStackedWidget->setCurrentWidget(ui->VideoTab);
    ui->VideoStackedPage->setCurrentWidget(ui->VideoTab);

    connect(ui->VideoListBtn, &QPushButton::clicked, [this] {
        ui->VideoStackedPage->setCurrentWidget(ui->VideoList);
    });
    connect(ui->VideoGridBtn, &QPushButton::clicked, [this] {
        ui->VideoStackedPage->setCurrentWidget(ui->VideoTab);
    });

    addVideoItem("123");
    addVideoItem("123");
    addVideoItem("123");

}

VideoWidget::~VideoWidget()
{
    delete ui;
}
void VideoWidget::addVideoItem(const std::string& bvid)
{
    ui->VideoGridWidget->addVideoItem(bvid);
    ui->VideoListWidget->addVideoItem(bvid);
}
