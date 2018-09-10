#include "GUISHLightingApp.h"

GUISHLightingApp::GUISHLightingApp(QWidget *parent)
	: QMainWindow(parent)
{
	mUI.setupUi(this);

	//setAttribute(Qt::WA_PaintOnScreen, true);
	mMainLoopDriverTimer.setTimerType(Qt::TimerType::PreciseTimer);
	mMainLoopDriverTimer.setInterval(1);
	mMainLoopDriverTimer.start(1);

	//"register "signal&slot (must after setupUI)
	connect(mUI.btn_SelectSphericalMap, &QPushButton::clicked, this, &GUISHLightingApp::Slot_LoadSphericalTexture);
	connect(mUI.actionExit, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_Exit);
	connect(mUI.actionAbout, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_About);

	//setup timer driven (no interval) frame update loop
	connect(&mMainLoopDriverTimer, SIGNAL(timeout), this, SLOT(update));

	bool initSucceeded = mMain3dApp.InitNoise3D(HWND(mUI.renderCanvas->winId()),UINT(mUI.renderCanvas->width()), UINT(mUI.renderCanvas->height()));
	if (!initSucceeded)
	{
		QMessageBox msg;
		msg.setText("The document has been modified.");
		msg.setInformativeText("Do you want to save your changes?");
		msg.setStandardButtons(QMessageBox::Yes);
		msg.setIcon(QMessageBox::Icon::Critical);
		msg.exec();
	}
}


GUISHLightingApp::~GUISHLightingApp()
{
	mMain3dApp.Cleanup();
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
	mMain3dApp.UpdateFrame();
}

/***********************************************

						Override Events

***************************************************/

void GUISHLightingApp::paintEvent(QPaintEvent * ev)
{
	mMain3dApp.UpdateFrame();
}

QPaintEngine * GUISHLightingApp::paintEngine() const
{
	return nullptr;
}
