#pragma once

#include <QWidget>

class DownloadCount : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadCount(QWidget* parent = nullptr);

    [[nodiscard]] int downloadCount() const;
    void setDownloadingCount(int count);
    [[nodiscard]] int downloadedCount() const;
    void setDownloadedCount(int count);

protected:
    void paintEvent(QPaintEvent* event) override;
    [[nodiscard]] QSize sizeHint() const override;

private:
private:
    int m_downloadingCount = 0;
    int m_downloadedCount = 0;
};
