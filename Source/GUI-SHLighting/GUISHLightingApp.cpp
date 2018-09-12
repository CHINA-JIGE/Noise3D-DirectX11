#include "GUISHLightingApp.h"

GUISHLightingApp::GUISHLightingApp(QWidget *parent)
	: QMainWindow(parent)
{
	mUI.setupUi(this);
	m_pRenderCanvas = new Widget_RenderCanvas(mUI.centralwidget);
	m_pRenderCanvas->Initialize(QRect(20, 20, 800, 600));

	//"register "signal&slot (must after setupUI)
	connect(mUI.btn_SelectSphericalMap, &QPushButton::clicked, this, &GUISHLightingApp::Slot_LoadSphericalTexture);
	connect(mUI.actionExit, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_Exit);
	connect(mUI.actionAbout, &QAction::triggered, this, &GUISHLightingApp::Slot_Menu_About);
}


GUISHLightingApp::~GUISHLightingApp()
{

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

/***********************************************

						Override Events

***************************************************/

/*void GUISHLightingApp::timerEvent(QTimerEvent * ev)
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
		QPoint localPos = ev->pos()-TL1-TL2;

		mUI.textBrowser_filePath->setText(QString::number(localPos.x()) +"," + QString::number(localPos.y()));
		if (mUI.renderCanvas->geometry().contains(localPos))
		{
				QMessageBox::information(this, "123", "233");
		}
	}
}
*/