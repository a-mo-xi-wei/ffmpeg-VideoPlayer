#include"VideoWidget.h"
#include"SAudioPlay.h"
#include"SFFmpeg.h"
#include"SLogger.h"
#include<QPainter>
std::once_flag flag;
VideoWidget::VideoWidget(QWidget *parent)
	: QWidget(parent)
{
	
	//开启定时器
	startTimer(1000 / 26);
}

VideoWidget::~VideoWidget()
{
	this->m_videoThread.stop();
}

void VideoWidget::PlayBeginVideo()
{
	SAudioPlay::instance()->setAudioFormat(SFFmpeg::instance().audioFormat());
	if (!SAudioPlay::instance()->start())return;
	std::string path = __FILE__;
	path += "/../Res/beginVideo.mp4";
	if (!SFFmpeg::instance().open(path)) {
		LOG_ERROR("文件打开失败");
	}
	SFFmpeg::instance().setPlay(true);

	this->m_videoThread.start();
}

void VideoWidget::paintEvent(QPaintEvent * ev)
{
	static int w = 0, h = 0;
	//qDebug() << "width : " << this->width() << " height ：" << this->height();
	std::call_once(flag, &VideoWidget::PlayBeginVideo,this);
	//分配每一帧缓冲区
	if (w != width() || h != height()) {
		w = width();
		h = height();
		SFFmpeg::instance().aspectSize(&w, &h);
		this->m_image.reset(new QImage(w, h, QImage::Format::Format_ARGB32));
	}
	if (!SFFmpeg::instance().toRGB((char*)m_image->bits(), w, h)) return;
	QPainter painter(this);
	auto x = (width() - w) / 2;
	auto y = (height() - h) / 2;
	painter.drawImage(QPoint(x, y), *this->m_image);
}

void VideoWidget::timerEvent(QTimerEvent* ev)
{
	update();
}
