#ifndef SMALLMETEXLAB_H
#define SMALLMETEXLAB_H


#include <QMainWindow>
#include <QTimer>
#include <QDir>
#include <QStringList>

#include <QSerialPort>
#include <QSerialPortInfo>

//#include "serialport.h"
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

	//serialPort port;
	QSerialPortInfo portInfo;
	QSerialPort * port = new QSerialPort;

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

	unsigned int countPlotX;
	QString lastFunction;
	bool isPlotEnable;
	bool isPlotSave;

public slots:
	void connectPort();
	void disconnectPort();
	void getData();
	void changeInterval();
	void changeMode();
	void changeDisplayMode();
	void setFileName();
	void setFileNameByDialog();
	void onOffLog();
	void savePlot();
	void clearPlot();
	void startPlot();
	void pauzePlot();
};

#endif // SMALLMETEXLAB_H
