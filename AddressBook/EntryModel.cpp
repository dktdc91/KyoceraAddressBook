#include "StdAfx.h"
#include "EntryModel.h"
#include "EntryItem.h"
#include "utility.h"
#include <algorithm>


inline bool equalFind(const EntryItem *l, const EntryItem *r)
{
	cout << "uniqueFind\n";

	if (l == r)
		return false;

	if (l->type() == r->type())
	{
		if (l->type() == ABE::ContactType)
		{
			// just compare name and fax
			string l_name = l->data(ABE::Role_Name);
			string r_name = r->data(ABE::Role_Name);
			string l_fax = l->data(ABE::Role_Fax);
			string r_fax = r->data(ABE::Role_Fax);

			return (l_name.compare(r_name) == 0 && l_fax.compare(r_fax) == 0);
		}
		else if (l->type() == ABE::GroupType)
		{
			string l_name = l->data(ABE::Role_Name);
			string r_name = r->data(ABE::Role_Name);
			return l_name.compare(r_name) == 0;
		}
	}

	return false;
}

inline bool sortByName(const EntryItem *l, const EntryItem *r)
{
	string l_name = l->data(ABE::Role_Name);
	string r_name = r->data(ABE::Role_Name);
	string l_fax = l->data(ABE::Role_Fax);
	string r_fax = r->data(ABE::Role_Fax);

	if(l_name.compare(r_name) == 0)
	{
		return l_fax.compare(r_fax) < 0;
	}

	if (l_name.compare(r_name) < 0)
	{
		return true;
	}

	return false;
}

inline bool sortById(const EntryItem *l, const EntryItem *r)
{
	if (l->type() == r->type())
	{
		string id1 = l->data(ABE::Role_Id);
		string id2 = r->data(ABE::Role_Id);
		int num1 = Util::ToNumber(id1);
		int num2 = Util::ToNumber(id2);

		return num1 < num2;
	}

	return false;
}

/*******************************************************************************/
/* @brief:The class FindItemX provides compare algorithm for find_if function  */
/* @auth:Yuri Yang <yuri.yang@liteon.com>                                      */
/* @date:2016-7-29                                                             */
/*******************************************************************************/
class FindItemX
{
public:
	enum ArgType
	{
		ID,
 		ITEM,
		NAME,
		NAME_FAX
	};
	FindItemX(ArgType type, int id, int itemType = ABE::ContactType):
		m_id(-1), m_argType(type), m_itemType(itemType) { m_id = id; }

	FindItemX(ArgType type, EntryItem *item, int itemType = ABE::ContactType) : m_item(item), m_argType(type), m_itemType(itemType) { }

	FindItemX(ArgType type, const string &name, int itemType = ABE::ContactType) :
		m_name(name), m_argType(type), m_itemType(itemType) {}

	FindItemX(ArgType type, const string &name, const string &fax, int itemType = ABE::ContactType) :
		m_name(name), m_fax(fax), m_argType(type), m_itemType(itemType) {}

	bool operator()(EntryItem *item)
	{
		if (m_itemType == item->type())
		{
			switch( m_argType )
			{
			case ID:
				return item->data(ABE::Role_Id) == Util::ToString(m_id);

			case ITEM:
				{
					if (m_item->type() == ABE::ContactType)
					{
						// just compare name and fax
						string l_name = m_item->data(ABE::Role_Name);
						string r_name = item->data(ABE::Role_Name);
						string l_fax = m_item->data(ABE::Role_Fax);
						string r_fax = item->data(ABE::Role_Fax);

						return (l_name.compare(r_name) == 0 && l_fax.compare(r_fax) == 0);
					}
					else if (m_item->type() == ABE::GroupType)
					{
						string l_name = m_item->data(ABE::Role_Name);
						string r_name = item->data(ABE::Role_Name);

						return l_name.compare(r_name) == 0;
					}
				}
				return item == m_item;

			case NAME:
				return m_name.compare(item->data(ABE::Role_Name)) == 0;

			case NAME_FAX:
				return (m_name.compare(item->data(ABE::Role_Name)) == 0) && (m_fax.compare(item->data(ABE::Role_Fax)) == 0);

			default:break;
			}

		}

		return false;
	}

private:
	int m_itemType;
	ArgType m_argType;
	int m_id;
	EntryItem *m_item;
	string m_name;
	string m_fax;
};

/************************************************************************/
/*The EntryModelPrivate class                                           */
/************************************************************************/
class EntryModelPrivate
{
public:
	EntryModelPrivate()
		: headerItem(new EntryItem(ABE::HeaderType)), entries(new vector<EntryItem*>())
	{
	}
	virtual ~EntryModelPrivate()
	{
	}

	vector<EntryItem*> *entries;
	EntryItem *headerItem;	// for list view header
	//EntryItem *root;
};

/************************************************************************/
/*The EntryModel class                                                  */
/************************************************************************/
EntryModel::EntryModel(void)
	: d(new EntryModelPrivate)
{
}

EntryModel::~EntryModel(void)
{
	cout << "[~EntryModel]\n";
	this->removeAll();
}

int EntryModel::rowCount() const
{
	return d->entries->size();
}

int EntryModel::columnCount() const
{
	if (d->headerItem->dataCount() != 0)
	{
		return d->headerItem->dataCount();
	}
	return 1;
}

int EntryModel::itemType(int index) const
{
	if( d->entries->empty() )
		return ABE::UnkownType;

	if ( index < 0 || (unsigned int)index >= d->entries->size() )
		return ABE::UnkownType;

	if (!d->entries->at(index))
		return ABE::UnkownType;

	return d->entries->at(index)->type();
}

void EntryModel::setItemType(int index, int type)
{
	if ( index < 0 || (unsigned int)index >= d->entries->size() )
		return;

	d->entries->at(index)->setType(type);
}

std::string EntryModel::data(int index, int role) const
{
	if( d->entries->empty() )
		return string();

	if ( index < 0 || (unsigned int)index >= d->entries->size() )
		return string();

	return d->entries->at(index)->data(role);
}

bool EntryModel::setData(int index, const string &data, int role)
{
	if ( index < 0 || (unsigned int)index >= d->entries->size() )
		return false;

	d->entries->at(index)->setData(data, role);
	return true;
}

std::string EntryModel::headerData(int section, int role) const
{
	_UNUSED(section);

	if (d->headerItem && d->headerItem->dataCount() != 0)
	{
		return d->headerItem->data(role);
	}

	return AbstractModel::headerData(section, role);
}

bool EntryModel::setHeaderData(int section, const string &value, int role)
{
	_UNUSED(section);

	if (d->headerItem)
	{
		d->headerItem->setData(value, role);
		return true;
	}
	
	return false;
}

bool EntryModel::insertRows(int index, int count)
{
	// if insert into the head?
	if (index < 0 )
		return false;

	// insert into tail
	if ( (unsigned int)index >= d->entries->size())
	{
		// get last (reverse) iterator pos and convert to (forward) iterator
		vector<EntryItem*>::reverse_iterator rlast = d->entries->rbegin();
		vector<EntryItem*>::iterator last = --(rlast.base());
		EntryItem *item = new EntryItem;
		d->entries->insert(last, count, item);
	}
	else
	{
		// nice(Yuri)
		vector<EntryItem*>::const_iterator it = d->entries->begin() + index;
		EntryItem *item = new EntryItem;
		d->entries->insert(it, count, item);
		/*
		// what the hell is the code (Yuri)
		int pos = 0;
		for (; it != d->entries->end();)
		{
			if ( pos == index )
			{
				for (int i = 0; i < count; ++i)
				{
					EntryItem *item = new EntryItem;
					d->entries->insert(it, item);
				}
				break;
			}
			else
			{
				++pos;
				++it;
			}
		}*/
	}

	return true;
}

bool EntryModel::removeRows(int index, int count)
{
	if ( (index < 0 || (unsigned int)index >= d->entries->size())
		&& count > 0)
	{
		return false;
	}

	size_t start = index;
	size_t end = index + count;
	if( end > d->entries->size())
		end = d->entries->size();

	vector<EntryItem*>::iterator it1 = d->entries->begin() + start;
	vector<EntryItem*>::iterator it2 = d->entries->begin() + end;

	if( distance( it1, it2 ) <= count )
	{
		cout << "[EntryModel]: Removes item distance : " << distance( it1, it2 ) << endl;
		int removed = 0;
		while(it1 != d->entries->end() && removed < count  )
		{
			delete *it1;
			it1 = d->entries->erase(it1);
			++removed;
		}
	}

	return true;
}

void EntryModel::appendEntry(EntryItem *item)
{
	if (!item)
		return;
	d->entries->push_back(item);
}

bool EntryModel::insertEntry(int index, EntryItem *item)
{
	if (!item)
		return false;

	if( index <= 0 )
	{
		d->entries->insert(d->entries->begin(), item);
	}
	else if ( (unsigned int)index >= d->entries->size() )
	{
		d->entries->push_back(item);
	}
	else
	{
		vector<EntryItem*>::iterator it = d->entries->begin() + index;
		d->entries->insert(it, 1, item);
	}
	return true;
}

bool EntryModel::removeAt(int index)
{
	if( index < 0 || (unsigned int)index >= d->entries->size() )
		return false;

	vector<EntryItem*>::iterator it = d->entries->begin() + index;
	delete *it;
	d->entries->erase(it);

	return true;
}

bool EntryModel::removeEntry(EntryItem *item)
{
	if ( !item || d->entries->empty() )
	{
		return false;
	}

	vector<EntryItem*>::iterator result = std::find(d->entries->begin(), d->entries->end(), item);
	if (result != d->entries->end())
	{
		delete *result;
		d->entries->erase(result);
		return true;
	}

	return false;
}

EntryItem * EntryModel::itemFromId(int id, int type) const
{
	if( d->entries->empty() )
		return NULL;

	EntryItem *item = NULL;
	vector<EntryItem*>::const_iterator result = std::find_if( d->entries->begin(), d->entries->end(), FindItemX( FindItemX::ID, id, type) );
	if (result != d->entries->end())
		item = *result;

	return item;
}

EntryItem * EntryModel::itemFromIndex(int index) const
{
	if( index < 0 || (unsigned int)index >= d->entries->size() )
		return NULL;

	return d->entries->at(index);
}

bool EntryModel::isEmpty() const
{
	return d->entries->empty();
}

int EntryModel::index(int id, int type) const
{
	int idx = -1;
	// more than faster
	// note: in distance() cannot pass const_iterator
	// (Yuri)
	vector<EntryItem*>::iterator result = std::find_if( d->entries->begin(), d->entries->end(), FindItemX(FindItemX::ID, id, type) );
	if(result != d->entries->end())
	{
		idx = std::distance(d->entries->begin(), result);
	}
	/*
	// fxxk this ugly code ...too slowly...(Yuri)
	vector<EntryItem*>::iterator it = d->entries->begin();
	int idx = -1;
	for (; it != d->entries->end(); ++it)
	{
		if ( (*it)->data(ABE::Role_Id) == strId )
		{
			idx = std::distance(d->entries->begin(), it);
			break;
		}
	}
	*/
	return idx;
}

int EntryModel::index(const EntryItem *item) const
{
	vector<EntryItem*>::iterator it = std::find(d->entries->begin(), d->entries->end(), item);
	if (it != d->entries->end())
	{
		int idx = std::distance(d->entries->begin(), it);
		return idx;
	}
	return -1;
}

bool EntryModel::hasId(int id, int type) const
{
	/*
	// fxxk the ugly code. ...too slowly ...
	vector<EntryItem*>::const_iterator it = d->entries->begin();
	for (; it != d->entries->end(); ++it)
	{
		int itemType = (*it)->type();
		if ( itemType != type )
			continue;

		string strId = (*it)->data(ABE::Role_Id);
		// string to int(using standard c++ function, don't using VC atoi())
		/*stringstream ss;
		ss << strId;
		int nId;
		ss >> nId;*/
		/*int nId = Util::ToNumber(strId);

		if (nId == id )
			return true;
	}
	*/
	// nice code, more than faster(Yuri)
	vector<EntryItem*>::const_iterator first = d->entries->begin();
	vector<EntryItem*>::const_iterator last = d->entries->end();
	vector<EntryItem*>::const_iterator result = std::find_if( first, last, FindItemX(FindItemX::ID, id, type) );
	if(result != last)
		return true;
	
	return false;
}

bool EntryModel::hasId(const string &id, int type) const
{
	int nId = Util::ToNumber(id);
	return hasId(nId, type);
}

void EntryModel::removeAll()
{
	if (d->entries != NULL)
	{
		vector<EntryItem*>::iterator it = d->entries->begin();
		while( it != d->entries->end() )
		{
			delete (*it);
			it = d->entries->erase(it);
		}
		d->entries->clear();
	}
}

int EntryModel::removeAll(EntryItem *item)
{
	vector<EntryItem*>::iterator first = d->entries->begin();
	vector<EntryItem*>::iterator curIt = first;

	int count = 0;
	int type = item->type();
	string name = item->data(ABE::Role_Name);
	string fax = item->data(ABE::Role_Fax);

	while ( first != d->entries->end() )
	{
		if ( type == (*first)->type())
		{
			if ( type == ABE::ContactType)
			{
				string r_name = (*first)->data(ABE::Role_Name);
				string r_fax = (*first)->data(ABE::Role_Fax);
				if ( name.compare(r_name) == 0 && fax.compare(r_fax) == 0 )
				{
					++count;
					delete *first;
					first = d->entries->erase(first);
					continue;
				}
			}
			else if ( type == ABE::GroupType)
			{
				string r_name = (*first)->data(ABE::Role_Name);
				if( name.compare(r_name) == 0 )
				{
					++count;
					delete *first;
					first = d->entries->erase(first);
					continue;
				}
			}
		}

		++first;
	}

	return count;
}

int EntryModel::removeDuplicates()
{
	// must be sort by name
	sort(d->entries->begin(), d->entries->end(), sortByName);

	vector<EntryItem*>::iterator result = std::unique( d->entries->begin(), d->entries->end(), equalFind);

	// cause' unique() doesn't really delete elements,the duplicate elements be moved to end of vector.
	int count = 0;
	count = std::distance(result, d->entries->end());

	vector<EntryItem*>::iterator fisrt = d->entries->begin();
	while( result != d->entries->end())
	{
		// unique() issue: some duplicate elements pointer to the same memory. 
		vector<EntryItem*>::iterator it = std::find(fisrt, result, *result);
		if (it == result)
			delete (*result);	// we need to free item memory
		result = d->entries->erase( result);
	}

	// re-order by id
	sort(d->entries->begin(), d->entries->end(), sortById);

	return count;
}
