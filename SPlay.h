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


	void on_playListView_doubleClicked(const QModelIndex& index);

protected:
    bool eventFilter(QObject* obj, QEvent* ev)override;
    void timerEvent(QTimerEvent* ev)override;
private:
	Ui::SPlay* ui{};
	QStandardItemModel* m_playListModel;
    CustomDelegate* m_delegate;
    int m_currentIndex{ -1 };       //当前正在播放的视频的索引
};
#endif SPLAY_H