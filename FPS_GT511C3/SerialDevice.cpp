#include <iostream>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "SerialDevice.h"

using namespace std;

SerialDevice::SerialDevice(const char* device)
{
	myDevice = device;
}

SerialDevice::~SerialDevice()
{
	close();
}

static int getBaudRateFlag(long speed)
{
	switch (speed) {
		case 0:      return B0;
		case 50:     return B50;
		case 75:     return B75;
		case 110:    return B110;
		case 134:    return B134;
		case 150:    return B150;
		case 200:    return B200;
		case 300:    return B300;
		case 600:    return B600;
		case 1200:   return B1200;
		case 1800:   return B1800;
		case 2400:   return B2400;
		case 4800:   return B4800;
		case 9600:   return B9600;
		case 19200:  return B19200;
		case 38400:  return B38400;
		case 57600:  return B57600;
		case 115200: return B115200;
		case 230400: return B230400;
	}
	return -ENOTSUP;
}

int SerialDevice::open(long speed)
{
	if (myFd >= 0) return -EBUSY;
	//myFd = ::open(myDevice.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	myFd = ::open(myDevice.c_str(), O_RDWR | O_NOCTTY);
	if (myFd < 0) {
		cout << "Error opening \"" << myDevice << "\": \"" << strerror(errno) << "\" (" << errno << ")\n";
		return -errno;
	}
	//fcntl(myFd, F_SETFL, O_NONBLOCK);

	// Save old serial port parameters
	mySavedTio = (struct termios*)malloc(sizeof(*mySavedTio));
	memset(mySavedTio, 0, sizeof(*mySavedTio));
	tcgetattr(myFd, mySavedTio);

	int baudRateFlag = getBaudRateFlag(speed);
	if (baudRateFlag < 0) {
		cout << "Invalid baudrate: " << speed << "\n";
		return -EINVAL;
	}

	// Configure serial port
	struct termios tioatr;
	tioatr.c_cflag     = baudRateFlag | CS8 | CLOCAL | CREAD;
	tioatr.c_iflag     = IGNPAR | IGNBRK;
	tioatr.c_oflag     = 0;
	tioatr.c_lflag     = 0;
	tioatr.c_cc[VMIN]  = 1;
	tioatr.c_cc[VTIME] = 0;
	tcflush(myFd, TCIFLUSH);
	int status = tcsetattr(myFd ,TCSANOW, &tioatr); // Apply settings
	if (status) {
		cout << "tcsetattr() failed: \"" << strerror(errno) << "\" (" << errno << ")\n";
		return -errno;
	}

	return 0;
}

void SerialDevice::close()
{
	if (myFd >= 0) {
		if (mySavedTio) {
			// Restore old serial port parameters
			tcsetattr(myFd, TCSANOW, mySavedTio);
			free(mySavedTio);
			mySavedTio = NULL;
		}
		::close(myFd);
		myFd = 0;
	}
	myDevice.clear();
}

size_t SerialDevice::write(uint8_t* data, size_t len)
{
	if (myFd < 0) return 0;

	return ::write(myFd, data, len);
}

int SerialDevice::read()
{
	if (myFd < 0) return -ENODEV;

	uint8_t byte;
	int bytes = ::read(myFd, &byte, 1);
	if (bytes < 0) {
		//printf("%s: Read failed: \"%s\" (%d)\n", getClassName(), strerror(errno), errno);
		return -errno;
	}
	return (int)byte;
	//printf("%s: Read %d bytes:", getClassName(), bytes);
}

bool SerialDevice::listen()
{
	return true;
}
