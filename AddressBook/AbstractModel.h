#pragma once
#include <string>
#include <map>

using namespace std;

class EntryItem;
/************************************************************************/
/*@brief The AbstractModel class provides maintaining all data.
 *@author Yuri Yang <yuri.yang@liteon.com> 2016-6-18
/************************************************************************/
class ADDRESS_BOOK_EXPORT AbstractModel
{
public:
	AbstractModel(void);
	virtual ~AbstractModel(void);

	/*!
		Returns the index position of the value "id"of entry and "type" item in the model.
		Returns -1 if no "id" matched.
		type(ABE::ContactType, ABE::GroupType)
	*/
	virtual int index(int id, int type) const = 0;

	/*! Returns the index position of the first occurrence of "item" in the model.*/
	virtual int index(const EntryItem *item) const = 0;

	/*! 
		Returns the number of entries(rows in the list) in the model.
	*/
	virtual int rowCount() const = 0;
	/*! 
		Returns the number of header(column in the list) in the model.
	*/
	virtual int columnCount() const = 0;

	/*! Returns the type of item at index position "index" in the model.*/
	virtual int itemType(int index) const = 0;
	/*!
		Set item "type" at index position "index".
		type(ABE::ContactType, ABE::GroupType)
	*/
	virtual void setItemType(int index, int type) = 0;

	/*! Returns the data stored under the given role for the item referred to by the index.*/
	virtual string data(int index, int role) const = 0;
	/*! Sets the role data for the item at index to value.*/
	virtual bool setData(int index, const string &value, int role);

	/*! Returns the data for the given role and section in the header.*/
	virtual string headerData(int section, int role) const;
	/*! Sets the data for the given role and section in the header to the value supplied.*/
	virtual bool setHeaderData(int section, const string &value, int role);

	/*!
		Inserts "count" rows into the model before the given "row(index)".
		Items in the new row will be represented by the model index(containing item).
		If "row(index)" is 0, the rows are prepended(push front) to any existing rows.
		If row is rowCount(), the rows are appended(push back) to any existing rows.
		Returns true if the rows were successfully inserted; otherwise returns false.
		Note:The base class implementation of this function does nothing and returns false.
	*/
	virtual bool insertRows(int index, int count);
	/*!
		Removes "count" rows starting with the given "row(index)" to the model.
		Returns true if the rows were successfully removed; otherwise returns false.
		Note:The base class implementation does nothing and returns false.
	*/
	virtual bool removeRows(int index, int count);

	/*! Appends a "item".*/
	virtual void appendEntry(EntryItem *item) = 0;
	/*! Inserts a item at "index" position*/
	virtual bool insertEntry(int index, EntryItem *item)  = 0;
	/*! Removes item for the given "index" position at. The item that was in the row(index position) was deleted.*/
	virtual bool removeAt(int index)  = 0;
	/*! Removes the given "item". The "item" that was in the row was deleted.*/
	virtual bool removeEntry(EntryItem *item)  = 0;

	/*! 
		Returns entry "item"  for the given "id" of entry.
		Returns the index position of the value "id"of entry, and "type" of entry  item in the model.
		Note: the "id" of contact entry is different from id of group entry.
		type(ABE::ContactType, ABE::GroupType)
	*/
	virtual EntryItem *itemFromId(int id, int type) const = 0;
	/*! Returns entry item for the given "index" of entry.*/
	virtual EntryItem *itemFromIndex(int index) const = 0;

	/*! Removes all entry items and the items will be deleted.*/
	virtual void removeAll();
	/*! Removes all occurrences of "item" in the model 
		and returns the number of entries removed.
	*/
	virtual int removeAll(EntryItem *item);
	/*! 
		Returns the numbers of duplicates be removed.
		Returns zero if none duplicate item,otherwise return no-zero.
	*/
	virtual int removeDuplicates();

	/*! 
		Returns true if size() is equal to zero in the model,
		otherwise returns false.
	*/
	virtual bool isEmpty() const;
	
	/*!
		If the value "id" of entry with "type" has existed in the model.
		Returns true if "id" has existed.
	*/
	virtual bool hasId(int id, int type) const;

	/*! Returns the data stored in the items by the index.*/
	virtual map<int, string> itemDatas(int index) const;

	/*! Sets the data for the item at index to values.*/
	virtual bool setItemDatas(int index, const  map<int, string> &roles);
};
