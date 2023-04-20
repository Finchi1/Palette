#include "widgetlayer.h"
#include <QResizeEvent>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QList>
#include <QTime>
#include <Windows.h>
#include <QSpacerItem>

WidgetLayer::WidgetLayer(QWidget* parent)
	: QWidget(parent)
{
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setMinimumSize(200, 150);
	mResizingFrame = new FrameLayer(this);
	mResizingFrame->show();
	mBtnMinimize = new QPushButton("🗕", this);
	mBtnMaximize = new QPushButton("🗗", this);
	mBtnClose = new QPushButton("✕", this);
	mBtnClose->setCursor(Qt::CursorShape::ArrowCursor);
	mBtnMaximize->setCursor(Qt::CursorShape::ArrowCursor);
	mBtnMinimize->setCursor(Qt::CursorShape::ArrowCursor);
	mBtnClose->setFixedSize(mSystemButtonsSize);
	mBtnMinimize->setFixedSize(mSystemButtonsSize);
	mBtnMaximize->setFixedSize(mSystemButtonsSize);
	mBtnClose->setStyleSheet(QString{
		R"(
			QPushButton:!hover
			{
				background-color: rgba(%1, %2, %3, 1);
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
			QPushButton:hover
			{
				background-color: red;
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
		)" }.arg(mSystemPanelColor.red()).arg(mSystemPanelColor.green()).arg(mSystemPanelColor.blue()));

	mBtnMaximize->setStyleSheet(QString{
		R"(
			QPushButton:!hover
			{
				background-color: rgba(%1, %2, %3, 1);
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
			QPushButton:hover
			{
				background-color: rgba(60, 60, 60, 1);
				color: white;
				border-width: 1px;
				border-color: rgba(%1, %2, %3, 1);
				font: bold;
				font-family: Times New Roman;
				font-size: 14;
				padding: 5px;
			}
		)" }.arg(mSystemPanelColor.red()).arg(mSystemPanelColor.green()).arg(mSystemPanelColor.blue()));
	mBtnMinimize->setStyleSheet(mBtnMaximize->styleSheet());
	auto widgetMaximize = [=]()
	{
		if (this->isMaximized())
		{
			this->showNormal();
			mResizingFrame->showNormal();
		}
		else
		{
			this->showMaximized();
			mResizingFrame->showMaximized();
		}
	};
	connect(mBtnMinimize, &QPushButton::clicked, this, &QWidget::showMinimized);
	connect(mBtnMaximize, &QPushButton::clicked, this, widgetMaximize);
	connect(mBtnClose, &QPushButton::clicked, qApp, &QApplication::quit);

	QList<QWidget*> systemWidgetsList{ mBtnMinimize, mBtnMaximize, mBtnClose };
	mResizingFrame->setSystemWidgetsInfo(systemWidgetsList);

	this->resize(640, 480);

	auto makeResize = [=]()
	{
		this->move(mResizingFrame->pos());
		this->resize(mResizingFrame->size());
		mIsFrameResizing = false;
		mAction = MoveType::NO_ACTION;
		this->repaint();
	};
	connect(mResizingFrame, &FrameLayer::makeResize, this, makeResize);

	auto frameInResizing = [=](MoveType action)
	{
		mIsFrameResizing = true;
		mAction = action;
		this->repaint();
	};
	connect(mResizingFrame, &FrameLayer::beginResizing, this, frameInResizing);
	setBlurBehind(true);

	// Content
	// Widget composition
	QVBoxLayout* mainLayout = new QVBoxLayout(this); // set as main layout
	QSpacerItem* systemSpacer = new QSpacerItem(this->width(), mBtnClose->height());
	mainLayout->addSpacerItem(systemSpacer);
	QHBoxLayout* mainHorizontalLayout = new QHBoxLayout;
	circle = new ColoredCircle(this);
	auto setCirclePointHandCursor = [=]() { circle->setCursor(Qt::PointingHandCursor); };
	auto setCircleArrowCursor = [=]() { circle->setCursor(Qt::ArrowCursor); };
	connect(circle, &ColoredCircle::isInsideCircle, this, setCirclePointHandCursor);
	connect(circle, &ColoredCircle::isOutsideCircle, this, setCircleArrowCursor);
	mainHorizontalLayout->addWidget(circle,1);
	QVBoxLayout* controlVerticalLayout = new QVBoxLayout;
	QHBoxLayout* redLayout = new QHBoxLayout;
	QHBoxLayout* greenLayout = new QHBoxLayout;
	QHBoxLayout* blueLayout = new QHBoxLayout;
	QHBoxLayout* alfaLayout = new QHBoxLayout;
	QString labelStyleSheet
	{
		R"(
			color: white;
			font-size: 14;
		)"
	};
	QString spinBoxStyleSheet
	{
		R"(
			background-color: rgba(90, 90, 90, 1);
			color: white;
			font-size: 14;
		)"
	};
	labelR = new QLabel("RED", this);
	labelR->setStyleSheet(labelStyleSheet);
	labelR->setCursor(Qt::ArrowCursor);
	redLayout->addWidget(labelR);
	spinBoxR = new QSpinBox(this);
	spinBoxR->setMinimum(0);
	spinBoxR->setMaximum(255);
	spinBoxR->setStyleSheet(spinBoxStyleSheet);
	spinBoxR->setCursor(Qt::ArrowCursor);
	redLayout->addWidget(spinBoxR);
	labelG = new QLabel("GREEN", this);
	labelG->setStyleSheet(labelStyleSheet);
	labelG->setCursor(Qt::ArrowCursor);
	greenLayout->addWidget(labelG);
	spinBoxG = new QSpinBox(this);
	spinBoxG->setMinimum(0);
	spinBoxG->setMaximum(255);
	spinBoxG->setStyleSheet(spinBoxStyleSheet);
	spinBoxG->setCursor(Qt::ArrowCursor);
	greenLayout->addWidget(spinBoxG);
	labelB = new QLabel("BLUE", this);
	labelB->setStyleSheet(labelStyleSheet);
	labelB->setCursor(Qt::ArrowCursor);
	blueLayout->addWidget(labelB);
	spinBoxB = new QSpinBox(this);
	spinBoxB->setMinimum(0);
	spinBoxB->setMaximum(255);
	spinBoxB->setStyleSheet(spinBoxStyleSheet);
	spinBoxB->setCursor(Qt::ArrowCursor);
	blueLayout->addWidget(spinBoxB);
	labelA = new QLabel("ALPHA", this);
	labelA->setStyleSheet(labelStyleSheet);
	labelA->setCursor(Qt::ArrowCursor);
	alfaLayout->addWidget(labelA);
	spinBoxA = new QSpinBox(this);
	spinBoxA->setMinimum(0);
	spinBoxA->setMaximum(255);
	spinBoxA->setValue(255);
	spinBoxA->setStyleSheet(spinBoxStyleSheet);
	spinBoxA->setCursor(Qt::ArrowCursor);
	alfaLayout->addWidget(spinBoxA);

	labelColor = new QLabel(this);
	labelColor->setCursor(Qt::ArrowCursor);

	monitorColor = new QPushButton("Get monitor color", this);
	monitorColor->setStyleSheet(spinBoxStyleSheet);

	controlVerticalLayout->addLayout(redLayout);
	controlVerticalLayout->addLayout(greenLayout);
	controlVerticalLayout->addLayout(blueLayout);
	controlVerticalLayout->addLayout(alfaLayout);
	controlVerticalLayout->addWidget(labelColor,1);
	controlVerticalLayout->addStretch(1);
	controlVerticalLayout->addWidget(monitorColor);

	mainHorizontalLayout->addLayout(controlVerticalLayout);

	mainLayout->addLayout(mainHorizontalLayout);

	statusBar = new QStatusBar(this);
	statusBar->setStyleSheet("color: white; font-size: 14;");
	statusBar->setCursor(Qt::ArrowCursor);
	statusBar->setSizeGripEnabled(false);
	statusBar->setMaximumHeight(20);
	mainLayout->addWidget(statusBar);

	auto changeLabelColor = [=]()
	{
		if (spinBoxA->value() != 1)
		{
			QString colorRGBA = QString{ "rgba(%1, %2, %3, %4)" }.arg(spinBoxR->value()).arg(spinBoxG->value()).arg(spinBoxB->value()).arg(spinBoxA->value());
			labelColor->setStyleSheet
			(
				"background-color: " + colorRGBA + ";"
				"border: 1px solid red;"
			);
		}
		QString r = intToHex(spinBoxR->value());
		if (r.length() == 1)
			r.prepend("0");
		QString g = intToHex(spinBoxG->value());
		if (g.length() == 1)
			g.prepend("0");
		QString b = intToHex(spinBoxB->value());
		if (b.length() == 1)
			b.prepend("0");
		QString a = intToHex(spinBoxA->value());
		if (a.length() == 1)
			a.prepend("0");

		QString hexColor = QString("color: #%1%2%3 and alpha: %4").arg(r).arg(g).arg(b).arg(a);
		statusBar->showMessage(hexColor);
	};
	changeLabelColor();
	connect(spinBoxR, &QSpinBox::valueChanged, this, changeLabelColor);
	connect(spinBoxG, &QSpinBox::valueChanged, this, changeLabelColor);
	connect(spinBoxB, &QSpinBox::valueChanged, this, changeLabelColor);
	connect(spinBoxA, &QSpinBox::valueChanged, this, changeLabelColor);

	auto circleClick = [=]()
	{
		QColor tempColor = circle->getColor();
		int red = tempColor.red();
		int green = tempColor.green();
		int blue = tempColor.blue();
		int alpha = tempColor.alpha();
		spinBoxR->setValue(red);
		spinBoxG->setValue(green);
		spinBoxB->setValue(blue);
		spinBoxA->setValue(alpha);
	};
	connect(circle, &ColoredCircle::onChangeColor, this, circleClick);

	monitorFrame = new MonitorFrame(this);
	auto showHideMonitorFrame = [=]()
	{
		if (monitorFrame->isVisible())
			monitorFrame->hide();
		else
		{
			monitorFrame->show();
			this->hide();
		}
	};
	connect(monitorColor, &QPushButton::clicked, this, showHideMonitorFrame);

	auto monitorFrameClicked = [=]()
	{
		QColor tempColor = monitorFrame->getColor();
		int red = tempColor.red();
		int green = tempColor.green();
		int blue = tempColor.blue();
		int alpha = tempColor.alpha();
		spinBoxR->setValue(red);
		spinBoxG->setValue(green);
		spinBoxB->setValue(blue);
		spinBoxA->setValue(alpha);
		monitorFrame->hide();
		this->show();
	};
	connect(monitorFrame, &MonitorFrame::colorChanged, this, monitorFrameClicked);
}

WidgetLayer::~WidgetLayer()
{

}

void WidgetLayer::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(this->rect(), mMainRectColor);

	painter.setBrush(mSystemPanelColor);
	painter.setPen(Qt::NoPen);
	QRect systemPanel(0, 0, this->width(), mBtnClose->height() + 1);
	painter.drawRect(systemPanel);
	painter.setBrush(Qt::NoBrush);

	// Widget corners points
	QPoint leftTopCorner(QPoint(0, 0));
	QPoint leftBottomCorner(QPoint(0, this->height() - painter.pen().width()));
	QPoint rightTopCorner(QPoint(this->width() - painter.pen().width(), 0));
	QPoint rightBottomCorner(QPoint(this->width() - painter.pen().width(), this->height() - painter.pen().width()));

	// Widget minimal size points
	QPoint leftTopOnMinimal(QPoint(0, this->minimumHeight()));
	QPoint leftBottomOnMinimal(QPoint(0, this->height() - this->minimumHeight()));
	QPoint rightTopOnMinimal(QPoint(this->width(), this->minimumHeight()));
	QPoint rightBottomOnMinimal(QPoint(this->width(), this->height() - this->minimumHeight()));
	QPoint topLeftOnMinimal(QPoint(this->minimumWidth(), 0));
	QPoint topRightOnMinimal(QPoint(this->width() - this->minimumWidth(), 0));
	QPoint bottomLeftOnMinimal(QPoint(this->minimumWidth(), this->height() - painter.pen().width()));
	QPoint bottomRightOnMinimal(QPoint(this->width() - this->minimumWidth(), this->height() - painter.pen().width()));

	// Widget frame lines
	QLine leftLine(leftTopCorner, leftBottomCorner);
	QLine rightLine(rightTopCorner, rightBottomCorner);
	QLine topLine(leftTopCorner, rightTopCorner);
	QLine bottomLine(leftBottomCorner, rightBottomCorner);

	painter.setPen(QPen(mResizingFrame->getPenColor(), mResizingFrame->getPenWidth()));
	if (mIsFrameResizing)
		switch (mAction)
		{
		case MoveType::LEFT_RESIZING:
			painter.drawLine(rightLine);
			painter.drawLine(QLine(topRightOnMinimal, rightTopCorner));
			painter.drawLine(QLine(bottomRightOnMinimal, rightBottomCorner));
			break;
		case MoveType::RIGHT_RESIZING:
			painter.drawLine(leftLine);
			painter.drawLine(QLine(topLeftOnMinimal, leftTopCorner));
			painter.drawLine(QLine(bottomLeftOnMinimal, leftBottomCorner));
			break;
		case MoveType::TOP_RESIZING:
			painter.drawLine(bottomLine);
			painter.drawLine(QLine(leftBottomOnMinimal, leftBottomCorner));
			painter.drawLine(QLine(rightBottomOnMinimal, rightBottomCorner));
			break;
		case MoveType::BOTTOM_RESIZING:
			painter.drawLine(topLine);
			painter.drawLine(QLine(leftTopOnMinimal, leftTopCorner));
			painter.drawLine(QLine(rightTopOnMinimal, rightTopCorner));
			break;
		case MoveType::LEFT_TOP_RESIZING:
			painter.drawLine(rightBottomCorner, rightBottomCorner - QPoint(0, 40));
			painter.drawLine(rightBottomCorner, rightBottomCorner - QPoint(40, 0));
			break;
		case MoveType::LEFT_BOTTOM_RESIZING:
			painter.drawLine(rightTopCorner, rightTopCorner + QPoint(0, 40));
			painter.drawLine(rightTopCorner, rightTopCorner - QPoint(40, 0));
			break;
		case MoveType::RIGHT_TOP_RESIZING:
			painter.drawLine(leftBottomCorner, leftBottomCorner - QPoint(0, 40));
			painter.drawLine(leftBottomCorner, leftBottomCorner + QPoint(40, 0));
			break;
		case MoveType::RIGHT_BOTTOM_RESIZING:
			painter.drawLine(leftTopCorner, leftTopCorner + QPoint(0, 40));
			painter.drawLine(leftTopCorner, leftTopCorner + QPoint(40, 0));
			break;
		default:
			painter.drawLine(leftLine);
			painter.drawLine(topLine);
			painter.drawLine(rightLine);
			painter.drawLine(bottomLine);
			break;
		}
	painter.end();
}

void WidgetLayer::resizeEvent(QResizeEvent* event)
{
	mBtnClose->setGeometry(this->width() - mBtnClose->width() - 1, 1, 0, 0);
	mBtnMaximize->setGeometry(this->width() - mBtnClose->width() - mBtnMaximize->width() - 1, 1, 0, 0);
	mBtnMinimize->setGeometry(this->width() - mBtnClose->width() - mBtnMaximize->width() - mBtnMinimize->width() - 1, 1, 0, 0);
	QList<QWidget*> systemWidgetsList{ mBtnMinimize, mBtnMaximize, mBtnClose };
	mResizingFrame->setSystemWidgetsInfo(systemWidgetsList);

	labelColor->setMaximumHeight(labelColor->width());
}
bool WidgetLayer::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
	static int i = 0;
	MSG* msg = reinterpret_cast<MSG*>(message);
	if(mBlurBehind && mNativeEventClear)
	{
		setWindowBlur(msg->hwnd);
		mNativeEventClear = false;
	}
	else if (!mBlurBehind && mNativeEventClear)
	{
		setWindowBlur(msg->hwnd, 0);
		mNativeEventClear = false;
	}
	return false;
}

void WidgetLayer::hideEvent(QHideEvent* event)
{
	mResizingFrame->hide();
}

void WidgetLayer::showEvent(QShowEvent* event)
{
	mResizingFrame->show();
}

void WidgetLayer::setWindowBlur(HWND hWnd, int state)
{
	const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
	if (hModule)
	{
		struct ACCENTPOLICY
		{
			int nAccentState;
			int nFlags;
			int nColor;
			int nAnimationId;
		};
		struct WINCOMPATTRDATA
		{
			int nAttribute;
			PVOID pData;
			ULONG ulDataSize;
		};
		typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
		const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
		if (SetWindowCompositionAttribute)
		{
			ACCENTPOLICY policy = { state, 0, 0, 0 };
			WINCOMPATTRDATA data = { 19, &policy, sizeof(ACCENTPOLICY) };
			SetWindowCompositionAttribute(hWnd, &data);
		}
		FreeLibrary(hModule);
	}
}

QString WidgetLayer::intToHex(int value)
{
	if (value == 0)
		return QString("0");
	QString result;
	while (value != 0)
	{
		int temp = value % 16;
		switch (temp)
		{
		case 0:
			result += "0";
			break;
		case 1:
			result += "1";
			break;
		case 2:
			result += "2";
			break;
		case 3:
			result += "3";
			break;
		case 4:
			result += "4";
			break;
		case 5:
			result += "5";
			break;
		case 6:
			result += "6";
			break;
		case 7:
			result += "7";
			break;
		case 8:
			result += "8";
			break;
		case 9:
			result += "9";
			break;
		case 10:
			result += "A";
			break;
		case 11:
			result += "B";
			break;
		case 12:
			result += "C";
			break;
		case 13:
			result += "D";
			break;
		case 14:
			result += "E";
			break;
		case 15:
			result += "F";
			break;
		default:
			break;
		}
		value /= 16;
	}
	std::reverse(result.begin(), result.end());
	return result;
}
