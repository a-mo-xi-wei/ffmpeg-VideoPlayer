#include "SPlay.h"
#include "ui_SPlay.h"
#include"SFFmpeg.h"
#include"SLogger.h"
#include"VideoWidget.h"
#include"SAudioPlay.h"
#include"SEventFilterObject.h"
#include<QPainter>
#include<QPainterPath>
#include<QFile>
#include<QFileDialog>

SPlay::SPlay(QWidget *parent)
	: QWidget(parent)
	,ui(new Ui::SPlay)
	,m_playListModel(new QStandardItemModel(this))
{
	ui->setupUi(this);
	initUi();
	startTimer(100);
}

SPlay::~SPlay()
{
	delete ui;
}

void SPlay::initUi()
{
	//去掉标题栏
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	move(200, 200);
	setMouseTracking(true);
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
	ui->volumeSlider->setRange(1, 100);
	ui->volumeSlider->setValue(50);
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
	SFFmpeg::instance().setVolume(value);

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

void SPlay::on_minBtn_clicked()
{
	this->showMinimized();
}

void SPlay::on_maxBtn_clicked()
{
	if (this->isMaximized()) {
		this->showNormal();
		ui->maxBtn->setIcon(QIcon(":Res/max.png"));
	}
	else {
		this->showMaximized();
		ui->maxBtn->setIcon(QIcon(":Res/resume.png"));
	}
}

void SPlay::on_closeBtn_clicked()
{
	this->close();
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
	if (this->m_playListModel->rowCount() == 0)
		return;
	//没有聚焦就播放第一行 
	if (!ui->playListView->currentIndex().isValid()) {
		this->m_currentIndex = 0;
		auto index = this->m_playListModel->index(this->m_currentIndex, 0);
		ui->playListView->setCurrentIndex(index);
		emit ui->playListView->doubleClicked(index);
		SFFmpeg::instance().setPlay(true);
		ui->playBtn->setIcon(QIcon(":/Res/play.png"));
		return;
	}
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
	if(!SAudioPlay::instance()->start())return;

	if(!SFFmpeg::instance().open(fileName.toStdString())){
		LOG_ERROR("文件打开失败");
		return;
	}

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
	ui->playBtn->setIcon(QIcon(":/Res/play.png"));
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

	//if(!SFFmpeg::instance().isPlay())
	//	ui->playBtn->setIcon(QIcon(":/Res/pause.png"));
}

void SPlay::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	QBrush brush(QColor("#CCFFEE"));
	painter.setBrush(brush);
	painter.setPen(Qt::NoPen);
	QRect rect = this->rect();
	QPainterPath path;
	path.addRoundedRect(rect, 20, 20);
	painter.drawPath(path);
}

void SPlay::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {

		// 如果是鼠标左键
		// 获取当前窗口位置,以窗口左上角
		windowsLastPs = pos();
		// 获取鼠标在屏幕的位置  就是全局的坐标 以屏幕左上角为坐标系
		mousePs = ev->globalPos();

		isPress = true;
		// 获取鼠标在那个区域
		mouse_press_region = GetMouseRegion(ev->pos().x(), ev->pos().y());

	}
}

void SPlay::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton) {
		isPress = false;
	}
	setCursor(QCursor{});
}

void SPlay::mouseMoveEvent(QMouseEvent* ev)
{
	// 设置鼠标的形状
	SetMouseCursor(ev->pos().x(), ev->pos().y());
	// 计算的鼠标移动偏移量, 就是鼠标全局坐标 - 减去点击时鼠标坐标
	QPoint point_offset = ev->globalPos() - mousePs;
	if ((ev->buttons() == Qt::LeftButton) && isPress)
	{
		if (mouse_press_region == kMousePositionMid)
		{
			// 如果鼠标是在窗口的中间位置,就是移动窗口
			move(windowsLastPs + point_offset);
		}
		else {
			// 其他部分 是拉伸窗口
			// 获取客户区
			QRect rect = geometry();
			switch (mouse_press_region)
			{
				// 左上角
			case kMousePositionLeftTop:
				rect.setTopLeft(rect.topLeft() + point_offset);
				break;
			case kMousePositionTop:
				rect.setTop(rect.top() + point_offset.y());
				break;
			case kMousePositionRightTop:
				rect.setTopRight(rect.topRight() + point_offset);
				break;
			case kMousePositionRight:
				rect.setRight(rect.right() + point_offset.x());
				break;
			case kMousePositionRightBottom:
				rect.setBottomRight(rect.bottomRight() + point_offset);
				break;
			case kMousePositionBottom:
				rect.setBottom(rect.bottom() + point_offset.y());
				break;
			case kMousePositionLeftBottom:
				rect.setBottomLeft(rect.bottomLeft() + point_offset);
				break;
			case kMousePositionLeft:
				rect.setLeft(rect.left() + point_offset.x());
				break;
			default:
				break;
			}
			setGeometry(rect);
			mousePs = ev->globalPos();
		}
	}
}

void SPlay::SetMouseCursor(int x, int y)
{
	// 鼠标形状对象
	Qt::CursorShape cursor{};
	int region = GetMouseRegion(x, y);
	switch (region)
	{
	case kMousePositionLeftTop:
	case kMousePositionRightBottom:
		cursor = Qt::SizeFDiagCursor; break;
	case kMousePositionRightTop:
	case kMousePositionLeftBottom:
		cursor = Qt::SizeBDiagCursor; break;
	case kMousePositionLeft:
	case kMousePositionRight:
		cursor = Qt::SizeHorCursor; break;
	case kMousePositionTop:
	case kMousePositionBottom:
		cursor = Qt::SizeVerCursor; break;
	case kMousePositionMid:
		cursor = Qt::ArrowCursor; break;
	default:
		break;
	}
	setCursor(cursor);
}

int SPlay::GetMouseRegion(int x, int y)
{
	int region_x = 0, region_y = 0;
	// 鼠标的X坐标小于 边界5 说明他在最上层区域 第一区域
	if (x < kMouseRegionLeft)
	{
		region_x = 1;
	}
	else if (x > (this->width()/*窗体宽度*/ - kMouseRegionRight/*边界宽度5*/)) {
		// 如果鼠标X坐标 大于 最右侧的边界 说明他在第三区域
		region_x = 3;
	}
	else {
		region_x = 2;
	}
	if (y < kMouseRegionTop)
	{
		// 同理 鼠标Y坐标 小于上层边界5  说明鼠标在第一区域
		region_y = 1;
	}
	else if (y > (this->height() - kMouseRegionBottom)) {
		// 鼠标Y坐标的 大于 最下面的坐标,鼠标就在 第三区
		region_y = 3;
	}
	else {
		region_y = 2;
	}
	// 最后计算 表示区域的 数值 (x=1, y=1) 计算 = 1*10+1 =11 
	// x=2,y=3 = 3*10+2 = 32 在图的 3,2 区域
	return region_y * 10 + region_x;
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

