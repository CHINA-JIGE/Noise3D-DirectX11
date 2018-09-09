#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtSHLightingUtApp.h"
#include "Main3D.h"

class QtSHLightingUtApp : public QMainWindow
{
	Q_OBJECT

public:

	QtSHLightingUtApp(QWidget *parent = Q_NULLPTR);

public slots:

	void Slot_LoadSphericalTexture();

	void Slot_LoadCubeMap();

private:
	Ui::MyUiWindow mUI;
	Main3DApp mMain3dApp;
};
