#include "stdafx.h"
#include "QtSHLightingUtApp.h"
#include <QMessageBox>

QtSHLightingUtApp::QtSHLightingUtApp(QWidget *parent)
	: QMainWindow(parent)
{
	mUI.setupUi(this);

	//"注册"signal&slot，必须在 setupUi 函数之后
	connect(mUI.btn_SelectSphericalMap, &QPushButton::clicked, this, &QtSHLightingUtApp::Slot_LoadSphericalTexture);

	mMain3dApp.InitNoise3D(mUI.renderCanvas->winId);
}


void QtSHLightingUtApp::Slot_LoadSphericalTexture()
{
	QMessageBox::information(this, tr("jigehahaah"), tr("jigeha66666"));
}

void QtSHLightingUtApp::Slot_LoadCubeMap()
{
}
