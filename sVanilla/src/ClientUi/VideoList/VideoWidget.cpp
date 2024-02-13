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
    detailAnimation = std::make_shared<GeometryAnimation>(this);
    detailAnimation->setTargetObject(detailPanel());
    ui->VideoDetailWidget->hide();
    ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);


    addVideoItem("123");
    addVideoItem("345");
    addVideoItem("567");
    auto VideoGridDetailsBtns = findChildren<QPushButton*>("VideoGridDetailsBtn", Qt::FindChildrenRecursively);
    for (const auto btn : VideoGridDetailsBtns)
    {
        btn->installEventFilter(this);
    }

    connect(ui->SwitchBtn, &ToggleSwitch::toggled, ui->VideoStackedPage, &QStackedWidget::setCurrentIndex);
    // connect(ui->VideoGridWidget, &VideoGridWidget::itemDetailBtnClick, this, &VideoWidget::showDetailPanel);
    // connect(ui->VideoListWidget, &VideoListWidget::itemDetailBtnClick,this,&VideoWidget::showDetailPanel);
    // connect(ui->VideoGridWidget, &VideoGridWidget::handleDetialCheckBtnClick, this, &VideoWidget::updateDetailPanelVisibility);
}

VideoWidget::~VideoWidget()
{
    delete ui;
}
void VideoWidget::addVideoItem(const std::string& bvid) const
{
    ui->VideoGridWidget->addVideoItem(bvid);
    ui->VideoListWidget->addVideoItem(bvid);
}

void VideoWidget::updateVideoItem(const std::shared_ptr<BiliApi::VideoView>& videoView) const
{
    ui->VideoGridWidget->addVideoItem(videoView->bvid);
    ui->VideoGridWidget->updateVideoItem(videoView);
}

bool VideoWidget::processDetailsBtnClickEvent(QObject* watched)
{
    const auto clickedButton = qobject_cast<QPushButton*>(watched);
    if (!clickedButton)
    {
        return false;
    }

    if (clickedButton->objectName() != "VideoGridDetailsBtn")
    {
        return false;
    }

    const auto videoGridItemWidgetObj = clickedButton->parentWidget()->parentWidget();
    if (!videoGridItemWidgetObj || videoGridItemWidgetObj->objectName() != "VideoGridItemWidget")
    {
        return false;
    }

    const auto videoGridItemWidget = qobject_cast<VideoGridItemWidget*>(videoGridItemWidgetObj);
    if (!videoGridItemWidget)
    {
        return false;
    }

    if (!detailPanelVisible())
    {
        showDetailPanel();
    }
    // if (const auto itemIdentifier = videoGridItemWidget->Identifier; detailSourceIdentifier != itemIdentifier)
    // {
    //     //  可以在此处更新详情栏data
    //     qDebug() << videoGridItemWidget->Identifier;
    //     detailSourceIdentifier = itemIdentifier;
    //     updateDetailPanel(videoGridItemWidget->Identifier);
    //     if (!detailPanelVisible())
    //     {
    //         showDetailPanel();
    //     }
    // }
    // else
    // {
    //     // hideDetailPanel();
    // }
    return true;
}

void VideoWidget::updateDetailPanel(const std::string& detail) const
{
    detailPanel()->updateUi(detail);
}
VideoDetailWidget* VideoWidget::detailPanel() const
{
    return ui->VideoDetailWidget;
}
bool VideoWidget::detailPanelVisible() const
{
    return detailPanel()->isVisible();
}
void VideoWidget::showDetailPanel() const
{
    detailPanel()->show();
    const auto rect = detailPanel()->geometry();
    detailAnimation->setValues(rect.adjusted(detailPanel()->width(), 0, 0, 0), rect);
    detailAnimation->start();
}
void VideoWidget::hideDetailPanel() const
{
    const auto rect = detailPanel()->geometry();
    detailAnimation->setValues(rect, rect.adjusted(detailPanel()->width(), 0, 0, 0));
    connect(detailAnimation.get(), &GeometryAnimation::finish, detailPanel(), &QWidget::hide);
    detailAnimation->start();

}
bool VideoWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() != QEvent::MouseButtonPress)
    {
        return false;
    }

    return processDetailsBtnClickEvent(watched);
}
