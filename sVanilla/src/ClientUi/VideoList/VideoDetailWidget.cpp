
#include "VideoDetailWidget.h"

#include <utility>
#include "ui_VideoDetailWidget.h"

VideoDetailWidget::VideoDetailWidget(std::string bvid, QWidget* parent)
    : QDialog(parent)
    , m_bvid(std::move(bvid))
    , ui(new Ui::VideoDetailWidget())
{
    ui->setupUi(this);
    setUi();

    signalsAndSlots();
}

VideoDetailWidget::~VideoDetailWidget()
{
    delete ui;
}

void VideoDetailWidget::setUi()
{
}

void VideoDetailWidget::signalsAndSlots()
{
}
