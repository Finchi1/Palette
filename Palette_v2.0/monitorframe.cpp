#include "monitorframe.h"
#include <QApplication>
#include <QRect>
#include <QScreen>
#include <QPainter>
#include <QKeyEvent>
#include <QScreen>

MonitorFrame::MonitorFrame(QWidget *parent)
	: QWidget(parent)
{
	mParent = parent;
	this->setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setAttribute(Qt::WA_Hover);
	this->setMouseTracking(true);
	QScreen* screen = QApplication::primaryScreen();
	QRect  screenGeometry = screen->geometry();
	this->resize(screenGeometry.width(), screenGeometry.height());
	setCursor(Qt::PointingHandCursor);
}

MonitorFrame::~MonitorFrame()
{}

void MonitorFrame::paintEvent(QPaintEvent * event)
{
	QPainter* painter = new QPainter(this);
	painter->setPen(QPen(QColor(255, 255, 255, 255), 2.0));
	painter->setBrush(Qt::NoBrush);
	painter->drawRect(this->rect());
	painter->fillRect(this->rect(), QColor(0, 0, 0, 1));
}

void MonitorFrame::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape && this-isVisible())
	{
		this->hide();
		mParent->show();
	}
}

void MonitorFrame::mouseMoveEvent(QMouseEvent* event)
{
}

void MonitorFrame::mousePressEvent(QMouseEvent* event)
{
	if (!event->button() == Qt::LeftButton)
		return;
	QScreen* screen = this->screen();
	QPixmap pixmap = screen->grabWindow(0, 0, 0, this->width(), this->height());
	mClickedColor = pixmap.toImage().pixelColor(event->pos());
	emit colorChanged();
}
