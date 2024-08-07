#include "SEventFilterObject.h"
#include<QWidget>
#include<QGraphicsDropShadowEffect>
#include<QMouseEvent>

SEventFilterObject::SEventFilterObject(QObject *parent)
	: QObject(parent)
{
    //如果指定了窗口为父对象，则设置窗口阴影
    auto* w = dynamic_cast<QWidget*>(parent);
    if (w)
    {
        //创建阴影效果
        auto* effect = new QGraphicsDropShadowEffect(this);
        //设置阴影的偏移量
        effect->setOffset(0);
        //设置阴影的模糊半径
        effect->setBlurRadius(10);
        //effect->setColor(QColor(60, 60, 60));
        //设置阴影的颜色为灰色
        effect->setColor(qRgb(60, 60, 60));
        //应用阴影效果
        w->setGraphicsEffect(effect);
    }
}

SEventFilterObject::~SEventFilterObject()
{}

bool SEventFilterObject::eventFilter(QObject * watch, QEvent * ev)
{
    QMouseEvent* mev = dynamic_cast<QMouseEvent*>(ev);
    if (ev->type() == QEvent::MouseButtonPress)
    {
        m_pressPos = mev->pos();
    }
    else if (ev->type() == QEvent::MouseMove && mev->buttons() & Qt::MouseButton::LeftButton)
    {
        QWidget* w = dynamic_cast<QWidget*>(watch);
        if (w)
            w->move(mev->globalPos() - m_pressPos);
        return true;
    }
    return false;
}

