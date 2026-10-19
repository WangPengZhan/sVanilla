// Plugin 模块单元测试。
//
// 覆盖四部分：
//   1. DynamicLibLoader —— 动态库加载/符号解析/卸载，含真实系统库的成功路径
//   2. PluginProxy      —— 对 IPlugin 调用的异常隔离与默认返回值
//   3. PluginConfig     —— 排序/哈希只看 id；JSON 序列化字段集合
//   4. PluginManager    —— 静态路径常量与配置读写（在临时目录中切换 CWD 避免污染仓库）

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include "Plugin/PluginManager.h"

namespace
{
// 用例独占的临时目录，析构时整体删除，避免在系统 temp 里留下垃圾。
class ScopedTempDir
{
public:
    explicit ScopedTempDir(std::string tag)
    {
        // runId 保证不同次运行不会复用同一目录：上一轮可能残留没删干净的文件。
        static const std::string runId = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        static std::atomic<unsigned> sequence{0};
        m_path = std::filesystem::temp_directory_path() / ("svanilla_" + tag + "_" + runId + "_" + std::to_string(sequence++));
        std::filesystem::create_directories(m_path);
    }

    ScopedTempDir(const ScopedTempDir&) = delete;
    ScopedTempDir& operator=(const ScopedTempDir&) = delete;

    ~ScopedTempDir()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    const std::filesystem::path& path() const
    {
        return m_path;
    }

    std::string file(const std::string& name) const
    {
        return (m_path / name).string();
    }

private:
    std::filesystem::path m_path;
};

std::string readAll(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream)
    {
        return {};
    }
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}
}  // namespace

namespace
{
// 可配置行为的 IPlugin 替身。
class MockPlugin : public plugin::IPlugin
{
public:
    MockPlugin()
    {
        m_message.pluginId = 99;
        m_message.name = "mock-plugin";
        m_message.version = "3.1.4";
        m_message.description = "unit test plugin";
        m_message.domain = "example.com";
        m_icon = {0x01, 0x02, 0x03};
    }

    const PluginMessage& pluginMessage() const override
    {
        ++metadataCalls;
        if (metadataThrows)
        {
            throw std::runtime_error("metadata failed");
        }
        return m_message;
    }

    const std::vector<uint8_t>& websiteIcon() override
    {
        if (callbacksThrow)
        {
            throw std::runtime_error("icon failed");
        }
        return m_icon;
    }

    bool canParseUrl(const std::string&) override
    {
        if (callbacksThrow)
        {
            throw std::runtime_error("parse failed");
        }
        return parseResult;
    }

    adapter::VideoView getVideoView(const std::string&) override
    {
        if (callbacksThrow)
        {
            throw std::runtime_error("view failed");
        }
        return {};
    }

    std::shared_ptr<download::FileDownloader> getDownloader(const VideoInfoFull&) override
    {
        if (callbacksThrow)
        {
            throw std::runtime_error("downloader failed");
        }
        return nullptr;
    }

    LoginProxy loginer() override
    {
        // 成功路径需要真实的 AbstractLoginApi 实现，这里只覆盖异常隔离路径。
        throw std::runtime_error("login failed");
    }

    mutable int metadataCalls{0};
    bool metadataThrows{false};
    bool callbacksThrow{false};
    bool parseResult{true};

private:
    // 注意：PluginMessage 定义在全局命名空间（见 BaseVideoView.h），不在 plugin 内。
    PluginMessage m_message;
    std::vector<uint8_t> m_icon;
};

#ifdef _WIN32
const std::string kSystemLibrary = "kernel32.dll";
const std::string kKnownSymbol = "GetCurrentProcessId";
#elif defined(__APPLE__)
const std::string kSystemLibrary = "libSystem.B.dylib";
const std::string kKnownSymbol = "malloc";
#else
const std::string kSystemLibrary = "libc.so.6";
const std::string kKnownSymbol = "malloc";
#endif
}  // namespace

// ---------- DynamicLibLoader ----------

TEST(DynamicLibLoaderTest, DefaultConstructedIsNotLoaded)
{
    plugin::DynamicLibLoader loader;
    EXPECT_FALSE(loader.isLoaded());
    EXPECT_EQ(loader.handle(), nullptr);
    EXPECT_TRUE(loader.libraryPath().empty());
}

TEST(DynamicLibLoaderTest, LoadLibraryFailsForEmptyPath)
{
    plugin::DynamicLibLoader loader;
    EXPECT_FALSE(loader.loadLibrary());
    EXPECT_FALSE(loader.isLoaded());
}

TEST(DynamicLibLoaderTest, LoadLibraryFailsForMissingFile)
{
    ScopedTempDir directory("plugin_lib");
    plugin::DynamicLibLoader loader(directory.file("no_such_library.dll"));
    EXPECT_FALSE(loader.loadLibrary());
    EXPECT_FALSE(loader.isLoaded());
}

TEST(DynamicLibLoaderTest, LoadPluginSymbolReturnsNullWhenNotLoaded)
{
    plugin::DynamicLibLoader loader("plugin-not-loaded.dll");
    // 未成功加载时不应尝试取符号
    EXPECT_EQ(loader.loadPluginSymbol(), nullptr);
}

TEST(DynamicLibLoaderTest, LibraryPathIsSettable)
{
    plugin::DynamicLibLoader loader("first.dll");
    EXPECT_EQ(loader.libraryPath(), "first.dll");

    loader.setLibraryPath("second.dll");
    EXPECT_EQ(loader.libraryPath(), "second.dll");
}

TEST(DynamicLibLoaderTest, StaticLoadSymbolHandlesNullHandle)
{
    EXPECT_EQ(plugin::DynamicLibLoader::loadSymbol(nullptr, "anything"), nullptr);
}

TEST(DynamicLibLoaderTest, StaticUnloadHandlesNullHandle)
{
    EXPECT_NO_THROW(plugin::DynamicLibLoader::unloadLibrary(nullptr));
}

TEST(DynamicLibLoaderTest, LoadsRealSystemLibraryAndResolvesSymbol)
{
    plugin::DynamicLibLoader loader(kSystemLibrary);

    ASSERT_TRUE(loader.loadLibrary());
    EXPECT_TRUE(loader.isLoaded());
    EXPECT_NE(loader.handle(), nullptr);
    EXPECT_NE(plugin::DynamicLibLoader::loadSymbol(loader.handle(), kKnownSymbol), nullptr);
    // 不存在的符号应返回空指针（错误信息被 FormatMessage 取到后未打印）
    EXPECT_EQ(plugin::DynamicLibLoader::loadSymbol(loader.handle(), "sv_no_such_symbol"), nullptr);

    loader.unloadLibrary();
    EXPECT_FALSE(loader.isLoaded());
    EXPECT_EQ(loader.handle(), nullptr);
}

TEST(DynamicLibLoaderTest, UnloadTwiceIsSafe)
{
    plugin::DynamicLibLoader loader(kSystemLibrary);
    ASSERT_TRUE(loader.loadLibrary());

    loader.unloadLibrary();
    EXPECT_NO_THROW(loader.unloadLibrary());
    EXPECT_FALSE(loader.isLoaded());
}

TEST(DynamicLibLoaderTest, LoadLibraryIsIdempotent)
{
    plugin::DynamicLibLoader loader(kSystemLibrary);
    ASSERT_TRUE(loader.loadLibrary());

    const void* firstHandle = loader.handle();
    EXPECT_TRUE(loader.loadLibrary());  // 已加载时直接返回 true
    EXPECT_EQ(loader.handle(), firstHandle);
}

// ---------- PluginProxy ----------

TEST(PluginProxyTest, ValidPluginCachesMetadataAtConstruction)
{
    auto realPlugin = std::make_shared<MockPlugin>();
    plugin::PluginProxy proxy(realPlugin);

    ASSERT_TRUE(proxy.isValid());
    EXPECT_EQ(proxy.pluginMessage().pluginId, 99);
    EXPECT_EQ(proxy.pluginMessage().name, "mock-plugin");
    EXPECT_EQ(proxy.pluginMessage().version, "3.1.4");
    // 构造时读取一次并缓存；之后读 message 不再回调插件
    EXPECT_EQ(realPlugin->metadataCalls, 1);
    EXPECT_EQ(proxy.pluginMessage().name, "mock-plugin");
    EXPECT_EQ(realPlugin->metadataCalls, 1);
}

TEST(PluginProxyTest, IsInvalidWhenInitialMetadataThrows)
{
    auto realPlugin = std::make_shared<MockPlugin>();
    realPlugin->metadataThrows = true;

    plugin::PluginProxy proxy(realPlugin);

    EXPECT_FALSE(proxy.isValid());
    EXPECT_EQ(proxy.pluginMessage().pluginId, 0);
}

TEST(PluginProxyTest, DelegatesCallbacksToRealPlugin)
{
    auto realPlugin = std::make_shared<MockPlugin>();
    plugin::PluginProxy proxy(realPlugin);
    ASSERT_TRUE(proxy.isValid());

    EXPECT_EQ(proxy.websiteIcon(), (std::vector<uint8_t>{0x01, 0x02, 0x03}));
    EXPECT_TRUE(proxy.canParseUrl("https://example.com/video"));
    EXPECT_TRUE(proxy.getVideoView("https://example.com/video").empty());
    EXPECT_EQ(proxy.getDownloader(VideoInfoFull{}), nullptr);
}

TEST(PluginProxyTest, SwallowsExceptionsAndReturnsDefaults)
{
    auto realPlugin = std::make_shared<MockPlugin>();
    realPlugin->callbacksThrow = true;
    // metadata 不抛，保证 proxy 有效
    plugin::PluginProxy proxy(realPlugin);
    ASSERT_TRUE(proxy.isValid());

    EXPECT_TRUE(proxy.websiteIcon().empty());        // 异常 → 静态空 vector
    EXPECT_FALSE(proxy.canParseUrl("https://any"));  // 异常 → false
    EXPECT_TRUE(proxy.getVideoView("https://any").empty());
    EXPECT_EQ(proxy.getDownloader(VideoInfoFull{}), nullptr);
    // loginer() 的 mock 恒抛，PluginProxy 兜底返回 EmptyLoginApi
    EXPECT_FALSE(proxy.loginer().supportsLogin());
}

// ---------- PluginConfig ----------

TEST(PluginConfigTest, DefaultsAreDisabledAndInvalid)
{
    plugin::PluginConfig config;
    EXPECT_FALSE(config.enabled);
    EXPECT_FALSE(config.isValid);
    // 注意：`int id;` 是本结构体里唯一没有默认成员初始化器的字段
    // （enabled / isValid 都有），默认构造后 id 的值是不确定的，因此这里不断言它。
    // 需要确定性默认值时应通过 JSON 反序列化（见下一个用例）。
}

TEST(PluginConfigTest, JsonDeserialisationFillsMissingFieldsWithDefaults)
{
    // NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT 会先构造一个默认对象再覆盖已有字段，
    // 因此缺字段时（包括上面的 id）都会得到确定性的零值。
    const nlohmann::json json = nlohmann::json::object();
    const auto config = json.get<plugin::PluginConfig>();

    EXPECT_EQ(config.id, 0);
    EXPECT_FALSE(config.enabled);
    EXPECT_FALSE(config.isValid);
    EXPECT_TRUE(config.name.empty());
    EXPECT_TRUE(config.version.empty());
}

TEST(PluginConfigTest, OrderingComparesIdOnly)
{
    plugin::PluginConfig lower;
    lower.id = 1;
    lower.name = "zzz";

    plugin::PluginConfig higher;
    higher.id = 2;
    higher.name = "aaa";

    // operator<=> 只比较 id，与 name 无关
    EXPECT_TRUE(lower < higher);
    EXPECT_FALSE(higher < lower);

    plugin::PluginConfig sameIdDifferentName;
    sameIdDifferentName.id = 1;
    sameIdDifferentName.name = "other";
    EXPECT_FALSE(lower < sameIdDifferentName);
    EXPECT_FALSE(sameIdDifferentName < lower);
}

// 注意：这里没有 std::hash<plugin::PluginConfig> 的用例。
// PluginManager.h 中该特化声明为 class（默认 private 访问级别）且未加 public:，
// 导致 operator()(const PluginConfig&) 是私有的 —— std::hash<PluginConfig>{}(x)
// 根本无法编译，unordered_set<PluginConfig> 也因此不可用。属实现缺陷，不在测试侧绕过。

TEST(PluginConfigTest, JsonRoundTripKeepsSerialisedFieldsButNotIsValid)
{
    plugin::PluginConfig config;
    config.name = "alpha";
    config.version = "1.2.3";
    config.libName = "alpha";
    config.id = 7;
    config.description = "desc";
    config.libFile = "alpha.dll";
    config.enabled = true;
    config.isValid = true;  // 不在序列化字段列表里

    const nlohmann::json json = config;
    const auto restored = json.get<plugin::PluginConfig>();

    EXPECT_EQ(restored.name, config.name);
    EXPECT_EQ(restored.version, config.version);
    EXPECT_EQ(restored.libName, config.libName);
    EXPECT_EQ(restored.id, config.id);
    EXPECT_EQ(restored.description, config.description);
    EXPECT_EQ(restored.libFile, config.libFile);
    EXPECT_TRUE(restored.enabled);
    EXPECT_FALSE(restored.isValid);  // 反序列化后回到默认值
}

// ---------- PluginManager（会读写 CWD，统一切到临时目录） ----------

TEST(PluginManagerTest, StaticPathConstants)
{
    EXPECT_EQ(plugin::PluginManager::pluginDir(), "plugin/");
#ifdef _WIN32
    EXPECT_EQ(plugin::PluginManager::m_dynamicExtension, ".dll");
#endif
}

TEST(PluginManagerTest, ConfigDirIsSettableAndRestorable)
{
    ScopedTempDir directory("plugin_dir");
    plugin::PluginManager::setPluginConfigDir(directory.path().string());
    EXPECT_EQ(plugin::PluginManager::configDir(), directory.path().string());

    plugin::PluginManager::setPluginConfigDir("");
    EXPECT_TRUE(plugin::PluginManager::configDir().empty());
}

TEST(PluginManagerTest, SetAndGetConfigReturnsSortedById)
{
    ScopedTempDir directory("plugin_cfg");
    const auto originalCwd = std::filesystem::current_path();
    std::filesystem::current_path(directory.path());
    plugin::PluginManager::setPluginConfigDir(directory.path().string());

    {
        plugin::PluginManager manager;

        plugin::PluginConfig beta;
        beta.id = 2;
        beta.name = "beta";

        plugin::PluginConfig alpha;
        alpha.id = 1;
        alpha.name = "alpha";

        manager.setPluginConfig({beta, alpha});
        const auto stored = manager.getPluginConfig();

        ASSERT_EQ(stored.size(), 2u);
        auto iterator = stored.begin();
        EXPECT_EQ(iterator->id, 1);  // std::set 按 id 排序
        EXPECT_EQ(iterator->name, "alpha");
        ++iterator;
        EXPECT_EQ(iterator->id, 2);
        EXPECT_EQ(iterator->name, "beta");
    }

    std::filesystem::current_path(originalCwd);
    plugin::PluginManager::setPluginConfigDir("");
}

TEST(PluginManagerTest, DestructorPersistsConfigToDisk)
{
    ScopedTempDir directory("plugin_cfg");
    const auto originalCwd = std::filesystem::current_path();
    std::filesystem::current_path(directory.path());
    plugin::PluginManager::setPluginConfigDir(directory.path().string());
    std::filesystem::create_directories(directory.path() / "config");  // 让 saveConfig() 能真正落盘

    {
        plugin::PluginManager manager;

        plugin::PluginConfig config;
        config.id = 5;
        config.name = "persisted-plugin";
        config.version = "0.9";
        config.enabled = true;
        manager.setPluginConfig({config});
    }  // 析构 → saveConfig()

    const auto configFile = directory.path() / "config" / "plugin_config.json";
    ASSERT_TRUE(std::filesystem::exists(configFile));

    const std::string content = readAll(configFile);
    EXPECT_NE(content.find("persisted-plugin"), std::string::npos);
    EXPECT_NE(content.find("0.9"), std::string::npos);

    std::filesystem::current_path(originalCwd);
    plugin::PluginManager::setPluginConfigDir("");
}

TEST(PluginManagerTest, QueriesReturnEmptyOnFreshManager)
{
    ScopedTempDir directory("plugin_cfg");
    const auto originalCwd = std::filesystem::current_path();
    std::filesystem::current_path(directory.path());
    plugin::PluginManager::setPluginConfigDir(directory.path().string());

    {
        plugin::PluginManager manager;
        EXPECT_TRUE(manager.getPluginConfig().empty());
        EXPECT_EQ(manager.getPlugin(1234), nullptr);
        EXPECT_TRUE(manager.pluginsSnapshot().empty());
        EXPECT_NO_THROW(manager.removePlugin(1234));  // 删除不存在的插件是安全的
    }

    std::filesystem::current_path(originalCwd);
    plugin::PluginManager::setPluginConfigDir("");
}

namespace
{
// PluginManager / PluginProxy 内部写日志，宏为 SPDLOG_LOGGER_CALL(spdlog::get("Plugin"), ...)：
// spdlog::get() 对未注册的名字返回空 shared_ptr 并被宏直接解引用（段错误），
// 因此在测试环境启动时把 "Plugin" 注册到一个丢弃输出的 null sink 上。
class NullLoggerEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        if (!spdlog::get("Plugin"))
        {
            spdlog::register_logger(std::make_shared<spdlog::logger>("Plugin", std::make_shared<spdlog::sinks::null_sink_mt>()));
        }
    }
};
}  // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new NullLoggerEnvironment);
    return RUN_ALL_TESTS();
}
