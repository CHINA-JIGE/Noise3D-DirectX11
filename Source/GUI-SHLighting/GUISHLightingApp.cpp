#include "GUISHLightingApp.h"

GUISHLightingApp::GUISHLightingApp(QWidget *parent)
	: QMainWindow(parent)
{
	mUI.setupUi(this);

	setAttribute(Qt::WA_PaintOnScreen, true);
	mMainLoopDriverTimer.start(10);
	mMainLoopDriverTimer.setTimerType(Qt::TimerType::PreciseTimer);

	//"注册"signal&slot，必须在 setupUi 函数之后
	connect(mUI.btn_SelectSphericalMap, &QPushButton::clicked, this, &GUISHLightingApp::Slot_LoadSphericalTexture);
	connect(&mMainLoopDriverTimer, &QTimer::timeout, this, &GUISHLightingApp::Slot_UpdateFrame);
	connect(mUI.actionExit, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_Exit);
	connect(mUI.actionAbout, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_About);

	mMain3dApp.InitNoise3D(HWND(mUI.renderCanvas->window()->winId()));
}


void GUISHLightingApp::Slot_Menu_About()
{
	QMessageBox::information(this, tr("About"), tr("Author:sunhonglian. Timi J2 internal use only."));
}

void GUISHLightingApp::Slot_Menu_Exit()
{
	QApplication::exit();
}

void GUISHLightingApp::Slot_LoadSphericalTexture()
{
	QMessageBox::information(this, tr("Prompt"), tr("Common texture(spherically mapped) successfully loaded!"));
}

void GUISHLightingApp::Slot_LoadCubeMap()
{
}

void GUISHLightingApp::Slot_UpdateFrame()
{
}
