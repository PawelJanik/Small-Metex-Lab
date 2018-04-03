#include "smallmetexlab.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	smallMetexLab w;
	w.show();

	return a.exec();
}
