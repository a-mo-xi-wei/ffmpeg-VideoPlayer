#include "VideoWidget.h"
#include"SFFmpeg.h"
#include"SLogger.h"
#include<QPainter>
VideoWidget::VideoWidget(QWidget *parent)
	: QWidget(parent)
{
	if (!SFFmpeg::instance().open("E:/Res/VideoRes/video.mp4")) {
		LOG_DEBUG("open video failed %s!",SFFmpeg::instance().errorString().data());
	}
	this->m_videoThread.start();
	//开启定时器
	startTimer(1000 / 26);
}

VideoWidget::~VideoWidget()
{
	this->m_videoThread.stop();
}


void VideoWidget::paintEvent(QPaintEvent * ev)
{
	static int w = 0, h = 0;
	//分配每一帧缓冲区
	if (w == 0 || h == 0) {
		w = width();
		h = height();
		this->m_image.reset(new QImage(w, h, QImage::Format::Format_ARGB32));
	}
	if (!SFFmpeg::instance().toRGB((char*)m_image->bits(), w, h)) return;
	QPainter painter(this);
	painter.drawImage(QPoint(0,0),*this->m_image);
}

void VideoWidget::timerEvent(QTimerEvent* ev)
{
	update();
}
