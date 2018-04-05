#ifndef ANALOGDISPLAY_H
#define ANALOGDISPLAY_H

#include <QWidget>

class AnalogDisplay : public QWidget
{
	Q_OBJECT
public:
	explicit AnalogDisplay(QWidget *parent = 0);

	void paintEvent(QPaintEvent *);

	void setRangeMin(int min);
	void setRangeMax(int max);
	void setScaleStep(int step);
	void setScaleSupStep(int supStep);

	void setValue(double Value);
	void setUnit(QString Unit);
	void setAcDc(QString AcDc);
	void setFunction(QString Function);

private:
	int rangeMin;
	int rangeMax;
	int scaleStep;
	int scaleSupStep;

	double value;
	QString unit;
	QString acdc;
	QString function;

signals:

public slots:
};

#endif // ANALOGDISPLAY_H
