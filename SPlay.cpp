#include "SPlay.h"
#include "ui_SPlay.h"
#include"SFFmpeg.h"
#include"SLogger.h"
#include"VideoWidget.h"
#include"SAudioPlay.h"
#include"SEventFilterObject.h"

#include<QFile>
#include<QFileDialog>
std::once_flag flag;

SPlay::SPlay(QWidget *parent)
	: QWidget(parent)
	,ui(new Ui::SPlay)
	,m_playListModel(new QStandardItemModel(this))
{
	ui->setupUi(this);
	initUi();
	startTimer(1000);
}

SPlay::~SPlay()
{
	delete ui;
}

void SPlay::initUi()
{
	//去掉标题栏
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	//让无边框窗口能够被拖动
	//installEventFilter(new SEventFilterObject(this));

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
	ui->volumeLab->setProperty("imagePath", QString(":/Res/volume.png"));
	ui->volumeSlider->setMinimum(1);   // 设置最小值
	ui->volumeSlider->setMaximum(100); // 设置最大值
	ui->volumeLab->installEventFilter(this);

	//playListWidget
	ui->playModeBtn->setIcon(QIcon(":/Res/order-play.png"));
	ui->addPlayListBtn->setIcon(QIcon(":/Res/add.png"));
	ui->clearPlayListBtn->setIcon(QIcon(":/Res/clear.png"));
	ui->playListView->setModel(this->m_playListModel);
	this->m_delegate = new CustomDelegate(ui->playListView);
	ui->playListView->setItemDelegate(this->m_delegate);
	ui->playListView->setEditTriggers(QListView::NoEditTriggers);

	//auto item = new QStandardItem();
	//item->setEnabled(false);
	//item->setSelectable(false);
	//item->setFlags(Qt::NoItemFlags);
	//this->m_playListModel->appendRow(item);
	
}

//#include <QElapsedTimer>
//#include <QDebug>
//
//void SPlay::testEfficientcy()
//{
//	const int iterations = 10000;
//
//	QElapsedTimer timer;
//
//	qint64 optimizedTime = timer.elapsed();
//	timer.start();
//	for (int i = 1; i < iterations; ++i) {
//		on_volumeSlider_valueChanged_original(i % 101);
//	}
//
//	qint64 originalTime = timer.elapsed();
//	timer.start();
//	for (int i = 1; i < iterations; ++i) {
//		on_volumeSlider_valueChanged_optimized(i % 101);
//	}
//
//	qDebug() << "Original code time:" << originalTime << "ms";
//	qDebug() << "Optimized code time:" << optimizedTime << "ms";
//}
//void SPlay::on_volumeSlider_valueChanged_original(int value)
//{
//	ui->volumeSlider->setValue(value);
//	//qInfo() << value;
//	if (value > 50) {
//		ui->volumeLab->setPixmap(QPixmap(":/Res/bigVolume.png").scaled(ui->volumeLab->size()));
//		ui->volumeLab->setProperty("imagePath", QString(":/Res/bigVolume.png"));
//	}
//	else {
//		ui->volumeLab->setPixmap(QPixmap(":/Res/volume.png").scaled(ui->volumeLab->size()));
//		ui->volumeLab->setProperty("imagePath", QString(":/Res/volume.png"));
//	}
//}
//void SPlay::on_volumeSlider_valueChanged_optimized(int value)
//{
//	ui->volumeSlider->setValue(value);
//
//	QString currentImagePath = ui->volumeLab->property("imagePath").toString();
//	QString newImagePath;
//
//	if (value > 50) {
//		newImagePath = ":/Res/bigVolume.png";
//	}
//	else {
//		newImagePath = ":/Res/volume.png";
//	}
//
//	// 仅在路径不同的情况下才更新图片和属性
//	if (currentImagePath != newImagePath) {
//		ui->volumeLab->setPixmap(QPixmap(newImagePath).scaled(ui->volumeLab->size()));
//		ui->volumeLab->setProperty("imagePath", newImagePath);
//	}
//}

void SPlay::on_playListBtn_clicked(){
	//qDebug() << __FUNCTION__;
	ui->playListWidget->isHidden() ? ui->playListWidget->show() : 
		ui->playListWidget->hide();
}

void SPlay::on_volumeSlider_valueChanged(int value)
{
	//ui->volumeSlider->setValue(value);
	////qInfo() << value;
	//if (value > 50) {
	//	ui->volumeLab->setPixmap(QPixmap(":/Res/bigVolume.png").scaled(ui->volumeLab->size()));
	//	ui->volumeLab->setProperty("imagePath", QString(":/Res/bigVolume.png"));
	//}
	//else {
	//	ui->volumeLab->setPixmap(QPixmap(":/Res/volume.png").scaled(ui->volumeLab->size()));
	//	ui->volumeLab->setProperty("imagePath", QString(":/Res/volume.png"));
	//}

	ui->volumeSlider->setValue(value);

	QString currentImagePath = ui->volumeLab->property("imagePath").toString();
	QString newImagePath;

	if (value > ui->volumeSlider->maximum() / 2) {
		newImagePath = ":/Res/bigVolume.png";
	}
	else {
		newImagePath = ":/Res/volume.png";
	}

	// 仅在路径不同的情况下才更新图片和属性
	if (currentImagePath != newImagePath) {
		ui->volumeLab->setPixmap(QPixmap(newImagePath).scaled(ui->volumeLab->size()));
		ui->volumeLab->setProperty("imagePath", newImagePath);
	}
	
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

void SPlay::on_clearPlayListBtn_clicked()
{
	this->m_playListModel->clear();
}

void SPlay::on_topBtn_clicked()
{
	Qt::WindowFlags flags = windowFlags();
	if (windowFlags() & Qt::WindowStaysOnTopHint) {
		flags &= ~Qt::WindowStaysOnTopHint;
		setWindowFlags(flags);
		ui->topBtn->setIcon(QIcon(":Res/top_off.png"));
	}
	else {
		flags |= Qt::WindowStaysOnTopHint;
		setWindowFlags(flags);
		ui->topBtn->setIcon(QIcon(":Res/top_on.png"));
	}
	show();
}

void SPlay::on_prevBtn_clicked()
{
	if (this->m_currentIndex == -1)return;
	this->m_currentIndex = (this->m_currentIndex + this->m_playListModel->rowCount() - 1) % this->m_playListModel->rowCount();
	auto index = this->m_playListModel->index(this->m_currentIndex, 0);
	ui->playListView->setCurrentIndex(index);
	emit ui->playListView->doubleClicked(index);
}

void SPlay::on_nextBtn_clicked()
{
	if (this->m_currentIndex == -1)return;
	this->m_currentIndex = (this->m_currentIndex + 1) % this->m_playListModel->rowCount();
	auto index = this->m_playListModel->index(this->m_currentIndex, 0);
	ui->playListView->setCurrentIndex(index); 
	emit ui->playListView->doubleClicked(index);
}

void SPlay::on_playBtn_clicked()
{
	if (SFFmpeg::instance().isPlay()) {
		SFFmpeg::instance().setPlay(false);
		ui->playBtn->setIcon(QIcon(":/Res/pause.png"));
	}
	else {
		SFFmpeg::instance().setPlay(true);
		ui->playBtn->setIcon(QIcon(":/Res/play.png"));
	}
}

void SPlay::on_playListView_doubleClicked(const QModelIndex& index)
{
	auto fileName = index.data(Qt::UserRole).toString();
	if (fileName.isEmpty()) {
		return;
	}
	this->m_currentIndex = index.row();
	SAudioPlay::instance()->setAudioFormat(SFFmpeg::instance().audioFormat());
	SAudioPlay::instance()->start();
	if(!SFFmpeg::instance().open(fileName.toStdString())){
		LOG_ERROR("文件打开失败");
		return;
	}

	std::call_once(flag,&VideoWidget::threadStart,ui->videoWidget);

	//显示总时长
	auto totalTime = SFFmpeg::instance().duration() / 1000;
	ui->totalTimeLab->setText(QString("%1:%2:%3")
	.arg(totalTime / 60 / 60,2,10,QChar('0'))
	.arg(totalTime/ 60 % 60, 2, 10, QChar('0'))
	.arg(totalTime % 60, 2, 10, QChar('0'))
	);
	qInfo() << fileName;
	ui->playListView->setCurrentIndex(index);
	SFFmpeg::instance().setPlay(true);

}

void SPlay::timerEvent(QTimerEvent* ev)
{
	//显示播放时长
	auto pastTime = SFFmpeg::instance().position() / 1000;
	ui->pastTimeLab->setText(QString("%1:%2:%3")
		.arg(pastTime / 60 / 60, 2, 10, QChar('0'))
		.arg(pastTime / 60 % 60, 2, 10, QChar('0'))
		.arg(pastTime % 60, 2, 10, QChar('0'))
	);

	//更新播放进度条
	auto totalTime = SFFmpeg::instance().duration();
	auto rate = SFFmpeg::instance().position() / static_cast<double>(totalTime);
	ui->playProcessSlider->setValue(rate * 10000);
}

bool SPlay::eventFilter(QObject* obj, QEvent* ev)
{
	if (obj == ui->volumeLab && ev->type() == QEvent::MouseButtonPress) {
		QString currentPath = ui->volumeLab->property("imagePath").toString();
		if (currentPath == ":/Res/volume.png" || currentPath == ":/Res/bigVolume.png") {
			ui->volumeLab->setPixmap(QPixmap(":/Res/noVolume.png").scaled(ui->volumeLab->size()));
			ui->volumeLab->setProperty("imagePath", QString(":/Res/noVolume.png"));
			ui->volumeSlider->hide();
		}
		else if (currentPath == ":/Res/noVolume.png") {
			if (ui->volumeSlider->value() > 50) {
				ui->volumeLab->setPixmap(QPixmap(":/Res/bigVolume.png").scaled(ui->volumeLab->size()));
				ui->volumeLab->setProperty("imagePath", QString(":/Res/bigVolume.png"));
			}
			else {
				ui->volumeLab->setPixmap(QPixmap(":/Res/volume.png").scaled(ui->volumeLab->size()));
				ui->volumeLab->setProperty("imagePath", QString(":/Res/volume.png"));
			}
			ui->volumeSlider->show();
		}
		return true; // 事件已经处理
	}
	return QWidget::eventFilter(obj, ev);
}

