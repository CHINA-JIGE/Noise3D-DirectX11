#include "stdafx.h"
#include "QtSHLightingUtility.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QtSHLightingUtility w;
	w.show();
	return app.exec();
}
