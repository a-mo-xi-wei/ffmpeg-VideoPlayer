#ifndef SPLAY_H
#define SPLAY_H

#include <QWidget>
#include<QStandardItemModel>
#include<QListView>
#include<QMouseEvent>
#include<QStyledItemDelegate>
#define OFFSET 20
class CustomDelegate : public QStyledItemDelegate {
public:
    CustomDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem modifiedOption = option;
        if (index.row() == 0) {
            modifiedOption.rect.adjust(0, OFFSET, 0, 0); // 向下偏移20像素
        }
        QStyledItemDelegate::paint(painter, modifiedOption, index);
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        if (index.row() == 0) {
            size.setHeight(size.height() + OFFSET); // 增加高度以适应偏移
        }
        return size;
    }
};

namespace {
    // 重点来讲下这个定义,好多教程没有做说明
    // 这块定义的四个边的边距大小都是5 ,这个可以自定义设置大小
    // 参考上面图片
    const int kMouseRegionLeft = 5;

    const int kMouseRegionTop = 5;

    const int kMouseRegionRight = 5;

    const int kMouseRegionBottom = 5;

}

// 鼠标的 活动范围的 枚举
enum MousePosition

{
   /* 这里我们将一个窗口划分为9个区域，分别为

    左上角（1, 1）、中上（1，2）、右上角（1, 3）

    左中  （2, 1）、 中间（2, 2）、右中  （2, 3）

    左下角（3, 1）、中下（3，2）、 右下角（3, 3）*/
    kMousePositionLeftTop = 11,

    kMousePositionTop = 12,

    kMousePositionRightTop = 13,

    kMousePositionLeft = 21,

    kMousePositionMid = 22,

    kMousePositionRight = 23,

    kMousePositionLeftBottom = 31,

    kMousePositionBottom = 32,

    kMousePositionRightBottom = 33,

};

namespace Ui { class SPlay; }

class SPlay : public QWidget
{
	Q_OBJECT
public:
	SPlay(QWidget *parent = nullptr);
	~SPlay();

	void initUi();
    /*测试效率*/
    //void testEfficientcy();
    //void on_volumeSlider_valueChanged_original(int value);
    //void on_volumeSlider_valueChanged_optimized(int value);

public slots:

    //controlWidget slots
	void on_prevBtn_clicked();
	void on_nextBtn_clicked();
	void on_playBtn_clicked();
	void on_playListBtn_clicked();
    void on_volumeSlider_valueChanged(int value);

    //playListWidget slots
	void on_addPlayListBtn_clicked();
	void on_clearPlayListBtn_clicked();

    //titleBar
    void on_topBtn_clicked();
    void on_minBtn_clicked();
    void on_maxBtn_clicked();
    void on_closeBtn_clicked();

	void on_playListView_doubleClicked(const QModelIndex& index);

protected:
    bool eventFilter(QObject* obj, QEvent* ev)override;
    void timerEvent(QTimerEvent* ev)override;
    void paintEvent(QPaintEvent* ev)override;
protected:
    void mousePressEvent(QMouseEvent* ev)override;
    void mouseReleaseEvent(QMouseEvent* ev)override;
    void mouseMoveEvent(QMouseEvent* ev)override;
private:
    //根据鼠标的设置鼠标样式，用于拉伸
    void SetMouseCursor(int x, int y);
    //判断鼠标的区域，用于拉伸
    int GetMouseRegion(int x, int y);
private:
    bool isPress;
    QPoint windowsLastPs;
    QPoint mousePs;
    int mouse_press_region = kMousePositionMid;
private:
	Ui::SPlay* ui{};
	QStandardItemModel* m_playListModel;
    CustomDelegate* m_delegate;
    int m_currentIndex{ -1 };       //当前正在播放的视频的索引
};
#endif SPLAY_H