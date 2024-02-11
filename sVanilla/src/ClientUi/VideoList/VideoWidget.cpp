
// You may need to build the project (run Qt uic code generator) to get "ui_VideoWidget.h" resolved

#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "VideoGridWidget.h"

#include <QtWidgets/QLabel>

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::VideoPage) {
    ui->setupUi(this);
    ui->VideoDetailWidget->hide();
    ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);

    connect(ui->VideoListBtn, &QPushButton::clicked, [this] {
        ui->VideoStackedPage->setCurrentWidget(ui->VideoList);
    });
    connect(ui->VideoGridBtn, &QPushButton::clicked, [this] {
        ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);
    });

    addVideoItem("123");
    addVideoItem("123");
    addVideoItem("123");
    connect(ui->VideoGridWidget, &VideoGridWidget::itemDetailBtnClick, this, &VideoWidget::showDetailPanel);
    connect(ui->VideoListWidget, &VideoListWidget::itemDetailBtnClick,this,&VideoWidget::showDetailPanel);
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
void VideoWidget::showDetailPanel()
{
    ui->VideoDetailWidget->show();
}
void VideoWidget::updateVideoItem(const std::shared_ptr<BiliApi::VideoView>& videoView)
{
    ui->VideoGridWidget->addVideoItem(videoView->bvid);
    ui->VideoGridWidget->updateVideoItem(videoView);
}