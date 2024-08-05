#ifndef SPLAY_H
#define SPLAY_H

#include <QWidget>
#include<QStandardItemModel>
#include<QListView>
#include<QMouseEvent>
#include<QStyledItemDelegate>

class CustomDelegate : public QStyledItemDelegate {
public:
    CustomDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QStyleOptionViewItem modifiedOption = option;
        if (index.row() == 0) {
            modifiedOption.rect.adjust(0, 20, 0, 0); // 向下偏移20像素
        }
        QStyledItemDelegate::paint(painter, modifiedOption, index);
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        if (index.row() == 0) {
            size.setHeight(size.height() + 20); // 增加高度以适应偏移
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

public slots:
	void on_playListBtn_clicked();
	void on_addPlayListBtn_clicked();

private:
	Ui::SPlay* ui{};
	QStandardItemModel* m_playListModel;
    CustomDelegate* delegate;
};
#endif SPLAY_H