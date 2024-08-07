#include "VideoWidget.h"
#include"SFFmpeg.h"
#include"SLogger.h"
#include<QPainter>
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

void VideoWidget::threadStart()
{
	this->m_videoThread.start();
}


void VideoWidget::paintEvent(QPaintEvent * ev)
{
	static int w = 0, h = 0;
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
	painter.drawImage(QPoint(x,y),*this->m_image);
}

void VideoWidget::timerEvent(QTimerEvent* ev)
{

	update();
}
