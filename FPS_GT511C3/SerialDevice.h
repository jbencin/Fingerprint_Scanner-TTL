#ifndef SERIALDEVICE_H_143462161014853
#define SERIALDEVICE_H_143462161014853

/*
  C++ Class around Linux's serial port API
*/

#include <cstdint>
#include <string>

struct termios;

class SerialDevice
{
	std::string     myDevice;
	int             myFd = -1;
	struct termios *mySavedTio = NULL;

public:
	SerialDevice(const char* name);
	~SerialDevice();

	int     open         (long speed);
	void    close        ();
	int     read         ();
	size_t  write        (std::uint8_t* data, std::size_t len);
	bool    listen       ();
	bool    available    () { return !(myFd < 0); }
	bool    isOpen       () { return !(myFd < 0); }
};

#endif // SERIALDEVICE_H_143462161014853
