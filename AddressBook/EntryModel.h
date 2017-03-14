#pragma once
#include "AbstractModel.h"

class EntryModelPrivate;
class EntryItem;
/************************************************************************/
/*@brief: The EntryModel derive class provides maintaining all data.it need
	a view class to show data.in addition,you can define a controller class
	to handle model data and communicating with view.
 *@author: Yuri Yang <yuri.yang@liteon.com>
/************************************************************************/
class EntryModel : public AbstractModel
{
public:
	EntryModel(void);
	~EntryModel(void);

	/*! returns entry index(row in) in model. id is entry's id property
	 * note: contact entry id value is different from group entry id value 
	*/
	int index(int id, int type) const;
	int index(const EntryItem *item) const;

	/*! returns total entries*/
	int rowCount() const;
	/*! returns column count,means header columns count*/
	int columnCount() const;

	/*! returns index item type(eg:Contact Type, Group Type)*/
	int itemType(int index) const;
	void setItemType(int index, int type);

	string data(int index, int role) const;
	bool setData(int index, const string &data, int role);

	/*! header for UI list control's report header. section means header column*/
	string headerData(int section, int role) const;
	bool setHeaderData(int section, const string &value, int role);

	/*! insert row(s) from index position, and insert row count*/
	bool insertRows(int index, int count);
	bool removeRows(int index, int count);

	/*! add item to back*/
	void appendEntry(EntryItem *item);
	/*! insert item at index*/
	bool insertEntry(int index, EntryItem *item);
	bool removeAt(int index);
	bool removeEntry(EntryItem *item);

	/*! returns entry item for the given entry's id
	 * note: contact entry id value is different from group entry id value
	*/
	EntryItem *itemFromId(int id, int type) const;
	EntryItem *itemFromIndex(int index) const;

	bool isEmpty() const;

	// do id number auto order...
	// note: contact entry id value is different from group entry id value
	bool hasId(int id, int type) const;
	bool hasId(const string &id,  int type) const;

	/*! Removes all entry items and the items will be deleted.*/
	virtual void removeAll();
	/*! Removes all occurrences of "item" in the model 
		and returns the number of entries removed.
	*/
	virtual int removeAll(EntryItem *item);

	/*! compare name and fax*/
	int removeDuplicates();

private:
	EntryModelPrivate *d;
};

/************************************************************************/
/* Use to sort algorithm                                               */
/************************************************************************/
/*! just compare name and fax, the unique item to find*/
inline bool equalFind(const EntryItem *l, const EntryItem *r);
/*! sort by item name*/
inline bool sortByName(const EntryItem *l, const EntryItem *r);
/*! sort by item id*/
inline bool sortById(const EntryItem *l, const EntryItem *r);
