#pragma once

#include <QWidget>
#include <QMessageBox>
#include <qtimer.h>
#include <qevent.h>
#include <QtWidgets/QMainWindow>
#include "Main3D.h"

//3d Render Canvas widget
class Widget_RenderCanvas : public QWidget
{
	Q_OBJECT

public:
	Widget_RenderCanvas(QWidget *parent = Q_NULLPTR);

	~Widget_RenderCanvas();

	void Initialize(QRect region);

	Main3DApp& GetMain3dApp();

protected:

	void UpdateFrame();

protected:

	//repetitive callback to update frame
	virtual void timerEvent(QTimerEvent* ev) override;

	virtual void mouseMoveEvent(QMouseEvent* ev) override;

	virtual void mousePressEvent(QMouseEvent* ev) override;

	virtual void mouseReleaseEvent(QMouseEvent* ev) override;

private:

	Main3DApp mMain3dApp;
	
	bool mIsLeftMouseDown;
	bool mIsPrevCursorPosInitialized;//is it initialized before every drag?
	QPoint mPrevCursorPos;
};
