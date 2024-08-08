#pragma once

#include <QWidget>
#include"VideoThread.h"

class VideoWidget  : public QWidget
{
	Q_OBJECT

public:
	VideoWidget(QWidget *parent = nullptr);
	~VideoWidget();
	void PlayBeginVideo();
protected:
	void paintEvent(QPaintEvent* ev)override;
	void timerEvent(QTimerEvent* ev)override;
private:
	std::unique_ptr<QImage> m_image;
	VideoThread m_videoThread;
};
