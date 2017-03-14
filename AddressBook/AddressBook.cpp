// AddressBook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AddressBook.h"
#include "EntryModel.h"
#include "EntryItem.h"
#include "PrinterDevice.h"
#include "AddressFile.h"

#include <signal.h>

#include <conio.h>
#include <iostream>

using namespace std;
using namespace ABE;


class AddressBookPrivate
{
public:
	AddressBookPrivate() : device(new PrinterDevice), model(new EntryModel)
	{
		device->setType(IODevice::USBDevice);
	}
	~AddressBookPrivate()
	{
		device->close();
		model->removeAll();
		delete model;
		delete device;
	}

	PrinterDevice *device;
	AbstractModel *model;
};


AddressBook::AddressBook() : d(new AddressBookPrivate)
{

}

AddressBook::~AddressBook()
{
	if (d->device->isOpen())
		disconnectDevice();
	delete d;
}

AbstractModel *AddressBook::sourceModel() const
{
	return d->model;
}

void AddressBook::setSourceModel(AbstractModel *model)
{
	if( !model )
		return;
	if (d->model == model)
		return;
	
	d->model->removeAll();
	delete d->model;
	d->model = NULL;
	d->model = model;
}

std::string AddressBook::modelHeader(int role) const
{

	return d->model->headerData(0, role);
}

void AddressBook::setModelHeader(const string &value, int role)
{

	d->model->setHeaderData(0, value, role);
}

void AddressBook::resetBook()
{
	// just clear data
	d->model->removeAll();
}

bool AddressBook::openBook(const string &filename)
{
	d->model->removeAll();

	AddressFile file;
	if ( file.open(filename, AddressFile::ReadOnly) )
	{
		while( !file.eof() )
		{
			int type = ABE::UnkownType;
			EntryItem *item = new EntryItem;
			if (file.read(type, item) != -1)
			{
				int total = d->model->rowCount();
				if( d->model->insertEntry(total, item) )
				{
					// Send message to viewer update
					// do nothing
				}
			}
		}
		file.close();
		return true;
	}


	return false;
}

bool AddressBook::saveBook(const string &filename)
{
	AddressFile file;
	if ( file.open(filename, AddressFile::WriteOnly) )
	{
		// write header
		string head = "// This is KyoceraDocumentSolutions Fax AddressBook  Utility Data.\r\n";
		const char *data = (head.c_str());
		file.write(data, head.length());

		for (int i = 0; i < d->model->rowCount(); ++i)
		{
			EntryItem *item = d->model->itemFromIndex(i);
			if( item )
			{
				file.write(item->type(), item);
			}
		}

		file.close();
		return true;
	}

	return false;
}

bool AddressBook::saveAsBook(const string &filename)
{
	return saveBook(filename);
}

void AddressBook::appendEntry(EntryItem *item)
{
	d->model->appendEntry(item);
}

bool AddressBook::insertEntry(int index, EntryItem *item)
{
	return d->model->insertEntry(index, item);
}

bool AddressBook::removeAt(int index)
{
	return d->model->removeAt(index);
}

bool AddressBook::removeEntry(EntryItem *item)
{
	return d->model->removeEntry(item);
}

void AddressBook::removeAll()
{
	d->model->removeAll();
}

int AddressBook::removeAll(EntryItem *item)
{
	return d->model->removeAll(item);
}

int AddressBook::countOf(int type) const
{
	int count = 0;
	for (int i = 0; i < d->model->rowCount(); ++i)
	{
		EntryItem *item = d->model->itemFromIndex(type);
		if (item)
		{
			item->type() == type ? ++count : count;// else continue
		}
	}

	return count;
}


int AddressBook::removeDuplicates()
{
	return d->model->removeDuplicates();
}

int AddressBook::count() const
{
	return d->model->rowCount();
}

int AddressBook::size() const
{
	return d->model->rowCount();
}

int AddressBook::entryType(int index) const
{

	return d->model->itemType(index);
}

void AddressBook::setEntryType(int index, int type)
{
	d->model->setItemType(index, type);
}

EntryItem * AddressBook::entryOf(int id, int type)
{
	
	return d->model->itemFromId(id, type);
}

EntryItem * AddressBook::entryOf(int index)
{
	
	return d->model->itemFromIndex(index);
}

int AddressBook::indexOf(EntryItem *item)
{

	return d->model->index(item);
}

int AddressBook::indexOf(int id, int type) const
{
	return d->model->index(id, type);
}

bool AddressBook::hasId(int id, int type) const
{

	return d->model->hasId(id, type);
}

bool AddressBook::connectToDevice(const string &printerName)
{
	d->device->setType(IODevice::USBDevice);
	if (d->device->isOpen())
		d->device->close();

	if( !d->device->open(printerName) )
		return false;
	// login
	int needed = d->device->needLogin();
	if( needed == 1)
	{
		if( d->device->login("0000") != IODevice::AUE_OK )
		{
			d->device->close();
			return false;
		}
	}
	else if (needed == -1)
	{
		return false;
	}

	return true;
}

bool AddressBook::isConnected()
{
	return d->device->isOpen();
}

void AddressBook::disconnectDevice()
{
	d->device->close();
}

bool AddressBook::loadFromDevice()
{
	bool ok = false;
	// read data
	if( d->device->read(ABE::FormCombined, 0, d->model) != -1)
	{
		ok = true;
	}
	if( d->device->read(ABE::FormFaxGroup, 0, d->model) != -1)
	{
		ok = true;
	}

	return ok;
}

bool AddressBook::saveToDevice()
{
	bool ok = false;
	if(d->device->write(ABE::FormCombined, 0, d->model) != -1)
		ok = true;
	if (d->device->write(ABE::FormFaxGroup, 0, d->model) != -1)
		ok = true;

	return ok;
}

bool AddressBook::isEmpty() const
{
	return d->model->isEmpty();
}
