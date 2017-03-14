#include "StdAfx.h"
#include "AddressFile.h"
#include "EntryItem.h"
#include "utility.h"
#include <fstream>
#include <sstream>

//using namespace std::ios;

class File
{
public:
	string filename;
	fstream stream;
};

/************************************************************************/
/*The AddressFile class implement                                       */
/************************************************************************/
AddressFile::AddressFile(void)
	: m_file(new File)
{
}

AddressFile::~AddressFile(void)
{
	if( m_file )
	{
		delete m_file;
		m_file = 0;
	}
}

bool AddressFile::open(const string &fileName)
{
	return open(fileName, ReadOnly);
}

bool AddressFile::open(const string &fileName, OpenMode mode)
{
	m_file->filename = fileName;
	ios::open_mode openMode = ios::in;
	switch( mode )
	{
	case AddressFile::ReadOnly:
		openMode = ios::in;	// Open a file for reading
		break;
	case AddressFile::WriteOnly:
		openMode = ios::out;	// Create a file for writing
		break;
	case AddressFile::ReadWrite:
		openMode = ios::in|ios::out;	// Create a file for read/write
		break;
	case AddressFile::Append:
		openMode = ios::app;	// Append to a file
		break;
	case AddressFile::NotOpen:
	case AddressFile::Unbuffered:
		break;
	}
	
	// convert to wstring
	std::wstring widestr = std::wstring(fileName.begin(), fileName.end());
	const wchar_t * wchar = widestr.c_str();
	m_file->stream.open(wchar, openMode);

	//return m_file->stream != NULL ? true : false;
	return m_file->stream.is_open();
}

void AddressFile::close()
{
	m_file->stream.close();
}

bool AddressFile::isOpen() const
{
	return m_file->stream.is_open();
}

std::string AddressFile::readAll() const
{
	string line;
	string result = "";
	while (getline (m_file->stream, line))
	{
		result.append(line);
		m_file->stream << line;
	}
	
	return result;
}

std::string AddressFile::readLine() const
{
	string line;
	getline (m_file->stream, line);
	return line;
}

size_t AddressFile::read(int type, void *arg)
{
	cout << "[AddressFile]: read.\n";

	EntryItem *item = (EntryItem*)arg;
	if( !item ) return -1;

	string search1 = "[FAX_ADDR]=";
	string search2 = "[FAX_ADDR_GM]=";
	string line = readLine();
	size_t bytes = line.length();
	if ( line.find(search1, 0) != -1)
	{
		line.erase(0, search1.length());
		vector<string> strings = Util::Split(line, ",");// id, type, name, fax,....
		for (size_t i = 0; i < strings.size(); ++i)
		{
			cout << strings.at(i);
			// begin id=1 type = 2 name=3 ... speed=9
			item->setData(strings.at(i), i+1);
		}
		type = item->type();
	}
	else if(line.find(search2, 0) != -1)	// group
	{
		line.erase(0, search2.length());
		vector<string> strings = Util::Split(line, ",");// id, type, name, children id
		item->setType(ABE::GroupType);
		for (size_t i = 0; i < strings.size(); ++i)
		{
			//cout << strings.at(i);
			// 0:id  1:type, 2:name 3:children id
			if( i < 3 )
			{
				item->setData( strings.at(i), i + 1 );
			}
			else
			{
				// convert string to int, using std c++ function(refer to utility.h)
				int id = Util::ToNumber(strings.at(i));
				item->addChild(id);
			}
		}
		type = item->type();
	}
	else
	{
		//arg = NULL;
		type = ABE::UnkownType;
		bytes = -1;
	}

	return bytes;
}

size_t AddressFile::read(void *data, size_t size)
{
	if (!m_file->stream)
		return -1;

	if( !isOpen())
		return -1;

	m_file->stream.read((char*)data, size);
	//m_file->stream >> data;
	return m_file->stream.gcount();
}

size_t AddressFile::write(int type, void *arg)
{
	cout << "[AddressFile]: write.\n";

	EntryItem *item = (EntryItem*)arg;
	if (!item)
		return -1;

	size_t writeBytes = 0;
	switch( type )
	{
	case ABE::ContactType:
		{
			string data = "[FAX_ADDR]=";
			for (int i = ABE::Role_Id; i < ABE::Role_User; ++i)
			{
				data.append(item->data(i));
				// remove last comma separator, and add a new line character
				//i == ABE::Role_User-1 ? data +="\r\n": data  += ",";
				data.append(",");
			}
			// remove last ","
			data.erase(data.length()-1, 1);
			writeBytes = data.length();
			m_file->stream << data << "\n";
		}
		break;
	case ABE::GroupType:
		{
			string data = "[FAX_ADDR_GM]=";
			data.append(item->data(ABE::Role_Id));
			data.append(",");
			data.append(item->data(ABE::Role_Type));
			data.append(",");
			data.append(item->data(ABE::Role_Name));
			data.append(",");
			// add children id
			vector<int> ids = item->children();
			for (size_t i = 0; i < ids.size(); ++i)
			{
				//convert int to string, using std c++ function
				string str = Util::ToString(ids.at(i));
				data.append(str);
				data.append(",");
			}
			// remove last ","
			data.erase(data.length()-1, 1);
			writeBytes = data.length();
			m_file->stream << data << "\n";
		}
		break;
	default:break;
	}

	return writeBytes;
}

size_t AddressFile::write(const void *data, size_t size)
{
	if (!m_file->stream)
		return -1;

	if( !isOpen())
		return -1;

	m_file->stream.write((char*)data, size);
	//m_file->stream << data;
	return m_file->stream.gcount();
}

bool AddressFile::eof()
{
	return m_file->stream.eof();
}
