#pragma once

#include <QMessageBox>
#include <qtimer.h>
#include <qevent.h>
#include <QtWidgets/QMainWindow>
#include "Main3D.h"
#include "GeneratedFiles/ui_GUISHLighting.h"

class GUISHLightingApp : public QMainWindow
{
	Q_OBJECT

public:
	GUISHLightingApp(QWidget *parent = Q_NULLPTR);

	~GUISHLightingApp();

	void UpdateFrame();

public slots:

	void Slot_Menu_Exit();

	void Slot_Menu_About();

	void Slot_LoadSphericalTexture();

	void Slot_LoadCubeMap();

	//void Slot_MouseInputProcess(QMouseEvent * ev);

protected:
		
	//repetitive callback to update frame
	virtual void timerEvent(QTimerEvent* ev) override;

	virtual void mouseMoveEvent(QMouseEvent* ev) override;

	virtual void mousePressEvent(QMouseEvent* ev) override;

	//virtual void mouseReleaseEvent(QMouseEvent* ev) override;

private:

	Ui::SHLightingGuiApp mUI;
	Main3DApp mMain3dApp;
};
