#pragma once
#include "IODevice.h"
#include <tchar.h>
#include <string>
using namespace std;

class PrinterDevicePrivate;

/************************************************************************/
/*@brief The PrinterDevice derive class provides communication with printer.
 *@author Yuri Yang <yuri.yang@liteon.com>
/************************************************************************/
class PrinterDevice : public IODevice
{
public:
	PrinterDevice(void);
	~PrinterDevice(void);

	/*! Open device by the specified device name. Returns true if successful; otherwise rturns false */
	bool open(const string &deviceName);
	/*! Open device by the specified device name and set its OpenType to type */
	bool open(const string &deviceName, IODevice::DeviceType type) ;

	/*! Close the device if it has opened*/
	void close();
	/*! Returns true if the device was opened, otherwise returns false */
	bool isOpen() const;

	/*! Returns the device type that USB/Network/WSD type value*/
	int type() const;
	/*! Set device type that USB/Network/WSD type value*/
	void setType(IODevice::DeviceType type);
	/*! Returned the device network address string */
	string networkAddress() const;


protected:
	/*! Reads at most size bytes from the device into buff, and returns the number of bytes read.
		0 is returned when no more data is available for reading,or -1 if an error occurred
	*/
	__int64 readData(void *buff, DWORD& dwSize);
	/*! Writes at most size bytes of data from buff to the device.
		Returns the number of bytes that were actually written, or -1 if an error occurred.
	*/
	__int64 writeData(const void *buff, DWORD& dwSize);

private:
	//USB
	bool connectByName(const string &deviceName) ;
	bool connectByVendorProductId(WORD &vid, WORD &pid);
	void disconnect(HANDLE hPrn);
	__int64 readUSBData(HANDLE hPrn, void *buff, DWORD size);
	__int64 writeUSBData(HANDLE hPrn, const void *buff, DWORD size);
	// Network
	__int64 readRemoteData(int sd, void *buff, DWORD size);
	__int64 writeRemoteData(int sd, const void *buff, DWORD size);

private:
	PrinterDevicePrivate *d;
};
