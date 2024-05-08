
#include "DownloadedInfoWidget.h"
#include "ui_DownloadedInfoWidget.h"

DownloadedInfoWidget::DownloadedInfoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadedInfoWidget)
{
    ui->setupUi(this);
    signalsAndSlots();
    hide();
}

DownloadedInfoWidget::~DownloadedInfoWidget()
{
    delete ui;
}

void DownloadedInfoWidget::signalsAndSlots()
{
    connect(ui->closeButton, &QPushButton::clicked, this, &DownloadedInfoWidget::hide);
}
