// SUI 模块单元测试。
//
// SUI 绝大多数是可复用 Qt 控件，属于「需要人工目视验收」的范畴，不适合断言。
// 这里挑选两个**不含业务依赖、且有可观察状态**的控件做单元测试：
//   - DownloadCount：计数读写与 sizeHint（纯数据）
//   - SlidingWaitBar：start/stop/reStart 生命周期与绘制不崩溃
//
// 为了避开 SUI 静态库带来的 WebEngine 依赖，这里直接编译这两个控件的源文件。

#include <QApplication>
#include <QPixmap>
#include <QSize>

#include <gtest/gtest.h>

#include "SUI/DownloadCount.h"
#include "SUI/SlidingWaitBar.h"

// ---------- DownloadCount ----------

TEST(DownloadCountTest, DefaultsToZero)
{
    DownloadCount widget;
    EXPECT_EQ(widget.downloadCount(), 0);
    EXPECT_EQ(widget.downloadedCount(), 0);
}

TEST(DownloadCountTest, SetDownloadingCountIsReadable)
{
    DownloadCount widget;
    widget.setDownloadingCount(7);
    EXPECT_EQ(widget.downloadCount(), 7);
    EXPECT_EQ(widget.downloadedCount(), 0);  // 两个计数互不影响
}

TEST(DownloadCountTest, SetDownloadedCountIsReadable)
{
    DownloadCount widget;
    widget.setDownloadedCount(13);
    EXPECT_EQ(widget.downloadedCount(), 13);
    EXPECT_EQ(widget.downloadCount(), 0);
}

TEST(DownloadCountTest, SizeHintIsTwiceTextWidthPlusPadding)
{
    DownloadCount widget;
    // sizeHint 在 DownloadCount 里被声明为 protected，只能通过 QWidget 基类接口访问。
    // textWidth = 30, padding = 3, textHeight = 18
    EXPECT_EQ(static_cast<QWidget&>(widget).sizeHint(), QSize(2 * 30 + 2 * 3, 18));
}

TEST(DownloadCountTest, PaintEventRendersWithoutCrash)
{
    DownloadCount widget;
    widget.resize(static_cast<QWidget&>(widget).sizeHint());
    widget.setDownloadingCount(3);
    widget.setDownloadedCount(9);

    const QPixmap rendered = widget.grab();  // 触发 paintEvent
    EXPECT_FALSE(rendered.isNull());
    EXPECT_EQ(rendered.size(), widget.size());
}

// ---------- SlidingWaitBar ----------

TEST(SlidingWaitBarTest, ConstructsAlreadyRunning)
{
    // 构造函数末尾会调用 start()，即默认就处于动画状态。
    EXPECT_NO_THROW({ SlidingWaitBar bar; });
}

TEST(SlidingWaitBarTest, StartStopReStartAreSafe)
{
    SlidingWaitBar bar;
    bar.resize(100, 10);

    EXPECT_NO_THROW(bar.stop());
    EXPECT_NO_THROW(bar.start());
    EXPECT_NO_THROW(bar.reStart());
}

TEST(SlidingWaitBarTest, RepeatedStopAndRestartAreSafe)
{
    SlidingWaitBar bar;
    bar.resize(100, 10);

    for (int index = 0; index < 3; ++index)
    {
        EXPECT_NO_THROW(bar.stop());
        EXPECT_NO_THROW(bar.reStart());
    }
}

TEST(SlidingWaitBarTest, PaintEventRendersWhileRunningAndStopped)
{
    SlidingWaitBar bar;
    bar.resize(120, 12);

    const QPixmap running = bar.grab();
    EXPECT_FALSE(running.isNull());

    bar.stop();
    const QPixmap stopped = bar.grab();
    EXPECT_FALSE(stopped.isNull());
    EXPECT_EQ(stopped.size(), bar.size());
}

TEST(SlidingWaitBarTest, ResizeIsHandled)
{
    SlidingWaitBar bar;
    EXPECT_NO_THROW(bar.resize(300, 20));
    EXPECT_NO_THROW(bar.resize(1, 1));  // 极小尺寸也不能崩（resizeEvent 会更新 m_maxOffset）
    EXPECT_EQ(bar.size(), QSize(1, 1));
}

int main(int argc, char** argv)
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
