#include "StdAfx.h"
#include "FileIODevice.h"
#include "EntryItem.h"
#include <stdio.h>
#include <errno.h>
#include <string>
#include <io.h>
#include <fstream>

using namespace std;


typedef struct TaskData
{
	char name[32];
	char fax[64];
	char company[64];
	char department[64];
	int id;
	int type;
	int speed;
	int ecm;
}EntryData;

FileIODevice::FileIODevice(void) : m_file(NULL)
{
}

FileIODevice::~FileIODevice(void)
{
	if(m_file)
	{
		fclose(m_file);
		//delete m_file;
		m_file = NULL;
	}
}

bool FileIODevice::open(const string &fileName)
{
	// w+ pure word read/write file, if file doesn't exist create it
	const char *file = fileName.c_str();
	const char *openMode = "r";
	int err = fopen_s(&m_file, file, openMode);
	if (err != 0)
		return false;
	if( !m_file )
		return false;

	return true;
}

bool FileIODevice::open(const string &fileName, IODevice::DeviceType type /*= LocalFile*/)
{
	return false;
}

void FileIODevice::close()
{
	int err = fclose(m_file);
}

bool FileIODevice::isOpen() const
{
	return (m_file != NULL);
}

int FileIODevice::type() const
{
	return IODevice::LocalFile;
}

void FileIODevice::setType(IODevice::DeviceType type)
{
	// do nothing
}

__int64 FileIODevice::read(int type, Read read_cb, void *arg)
{
	cout << "[FileIODevice]: read.\n";
	if (m_file == NULL)
	{
		cerr << "File cannot open!!!" << "\n";
		return -1;
	}

	// obtain file size
	fseek(m_file , 0 , SEEK_END); // m_file seek to file end
	const size_t size = ftell(m_file);  // get file size
	rewind(m_file);// reset seek file to start and flush stream

// 	const size_t size = filelength(fileno(m_file));

	__int64 readBytes = 0;
	char *buffer = NULL;
	//allocate memory to contain the whole file
	buffer = (char*)malloc(sizeof(char) * size);
	memset(buffer, 0x00, size);
	if (buffer == NULL)
	{
		cerr << "Memory error!!!" << stderr << "\n";
		return -1;
	}

	readBytes = readData(buffer, size);	
	if (readBytes == -1)
	{
		cerr << "Reading error!!!" << stderr << "\n";
		return -1;
	}

	EntryItem *item = new EntryItem;


	cout << buffer;

	free(buffer);
	return readBytes;
}

char* FileIODevice::readLine()
{
	cout << "[FileIODevice]: read line.\n";
	if (!m_file)
	{
		cerr << "File cannot open!!!" << "\n";
		return NULL;
	}

	char *buffer;
	buffer = (char*)malloc(sizeof(char) * 128);
	memset(buffer, 0x00, 128);
	while ( !feof(m_file) )
	{
		fgets(buffer, sizeof(buffer), m_file);
		cout << buffer;
	}

	return buffer;
}

char* FileIODevice::readAll()
{
	cout << "[FileIODevice]: read all.\n";
	if (m_file == NULL)
	{
		cerr << "File cannot open!!!" << "\n";
		return NULL;
	}
	// obtain file size
	fseek(m_file , 0 , SEEK_END); // m_file seek to file end
	const size_t size = ftell(m_file);  // get file size
	rewind(m_file);// reset seek file to start and flush stream

	__int64 readBytes = 0;
	char *buffer = NULL;
	//allocate memory to contain the whole file
	buffer = (char*)malloc(sizeof(char) * size);
	memset(buffer, 0x00, size);
	if (buffer == NULL)
	{
		cerr << "Memory error!!!" << stderr << "\n";
		return NULL;
	}

	readBytes = readData(buffer, size);	
	if (readBytes == -1)
	{
		cerr << "Reading error!!!" << stderr << "\n";
		return NULL;
	}

	//free(buffer);

	return buffer;
}

__int64 FileIODevice::write(int type, Write write_cb, void *arg)
{
	cout << "[FileIODevice]: write.\n";

	EntryItem *item = (EntryItem*)arg;
	if (!item)
		return -1;

	__int64 writeBytes = 0;
	switch( type )
	{
	case ABE::ContactType:
		{
			/*EntryData *entry = (EntryData*)malloc(sizeof(EntryData));
			strcpy_s(entry->name, 32, item->data(ABE::Role_Name).c_str());
			strcpy_s(entry->fax, 32, item->data(ABE::Role_Name).c_str());
			strcpy_s(entry->company, 32, item->data(ABE::Role_Name).c_str());
			strcpy_s(entry->department, 32, item->data(ABE::Role_Name).c_str());
			entry->id = atoi(item->data(ABE::Role_Id).c_str());
			entry->type = atoi(item->data(ABE::Role_Id).c_str());
			entry->speed = atoi(item->data(ABE::Role_Id).c_str());
			entry->ecm = atoi(item->data(ABE::Role_Id).c_str());
			size_t len = sizeof(EntryData);*/

			string data = "[FAX_ADDR]=";
			for (int i = ABE::Role_Id; i < ABE::Role_User; ++i)
			{
				data +=  item->data(i);
				// remove last comma separator, and add a new line character
				i == ABE::Role_User-1 ? data +="\r\n": data  += ",";
			}
			size_t len = data.length();
			char *val = const_cast<char*>(data.c_str());
			writeBytes += writeData(val, len);
		}
		break;
	case ABE::GroupType:
		{
			string data = "[FAX_ADDR_GM]=";
		}
		break;
	default:break;
	}

	return writeBytes;
}

__int64 FileIODevice::write(const char *data, size_t size)
{
	__int64 writeBytes = 0;
	writeBytes += writeData(data, size);
	return writeBytes;
}

__int64 FileIODevice::readData(void *buff, size_t size)
{
	if (size < 0)
		return -1;

	__int64 readBytes = 0;
	bool eof = false;

	if( m_file )
	{
		size_t result;
		bool retry = true;
		do
		{
			result = fread((char*)buff + readBytes, 1, size_t(size - readBytes), m_file);
			eof = feof(m_file);
			if (retry && eof && result == 0)
			{
				retry = false;
				continue;
			}
			readBytes += result;
		} while (!eof && (result == 0 ? errno == EINTR : readBytes < size));
	}

	if (!eof && readBytes == 0)
	{
		readBytes = -1;
	}

	return readBytes;
}

__int64 FileIODevice::writeData(const void *buff, size_t size)
{
	if (size < 0)
		return -1;

	__int64 writtenBytes = 0;
	if (m_file)
	{
		size_t result;
		//fseek(m_file,0L,SEEK_END);
		do
		{
			result = fwrite((char*)buff, sizeof(char), size_t(size - writtenBytes), m_file);
			//result = fwrite(buff, sizeof(EntryData), 1, m_file);
			writtenBytes += result;
		} while (result == 0 ? errno == EINTR : writtenBytes < size);
	}

	if (size &&  writtenBytes == 0)
		writtenBytes = -1;

	return writtenBytes;
}
