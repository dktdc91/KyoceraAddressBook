#pragma once
#include <string>
using namespace std;

/************************************************************************/
/*@brief The IODevice class provides communication with printer.
 *@author Yuri Yang <yuri.yang@liteon.com>
/************************************************************************/
class IODevice
{
public:
	typedef bool (callback *Read)(void *buffer, DWORD size, int type, void *arg);
	typedef bool (callback *Write)(void *buffer, DWORD size, int type, void *arg);
	enum AuthorityState
	{
		AUE_OK,
		AUE_CONNECT,
		AUE_PASSWORD,
		AUE_GENERAL,
		AUE_LOCKED,
	};

	enum DeviceType
	{
		UnknownDevice = -1,
		LocalFile = 0,
		NetworkDevice = 1,
		USBDevice = 2,
		WSDDevice = 3
	};

	IODevice(void);
	virtual ~IODevice(void);

	/*! Open device by the specified device name. Returns true if successful; otherwise rturns false */
	virtual bool open(const string &deviceName) = 0;
	/*! Open device by the specified device name and set its OpenType to type */
	virtual bool open(const string &deviceName, IODevice::DeviceType type)  = 0;

	/*! Close the device if it has opened*/
	virtual void close() = 0;
	/*! Returns true if the device was opened, otherwise returns false */
	virtual bool isOpen() const = 0;

	/*! Check device if need to login, returns 0 if needn't login, otherwise returns 1, or -1 if an error occurred.*/
	virtual int needLogin();
	/*! Returns enum AuthorityState state value when try to login the device */
	virtual int login(const string &password);
	/*! Logout from the device */
	virtual void logout();
	virtual bool isLogin() const;

	/*! Returns the device type that USB/Network/WSD type value*/
	virtual int type() const = 0;
	/*! Set device type that USB/Network/WSD type value*/
	virtual void setType(IODevice::DeviceType type) = 0;
	/*! Returned the device network address string */
	//virtual string networkAddress() const = 0;

	virtual __int64 read(int type, Read read_cb, void *arg);
	virtual __int64 write(int type, Write write_cb, void *arg);

private:
	/*! encode data to device*/
	virtual bool encode(unsigned char *buffer, DWORD& size, int type, void *arg);
	/*! decode data from device */
	virtual bool decode(const char* buffer, DWORD& size, int type, void *arg);
	
	char *encodeGroup(void *buffer, DWORD &size, void *arg);
	char *encodeContact(void *buffer, DWORD &size, void *arg);

	bool decodeGroup(const char *buffer, void *arg, int index);
	bool decodeContact(const char *buffer, void *arg);

protected:
	/*! Reads at most size bytes from the device into buff, and returns the number of bytes read.
		0 is returned when no more data is available for reading,or -1 if an error occurred
	*/
	virtual __int64 readData(void *buff, DWORD& dwSize);
	/*! Writes at most size bytes of data from buff to the device.
		Returns the number of bytes that were actually written, or -1 if an error occurred.
	*/
	virtual __int64 writeData(const void *buff, DWORD& dwSize);

private:
	DWORD m_token;
	bool m_hasLogin;
};