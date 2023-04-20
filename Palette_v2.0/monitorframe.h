#pragma once

#include <QWidget>

class MonitorFrame  : public QWidget
{
	Q_OBJECT

public:
	MonitorFrame(QWidget *parent = nullptr);
	~MonitorFrame();

	QColor getColor() { return mClickedColor; }

signals:
	void colorChanged();

protected:
	void paintEvent(QPaintEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;

private:
	QWidget* mParent{ nullptr };
	QColor mClickedColor{ QColor(0,0,0,0) };
};
