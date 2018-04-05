#ifndef SMALLMETEXLAB_H
#define SMALLMETEXLAB_H


#include <QMainWindow>
#include <QTimer>
#include <QDir>
#include <QStringList>

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <fcntl.h>

#include "serialport.h"
#include "analogdisplay.h"

namespace Ui {
class smallMetexLab;
}

class smallMetexLab : public QMainWindow
{
	Q_OBJECT

public:
	explicit smallMetexLab(QWidget *parent = 0);
	~smallMetexLab();

private:
	Ui::smallMetexLab *ui;

	serialPort port;
	QTimer *timer = new QTimer(this);
	int d;

	void analyzeData(QString data);
	void logToFile(double value);

	QString fileName;
	QFile file;
	unsigned int countX;
	bool isLog;

	void upDigitalDisplay(double value, QString acdc, QString function, QString unit);
	void upAnalogDisplay(double value, QString acdc, QString function, QString unit);
	void upPlotDisplay(double value, QString acdc, QString function, QString unit);

	AnalogDisplay * analogDisplay = new AnalogDisplay;

public slots:
	void connectPort();
	void disconnectPort();
	void getDataAuto();
	void changeInterval();
	void getDataManual();
	void changeMode();
	void changeDisplayMode();
	void setFileName();
	void setFileNameByDialog();
	void onOffLog();
};

#endif // SMALLMETEXLAB_H
