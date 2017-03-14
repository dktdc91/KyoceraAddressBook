#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H
#include "utility.h"
#include <string>

using namespace std;

#ifdef _DEBUG
	#define _UNUSED(x)
#else
	#define _UNUSED(x) x
#endif

#define ADDRESS_BOOK_LIBRARY
#define callback __stdcall

#if defined ADDRESS_BOOK_LIBRARY
#  define ADDRESS_BOOK_EXPORT _declspec(dllexport)
#else
#  define ADDRESS_BOOK_EXPORT _declspec(dllimport)
#endif

// address book messages
#define WM_ADDRBOOK_OPEN			(WM_USER+100)
#define WM_ADDRBOOK_RERLOAD			(WM_USER+101)
#define WM_ADDRBOOK_CLEAR			(WM_USER+102)
#define WM_ADDRBOOK_SAVE			(WM_USER+103)
#define WM_ADDRBOOK_SAVEAS			(WM_USER+104)
#define WM_ADDRBOOK_ADD				(WM_USER+105)
#define WM_ADDRBOOK_DELETE			(WM_USER+106)
#define WM_ADDRBOOK_EDIT			(WM_USER+107)


#ifndef FormAddr
#define FormAddr( ch0, ch1, ch2, ch3 )				\
	( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |	\
	( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )
#endif

class EntryItem;
class AbstractModel;
class AddressBookPrivate;

namespace ABE
{

	//data type
	enum EntryType
	{
		UnkownType = -1,
		HeaderType = 0,
		ContactType,
		GroupType
	};

	// properties/data
	enum EntryRole
	{
		Role_Unuse = 0,
		Role_Id,
		Role_Type,
		Role_Name,
		Role_Source,
		Role_Fax,
		Role_Company,
		Role_Department,
		Role_ECM,
		Role_Speed,
		Role_User
	};

	enum FormType
	{
		FormCombined = FormAddr('F', 'C', 'M', 'B'),
		FormFaxGroup = FormAddr('F', 'F', 'X', 'G'),
		FormEmailGroup = FormAddr('F', 'E', 'M', 'G'),
		FormServer = FormAddr('F', 'S', 'R', 'V'),
		FormPCFax = FormAddr('F', 'P', 'C', 'F'),
		FormPCFaxGroup = FormAddr('F', 'P', 'F', 'G'),
		FormEmailSetting = FormAddr('M', 'A', 'I', 'L')
	};

	enum Orientation
	{
		Horizontal,
		Vertical
	};
} // end namespace

/************************************************************************/
/*@brief: The AddressBook class provides interfaces to process all data.
 *@author: Yuri Yang <yuri.yang@liteon.com>
/************************************************************************/
class ADDRESS_BOOK_EXPORT AddressBook
{
public:
	AddressBook();
	virtual ~AddressBook();

	// model
	/*! Returns the default source model*/
	AbstractModel *sourceModel() const;
	/*! Specified the source model, if no source model will be placeholder default model*/
	void setSourceModel(AbstractModel *model);

	/*!	Returns the header the given "role" for list control column in the address book*/
	string modelHeader(int role) const;
	/*! Set/Changed the header of "role" with given "value" for list control column*/
	void setModelHeader(const string &value, int role);

	// address book file operation members
	/*! Create a new address book:just clear all entries.*/
	void resetBook();

	/*! Opens existing address book file with the given "filename"*/
	bool openBook(const string &filename);

	/*! Saves address book file to the given "filename"*/
	bool saveBook(const string &filename);

	/*! Save as address book file to the given "filename"*/
	bool saveAsBook(const string &filename);

	// data
	/*! Inserts entry "item" at the end of the address book*/
	void appendEntry(EntryItem *item);

	/*!
		Inserts entry "item" at index position "index" in the address book.
		If "index" is 0 or less 0, the "item" is prepended(at begin) to the address book.
		If "index" is size(), the "item" is appended to the address book.
	*/
	bool insertEntry(int index, EntryItem *item);

	/*! 
		Removes the item at index position "index". 
		"index" must be a valid index position in the address book (i.e., 0 <= index < size()).
	 */
	bool removeAt(int index);

	/*! 
		Removes the first occurrence of "item" in the address book
		and returns true on success;otherwise returns false.
	*/
	bool removeEntry(EntryItem *item);

	/*!
		Removes all occurrences of entries in the address book.
	*/
	void removeAll();

	/*! Removes all occurrences of "item" in the address book 
		and returns the number of entries removed.
		//Requires compare the name of item and fax of item.
		//bool removeDuplicates()
	*/
	int removeAll(EntryItem *item);

	/*! 
		Returns the numbers of duplicates be removed.
		Returns zero if none duplicate item,otherwise return no-zero.
	*/
	int removeDuplicates();

	/*! 
		Returns the count of occurrences of "type" in the address book,
		Returns 0 if no "type" matched.
		type(ABE::ContactType, ABE::GroupType)
	*/
	int countOf(int type) const;

	/*! 
		Returns the number of entries in the address book.
		This is effectively the same as size().
	*/
	int count() const;

	/*! 
		Returns the number of entries in the address book.
		This is effectively the same as count().
	*/
	int size() const;

	/*! 
		Returns true if size() is equal to zero in the address book,
		otherwise returns false.
	*/
	bool isEmpty() const;

	/*! Returns the type of entry at index position "index" in the address book.*/
	int entryType(int index) const;

	/*!
		Set entry "type" at index position "index".
		type(ABE::ContactType, ABE::GroupType)
	*/
	void setEntryType(int index, int type);

	/*!
		Returns the "type" of entry item that occupies the given "id" in the address book.
		type(ABE::ContactType, ABE::GroupType)
	*/
	EntryItem *entryOf(int id, int type);

	/*!
		Returns the entry at index position "index" in the address book.
		"index" must be a valid index position in the address book (i.e., 0 <= index < size()).
	*/
	EntryItem *entryOf(int index);

	/*! Returns the index position of the first occurrence of "item" in the address book,*/
	int indexOf(EntryItem *item);

	/*!
		Returns the index position of the value "id" of entry in the address book.
		Returns -1 if no "id" matched.
	*/
	int indexOf(int id, int type) const;

	/*!
		If the value "id" of entry has existed in the address book.
		Returns true if "id" has existed.
	*/
	bool hasId(int id, int type) const;
	
	// USB device operation members
	/*! Returns true if connected printer device with the "printerName" */
	bool connectToDevice(const string &printerName);
	/*! 
		Returns true if this AddressBook object is connected.
		If it isn't connected, you need to call the connectToDevice again.
	*/
	bool isConnected();

	/*! Disconnects the AddressBook from the transport device.*/
	void disconnectDevice();
	/*!
		Read data from the connected printer device.
		If read data succeed return true, otherwise return false.
	*/
	bool loadFromDevice();
	/*!
		Write data to the connected printer device.
		If write data succeed return true, otherwise return false.
	*/
	bool saveToDevice();

private:
	AddressBookPrivate *d;
};

#endif