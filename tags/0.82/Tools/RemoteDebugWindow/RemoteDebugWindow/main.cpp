#include "remotedebugwindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	RemoteDebugWindow w;
	w.show();
	return a.exec();
}
