#include <QClipboard>
#include <QTimer>
#include <QDebug>
#include "HomePage.h"
#include "ui_HomePage.h"
#include "Aria2Net/AriaClient/AriaClient.h"
#include "Aria2Net/Protocol/Protocol.h"
#include "BiliApi/BilibiliClient.h"
#include "NetWork/CNetWork.h"
#include "ThreadPool/ThreadPool.h"
#include "Util/UrlProcess.h"
#include <QStandardPaths>

HomePage::HomePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    signalsAndSlots();

    auto* timer = new QTimer(this);
    timer->setInterval(5000);
    connect(timer, &QTimer::timeout, this, [this] {
        ui->ErrorMsgLabel->clear();
    });
    timer->start();

    ui->HistoryBtn->setIcon(QIcon(":icon/home/history.svg"));
    ui->ClipBoardBtn->setIcon(QIcon(":icon/home/clipboard.svg"));
    ui->HomePageSubmit->setIcon(QIcon(":icon/home/enter.svg"));
    ui->HomePageSubmit->setLabel(QStringLiteral("Submit"));
    ui->HomePageSubmit->setIconFirst(false);
    ui->IconBtn->setIcon(QIcon(":icon/home/icon.svg"));
    ui->IconBtn->setIconSize({40,40});
    ui->LearnBtn->setIcon(QIcon(":icon/home/openurl.svg"));
    ui->HomeLineEdit->setFixedHeight(30);
}

HomePage::~HomePage()
{
    delete ui;
}
void HomePage::setMsgLabel(const std::string& msg)
{
    ui->ErrorMsgLabel->setText(QString::fromStdString(msg));
}
void HomePage::updateMsg(const std::string& msg)
{
    setMsgLabel(msg);
}

void HomePage::signalsAndSlots()
{
    // ClipBoardBtn clicked signal -> Event::AddUri (ui -> core)
    connect(ui->ClipBoardBtn, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        if (const QString originalText = clipboard->text(); !util::UrlProcess::IsUrl(originalText))
        {
            setMsgLabel("Not a valid url");
        }
        else
        {
            parseUri({originalText.toStdString()});
        }
    });

    // HomeLineEdit start signal -> Event::AddUri (ui -> core)
    connect(ui->HomeLineEdit, &SearchLineEdit::Complete, this, [this] {
        qDebug() << ui->HomeLineEdit->text();
        parseUri({ui->HomeLineEdit->text().toStdString()});
        ui->HomeLineEdit->clear();
    });

}

void HomePage::parseUri(const std::string& uri)
{
    // if bili
    const std::list<std::string> h = {"Referer: https://www.bilibili.com"};
    aria2net::AriaSendOption option;
    option.header = h;
    auto m_biliClient = BiliApi::BilibiliClient::globalClient();
    const auto res = m_biliClient.GetVideoView(uri);
    const auto playUrl = m_biliClient.GetPlayUrl(res.data.cid, 64, res.data.bvid);
    std::list<std::string> video_urls;
    std::list<std::string> audio_urls;
    if (playUrl.code != 0)
    {
        PRINTS("play url error", playUrl.message)
        PRINTS("play url error", playUrl.message)
        return;
    }
    const auto videos = playUrl.data.durl;
    PRINTS("accept_format: ", playUrl.data.accept_format)
    for (const auto& video : videos)
    {
        video_urls.push_back(video.url);
        PRINTS("video url", video.url)
    }
    if (!video_urls.empty())
    {
        option.out = res.data.title + ".mp4";
        addUri(video_urls);
    }
}
void HomePage::addUri(const std::list<std::string>& uris)
{
    const QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    // option : come from gobal
    aria2net::AriaSendOption option;
    // option.dir = SingleConfig::instance().getAriaConfig().downloadDir;
    option.dir = downloadPath.toStdString();


}
