#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtSHLightingUtility.h"

class QtSHLightingUtility : public QMainWindow
{
	Q_OBJECT

public:
	QtSHLightingUtility(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtSHLightingUtilityClass ui;
};
