#include "serialport.h"
#include <sys/time.h>

serialPort::serialPort()
{

}

serialPort::~serialPort()
{
	close(port);
}

void serialPort::connect(std::string portName)
{
	port = open(portName.c_str(),O_RDWR | O_NOCTTY);
	if(port == -1)
		isConnect = false;
	else
	{
		isConnect = true;

		fcntl(port, F_SETFL, 0);

		ioctl(port,TIOCMBIC,&RTS_flag);

		tcgetattr(port, &portSettings);
		cfsetispeed(&portSettings, B1200);
		cfsetospeed(&portSettings, B1200);
		portSettings.c_cflag |= (CLOCAL | CREAD);

		portSettings.c_cflag &= ~PARENB;
		portSettings.c_cflag &= ~CSTOPB;
		portSettings.c_cflag &= ~CSIZE;
		portSettings.c_cflag |= CS7;

		portSettings.c_lflag &= ~(ICANON | ECHO | ECHOE);

		tcsetattr(port,TCSANOW,&portSettings);

		//d = 0;
	}
}

void serialPort::disconnect()
{
	ioctl(port,TIOCMBIS,&RTS_flag);
	close(port);
	isConnect = false;
}

void serialPort::writePort()
{
	write(port, "D", 1);
}

std::string serialPort::readPort()
{
	struct timeval time, _time;
	writePort();
	char readBuffer[20] = {0};
	char c = 0;
	int i = 0;
	unsigned int countChar = 0;
	gettimeofday(&time, NULL);
	while (c != 10 && c != 13 &&  i < 14)
	{
		countChar = read(port,&c,1);
		if(countChar > 0)
		{
			readBuffer[i] = c;
			i++;
		}

		gettimeofday(&_time, NULL);
		if(_time.tv_sec - time.tv_sec > 2)
		{
			isConnect = false;
			break;
		}
	}
	std::string stringBuffer(readBuffer,14);

	return stringBuffer;
}
