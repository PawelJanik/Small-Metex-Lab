#include <QPainter>
#include <iostream>

#include "analogdisplay.h"

AnalogDisplay::AnalogDisplay(QWidget *parent) : QWidget(parent)
{
	rangeMin = 0;
	rangeMax = 10;
	scaleStep = 5;
	scaleSupStep = 1;

	unit = "V";
	acdc = "DC";
}

void AnalogDisplay::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(Qt::darkGreen);

	int h = height();
	QRectF rectangle;
	if(width()/2 > h)
	{
		rectangle.setTopLeft(QPointF((width()/2)-h,30));
		rectangle.setBottomRight(QPointF(h+(width()/2),h*2+30));
	}
	else
	{
		rectangle.setTopLeft(QPointF(0,30));
		rectangle.setBottomRight(QPointF(width(),width()+20));
	}

	//painter.drawRect(rectangle);
	painter.drawArc(rectangle,40*16,100*16);

	if(width()/2 > height())
	{
		painter.translate(width()/2, h + 30);
	}
	else
	{
		h = width()/2;
		painter.translate(width()/2, h + 30);
	}

	painter.save();
	QFont textFont;
	textFont.setPixelSize(15);					//font
	painter.setFont(textFont);

	painter.rotate(qreal(-50));
	float step = scaleStep*100.0/(rangeMax-rangeMin);
	for(double i = rangeMin; i <= rangeMax; i += scaleStep)
	{
		painter.drawLine(0, -h+10, 0, -h-10);
		QString num;
		painter.drawText(QPoint(-5, -h-15),num.setNum(i));
		painter.rotate(qreal(step));
	}
	painter.restore();

	if(scaleSupStep != 0)
	{
		painter.save();
		painter.rotate(qreal(-50));
		step = scaleSupStep*100.0/(rangeMax-rangeMin);
		for(double i = rangeMin; i <= rangeMax; i += scaleSupStep)
		{
			painter.drawLine(0, -h+5, 0, -h-5);
			painter.rotate(qreal(step));
		}
		painter.restore();
	}

	textFont.setPixelSize(35);
	painter.setFont(textFont);
	painter.drawText(QPoint(-15, -100),unit);
	int acdcPoint = -10;
	if(unit == "mA" || unit == "mV")
		acdcPoint = 0;

	if(acdc == "AC")
		painter.drawText(QPoint(acdcPoint-5, -70),"~");
	else if(acdc == "DC")
		painter.drawText(QPoint(acdcPoint, -70),"-");

	textFont.setPixelSize(15);
	painter.setFont(textFont);
	painter.drawText(QPoint(-250, -40),function);

	painter.setPen(Qt::red);
	painter.rotate(qreal((100.0/(rangeMax-rangeMin)*value)-50.0));
	painter.drawLine(0, 0, 0, -h-10);

}

void AnalogDisplay::setRangeMin(int min)
{
	rangeMin = min;
	this->update();
}

void AnalogDisplay::setRangeMax(int max)
{
	rangeMax = max;
	this->update();
}

void AnalogDisplay::setScaleStep(int step)
{
	scaleStep = step;
	this->update();
}
void AnalogDisplay::setScaleSupStep(int supStep)
{
	scaleSupStep = supStep;
	this->update();
}

void AnalogDisplay::setValue(double Value)
{
	value = Value;
	this->update();
}

void AnalogDisplay::setUnit(QString Unit)
{
	unit = Unit;
	this->update();
}
void AnalogDisplay::setAcDc(QString AcDc)
{
	acdc = AcDc;
	this->update();
}
void AnalogDisplay::setFunction(QString Function)
{
	function = Function;
	this->update();
}
