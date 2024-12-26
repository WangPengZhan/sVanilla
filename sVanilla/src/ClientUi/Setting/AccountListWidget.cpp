#include <QPixmap>
#include <QDir>
#include <QPainterPath>
#include <QPainter>

#include <Login.h>
#include <BaseVideoView.h>

#include "AccountListWidget.h"
#include "ui_AccountListWidget.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "ClientUi/Utils/RunTask.h"

constexpr char userfaceDir[] = "userface";

QPixmap createRoundedPixmap(const QPixmap& src, int radius)
{
    QSize size = src.size();
    QPixmap roundedPixmap(size);
    roundedPixmap.fill(Qt::transparent);

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPainterPath path;
    path.addRoundedRect(0, 0, size.width(), size.height(), radius, radius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, src);
    return roundedPixmap;
}

AccountItemWidget::AccountItemWidget(std::shared_ptr<LoginProxy> loginer, QListWidgetItem* item, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AccountItemWidget)
    , m_item(nullptr)
    , m_loginer(loginer)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    setListWidgetItem(item);
    requestUserInfo();
}

AccountItemWidget::~AccountItemWidget()
{
    delete ui;
}

void AccountItemWidget::setUserInfo(UserInfo userInfo)
{
    QPixmap pixmap = QPixmap(QString::fromStdString(userInfo.facePath)).scaled(ui->labelFace->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap = createRoundedPixmap(pixmap, pixmap.width() / 2);
    ui->labelFace->setPixmap(pixmap);
    ui->labelFace->setStyleSheet(".QLabel{ border-radius:24px; background: transparent; }");
    ui->labelName->setText(QString::fromStdString(userInfo.uname));
}

void AccountItemWidget::setListWidgetItem(QListWidgetItem* item)
{
    m_item = item;
}

void AccountItemWidget::setUi()
{
}

void AccountItemWidget::signalsAndSlots()
{
    connect(this, &AccountItemWidget::signalUserInfo, this, &AccountItemWidget::setUserInfo);
    connect(this, &AccountItemWidget::signalLogout, this, [this](bool isSucceed) {
        if (isSucceed && m_item)
        {
            delete m_item;
        }
    });
    connect(ui->btnLogout, &QPushButton::clicked, this, [this]() {
        auto taskFunc = [this]() {
            return m_loginer->logout();
        };
        auto callback = [this](bool isSucceed) {
            emit signalLogout(isSucceed);
        };
        runTask(taskFunc, callback, this);
    });
    connect(ui->btnHistory, &QPushButton::clicked, this, [this]() {
        auto taskFunc = [this]() {
            return m_loginer->history();
        };
        auto callback = [this](std::vector<adapter::BaseVideoView> historys) {
            emit signalHistoryInfo(historys);
        };
        runTask(taskFunc, callback, this);
    });
}

void AccountItemWidget::requestUserInfo()
{
    auto taskFunc = [this]() {
        QString dir = SApplication::appDir() + "/" + QString(userfaceDir);
        QDir dir1(dir);
        if (!dir1.exists())
        {
            dir1.mkpath(dir);
        }

        return m_loginer->getUserInfo(dir.toLocal8Bit().toStdString());
    };
    auto callback = [this](UserInfo userInfo) {
        setUserInfo(userInfo);
    };
    runTask(taskFunc, callback, this);
}

AccountListWidget::AccountListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

void AccountListWidget::addLoginer(std::shared_ptr<LoginProxy> loginer)
{
    auto* pItem = new QListWidgetItem(this);
    auto* pWidget = new AccountItemWidget(loginer, pItem, this);
    pItem->setSizeHint(QSize(0, pWidget->sizeHint().height()));
    setItemWidget(pItem, pWidget);
    connect(pWidget, &AccountItemWidget::signalHistoryInfo, this, &AccountListWidget::signalHistoryInfo);
}
