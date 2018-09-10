#include "GUISHLightingApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GUISHLightingApp w;
	w.show();
	return a.exec();
}
