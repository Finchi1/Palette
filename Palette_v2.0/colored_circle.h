#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

class ColoredCircle : public QWidget
{
	Q_OBJECT

public:
	explicit ColoredCircle(QWidget *parent = nullptr);
	virtual ~ColoredCircle();
	QColor getColor() { return mClickedColor; }

signals:
	void onChangeColor();
signals:
	void isInsideCircle();
signals:
	void isOutsideCircle();

protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

private:
	QColor hsvToRgb(qreal h, qreal s, qreal v);

private:
	QPointF mCenter{0, 0};
	qreal mDiameter{0.0};
	QColor mClickedColor{255,255,255};
};
