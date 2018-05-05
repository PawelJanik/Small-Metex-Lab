#include "smallmetexlab.h"
#include "ui_smallmetexlab.h"

#include <iostream>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QTime>

smallMetexLab::smallMetexLab(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::smallMetexLab)
{
	ui->setupUi(this);
	connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectPort()));
	connect(ui->intervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeInterval()));

	connect(ui->manualButton, SIGNAL(clicked()), this, SLOT(getData()));
	connect(timer, SIGNAL(timeout()), this, SLOT(getData()));

	connect(ui->manualRadioButton, SIGNAL(clicked()), this, SLOT(changeMode()));
	connect(ui->autoRadioButton, SIGNAL(clicked()), this, SLOT(changeMode()));

	connect(ui->digitalRadioButton, SIGNAL(clicked()), this, SLOT(changeDisplayMode()));
	connect(ui->analogRadioButton, SIGNAL(clicked()), this, SLOT(changeDisplayMode()));
	connect(ui->plotRadioButton, SIGNAL(clicked()), this, SLOT(changeDisplayMode()));

	connect(ui->filePushButton, SIGNAL(clicked()), this, SLOT(setFileNameByDialog()));
	connect(ui->fileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setFileName()));
	connect(ui->logPushButton, SIGNAL(clicked()), this, SLOT(onOffLog()));

	connect(ui->savePlotButton, SIGNAL(clicked()), this, SLOT(savePlot()));
	connect(ui->clearPlotButton, SIGNAL(clicked()), this, SLOT(clearPlot()));
	connect(ui->startPauzePlotButton, SIGNAL(clicked()), this, SLOT(startPlot()));

	QList<QSerialPortInfo> portList = portInfo.availablePorts();

	for(int i = 0; i < portList.count(); i++)
	ui->portBox->addItem(portList.at(i).portName());

	isLog = false;

	ui->analogLayout->addWidget(analogDisplay);
	analogDisplay->show();

	ui->plot->addGraph();
	isPlotEnable = false;
	countPlotX = 0;
	isPlotSave = true;

	changeDisplayMode();

	port->setBaudRate(QSerialPort::Baud1200);
	port->setDataBits(QSerialPort::Data7);
	port->setParity(QSerialPort::NoParity);
	port->setStopBits(QSerialPort::TwoStop);
}

smallMetexLab::~smallMetexLab()
{
	if(isPlotSave == false)
		savePlot();
	delete ui;
}

void smallMetexLab::connectPort()
{
	port->setPortName(ui->portBox->currentText());

	if(port->open(QIODevice::ReadWrite))
	{
		ui->statusBar->showMessage("Port is connected");
		ui->connectButton->setText("Disconnect");
		disconnect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectPort()));
		connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(disconnectPort()));
		ui->portBox->setEnabled(false);
		changeMode();
		ui->startPauzePlotButton->setEnabled(true);
		port->setRequestToSend(false);
		port->pinoutSignals();
		port->write("D");
	}
	else
		ui->statusBar->showMessage("Unable connect to port");
}

void smallMetexLab::disconnectPort()
{
	port->clear();
	port->setRequestToSend(true);
	port->pinoutSignals();
	if(port->isOpen())
		port->close();
	ui->connectButton->setText("Connect");
	ui->statusBar->clearMessage();
	disconnect(ui->connectButton, SIGNAL(clicked()), this, SLOT(disconnectPort()));
	connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectPort()));
	ui->portBox->setEnabled(true);
	ui->intervalSpinBox->setEnabled(false);
	ui->manualButton->setEnabled(false);
	ui->startPauzePlotButton->setEnabled(false);
	timer->stop();
}

void smallMetexLab::getData()
{
	int ms = QTime::currentTime().msecsSinceStartOfDay();
	if(port->isOpen())
	{
		QString data;
		while(data.length() < 14)
		{
			QByteArray d = port->read(1);
			data.append(QString::fromUtf8(d));
			if(ms + 2000 < QTime::currentTime().msecsSinceStartOfDay())
			{
				disconnectPort();
				ui->statusBar->showMessage("Check the device");
				break;
			}
		}

		if(port->isOpen())
		{
			timer->start(ui->intervalSpinBox->value());

			port->clear();
			port->write("D");
			analyzeData(data);
		}
	}
}

void smallMetexLab::changeInterval()
{
	timer->setInterval(ui->intervalSpinBox->value());
}

void smallMetexLab::changeMode()
{
	if(ui->autoRadioButton->isChecked() == true)
	{
		timer->start(ui->intervalSpinBox->value());
		ui->manualButton->setEnabled(false);
		ui->intervalSpinBox->setEnabled(true);
		ui->intervalLabel->setEnabled(true);
	}
	else if(ui->manualRadioButton->isChecked() == true)
	{
		timer->stop();
		ui->manualButton->setEnabled(true);
		ui->intervalSpinBox->setEnabled(false);
		ui->intervalLabel->setEnabled(false);
	}
}

void smallMetexLab::analyzeData(QString data)
{
	QString function;
	QString acdc;
	double value;
	QString unit;

	if(data[0] == 'D' && data[1] == 'C')
	{
		acdc = "DC";
		if(data.at(12) == 'V')
			function = "Voltage";
		else
			function = "Current";
	}
	else if (data[0] == 'A' && data[1] == 'C')
	{
		acdc = "AC";
		if(data.at(12) == 'V')
			function = "Voltage";
		else
			function = "Current";
	}

	else if(data[0] == 'O' && data[1] == 'H')
		function = "Resistance";
	else if(data[0] == 'D' && data[1] == 'I')
		function = "Diode test";
	else if(data[0] == 'F' && data[1] == 'R')
		function = "Frequency";
	else if(data[0] == 'C' && data[1] == 'A')
		function = "Capacity";
	else if(data[0] == 'H' && data[1] == 'F')
		function = "Transistor test";
	else if(data[0] == 'L' && data[1] == 'O')
		function = "Logic";
	else if(data[0] == 'D' && data[1] == 'B')
		function = "Decibel";
	else if(data[0] == 'T' && (data[1] == 'M' || data[1] == 'E'))
		function = "Temperature";

	QString stringValue = data.at(3);
	stringValue.append(data.at(4));
	stringValue.append(data.at(5));
	stringValue.append(data.at(6));
	stringValue.append(data.at(7));
	if(data.at(8) != QString(" "))
		stringValue.append(data.at(8));

	value = stringValue.toDouble();


	if(function == "Temperature")
	{
		unit = "*C";
	}
	else
	{
		unit = data.at(9);
		unit.append(data.at(10));
		unit.append(data.at(11));
		unit.append(data.at(12));
		unit.replace(" ","");
	}

	if(ui->digitalRadioButton->isChecked() == true)
		upDigitalDisplay(value, acdc, function, unit);
	else if(ui->analogRadioButton->isChecked() == true)
		upAnalogDisplay(value, acdc, function, unit);
	else if(ui->plotRadioButton->isChecked() == true)
		upPlotDisplay(value, acdc, function, unit);

	if(isLog == true)
		logToFile(value);
}

void smallMetexLab::logToFile(double value)
{
	QTextStream stream(&file);
	//stream << "something" << "\n";
	if(ui->xAxisComboBox->currentText() == "Measurement number")
	{
		stream << countX;
		countX++;
	}
	else if(ui->xAxisComboBox->currentText() == "Time")
	{
		stream << QTime::currentTime().toString();
	}

	if(ui->separatorComboBox->currentText() == ";")
		stream << ";";
	else if(ui->separatorComboBox->currentText() == "Space")
		stream << " ";
	else if(ui->separatorComboBox->currentText() == "Tab")
		stream << "\t";

	stream << value << "\n";
}

void smallMetexLab::setFileName()
{
	fileName = ui->fileLineEdit->text();
	file.setFileName(fileName);
}

void smallMetexLab::setFileNameByDialog()
{
	fileName = QFileDialog::getSaveFileName(this, tr("Log to file"), "/home/pawel", tr("All file"));
	ui->fileLineEdit->setText(fileName);
	file.setFileName(fileName);
}

void smallMetexLab::onOffLog()
{
	if(isLog == false)
	{
		if(!file.open(QIODevice::ReadWrite))
		{
			QMessageBox::information(this, tr("Unable to open file"),file.errorString());
		}
		else
		{
			ui->fileLineEdit->setEnabled(false);
			ui->filePushButton->setEnabled(false);
			ui->separatorComboBox->setEnabled(false);
			ui->xAxisComboBox->setEnabled(false);
			ui->logPushButton->setText("Off log");
			isLog = true;
		}
	}
	else if(isLog == true)
	{
		ui->fileLineEdit->setEnabled(true);
		ui->filePushButton->setEnabled(true);
		ui->separatorComboBox->setEnabled(true);
		ui->xAxisComboBox->setEnabled(true);
		ui->logPushButton->setText("On log");
		isLog = false;
	}
}

void smallMetexLab::changeDisplayMode()
{
	if(ui->digitalRadioButton->isChecked() == true)
	{
		clearPlot();

		ui->digitalDisplayBox->show();
		ui->analogDisplayBox->hide();
		ui->plotBox->hide();
	}
	else if(ui->analogRadioButton->isChecked() == true)
	{
		clearPlot();

		ui->digitalDisplayBox->hide();
		ui->analogDisplayBox->show();
		ui->plotBox->hide();
	}
	else if(ui->plotRadioButton->isChecked() == true)
	{
		ui->digitalDisplayBox->hide();
		ui->analogDisplayBox->hide();
		ui->plotBox->show();
	}
}

void smallMetexLab::upDigitalDisplay(double value, QString acdc, QString function, QString unit)
{
	ui->acdcLabel->setText(acdc);
	ui->funcionLabel->setText(function);
	ui->unitLable->setText(unit);
	ui->lcdNumber->display(value);
}

void smallMetexLab::upAnalogDisplay(double value, QString acdc, QString function, QString unit)
{
	if(value < 0)
		value = value * (-1);

	int rangeMaxTable[] = {2, 10, 20, 50, 100, 200, 500, 1000, 2000};
	int scaleStepTable[] = {1, 2, 5, 10, 20, 50, 100, 200, 200};
	int scaleSupStepable[] = {0, 1, 2, 5, 10, 20, 50, 100, 200};

	analogDisplay->setRangeMax(rangeMaxTable[0]);

	for(int i = 0; i < 8; i++)
	{
		if(value < rangeMaxTable[i])
		{
			analogDisplay->setRangeMax(rangeMaxTable[i]);
			analogDisplay->setScaleStep(scaleStepTable[i]);
			analogDisplay->setScaleSupStep(scaleSupStepable[i]);
			break;
		}
	}

	analogDisplay->setValue(value);
	analogDisplay->setUnit(unit);
	analogDisplay->setFunction(function);
	analogDisplay->setAcDc(acdc);
}

void smallMetexLab::upPlotDisplay(double value, QString acdc, QString function, QString unit)
{
	ui->plot->xAxis->setLabel("Count");
	QString ylabel = function;
	ylabel.append(" [");

	if(acdc == "AC")
		ylabel.append("~");
	else if(acdc == "DC")
		ylabel.append("-");

	if(unit.isEmpty() == false)
		ylabel.append(unit + "]");

	if(lastFunction.isEmpty() == false && ylabel != lastFunction)
	{
		timer->stop();
		savePlot();
		lastFunction.clear();
		countPlotX = 0;
		ui->plot->clearGraphs();
		ui->plot->addGraph();
		timer->start(ui->intervalSpinBox->value());
	}
	lastFunction = ylabel;

	ui->plot->yAxis->setLabel(ylabel);
	if(isPlotEnable == true)
	{
		ui->plot->graph(0)->addData(countPlotX,value);
		countPlotX++;
		ui->plot->xAxis->rescale();
		ui->plot->yAxis->rescale();
		ui->plot->yAxis->setRangeLower(0);
		ui->plot->replot();
		isPlotSave = false;
		ui->clearPlotButton->setEnabled(true);
		ui->savePlotButton->setEnabled(true);
	}
}

void smallMetexLab::savePlot()
{
	QMessageBox msgBox;
	msgBox.setText("You are change finction on multimetr");
	msgBox.setInformativeText("Do you want to save plot?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
	msgBox.setDefaultButton(QMessageBox::Save);
	int ret = msgBox.exec();

	if (ret == QMessageBox::Save)
	{
		QString fileName("/home/pawel/plot.png");

		fileName = QFileDialog::getSaveFileName(this, tr("Save plot"), "/home/pawel", tr("*.png"));

		QFile file(fileName);

		if (!file.open(QIODevice::WriteOnly))
			qDebug() << file.errorString();
		else
		{
			ui->plot->savePng(fileName);
			isPlotSave = true;
			ui->savePlotButton->setEnabled(false);
		}
	}
}

void smallMetexLab::clearPlot()
{
	timer->stop();
	if(isPlotSave == false)
		savePlot();

	lastFunction.clear();
	countPlotX = 0;
	ui->plot->clearGraphs();
	ui->plot->addGraph();
	ui->plot->replot();
	isPlotSave = true;
	ui->clearPlotButton->setEnabled(false);
	ui->savePlotButton->setEnabled(false);
	ui->plot->replot();
	timer->start(ui->intervalSpinBox->value());
}

void smallMetexLab::startPlot()
{
	isPlotEnable = true;
	ui->startPauzePlotButton->setText("Pauze");
	disconnect(ui->startPauzePlotButton, SIGNAL(clicked()), this, SLOT(startPlot()));
	connect(ui->startPauzePlotButton, SIGNAL(clicked()), this, SLOT(pauzePlot()));
}

void smallMetexLab::pauzePlot()
{
	isPlotEnable = false;
	ui->startPauzePlotButton->setText("Start");
	disconnect(ui->startPauzePlotButton, SIGNAL(clicked()), this, SLOT(pauzePlot()));
	connect(ui->startPauzePlotButton, SIGNAL(clicked()), this, SLOT(startPlot()));
}
