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
	connect(ui->actionZamknij, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui->intervalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeInterval()));
	connect(ui->manualButton, SIGNAL(clicked()), this, SLOT(getDataManual()));

	connect(timer, SIGNAL(timeout()), this, SLOT(getDataAuto()));

	connect(ui->manualRadioButton, SIGNAL(clicked()), this, SLOT(changeMode()));
	connect(ui->autoRadioButton, SIGNAL(clicked()), this, SLOT(changeMode()));

	connect(ui->digitalRadioButton, SIGNAL(clicked()), this, SLOT(changeDisplayMode()));
	connect(ui->analogRadioButton, SIGNAL(clicked()), this, SLOT(changeDisplayMode()));
	connect(ui->plotRadioButton, SIGNAL(clicked()), this, SLOT(changeDisplayMode()));

	connect(ui->filePushButton, SIGNAL(clicked()), this, SLOT(setFileNameByDialog()));
	connect(ui->fileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setFileName()));
	connect(ui->logPushButton, SIGNAL(clicked()), this, SLOT(onOffLog()));

	QDir directory("/dev");
	QStringList portList = directory.entryList(QStringList() << "*ttyS*", QDir::NoDotAndDotDot | QDir::System);
	portList << directory.entryList(QStringList() << "*ttyUSB*", QDir::NoDotAndDotDot | QDir::System);

	for(int i = 0; i < portList.size(); i++)
	{
		portList[i].prepend("/dev/");
	}
	//portList.sort();
	ui->portBox->addItems(portList);

	isLog = false;
	changeDisplayMode();
}

smallMetexLab::~smallMetexLab()
{
	delete ui;
}

void smallMetexLab::connectPort()
{
	port.connect(ui->portBox->currentText().toStdString());

	if(port.isConnect == false)
		ui->statusBar->showMessage("Connecting falied");
	else
	{
		ui->statusBar->showMessage("Port is connect");
		ui->connectButton->setText("Disconnect");
		disconnect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectPort()));
		connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(disconnectPort()));
		ui->portBox->setEnabled(false);
		changeMode();
	}
}

void smallMetexLab::disconnectPort()
{
	port.disconnect();
	ui->connectButton->setText("Connect");
	ui->statusBar->clearMessage();
	disconnect(ui->connectButton, SIGNAL(clicked()), this, SLOT(disconnectPort()));
	connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectPort()));
	ui->portBox->setEnabled(true);
	ui->intervalSpinBox->setEnabled(false);
	ui->manualButton->setEnabled(false);
	timer->stop();
}

void smallMetexLab::getDataAuto()
{
	timer->stop();
	if(port.isConnect == true)
	{
		QString data = QString::fromStdString(port.readPort());
		timer->start(ui->intervalSpinBox->value());
		analyzeData(data);
	}
	else
	{
		disconnectPort();
		ui->statusBar->showMessage("Check the device");
	}
}

void smallMetexLab::changeInterval()
{
	timer->setInterval(ui->intervalSpinBox->value());
}


void smallMetexLab::getDataManual()
{
	if(port.isConnect == true)
	{
		QString data = QString::fromStdString(port.readPort());
		ui->manualButton->setEnabled(true);
		analyzeData(data);
	}
	else
	{
		disconnectPort();
		ui->statusBar->showMessage("Check the device");
	}
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
	ui->acdcLabel->setText("");
	if(data[0] == 'D' && data[1] == 'C')
	{
		ui->acdcLabel->setText("DC");
		if(data.at(12) == 'V')
			ui->funcionLabel->setText("Voltage");
		else
			ui->funcionLabel->setText("Current");
	}
	else if (data[0] == 'A' && data[1] == 'C')
	{
		ui->acdcLabel->setText("AC");
		if(data.at(12) == 'V')
			ui->funcionLabel->setText("Voltage");
		else
			ui->funcionLabel->setText("Current");
	}

	else if(data[0] == 'O' && data[1] == 'H')
		ui->funcionLabel->setText("Resistance");
	else if(data[0] == 'D' && data[1] == 'I')
		ui->funcionLabel->setText("Diode test");
	else if(data[0] == 'F' && data[1] == 'R')
		ui->funcionLabel->setText("Frequency");
	else if(data[0] == 'C' && data[1] == 'A')
		ui->funcionLabel->setText("Capacity");
	else if(data[0] == 'H' && data[1] == 'F')
		ui->funcionLabel->setText("Transistor test");
	else if(data[0] == 'L' && data[1] == 'O')
		ui->funcionLabel->setText("Logic");
	else if(data[0] == 'D' && data[1] == 'B')
		ui->funcionLabel->setText("Decibel");
	else if(data[0] == 'T' && (data[1] == 'M' || data[1] == 'E'))
		ui->funcionLabel->setText("Temperature");

	QString stringValue = data.at(4);
	stringValue.append(data.at(5));
	stringValue.append(data.at(6));
	stringValue.append(data.at(7));
	if(data.at(8) != QString(" "))
		stringValue.append(data.at(8));
	double value = stringValue.toDouble();
	if(data.at(3) == QString("-"))
		value = value * (-1);
	ui->lcdNumber->display(value);


	QString unit = data.at(9);
	unit.append(data.at(10));
	unit.append(data.at(11));
	unit.append(data.at(12));
	ui->unitLable->setText(unit);

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
		ui->digitalDisplayBox->show();
		ui->analogDisplayBox->hide();
		ui->plotBox->hide();
		ui->xAxisPlotComboBox->setEnabled(false);
		ui->xAxisPlotLabel->setEnabled(false);
	}
	else if(ui->analogRadioButton->isChecked() == true)
	{
		ui->digitalDisplayBox->hide();
		ui->analogDisplayBox->show();
		ui->plotBox->hide();
		ui->xAxisPlotComboBox->setEnabled(false);
		ui->xAxisPlotLabel->setEnabled(false);
	}
	else if(ui->plotRadioButton->isChecked() == true)
	{
		ui->digitalDisplayBox->hide();
		ui->analogDisplayBox->hide();
		ui->plotBox->show();
		ui->xAxisPlotComboBox->setEnabled(true);
		ui->xAxisPlotLabel->setEnabled(true);
	}
}

void smallMetexLab::upDigitalDisplay(double value, QString acdc, QString function, QString unit)
{

}

void smallMetexLab::upAnalogDisplay(double value, QString acdc, QString function, QString unit)
{
	//comm
}

void smallMetexLab::upPlotDisplay(double value, QString acdc, QString function, QString unit, QString xAxis)
{

}
