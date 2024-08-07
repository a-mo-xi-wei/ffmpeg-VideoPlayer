#pragma once

#include <QObject>
#include<QPoint>
class SEventFilterObject  : public QObject
{
	Q_OBJECT

public:
	SEventFilterObject(QObject *parent = nullptr);
	~SEventFilterObject();
protected:
	bool eventFilter(QObject* watched, QEvent* ev)override;
private:
	QPoint m_pressPos;       //左键点击的坐标
};
