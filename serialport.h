#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <string>
#include <iostream>

class serialPort
{
public:
	serialPort();
	~serialPort();

	void connect(std::string portName);
	void disconnect();

	void writePort();
	std::string readPort();

	bool isConnect;

private:
	int port;

	int RTS_flag = TIOCM_RTS;
	struct termios portSettings;

	//int d;
};

#endif // SERIALPORT_H
