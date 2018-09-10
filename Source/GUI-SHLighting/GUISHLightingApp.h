#pragma once

#include <QMessageBox>
#include <qtimer.h>
#include <QtWidgets/QMainWindow>
#include "Main3D.h"
#include "GeneratedFiles/ui_GUISHLighting.h"

class GUISHLightingApp : public QMainWindow
{
	Q_OBJECT

public:
	GUISHLightingApp(QWidget *parent = Q_NULLPTR);

public slots:

	void Slot_Menu_Exit();

	void Slot_Menu_About();

	void Slot_LoadSphericalTexture();

	void Slot_LoadCubeMap();

	void Slot_UpdateFrame();


private:

	Ui::SHLightingGuiApp mUI;
	QTimer mMainLoopDriverTimer;
	Main3DApp mMain3dApp;
};
