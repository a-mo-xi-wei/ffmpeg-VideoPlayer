#include "SPlay.h"
#include "ui_SPlay.h"
#include<QFile>
SPlay::SPlay(QWidget *parent)
	: QWidget(parent)
	,ui(new Ui::SPlay)
{
	ui->setupUi(this);
	initUi();
}

SPlay::~SPlay()
{
	delete ui;
}

void SPlay::initUi()
{
	QFile file(":/Res/style.qss");
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "样式表打开失败QAQ";
		return;
	}

	QString styleStr = file.readAll();
	this->setStyleSheet(styleStr);


	this->setWindowIcon(QIcon(":/Res/SPlay.png"));
	ui->playBtn->setIcon(QIcon(":/Res/pause.png"));

	//ui->playSpeedCmb->setView(new QListView());
	ui->playSpeedCmb->addItem(QIcon(":/Res/1.png"), "1 倍速", 1);
	ui->playSpeedCmb->addItem(QIcon(":/Res/1.5.png"), "1.5 倍速", 1.5);
	ui->playSpeedCmb->addItem(QIcon(":/Res/2.png"), "2 倍速", 2);
	ui->playSpeedCmb->setCurrentIndex(0);//即第一项
}
