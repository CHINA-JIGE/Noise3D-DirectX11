#include "stdafx.h"
#include "QtSHLightingUtApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QtSHLightingUtApp w;
	w.show();
	return app.exec();
}
