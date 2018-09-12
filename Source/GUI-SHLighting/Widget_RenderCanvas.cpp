#include "Widget_RenderCanvas.h"

Widget_RenderCanvas::Widget_RenderCanvas(QWidget *parent)
	: QWidget(parent),
	mPrevCursorPos(QPoint(0, 0)),
	mIsPrevCursorPosInitialized(false),
	mIsLeftMouseDown(false)
{
}

Widget_RenderCanvas::~Widget_RenderCanvas()
{
	mMain3dApp.Cleanup();
}

//invoke and initialized from outside
void Widget_RenderCanvas::Initialize(QRect region)
{
	//mUI.setupUi(this);
	this->setObjectName(QStringLiteral("RenderCanvas"));
	this->setGeometry(region);

	//...=.=
	this->setAttribute(Qt::WA_PaintOnScreen, true);
	WId hwnd = this->winId();

	//but i dont why a QTimer doesn't work (no repetitive callbacks)
	Widget_RenderCanvas::QObject::startTimer(0, Qt::TimerType::PreciseTimer);

	bool initSucceeded = mMain3dApp.InitNoise3D(HWND(this->winId()), HWND(this->winId()), UINT(this->width()), UINT(this->height()));
	if (!initSucceeded)
	{
		QMessageBox msg;
		msg.setText("Initialization of Noise3D failed!");
		msg.setStandardButtons(QMessageBox::Yes);
		msg.setWindowTitle("Critical Error!");
		msg.setIcon(QMessageBox::Icon::Critical);
		msg.exec();
	}
}

void Widget_RenderCanvas::UpdateFrame()
{
	mMain3dApp.UpdateFrame();
}

/***********************************************

						Override Events

***************************************************/

void Widget_RenderCanvas::timerEvent(QTimerEvent * ev)
{
	Widget_RenderCanvas::UpdateFrame();
}

void Widget_RenderCanvas::mouseMoveEvent(QMouseEvent * ev)
{
	if (mIsLeftMouseDown)
	{
		mMain3dApp.RotateBall(0, float(ev->pos().rx())/100.0f,0.0f);
	}
}

void Widget_RenderCanvas::mousePressEvent(QMouseEvent * ev)
{
	if (ev->button() == Qt::MouseButton::LeftButton)mIsLeftMouseDown = true;
}

void Widget_RenderCanvas::mouseReleaseEvent(QMouseEvent * ev)
{
	if (ev->button() == Qt::MouseButton::LeftButton)mIsLeftMouseDown = false;
}
