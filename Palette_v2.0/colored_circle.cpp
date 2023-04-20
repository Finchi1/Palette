#include "colored_circle.h"
#include <QScreen>
#include <QImage>
#include <QColor>

ColoredCircle::ColoredCircle(QWidget* parent)
	: QWidget(parent)
{
	this->resize(100, 100);
	setMouseTracking(true);
}

ColoredCircle::~ColoredCircle()
{}

void ColoredCircle::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Fill background
	QRect backgroundRect(0, 0, this->width(), this->height());
	QColor backgroundColor(255, 255, 255, 0);
	painter.fillRect(event->rect(), backgroundColor);;

	// Draw circle
	const QColor white(255, 255, 255);
	const int margin = 10;
	mDiameter = this->width() < this->height() ? this->width() - 2 * margin : this->height() - 2 * margin;
	mCenter = QPointF(this->width() / 2, this->height() / 2);
	QRectF sector(mCenter.x() - mDiameter / 2.0, mCenter.y() - mDiameter / 2.0, mDiameter, mDiameter);
	for (qreal angle = 0.0; angle < 360; angle += 1.0)
	{
		QColor sectorColor(hsvToRgb(angle, 1.0, 1.0));
		QRadialGradient gradient(mCenter, mDiameter / 2.0);
		gradient.setColorAt(0, white);
		gradient.setColorAt(1, sectorColor);
		QBrush brush(gradient);
		QPen pen(brush, 1.0);
		painter.setPen(pen);
		painter.setBrush(brush);
		painter.drawPie(sector, angle * 16, 16);
	}
	painter.end();
}

void ColoredCircle::mousePressEvent(QMouseEvent* event)
{
	if (!event->button() == Qt::LeftButton)
		return;

	// Determine that was clicked inside the circle
	// (x-a)^2+(y-b)^2<=R^2
	if (std::pow(event->x() - mCenter.x(), 2) + std::pow(event->y() - mCenter.y(), 2) <= std::pow(mDiameter/2.0, 2))
	{
		// Get current pixel color
		mClickedColor = QWidget::grab(QRect(event->x(), event->y(), 1, 1)).toImage().pixelColor(0, 0);
		emit onChangeColor();
	}
}

void ColoredCircle::mouseMoveEvent(QMouseEvent* event)
{
	if (std::pow(event->x() - mCenter.x(), 2) + std::pow(event->y() - mCenter.y(), 2) <= std::pow(mDiameter / 2.0, 2))
		emit isInsideCircle();
	else
		emit isOutsideCircle();
}

QColor ColoredCircle::hsvToRgb(qreal h, qreal s, qreal v)
{
	qreal hp = h / 60.0;
	qreal c = s * v;
	qreal x = c * (1 - std::abs(std::fmod(hp, 2) - 1));
	qreal m = v - c;
	qreal r = 0, g = 0, b = 0;
	if (hp <= 1) {
		r = c;
		g = x;
	}
	else if (hp <= 2) {
		r = x;
		g = c;
	}
	else if (hp <= 3) {
		g = c;
		b = x;
	}
	else if (hp <= 4) {
		g = x;
		b = c;
	}
	else if (hp <= 5) {
		r = x;
		b = c;
	}
	else {
		r = c;
		b = x;
	}
	r += m;
	g += m;
	b += m;
	return QColor(r * 255, g * 255, b * 255);
}
