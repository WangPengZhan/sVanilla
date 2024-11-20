#include <QPixmap>
#include <QDir>
#include <QPainterPath>
#include <QPainter>

#include "AccountListWidget.h"
#include "ui_AccountListWidget.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "Login/login.h"
#include "ClientUi/Utils/RunTask.h"
#include "ClientUi/Adapter/BaseVideoView.h"

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

AccountItemWidget::AccountItemWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AccountItemWidget)
    , m_item(nullptr)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

AccountItemWidget::AccountItemWidget(std::shared_ptr<AbstractLogin> loginer, QListWidgetItem* item, QWidget* parent)
    : AccountItemWidget(parent)
{
    setLoginer(loginer);
    setListWidgetItem(item);
}

AccountItemWidget::AccountItemWidget(UserInfo userInfo, QListWidgetItem* item, QWidget* parent)
    : AccountItemWidget(parent)
{
    setUserInfo(userInfo);
    setListWidgetItem(item);
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

void AccountItemWidget::setLoginer(std::shared_ptr<AbstractLogin> loginer)
{
    if (loginer)
    {
        auto taskFunc = [loginer]() {
            QString dir = SApplication::appDir() + "/" + QString(userfaceDir);
            QDir dir1(dir);
            if (!dir1.exists())
            {
                dir1.mkpath(dir);
            }

            return loginer->getUserInfo(dir.toLocal8Bit().toStdString());
        };
        auto callback = [this](UserInfo userInfo) {
            emit signalUserInfo(userInfo);
        };
        runTask(taskFunc, callback, this);
    }

    m_loginer = loginer;
}

std::shared_ptr<AbstractLogin> AccountItemWidget::loginer() const
{
    return m_loginer;
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
        if (m_loginer)
        {
            auto taskFunc = [this]() {
                return m_loginer->logout();
            };
            auto callback = [this](bool isSucceed) {
                emit signalLogout(isSucceed);
            };
            runTask(taskFunc, callback, this);
        }
    });
    connect(ui->btnHistory, &QPushButton::clicked, this, [this]() {
        if (m_loginer)
        {
            auto taskFunc = [this]() {
                return m_loginer->history();
            };
            auto callback = [this](std::vector<Adapter::BaseVideoView> historys) {
                Adapter::Views views;
                for (const auto& history : historys)
                {
                    views.emplace_back(std::make_shared<Adapter::BaseVideoView>(history));
                }

                emit signalHistoryInfo(views);
            };
            runTask(taskFunc, callback, this);
        }
    });
}

AccountListWidget::AccountListWidget(QWidget* parent)
    : QListWidget(parent)
{
}

void AccountListWidget::addLoginer(std::shared_ptr<AbstractLogin> loginer)
{
    auto* pItem = new QListWidgetItem(this);
    auto* pWidget = new AccountItemWidget(loginer, pItem, this);
    pItem->setSizeHint(QSize(0, pWidget->sizeHint().height()));
    setItemWidget(pItem, pWidget);
    connect(pWidget, &AccountItemWidget::signalHistoryInfo, this, &AccountListWidget::signalHistoryInfo);
}

void AccountListWidget::addUserInfo(const UserInfo& license)
{
    auto* pItem = new QListWidgetItem(this);
    auto* pWidget = new AccountItemWidget(license, pItem, this);
    pItem->setSizeHint(QSize(0, pWidget->sizeHint().height()));
    setItemWidget(pItem, pWidget);
    connect(pWidget, &AccountItemWidget::signalHistoryInfo, this, &AccountListWidget::signalHistoryInfo);
}

void AccountListWidget::addUserInfos(const std::vector<UserInfo>& licenses)
{
    for (const auto& license : licenses)
    {
        addUserInfo(license);
    }
}
