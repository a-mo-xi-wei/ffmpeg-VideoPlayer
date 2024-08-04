#ifndef SPLAY_H
#define SPLAY_H

#include <QWidget>

namespace Ui { class SPlay; }

class SPlay : public QWidget
{
	Q_OBJECT

public:
	SPlay(QWidget *parent = nullptr);
	~SPlay();

	void initUi();

private:
	Ui::SPlay* ui{};
};
#endif SPLAY_H