#pragma once
#include <QWidget>

#include <nlohmann/json.hpp>

struct LicenseInfo
{
    std::string name;
    std::string version;
    std::string github;
    std::string license;
    std::string licenseType;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(LicenseInfo, name, version, github, license, licenseType)
};

QT_BEGIN_NAMESPACE
namespace Ui
{
class LicenseInfo;
}
QT_END_NAMESPACE

class LicenseInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LicenseInfoWidget(QWidget* parent = nullptr);
    ~LicenseInfoWidget();

private:
    void setUi();
    void signalsAndSlots();
    void loadFile();
    void loadVersion(std::vector<LicenseInfo>& licenses);

private:
    Ui::LicenseInfo* ui;
};
