#include <fstream>

#include <QApplication>

#include <zlib.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include <openssl/opensslv.h>
#include <nlohmann/json.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/base.h>

#include "LicenseInfoWidget.h"
#include "ui_LicenseInfoWidget.h"

LicenseInfoWidget::LicenseInfoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LicenseInfo)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();

    loadFile();
}

LicenseInfoWidget::~LicenseInfoWidget()
{
    delete ui;
}

void LicenseInfoWidget::setUi()
{
}

void LicenseInfoWidget::signalsAndSlots()
{
}

void LicenseInfoWidget::loadFile()
{
    std::string jsonPath = QApplication::applicationDirPath().toLocal8Bit().toStdString() + "/licenses.json";
    std::ifstream f(jsonPath);
    auto json = nlohmann::json::parse(f);

    std::vector<LicenseInfo> licenses = json;
    loadVersion(licenses);
    ui->listWidget->addLicenses(licenses);
}

void LicenseInfoWidget::loadVersion(std::vector<LicenseInfo>& licenses)
{
    for (auto& license : licenses)
    {
        if (license.name == "Qt")
        {
            license.version = QT_VERSION_STR;
        }
        else if (license.name == "zlib")
        {
            license.version = ZLIB_VERSION;
        }
        else if (license.name == "curl")
        {
            license.version = curl_version();
        }
        else if (license.name == "sqlite")
        {
            license.version = SQLITE_VERSION;
        }
        else if (license.name == "openssl")
        {
            license.version = OPENSSL_VERSION_TEXT;
        }
        else if (license.name == "nlohmann-json")
        {
            license.version = nlohmann::json::meta()["version"]["string"];
        }
        else if (license.name == "fmt")
        {
            license.version = std::to_string(FMT_VERSION / 10000) + "." + std::to_string((FMT_VERSION / 100) % 100) + "." + std::to_string(FMT_VERSION % 100);
        }
        else if (license.name == "spdlog")
        {
            license.version = std::to_string(SPDLOG_VER_MAJOR) + "." + std::to_string(SPDLOG_VER_MINOR) + "." + std::to_string(SPDLOG_VER_PATCH);
        }
        if (license.version.empty())
        {
            license.version = "Unknown";
        }
    }
}
