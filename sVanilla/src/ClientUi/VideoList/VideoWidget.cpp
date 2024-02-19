#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "VideoGridWidget.h"
#include "SUI/ToggleSwitch.h"
#include <QPushButton>
#include <QtWidgets/QLabel>

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPage)
{
    ui->setupUi(this);

    // ui->VideoDetailWidget->hide();
    ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);
    connect(ui->SwitchBtn, &ToggleSwitch::toggled, ui->VideoStackedPage, &QStackedWidget::setCurrentIndex);

    ui->VideoGridWidget->getSignalPointer(ui->VideoGrid);
    ui->VideoListWidget->getSignalPointer(ui->VideoList);
#if 1
    for (int i = 0; i < 10; i++)
    {
        Adapter::BaseVideoView view = {std::to_string(i)};
        const auto videoView = std::make_shared<Adapter::BaseVideoView>(view);
        updateVideoItem(videoView);
    }
#endif
}

VideoWidget::~VideoWidget()
{
    delete ui;
}

void VideoWidget::updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{

    ui->VideoGridWidget->addVideoItem(videoView->Identifier);
    ui->VideoGridWidget->updateVideoItem(videoView);

    ui->VideoListWidget->addVideoItem(videoView->Identifier);
    ui->VideoListWidget->updateVideoItem(videoView);
}

void VideoWidget::clearVideo() const
{
    ui->VideoGridWidget->clearVideo();
    ui->VideoListWidget->clearVideo();
}

