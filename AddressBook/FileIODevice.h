#pragma once
#include "IODevice.h"
#include <string>
using namespace std;

/************************************************************************/
/*@brief The FileIODevice sub class provides a local file witch write date to.
 *@author Yuri Yang <yuri.yang@liteon.com> 2016-6-23
/************************************************************************/
class ADDRESS_BOOK_EXPORT FileIODevice : public IODevice
{
public:
	FileIODevice(void);
	~FileIODevice(void);

	/*! Open device by the specified device name. Returns true if successful; otherwise rturns false */
	bool open(const string &fileName);
	/*! Open device by the specified device name and set its OpenType to type */
	bool open(const string &fileName, IODevice::DeviceType type = LocalFile);

	/*! Close the device if it has opened*/
	void close();
	/*! Returns true if the device was opened, otherwise returns false */
	bool isOpen() const;
	/*! Returns the device type that USB/Network/WSD type value*/
	int type() const ;
	/*! Set device type that USB/Network/WSD type value*/
	void setType(IODevice::DeviceType type);

	__int64 read(int type, Read read_cb, void *arg);
	char* readLine();
	char* readAll();
	__int64 write(int type, Write write_cb, void *arg);
	__int64 write(const char *data, size_t size);

protected:
	/*! Reads at most size bytes from the device into buff, and returns the number of bytes read.
		0 is returned when no more data is available for reading,or -1 if an error occurred
	*/
	virtual __int64 readData(void *buff, size_t size);
	/*! Writes at most size bytes of data from buff to the device.
		Returns the number of bytes that were actually written, or -1 if an error occurred.
	*/
	virtual __int64 writeData(const void *buff, size_t size);

private:
	FILE*	m_file;// the file handle
};
