#include "GUISHLightingApp.h"

GUISHLightingApp::GUISHLightingApp(QWidget *parent)
	: QMainWindow(parent)
{
	mUI.setupUi(this);

	setAttribute(Qt::WA_PaintOnScreen, true);

	//but i dont why a QTimer doesn't work (no repetitive callbacks)
	GUISHLightingApp::QObject::startTimer(0, Qt::TimerType::PreciseTimer);

	//"register "signal&slot (must after setupUI)
	connect(mUI.btn_SelectSphericalMap, &QPushButton::clicked, this, &GUISHLightingApp::Slot_LoadSphericalTexture);
	connect(mUI.actionExit, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_Exit);
	connect(mUI.actionAbout, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_About);
	//connect(mUI.renderCanvas, SIGNAL(mousePressEvent), this, SLOT(Slot_MouseInputProcess));

	bool initSucceeded = mMain3dApp.InitNoise3D(HWND(mUI.renderCanvas->winId()), HWND(this->winId()) ,UINT(mUI.renderCanvas->width()), UINT(mUI.renderCanvas->height()));
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


GUISHLightingApp::~GUISHLightingApp()
{
	mMain3dApp.Cleanup();
}

void GUISHLightingApp::UpdateFrame()
{
	mMain3dApp.UpdateFrame();
}

void GUISHLightingApp::Slot_Menu_About()
{
	QMessageBox::information(this, tr("About"), tr("Author:sunhonglian."));
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


/*void GUISHLightingApp::Slot_MouseInputProcess(QMouseEvent * ev)
{
	if (ev->button() == Qt::MouseButton::LeftButton)
	{
		QMessageBox::information(this, "123", "233");
	}
}*/

/***********************************************

						Override Events

***************************************************/

void GUISHLightingApp::timerEvent(QTimerEvent * ev)
{
	GUISHLightingApp::UpdateFrame();
}

void GUISHLightingApp::mouseMoveEvent(QMouseEvent * ev)
{
	
}

void GUISHLightingApp::mousePressEvent(QMouseEvent * ev)
{
	if (ev->button() == Qt::MouseButton::LeftButton)
	{
		//calculate local pos with repsect to renderCanvas
		/*QPoint TL1 = mUI.renderCanvas->geometry().topLeft();
		QPoint TL2 = mUI.centralwidget->geometry().topLeft();
		QPoint localPos = ev->pos()-TL1-TL2;*/

		mUI.textBrowser_filePath->setText(QString::number(localPos.x()) +"," + QString::number(localPos.y()));
		if (mUI.renderCanvas->geometry().contains(localPos))
		{
				QMessageBox::information(this, "123", "233");
		}
	}
}
