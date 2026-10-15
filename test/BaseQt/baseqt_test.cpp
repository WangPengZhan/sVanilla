// BaseQt 模块单元测试。
//
// 覆盖 TreeItem（纯树结构逻辑）、TreeViewIterator（在 QStandardItemModel 上遍历）、
// util 工具函数（文件读取、二进制转图像、历史菜单）以及 BaseQt 的跨线程投递设施
// （SenderForQt / TransceiverForQt）。
//
// 本目标自带 main()：util::binToImage / util::createMenu 需要 QGuiApplication，
// 因此强制使用 offscreen 平台插件，避免在无显示环境下失败。

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QImage>
#include <QMenu>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>

#include <gtest/gtest.h>

#include "BaseQt/SenderForQt.h"
#include "BaseQt/TreeItem.h"
#include "BaseQt/TreeViewIterator.h"
#include "BaseQt/Utility.h"

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

// 统计析构次数，用来验证 clearChild(true) 真的删除了子树。
class CountingTreeItem : public TreeItem
{
public:
    using TreeItem::TreeItem;

    ~CountingTreeItem() override
    {
        ++destroyCount;
    }

    static inline int destroyCount = 0;
};

const char* kMinimalSvg = R"(<svg xmlns="http://www.w3.org/2000/svg" width="4" height="4"><rect width="4" height="4" fill="red"/></svg>)";

std::vector<uint8_t> toBytes(const std::string& text)
{
    return std::vector<uint8_t>(text.begin(), text.end());
}
}  // namespace

// ---------- TreeItem ----------

TEST(TreeItemTest, ParentConstructorRegistersItself)
{
    TreeItem parent;
    TreeItem child(&parent);

    ASSERT_EQ(parent.children().size(), 1);
    EXPECT_EQ(parent.children().at(0), &child);
    EXPECT_EQ(child.parent(), &parent);
}

TEST(TreeItemTest, SetParentReparentsBetweenNodes)
{
    TreeItem first;
    TreeItem second;
    TreeItem child(&first);

    child.setParent(&second);

    EXPECT_TRUE(first.children().isEmpty());  // 从旧父节点摘除
    ASSERT_EQ(second.children().size(), 1);
    EXPECT_EQ(child.parent(), &second);

    child.setParent(nullptr);
    EXPECT_TRUE(second.children().isEmpty());
    EXPECT_EQ(child.parent(), nullptr);
}

TEST(TreeItemTest, AddChildIgnoresNull)
{
    TreeItem parent;
    parent.addChild(nullptr);
    EXPECT_TRUE(parent.children().isEmpty());

    TreeItem child;
    parent.addChild(&child);
    EXPECT_EQ(parent.children().size(), 1);
}

TEST(TreeItemTest, AddChildrenAcceptsMultipleNodes)
{
    TreeItem parent;
    TreeItem first;
    TreeItem second;
    parent.addChildren({&first, &second});

    EXPECT_EQ(parent.children().size(), 2);
    EXPECT_EQ(first.parent(), &parent);
    EXPECT_EQ(second.parent(), &parent);
}

TEST(TreeItemTest, ClearChildWithoutDeleteKeepsNodesAlive)
{
    TreeItem parent;
    auto* child = new TreeItem(&parent);

    parent.clearChild(false);

    EXPECT_TRUE(parent.children().isEmpty());
    // 未删除：对象仍然可用，只是从父节点列表里摘掉了
    EXPECT_EQ(child->parent(), &parent);
    delete child;
}

TEST(TreeItemTest, ClearChildWithDeleteDestroysWholeSubtree)
{
    CountingTreeItem::destroyCount = 0;
    {
        auto* root = new CountingTreeItem();
        auto* middle = new CountingTreeItem(root);
        new CountingTreeItem(middle);
        new CountingTreeItem(middle);

        root->clearChild(true);
        EXPECT_TRUE(root->children().isEmpty());
        // middle 及其两个子节点共 3 个
        EXPECT_EQ(CountingTreeItem::destroyCount, 3);

        delete root;
    }
    EXPECT_EQ(CountingTreeItem::destroyCount, 4);
}

TEST(TreeItemTest, CloneDeepCopiesSubtreeWithoutParent)
{
    CountingTreeItem::destroyCount = 0;
    {
        auto* root = new CountingTreeItem();
        auto* child = new CountingTreeItem(root);
        new CountingTreeItem(child);

        auto* clone = root->clone();

        ASSERT_NE(clone, root);
        EXPECT_EQ(clone->parent(), nullptr);  // 克隆体不挂到原父节点上
        ASSERT_EQ(clone->children().size(), 1);
        EXPECT_NE(clone->children().at(0), child);  // 子节点也是拷贝
        EXPECT_EQ(clone->children().at(0)->parent(), clone);
        ASSERT_EQ(clone->children().at(0)->children().size(), 1);
        EXPECT_EQ(root->children().size(), 1);  // 原树不受影响

        delete clone;
        delete root;
    }
}

TEST(TreeItemTest, BaseTextIsEmpty)
{
    TreeItem item;
    EXPECT_TRUE(item.text(0).isEmpty());
}

// ---------- TreeViewIterator ----------

TEST(TreeViewIteratorTest, WalksDepthFirstPreOrder)
{
    QStandardItemModel model;
    model.appendRow(new QStandardItem("a"));

    auto* branch = new QStandardItem("b");
    branch->appendRow(new QStandardItem("b1"));
    branch->appendRow(new QStandardItem("b2"));
    model.appendRow(branch);

    model.appendRow(new QStandardItem("c"));

    std::vector<std::string> visited;
    for (TreeViewIterator iterator(&model); iterator.hasNext(); ++iterator)
    {
        visited.push_back(iterator.currentIndex().data().toString().toStdString());
    }

    const std::vector<std::string> expected{"a", "b", "b1", "b2", "c"};
    EXPECT_EQ(visited, expected);
}

TEST(TreeViewIteratorTest, EmptyModelHasNoNext)
{
    QStandardItemModel model;
    TreeViewIterator iterator(&model);
    EXPECT_FALSE(iterator.hasNext());
    EXPECT_FALSE(iterator.currentIndex().isValid());
}

TEST(TreeViewIteratorTest, NullModelIsSafe)
{
    TreeViewIterator iterator(static_cast<QAbstractItemModel*>(nullptr));
    EXPECT_FALSE(iterator.hasNext());
}

TEST(TreeViewIteratorTest, PostIncrementReturnsPreviousPosition)
{
    QStandardItemModel model;
    model.appendRow(new QStandardItem("a"));
    model.appendRow(new QStandardItem("b"));

    TreeViewIterator iterator(&model);
    const TreeViewIterator previous = iterator++;

    EXPECT_EQ(previous.currentIndex().data().toString(), QString("a"));
    EXPECT_EQ(iterator.currentIndex().data().toString(), QString("b"));
}

// ---------- util ----------

TEST(UtilityTest, ReadFileToVectorReadsBinaryContent)
{
    ScopedTempDir directory("baseqt_util");
    const std::string path = directory.file("payload.bin");
    const std::vector<uint8_t> content{0x00, 0x01, 0xFE, 0xFF, 0x7F};
    {
        std::ofstream stream(path, std::ios::binary);
        stream.write(reinterpret_cast<const char*>(content.data()), static_cast<std::streamsize>(content.size()));
    }

    EXPECT_EQ(util::readFileToVector(path), content);
}

TEST(UtilityTest, ReadFileToVectorThrowsForMissingFile)
{
    ScopedTempDir directory("baseqt_util");
    EXPECT_THROW(util::readFileToVector(directory.file("absent.bin")), std::runtime_error);
}

TEST(UtilityTest, BinToImageRendersSvgBranch)
{
    const QPixmap pixmap = util::binToImage(toBytes(kMinimalSvg), QSize(8, 8));
    EXPECT_FALSE(pixmap.isNull());
    EXPECT_EQ(pixmap.size(), QSize(8, 8));
}

TEST(UtilityTest, BinToImageRendersRasterBranch)
{
    QImage image(4, 4, QImage::Format_ARGB32);
    image.fill(Qt::blue);

    QByteArray encoded;
    QBuffer buffer(&encoded);
    buffer.open(QIODevice::WriteOnly);
    ASSERT_TRUE(image.save(&buffer, "PNG"));
    buffer.close();

    std::vector<uint8_t> bytes(static_cast<size_t>(encoded.size()));
    std::memcpy(bytes.data(), encoded.constData(), static_cast<size_t>(encoded.size()));

    const QPixmap pixmap = util::binToImage(bytes, QSize(8, 8));
    EXPECT_FALSE(pixmap.isNull());
}

TEST(UtilityTest, BinToImageReturnsNullForNonImage)
{
    EXPECT_TRUE(util::binToImage(toBytes("this is definitely not an image"), QSize(8, 8)).isNull());
}

TEST(UtilityTest, CreateMenuAddsOneActionPerHistoryEntry)
{
    QMenu menu;
    const std::vector<std::string> history{"https://example.com/a", "https://example.com/b"};

    util::createMenu(&menu, 10000, history, [](const QString&) {});

    EXPECT_EQ(menu.actions().size(), 2);
    EXPECT_EQ(menu.actions().at(0)->text(), QString::fromStdString(history.at(0)));
    // 未截断时 tooltip 与文本一致
    EXPECT_EQ(menu.actions().at(0)->toolTip(), QString::fromStdString(history.at(0)));
}

TEST(UtilityTest, CreateMenuInvokesCallbackWithFullText)
{
    QMenu menu;
    const std::vector<std::string> history{"https://example.com/first", "https://example.com/second"};
    QString captured;

    util::createMenu(&menu, 10000, history, [&captured](const QString& text) {
        captured = text;
    });
    menu.actions().at(1)->trigger();

    EXPECT_EQ(captured, QString::fromStdString(history.at(1)));
}

TEST(UtilityTest, CreateMenuElidesLongTextButKeepsFullToolTip)
{
    QMenu menu;
    const std::string longUri = "https://example.com/" + std::string(200, 'x');

    util::createMenu(&menu, 20, {longUri}, [](const QString&) {});

    ASSERT_EQ(menu.actions().size(), 1);
    EXPECT_NE(menu.actions().at(0)->text(), QString::fromStdString(longUri));     // 已被省略号截断
    EXPECT_EQ(menu.actions().at(0)->toolTip(), QString::fromStdString(longUri));  // tooltip 保留全文
}

TEST(UtilityTest, CreateMenuHandlesEmptyHistory)
{
    QMenu menu;
    util::createMenu(&menu, 100, {}, [](const QString&) {});
    EXPECT_TRUE(menu.actions().isEmpty());
}

// ---------- 跨线程投递 ----------

TEST(SenderForQtTest, SendTaskRunsOnReceiverInSameThread)
{
    ActivateQbject receiver;
    SenderForQt sender(&receiver);
    ASSERT_TRUE(sender.enable());

    int calls = 0;
    sender.sendTask([&calls] {
        ++calls;
    });

    // 收发双方同线程 → AutoConnection 退化为直接调用
    EXPECT_EQ(calls, 1);
}

TEST(TransceiverForQtTest, SendTaskRunsOnWorkerThread)
{
    TransceiverForQt transceiver;
    std::promise<std::thread::id> promise;
    auto future = promise.get_future();

    transceiver.sendTask([&promise] {
        promise.set_value(std::this_thread::get_id());
    });

    ASSERT_EQ(future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    EXPECT_NE(future.get(), std::this_thread::get_id());
}

int main(int argc, char** argv)
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
