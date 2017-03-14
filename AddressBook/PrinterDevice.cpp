#include "StdAfx.h"
#include "PrinterDevice.h"
#include <iostream>

#ifdef _FOR_AMD64
	#define USBIO_DLL		_T"dlp4usbio64.dll")
	#define NETIO_DLL		_T("dlp4netio64.dll")
	#define WSD_DLL			_T("dlp4wsd64.dll")
#else
	#define USBIO_DLL		_T("dlp4usbio.dll")
	#define NETIO_DLL		_T("dlp4netio.dll")
	#define WSD_DLL			_T("dlp4wsd.dll")
#endif

class PrinterDevicePrivate
{
public:
	PrinterDevicePrivate(): opened(false), logged(false), deviceType(-1) 
	{
		memset((void*)&deviceHandle, 0xFF, sizeof(deviceHandle));

		usbModule = NULL;
		networkModule = NULL;
#if defined(_DPH_)
		wsdModule = NULL;
#endif
	}

// data members
	// USB IO
	HMODULE	usbModule;
	// Network IO
	HMODULE	networkModule;
	// WSD IO
#if defined(_DPH_)
	HMODULE	wsdModule;
#endif

	union
	{
		int		netPrinter;
		HANDLE	usbPrinter;
	} deviceHandle;


	bool	opened;
	bool	logged;
	int		deviceType;
	string	netAddress;
};

/************************************************************************/
/* Implements PrinterDevice class                                     */
/************************************************************************/
PrinterDevice::PrinterDevice(void) : d(new PrinterDevicePrivate)
{
	cout << "[PrinterDevice] initialize ...\n";
	// Loading driver library
	d->usbModule = ::LoadLibrary(USBIO_DLL);
	d->networkModule = ::LoadLibrary(NETIO_DLL);
#if defined(_DPH_)
	d->wsdModule = ::LoadLibrary(WSD_DLL);
#endif

}

PrinterDevice::~PrinterDevice(void)
{
	cout << "[PrinterDevice] uninitialized ...\n";

	if (d->usbModule != NULL)
	{
		::FreeLibrary(d->usbModule);
		d->usbModule = NULL;
	}
	if (d->networkModule != NULL)
	{
		::FreeLibrary(d->networkModule);
		d->networkModule = NULL;
	}
#if defined(_DPH_)
	if (d->wsdModule != NULL)
	{
		::FreeLibrary(d->wsdModule);
		d->wsdModule = NULL;
	}
#endif
	delete d;
}

bool PrinterDevice::open(const string &deviceName)
{
	d->opened = false;
	switch( d->deviceType )
	{	
	case USBDevice:
		if( this->connectByName(deviceName) )
		{
			d->opened = true;
			cout << "[PrinterDevice]:" << deviceName << " has been opened.\n";
		}
		break;
	case NetworkDevice:
		d->opened = false;
		break;
#if defined(_DPH_)
	case WSDDevice:
		d->opened = false;
		break;
#endif
	case UnknownDevice:
	default:
		d->opened = false;
		break;
	}

	return d->opened;
}


bool PrinterDevice::open(const string &deviceName, IODevice::DeviceType type)
{
	d->deviceType = type;
	return 	this->open(deviceName);
}


void PrinterDevice::close()
{
	this->logout();

	cout << "[PrinterDevice]: Close device.\n";
	switch( d->deviceType )
	{	
	case USBDevice:
		this->disconnect(d->deviceHandle.usbPrinter);
		break;
	case NetworkDevice:
		break;
#if defined(_DPH_)
	case WSDDevice:
		break;
#endif
	case UnknownDevice:
	default:
		break;
	}

	d->deviceType = -1;
	d->opened = false;
}

bool PrinterDevice::isOpen() const
{
	return d->opened;
}

int PrinterDevice::type() const
{
	return d->deviceType;
}

void PrinterDevice::setType(IODevice::DeviceType type)
{
	d->deviceType = type;
}

std::string PrinterDevice::networkAddress() const
{
	return d->netAddress;
}

__int64 PrinterDevice::readData(void *buff, DWORD& size)
{
	cout << "[PrinterDevice]: read ...\n";

	__int64 r = 0;
	switch(d->deviceType)
	{

	case USBDevice:
		r = this->readUSBData(d->deviceHandle.usbPrinter, buff, size);
		break;
	case NetworkDevice:
		r = this->readRemoteData(d->deviceHandle.netPrinter, buff, size);
		break;
#if defined(_DPH_)
	case WSDDevice:
		r = this->readRemoteData(d->deviceHandle.netPrinter, buff, size);
		break;
#endif
	case UnknownDevice:
	default:
		break;
	}
	return r;
}

__int64 PrinterDevice::writeData(const void *buff, DWORD& size)
{
	cout << "[PrinterDevice]: write ...\n";
	__int64 r = 0;
	switch(d->deviceType)
	{
	case USBDevice:
		r = this->writeUSBData(d->deviceHandle.usbPrinter, buff, size);
		break;
	case NetworkDevice:
		r = this->writeRemoteData(d->deviceHandle.netPrinter, buff, size);
		break;
#if defined(_DPH_)
	case WSDDevice:
		r = this->writeRemoteData(d->deviceHandle.netPrinter, buff, size);
		break;
#endif
	case UnknownDevice:
	default:
		break;
	}
	return r;
}

///////////////////////////////////////////////////////////////////////////
bool PrinterDevice::connectByName(const string &deviceName)
{
	typedef HANDLE (*USBConnectFn)(TCHAR *deviceName);
	USBConnectFn connFn = (USBConnectFn)::GetProcAddress(d->usbModule, "USBConnectByPrinterName");
	if (!connFn)
	{
		cout << "[PrinterDevice]: Load connectByName model failed.\n";
		return false;
	}

	TCHAR *param = new TCHAR[deviceName.size()+1];
	param[deviceName.size()] = 0;
	//we can't use memcpy() because
	//sizeof(TCHAR)==sizeof(char) may not be true:
	std::copy(deviceName.begin(), deviceName.end(), param);

// 	std::string wstr (deviceName.begin(), deviceName.end());
// 	TCHAR* Tstr = (TCHAR*)(&wstr[0]);

	d->deviceHandle.usbPrinter = connFn(param);
	if (d->deviceHandle.usbPrinter == NULL)
	{
		cout << "[PrinterDevice]:Connect to device by name \"" << param << "\" failed.\n";
		return false;
	}

	return true;
}

bool PrinterDevice::connectByVendorProductId(WORD &vid, WORD &pid)
{
	typedef HANDLE (*USBConnectByVidPid)(WORD Vid, WORD Pid);
	USBConnectByVidPid vidpidFn = (USBConnectByVidPid)::GetProcAddress(d->usbModule,"USBConnectByVidPid");
	if (!vidpidFn)
	{
		cout << "[PrinterDevice]:Load connectByVendorProductId model failed.\n";
		return false;
	}
	d->deviceHandle.usbPrinter = vidpidFn(vid, pid);
	if( !d->deviceHandle.usbPrinter )
	{
		cout << "[PrinterDevice]:Connect by vid/pid failed.\n";
		return false;
	}
	return true;
}

void PrinterDevice::disconnect(HANDLE hPrn)
{
	typedef void (*USBDisconnectFn)(HANDLE hPrn);
	USBDisconnectFn disconnectFn = (USBDisconnectFn)::GetProcAddress(d->usbModule, "USBDisconnect");

	if (!disconnectFn)
	{
		cout << "[PrinterDevice]: Load disconnect model failed.\n";
		return;
	}
	disconnectFn(hPrn);
	cout << "[PrinterDevice]: disconnect.\n";
}

__int64 PrinterDevice::readUSBData(HANDLE hPrn, void *buff, DWORD size)
{
	if( !hPrn )
		return -1;

	typedef int (*USBReadFn)(HANDLE, void*, DWORD);
	USBReadFn readFn = (USBReadFn)::GetProcAddress(d->usbModule, "USBRead");
	if (!readFn)
	{
		cout << "PrinterDevice: Load read model failed.\n";
		return -1;
	}

	return readFn(hPrn, buff, size);
}

__int64 PrinterDevice::writeUSBData(HANDLE hPrn, const void *buff, DWORD size)
{
	if( !hPrn )
		return -1;

	typedef int (*USBWriteFn)(HANDLE hPrn, const void* buff, DWORD size);
	USBWriteFn writeFn = (USBWriteFn)::GetProcAddress(d->usbModule, "USBWrite");
	if (!writeFn)
	{
		cout << "[PrinterDevice]:Load write model failed.\n";
		return -1;
	}

	return writeFn(hPrn, buff, size);
}

__int64 PrinterDevice::readRemoteData(int sd, void *buff, DWORD size)
{
	if( sd <= 0 )
		return -1;

	typedef int (*NetworkReadFn)(int sd, void* buff, DWORD  size) ;
	NetworkReadFn readFn = (NetworkReadFn)::GetProcAddress(d->networkModule, "NetworkRead");
	if (!readFn)
	{
		cout << "[PrinterDevice]:Load network read model failed.\n";
		return -1;
	}
	return readFn(sd, buff, size);
}

__int64 PrinterDevice::writeRemoteData(int sd, const void *buff, DWORD size)
{
	if( sd <= 0 )
		return -1;

	typedef int (*NetworkWriteFn)(int sd, const void* buff, DWORD  size) ;
	NetworkWriteFn writeFn = (NetworkWriteFn)::GetProcAddress(d->networkModule, "NetworkWrite");
	if (!writeFn)
	{
		cout << "[PrinterDevice]:Load network write model failed.\n";
		return -1;
	}
	return writeFn(sd, buff, size);
}
