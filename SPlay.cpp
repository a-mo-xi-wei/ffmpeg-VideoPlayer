#include "SPlay.h"
#include "ui_SPlay.h"
#include<QFile>
#include<QFileDialog>
SPlay::SPlay(QWidget *parent)
	: QWidget(parent)
	,ui(new Ui::SPlay)
	,m_playListModel(new QStandardItemModel(this))
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

	this->setWindowIcon(QIcon(":/Res/player.png"));

	//titleBar
	ui->iconLab->setPixmap(QPixmap(":/Res/player.png").scaled(ui->iconLab->size()));
	ui->closeBtn->setIcon(QIcon(":Res/close.png"));
	ui->maxBtn->setIcon(QIcon(":Res/max.png"));
	ui->minBtn->setIcon(QIcon(":Res/min.png"));
	ui->topBtn->setIcon(QIcon(":Res/top_off.png"));

	//controlWidget
	ui->playBtn->setIcon(QIcon(":/Res/pause.png"));
	ui->prevBtn->setIcon(QIcon(":/Res/prev.png"));
	ui->nextBtn->setIcon(QIcon(":/Res/next.png"));
	ui->playListBtn->setIcon(QIcon(":/Res/playlist.png"));
	ui->volumeLab->setPixmap(QPixmap(":/Res/volume.png").scaled(ui->volumeLab->size()));

	//playListWidget
	ui->playModeBtn->setIcon(QIcon(":/Res/order-play.png"));
	ui->addPlayListBtn->setIcon(QIcon(":/Res/add.png"));
	ui->clearPlayListBtn->setIcon(QIcon(":/Res/clear.png"));
	ui->playListView->setModel(this->m_playListModel);
	delegate = new CustomDelegate(ui->playListView);
	//ui->playListView->setItemDelegate(delegate);
	ui->playListView->setEditTriggers(QListView::NoEditTriggers);
	auto item = new QStandardItem();
	item->setEnabled(false);
	item->setSelectable(false);
	item->setFlags(Qt::NoItemFlags);
	this->m_playListModel->appendRow(item);

}

void SPlay::on_playListBtn_clicked(){
	//qDebug() << __FUNCTION__;
	ui->playListWidget->isHidden() ? ui->playListWidget->show() : 
		ui->playListWidget->hide();
}

void SPlay::on_addPlayListBtn_clicked(){
	auto filter = "mp4 (*.mp4);;flv (*.flv);;wmv (*.wmv);;\
		video (*.mp4 *.flv *.wmv);;all (*.*)";
	auto fileNames = QFileDialog::getOpenFileNames(this, "选择文件",
		QDir::currentPath(), filter);
	if (fileNames.isEmpty())return;
	for (auto& file : fileNames) {
		auto item = new QStandardItem(QFileInfo(file).fileName());
		item->setData(file, Qt::UserRole);
		this->m_playListModel->appendRow(item);
	}
}