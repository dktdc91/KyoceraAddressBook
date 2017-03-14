#include "StdAfx.h"
#include "IODevice.h"
#include "AddressBook.h"
#include "EntryModel.h"
#include "EntryItem.h"
#include "utility.h"
#include <iostream>

// For receive data from / send data to device 
/*Macro defined for Phone book end*/
#define BOOK_MAX_NAME_SIZE			32
#define BOOK_MAX_EADDRESS_SIZE		64
#define BOOK_MAX_PHONENUMBER_SIZE	52

#define BOOK_MAX_GROUPNAME_SIZE		32
#define BOOK_MAX_GROUP_SDNO_SIZE	14

#define MAX_FAXGROUP_ITEM			6

#define MAX_COMPANYNAME				64
#define MAX_FAXDESC					254

#define MAX_DEVICE_USERNAME			100
#define MAX_DEVICE_PASSWORD			4

#define MAX_SERVERNAME				32
#define MAX_SERVERADDRESS			64
#define MAX_LOGINNAME				97
#define MAX_LOGINPWD				32
#define MAX_SHARENAME				32
#define MAX_SERVERPATH				96

/************************************************************************/
/* send data to and receive data from  device                            */
/************************************************************************/
typedef struct Request
{
	BYTE	cmd_mark;						// 0x1B
	BYTE	utility;						// ¡®A¡¯ (0x41): Stands for Address Book Editor
	WORD	identifyWord;					// ¡°DB¡± (0x4244): Identify Word
	WORD	reserved;						// Don't care, set to 0x0.
	BYTE	command;						// Command code
	BYTE	serial;							// Serial number of packet (optional)
	DWORD	size;							// the size of payload. Zero is allowed.
	DWORD	token;							// the checksum value for validating data
	//BYTE	payload_for_current_command[size];	// payload_for_current_command[size], Optional, depends on the command.
} RequestCommand, *pRequestCommand;

typedef struct Response
{
	BYTE	rsp_mark;								// 0x1D
	BYTE	status;									// 0x00: Success; Nonzero: Status (code TBD).
	BYTE	command;								// Refer to the table below.
	BYTE	serial;									// Serial number of packet (optional)
	DWORD	size;          							// the size of payload. Zero is allowed.
	DWORD	checksum;								// the checksum value for validating data
	//BYTE	payload_for_current_response[size]; 	// payload_for_current_response[size], Optional, depends on the response.
} ResponseCommand, *pResponseCommand;


/************************************************************************/
/*encode data and decode data                                            */
/************************************************************************/
/*structures of BOOK start*/
typedef struct FlashSrv_Book_Data_Content//160bytes
{                    
	BYTE          Name[BOOK_MAX_NAME_SIZE];               // 32byte
	BYTE          EAddress[BOOK_MAX_EADDRESS_SIZE];        // 64byte
	BYTE          PNumber[BOOK_MAX_PHONENUMBER_SIZE];         // 52byte
	BYTE          DataIndex;             // 1byte  //1~99 be careful!!
	BYTE          NameLeng;                     // 1byte
	BYTE          EAddressLeng;    // 1byte
	BYTE          PNumberLeng;    // 1byte
	BYTE          Reserved1[8];       // 8byte
}tFlashSrv_Book_Data_Content;

typedef struct FlashSrv_Book_Group_Content//48 bytes
{
	BYTE          GroupType;           // 1byte(0xff new,1:phone 2: email)
	BYTE           GroupNameLeng;          // 1byte         
	//BYTE         Reserved[2];                    // 2byte
	BYTE          GroupName[BOOK_MAX_GROUPNAME_SIZE];          // 32byte
	BYTE           GroupMemberBitwise[BOOK_MAX_GROUP_SDNO_SIZE]; // 14byte
	//BYTE         Reserved1[4];       // 4byte
}tFlashSrv_Book_Group_Content;

typedef struct FlashSrv_Book_Server_Content//  368bytes
{
	BYTE           ServerNetworkType;                            // 1byte(0xff: new 1:Server(FTP) 2:Computer(SMB))
	BYTE           PortNumberLeng;          // 1byte
	BYTE           ServerNameLeng;         // 1byte
	BYTE           ServerAddressLeng;     // 1byte
	BYTE           LoginNameLeng;                     // 1byte
	BYTE           LoginPWDLeng;             // 1byte
	BYTE           ShareNameLeng;                     // 1byte
	BYTE           ServerPathLeng;            // 1byte
	DWORD		   PortNumber;                              // 4byte
	BYTE           ServerName[MAX_SERVERNAME];            // 32byte
	BYTE           ServerAddress[MAX_SERVERADDRESS];        // 64byte
	BYTE           LoginName[MAX_LOGINNAME];             // 97byte      
	BYTE           Reserved[3];                    // 3byte
	BYTE           LoginPWD[MAX_LOGINPWD];               // 32byte
	BYTE           ShareName[MAX_SHARENAME];             // 32byte
	BYTE           ServerPath[MAX_SERVERPATH];               // 96byte
}tFlashSrv_Book_Server_Content;


/************************************************************************/
/* The IODevice class operation                                         */
/************************************************************************/
IODevice::IODevice(void) : m_token(0), m_hasLogin(false) 
{

}

IODevice::~IODevice(void)
{
}

__int64 IODevice::read(int type, Read read_cb, void *arg)
{
	RequestCommand request;
	ResponseCommand response;
	
	//::memset((void*)&request, 0xFF, sizeof(RequestCommand));
	ZeroMemory(&request, sizeof(RequestCommand));

	switch( type )
	{
	case ABE::FormCombined:
		// user data: id,name,fax,email
		request.command = 0x21;
		break;
	case ABE::FormFaxGroup:
		// group data
		request.command = 0x22;
		break;
	default:
		// phone book entries
		request.command = 0x20;
		return -1;
	}

	request.cmd_mark = 0x1B;
	request.utility = 0x41;
	request.identifyWord = 0x4244;
	request.serial = 0;
	request.size = 0;
	request.token = m_token;
	DWORD readBytes = sizeof(RequestCommand);
	DWORD totalBytes = 0;

	if ( writeData(&request, readBytes) <= 0 || readBytes == 0)
	{
		cout << "[IODevice]: Write data to device failed.\n";
		return readBytes;
	}

	while (1)
	{
		readBytes = sizeof(response);
		if (  readData(&response, readBytes) <= 0 || readBytes != sizeof(response) || response.command != request.command || response.size == 0)
		{
			cout << "1.[IODevice]: Read data from device failed.\n";
			if(response.command != request.command)
			{
				cout.setf(ios::hex,ios::basefield);
				cout.setf(ios::showbase);
				cout << "[IODevice]: Error:response.command != request.command:" << (short)response.command << " - " << (short)request.command << "\n";
			}

			if( response.size == 0)
			{
				cout << "[IODevice]: Error:response.size == 0 -> " <<  readBytes << "---" << response.size << endl;
			}
			return response.size;
		}

		LPBYTE buffer = (LPBYTE)VirtualAlloc(NULL, response.size, MEM_COMMIT, PAGE_READWRITE);
		if (buffer == NULL)
		{
			return -1;
		}
		readBytes = response.size;
		if ( totalBytes += readData(buffer, readBytes)  <= 0 || readBytes != response.size || response.status != 0)
		{
			cout << "2.[IODevice]: Read data from device failed.\n";
			if(response.status != 0)
			{
				cout << "[IODevice]: Error:response.status != 0";
			}
			VirtualFree(buffer, 0, MEM_RELEASE);
			break;
		}
		totalBytes += readBytes;

		char* data = reinterpret_cast<char*>(buffer);
		if (!decode(data, readBytes, type, arg))
		{
			VirtualFree(buffer, 0, MEM_RELEASE);
			buffer = NULL;
			return -1;
		}

		if( read_cb && !read_cb(data, readBytes, type, arg)  )
		{
			VirtualFree(buffer, 0, MEM_RELEASE);
			buffer = NULL;
			break;
		}
		
		VirtualFree(buffer, 0, MEM_RELEASE);
		buffer = NULL;
		if (response.serial == 0)
		{
			break;
		}
	}
	
	return totalBytes;
}

__int64 IODevice::write(int type, Write write_cb, void *arg)
{
	if(this->type() == -1)
	{
		return -1;
	}
	// Send Command
	BYTE  command;
	DWORD writeabledSize = 0;
	pRequestCommand lpRequire = NULL;

	switch( type )
	{
	case ABE::FormCombined:
		// user data: id,name,fax,email
		command = 0x41;
		writeabledSize = 15840;
		break;
	case ABE::FormFaxGroup:
		// group data
		command = 0x42;
		writeabledSize = 768;
		break;
	default:
		// phone book entries
		command = 0x40;
		writeabledSize = 28384;
		break;
	}

	lpRequire = (pRequestCommand)VirtualAlloc(NULL, sizeof(RequestCommand) + writeabledSize, MEM_COMMIT, PAGE_READWRITE);
	ZeroMemory(lpRequire, sizeof(RequestCommand));
	memset((void*)((LPBYTE)lpRequire + sizeof(RequestCommand)), 0xFF,  writeabledSize);

	lpRequire->cmd_mark = 0x1B;
	lpRequire->utility = 0x41;
	lpRequire->identifyWord = 0x4244;
	lpRequire->command = command;
	lpRequire->size = writeabledSize;
	lpRequire->serial = 0;
	lpRequire->token = m_token;



	unsigned char *buffer = (unsigned char*)lpRequire + sizeof(RequestCommand);
	// encode data
	bool bOk = encode( buffer, writeabledSize, type, arg );
	if ( !bOk )
	{
		VirtualFree(lpRequire, 0, MEM_RELEASE);
		return -1;
	}

	if ( write_cb && !write_cb(buffer, writeabledSize, type, arg) )
	{
		VirtualFree(lpRequire, 0, MEM_RELEASE);
		return false;
	}

	writeabledSize += sizeof(RequestCommand);
	if ( writeData(lpRequire, writeabledSize) < 0 || writeabledSize == 0)
	{
		VirtualFree(lpRequire, 0, MEM_RELEASE);
		return FALSE;
	}

	VirtualFree(lpRequire, 0, MEM_RELEASE);

	// Get Response
	ResponseCommand response;
	writeabledSize = sizeof(response);
	if (readData(&response, writeabledSize) < 0 || writeabledSize != sizeof(response) || command != response.command)
	{
		return false;
	}
	bool ok = (response.status == 0);
	return ok;
}

int IODevice::needLogin()
{
	cout << "[IODevice]: Request login device...\n";
	if (type() == -1)
	{
		return -1;
	}

	bool needed = false;
	// send command
	RequestCommand request;
	memset((void*)&request, 0xFF, sizeof(RequestCommand));
	request.cmd_mark = 0x1B;
	request.utility = 0x41;
	request.identifyWord = 0x4244;
	request.command = 0x10;
	request.size = 0;
	request.serial = 0;
	request.token = 0;

	DWORD dwSize = sizeof(request);
	if ( writeData(&request, dwSize) == -1 || dwSize == 0)
	{
		cout << "[IODevice]: 1.Request login device failed.\n";
		return -1;
	}

	// receive command
	ResponseCommand response;
	dwSize = sizeof(response);
	if ( readData(&response, dwSize) == -1 || dwSize != sizeof(response) || response.status != 0 || response.size != 4)
	{
		if (response.size != 4)
		{
			cout << "[IODevice]: 2.Needn't to login device.\n";
		}
		cout << dwSize << "-" << response.size << "-" << response.status << "-" << response.command << endl; 
		return false;
	}

	BYTE buffer[4];
	dwSize = sizeof(buffer);
	if ( readData(buffer, dwSize) == -1 || dwSize != sizeof(buffer))
	{
		cout << "[IODevice]: 3.Request login device failed.\n";
		return false;
	}

	needed = (buffer[0] == 0);

	return needed;
}

int IODevice::login(const string &password)
{
	if ( type() == -1 )
	{
		return AUE_GENERAL;
	}
	DWORD wdSize = 4;

	// send command
	pRequestCommand request = (pRequestCommand)new char[sizeof(RequestCommand) + wdSize + 1];
	::memset((void*)request, 0xFF, sizeof(pRequestCommand) +  wdSize + 1);

	request->cmd_mark = 0x1B;
	request->utility = 0x41;
	request->identifyWord = 0x4244;
	request->command = 0x11;
	request->size = wdSize;
	request->serial = 0;
	request->token = 0;

	char *lpszPassword = (char *)request + sizeof(RequestCommand);
	::memcpy(lpszPassword, password.c_str(), wdSize);
	wdSize += sizeof(RequestCommand);
	if ( writeData(request, wdSize) == -1 || wdSize == 0)
	{
		cout << "[IODevice]: Request login device connecting...\n";
		return AUE_CONNECT;
	}
	delete [] request;
	request = NULL;

	// receive command
	ResponseCommand response;
	wdSize = sizeof(response);
	if ( readData(&response, wdSize) == -1 || wdSize != sizeof(response) || response.status != 0 )
	{
		cout << "[IODevice]: Password length: " << wdSize << endl;
		if(response.status == 0x10)
		{
			return AUE_LOCKED;
		}
		return AUE_PASSWORD;
	}

	wdSize = sizeof(m_token);
	if ( readData(&m_token, wdSize) == -1 || wdSize != sizeof(m_token))
	{
		return AUE_PASSWORD;
	}
	m_hasLogin = true;
	return AUE_OK;
}

void IODevice::logout()
{
	if( type() == -1 || !isLogin() )
		return ;

	cout << "[IODevice]: Request logout device.\n";

	// send command
	RequestCommand request;
	::memset((void*)&request, 0xFF, sizeof(RequestCommand));
	request.cmd_mark = 0x1B;
	request.utility = 0x41;
	request.identifyWord = 0x4244;
	request.command = 0x1F;
	request.size = 0;
	request.serial = 0;
	request.token = m_token;

	m_hasLogin = false;
	m_token = 0;

	DWORD dwSize = sizeof(request);
	if (writeData(&request, dwSize) == -1 || dwSize == 0)
	{
		cout << "[IODevice]: 1.Logout failed.\n";
		return;
	}



	ResponseCommand response;
	dwSize = sizeof(response);
	if (readData(&response, dwSize) == -1 || dwSize != sizeof(response) || response.status != 0)
	{
		cout << "[IODevice]: 2.Logout succeed:"<< (char)response.command <<  "\n";
	}
}

bool IODevice::isLogin() const
{
	return m_hasLogin;
}

__int64 IODevice::readData(void *buff, DWORD& dwSize)
{
	cout << "[IODevice]: readData().\n";
	return -1;
}

__int64 IODevice::writeData(const void *buff, DWORD& dwSize)
{
	cout << "[IODevice]: writeData().\n";
	return -1;
}


bool IODevice::encode(unsigned char * buffer, DWORD& size, int type, void *arg)
{
	DWORD dwLeft = size, dwItemSize = 0, dwOffset = 0;

	EntryModel *model = (EntryModel *)arg;
	if (!model)
		return false;

	switch( type )
	{
	case ABE::FormCombined:
		{
			dwItemSize = sizeof(tFlashSrv_Book_Data_Content);
			if ( dwLeft >= 15840)
			{
				for (int i = 0; i < model->rowCount(); ++i)
				{
					EntryItem *item = model->itemFromIndex(i);
					if (item)
					{
						int itemType = item->type();
						if (itemType == ABE::ContactType )
						{
							dwLeft = size - dwOffset;
							encodeContact(buffer + dwOffset, dwLeft, item);
							size += dwItemSize;
							dwOffset += dwItemSize;
							dwLeft -= dwItemSize;
						}
					}
				}
				dwOffset += 15840;
				dwLeft -= 15840;
			}
		}
		break;
	case ABE::FormFaxGroup:
		{
			dwItemSize = sizeof(tFlashSrv_Book_Group_Content);
			//size = dwItemSize*MAX_FAXGROUP_ITEM;

			if (dwLeft >= 768)
			{
				for (int i = 0; i < model->rowCount(); ++i)
				{
					EntryItem *item = model->itemFromIndex(i);
					if (item)
					{
						int itemType = item->type();
						//int nItem = 0;
						if (itemType == ABE::GroupType)
						{
							//dwOffset = dwItemSize * nItem;
							dwLeft = size - dwOffset;

							encodeGroup(buffer + dwOffset, dwLeft, item);

							size += dwItemSize;
							dwOffset += dwItemSize;
							dwLeft -= dwItemSize;
							//++nItem;
						}
					}
				}
				dwOffset += dwItemSize*MAX_FAXGROUP_ITEM;
				dwLeft -= 768;
			}
		}
		break;

	default: return false;
	}

	size = dwOffset;

	return true;
}

bool IODevice::decode(const char* buffer, DWORD& size, int type, void *arg)
{
	EntryModel *model = (EntryModel *)arg;
	if (!model)
		return false;

	DWORD left = size;
	unsigned long offset = 0;
	unsigned long entrySize = 0;

	if (type == ABE::FormCombined)
	{
		entrySize = sizeof(tFlashSrv_Book_Data_Content);
	}
	else if (type == ABE::FormFaxGroup)
	{
		entrySize = sizeof(tFlashSrv_Book_Group_Content);
	}
	else
	{
		return false;
	}

	int index = 0;
	while( left >= entrySize )
	{
		// check if data is valid
		if (((unsigned char)buffer[offset]) != 0xFF && ((unsigned char)buffer[offset]) != 0x0)
		{
			switch( type )
			{
			case ABE::FormCombined:
				{// contact data: id,name,fax
					if (buffer != NULL && size >= entrySize)
					{
						cout <<  "No.\t" << "Index\t" << "Name\t" << "FAX\t" << "\tEmail" << endl;
						EntryItem *item = new EntryItem;
						if( decodeContact(buffer + offset, item) )
							model->appendEntry(item);
					}
				}
				break;
			case ABE::FormFaxGroup:
				{// group data: id,name,children id
					if (buffer != NULL && size >= entrySize)
					{
						EntryItem *item = new EntryItem;
						if( decodeGroup(buffer + offset, item, index) )
							model->appendEntry(item);
					}
				}
				break;
			default:
				break;;
			}
		}
		++index;
		size += entrySize;
		offset += entrySize;
		left -= entrySize;
	}

	return true;
}

bool IODevice::decodeGroup(const char *buffer, void *arg, int index)
{
	EntryItem *item = (EntryItem*)arg;
	if( !item )
		return false;

	tFlashSrv_Book_Group_Content* pGroupContent = (tFlashSrv_Book_Group_Content*)buffer;

	// Group Type
	string strId;
	if (index < MAX_FAXGROUP_ITEM &&  pGroupContent->GroupType == 1) // 1:ABE::FormFaxGroup
	{
		int id = index + 1;
		strId = Util::ToString(id);
	}
	else
	{
		return false;
	}

	// Name
	string name = "";
	if (pGroupContent->GroupNameLeng > 0)
		name.assign((char*)pGroupContent->GroupName, pGroupContent->GroupNameLeng);

	item->setType(ABE::GroupType);
	item->setData(strId, ABE::Role_Id);
	item->setData(name, ABE::Role_Name);

	// children
	BYTE temp;
	//WORD wID;
	string strIds;
	for (int i = 0; i < BOOK_MAX_GROUP_SDNO_SIZE; ++i)
	{
		if (pGroupContent->GroupMemberBitwise[i] != 0xFF)
		{
			temp = 0x80;
			for (int j = 0; j < 8; j++)
			{
				if (!(temp & pGroupContent->GroupMemberBitwise[i]))
				{
					int id = i * 8 + j + 1;

					//DWORD dwKey = MAKELONG(wID, 1);
					//DWORD dwChild = dwKey & 0x10000;
					//dwChild != 0x10000 ? dwChild = 1 << 16 | dwKey : dwChild = dwKey;

					item->addChild(id);
				}
				temp >>= 1;
			}//end for
		}
	} //! end for
	
	return true;
}

bool IODevice::decodeContact(const char *buffer, void *arg)
{
	EntryItem *item = (EntryItem*)arg;
	if( !item )
		return false;

	tFlashSrv_Book_Data_Content* pDataContent = (tFlashSrv_Book_Data_Content*)buffer;
	// ID
	int index = (int)pDataContent->DataIndex;
	// Name
	string name = "";
	if (pDataContent->NameLeng > 0)
		name.assign((char*)pDataContent->Name, pDataContent->NameLeng);
	// FAX
	string fax = "";
	if (pDataContent->PNumberLeng > 0)
		fax.assign((char*)pDataContent->PNumber, pDataContent->PNumberLeng);
// 		{
// 			fax = new char[pDataContent->PNumberLeng + 1];
// 			memcpy(fax, pDataContent->PNumber, pDataContent->PNumberLeng);
// 			fax[pDataContent->PNumberLeng] = '\0';
// 		}
	// Email
	string email = "";
	if (pDataContent->EAddressLeng > 0)
		email.assign((char*)pDataContent->EAddress, pDataContent->EAddressLeng);

	cout << index << "\t" << name << "\t" << fax << "\t\t" << email << endl;

	string strId = Util::ToString(index);
	item->setData(strId, ABE::Role_Id);
	item->setData(name, ABE::Role_Name);
	item->setData(fax, ABE::Role_Fax);
	// other default sections
	item->setData( "HOST", ABE::Role_Source );
	item->setData( "On", ABE::Role_ECM );
	item->setData( "33600 bps", ABE::Role_Speed);

	return true;
}

char * IODevice::encodeGroup(void *buffer, DWORD &size, void *data)
{
	if (buffer == NULL  || size < sizeof(tFlashSrv_Book_Group_Content))
		return NULL;

	EntryItem *item = (EntryItem*)data;
	if( !item )
		return NULL;

	size = 0;

	tFlashSrv_Book_Group_Content* pGroupContent = (tFlashSrv_Book_Group_Content*)buffer;
	memset((void*)pGroupContent, 0xFF, sizeof(tFlashSrv_Book_Group_Content));

	// Group Type
	pGroupContent->GroupType = 1;

	// name
	string name = item->data(ABE::Role_Name);
	pGroupContent->GroupNameLeng = name.length() > BOOK_MAX_GROUPNAME_SIZE ? BOOK_MAX_GROUPNAME_SIZE : name.length();
	memcpy(pGroupContent->GroupName, name.c_str(), pGroupContent->GroupNameLeng);

	//children
	vector<int> children = item->children();
	vector<int>::iterator it = children.begin();
	memset((void*)pGroupContent->GroupMemberBitwise, 0xFF, BOOK_MAX_GROUP_SDNO_SIZE);
	for (; it != children.end(); ++it)
	{
		int id = *it;
		int index = (id - 1) / 8;
		BYTE temp = 0x80;
		temp >>= ((id - 1) % 8);
		temp = ~temp;
		if (index < BOOK_MAX_GROUP_SDNO_SIZE)
		{
			pGroupContent->GroupMemberBitwise[index] &= temp;
		}
	}

	size = sizeof(tFlashSrv_Book_Group_Content);

	return (char*)buffer;
}

char * IODevice::encodeContact(void *buffer, DWORD &size, void *data)
{
	if (size < sizeof(tFlashSrv_Book_Data_Content))
		return NULL;

	EntryItem *item = (EntryItem*)data;
	if( !item )
		return NULL;

	tFlashSrv_Book_Data_Content* pDataContent = (tFlashSrv_Book_Data_Content*)buffer;
	memset((void*)pDataContent, 0xFF, sizeof(tFlashSrv_Book_Data_Content));

	// id
	string strId = item->data(ABE::Role_Id);
	int id = Util::ToNumber(strId);
	pDataContent->DataIndex = id;

	// name
	string name = item->data(ABE::Role_Name);
	//pDataContent->NameLeng = ABE::NameMaxLength;
	pDataContent->NameLeng = name.length() > BOOK_MAX_NAME_SIZE ? BOOK_MAX_NAME_SIZE : name.length();
	memcpy(pDataContent->Name, name.c_str(), pDataContent->NameLeng);
	//name.copy(pDataContent->Name, name.length());

	// fax
	string fax = item->data(ABE::Role_Fax);
	size_t len = fax.length();
	pDataContent->PNumberLeng = len > BOOK_MAX_PHONENUMBER_SIZE ? BOOK_MAX_PHONENUMBER_SIZE : len;
	memcpy(pDataContent->PNumber, fax.c_str(), pDataContent->PNumberLeng);
	//fax.copy(pDataContent->PNumber, len);
	
	// No email section

	size = sizeof(tFlashSrv_Book_Data_Content);

	return (char*)buffer;
}


