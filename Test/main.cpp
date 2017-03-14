// AddressBookConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "../AddressBook/AddressBook.h"
#include "../AddressBook/EntryItem.h"
#include "../AddressBook/EntryModel.h"
#include "../AddressBook/PrinterDevice.h"

#include <hash_map>
#include <map>

using namespace ABE;
using namespace stdext;

void test_entry();
void test_model();
void test_device();
void test_write();
void test_read();
void decode(char *data, DWORD dwSize);
void test_Find();

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "[Test begin]\n";
	cout << "----------------------------------------------------------\n";


	//test_entry();
	//test_model();
	//test_device();
	//test_write();
	//test_read();
	test_Find();

	cout << "----------------------------------------------------------\n";
	cout << "[Test end]\n";

	return 0;
}

void test_entry()
{
	AddressBook book;

}

bool __stdcall readCallbackFn(void *buffer, DWORD size, int type, void *arg)
{
	cout << "[Callback function - read]:";
	char* data = reinterpret_cast<char*>(buffer);
	cout << "reply data size: " << size << " data: " << data << endl;
	
	//device.decode(data, size);

	return true;
}

void test_device()
{
	PrinterDevice device;

	const string printerName = "Dell Color MFP E525w Fax";

	device.setType(IODevice::USBDevice);
	if( !device.open(printerName) )
	{
		cout << "Error: Open device failed, cannot open device: \"" << printerName << "\".\n";
		return;
	}

	// login
	int needed = device.needLogin();
	if( needed == 1)
	{
		cout << "Login to device ...\n";
		if( device.login("0000") != IODevice::AUE_OK )
		{
			cout << "Error: Login device failed.\n";
			return;
		}

		cout << "Login to device succeed.\n";
	}
	else if (needed == -1)
	{
		cout << "Error: Request to login device failed.\n";
		return;
	}

	// read data
	cout << "Ready reading data from device ...\n";
	if( device.read(FormCombined, readCallbackFn, 0) != -1)
	{
		cout << "Read data done.\n";
	}
	device.close();
}

void test_model()
{
	EntryModel model;
	model.insertRows(0, 4);
	for (int i = 0; i < 4; ++i)
	{
		char buf[10];
		sprintf_s(buf, "%d", i);
		model.setData(i, string(buf), ABE::Role_Id);
		model.setData(i, "Lily" + string(buf), ABE::Role_Name);
		model.setData(i, "Fax 020-34535"+ string(buf), ABE::Role_Fax);
		model.setData(i, "Liteon", ABE::Role_Company);
		if (i == 2)
		{
			model.setItemType(i, ABE::GroupType);
		}
	}

	model.insertRows(3, 1);
	model.setData(3, "3-3", ABE::Role_Id);
	model.setData(3, "Alen", ABE::Role_Name);
	model.setData(3, "Fax 020-34535", ABE::Role_Fax);
	model.setData(3, "Liteon", ABE::Role_Company);

	EntryItem *item = new EntryItem("Alen", "Fax 020-34535", "Liteon");
	item->setData("3-3", ABE::Role_Id);
	model.insertEntry(4, item);
	//cout << model.itemOf(3) << endl;

	for (int i  = 0; i < model.rowCount(); ++i)
	{
		cout << model.itemFromIndex(i) << endl;
	}
	cout << "removed: \n";
	model.removeRows(2, 0);
	for (int i  = 0; i < model.rowCount(); ++i)
	{
		cout << model.itemFromIndex(i) << endl;
	}
}

void test_write()
{
	EntryItem *item = new EntryItem;
	item->setData("3", ABE::Role_Id);
	item->setData("ÀòÀò", ABE::Role_Name);
	item->setData("020-34535", ABE::Role_Fax);
	item->setData("Liteon", ABE::Role_Company);
	item->setData("33600pds", ABE::Role_Speed);
	item->setData("On", ABE::Role_ECM);
	cout << item << endl;

	cout << "Writing....\n";
	

}

void test_read()
{

}

void decode(char *data, DWORD dwSize)
{
	cout << "decode data ...\n";


}

void test_Find()
{
	EntryModel model;

	for (int i = 0; i < 10; ++i)
	{
		char buf[10];
		sprintf_s(buf, "%d", i);

		EntryItem *item = new EntryItem;
		item->setData( string(buf), ABE::Role_Id);

		string name = "Lily";
		string fax = "020-34535";

		if( i % 2 == 0)
		{
			name += "A";
			fax += "88";
		}

		if( i == 3)
			fax += "9";

		item->setData( name, ABE::Role_Name);
		item->setData( fax, ABE::Role_Fax);
		item->setData( "Liteon", ABE::Role_Company);
		model.appendEntry(item);
	}

	for (unsigned int i = 0; i < model.rowCount(); ++i)
	{
		cout << model.itemFromIndex(i) << "\n";
	}

// 	for (int i = 0; i < model.rowCount(); ++i)
// 	{
// 		bool ok = model.find(model.data(0, ABE::Role_Name), model.data(0, ABE::Role_Fax));
// 		if (ok)
// 		{
// 			cout << "found:"<< "\n";
// 		}
// 	}

	cout << "================================\n";

	// check item from id
	cout << model.itemFromId(3, ABE::ContactType) << endl;

	// check id
	if (model.hasId(100, ABE::ContactType))
	{
		cout << "Id has existed.\n";
	}
	else
	{
		cout << "Id no existed.\n";
	}

	// check index from id/item
	cout << "Index from id: " << model.index(5, ABE::ContactType) << endl;
	cout << "Index from Item: " << model.index(model.itemFromId(9, ABE::ContactType)) << endl;

	// remove duplicate
	model.removeDuplicates();
	for (unsigned int i = 0; i < model.rowCount(); ++i)
	{
		cout << model.itemFromIndex(i) << "\n";
	}

}

