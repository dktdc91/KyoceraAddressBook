#pragma once
#include <string>
#include <vector>

using namespace std;

class File;

/************************************************************************/
/*@brief The AddressFile class provides file stream read and write operation.
 *@author Yuri Yang <yuri.yang@liteon.com>
/************************************************************************/
class AddressFile
{
public:
	enum OpenMode
	{
		NotOpen,
		ReadOnly,
		WriteOnly,
		ReadWrite,
		Append,
		Unbuffered
	};

	AddressFile(void);
	~AddressFile(void);

	/*! Open device by the specified device name. Returns true if successful; otherwise rturns false */
	bool open(const string &fileName);
	/*! Open device by the specified device name and open mode for read/write/readwrite. Returns true if successful; otherwise rturns false */
	bool open(const string &fileName, AddressFile::OpenMode mode);

	/*! Close the device if it has opened*/
	void close();
	/*! Returns true if the device was opened, otherwise returns false */
	bool isOpen() const;

	bool eof();

	string readAll() const;
	string readLine() const;
	size_t read(void *data, size_t size);
	size_t read(int type, void *arg);

	size_t write(const void *data, size_t size);
	size_t write(int type, void *arg);

private:
	File *m_file;
};
