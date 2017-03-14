#include "StdAfx.h"
#include "EntryItem.h"
#include "utility.h"

/************************************************************************/
/*@brief The EntryItemData class provides data for a contact.
 *@author Yuri Yang <yuri.yang@liteon.com>
/************************************************************************/
class EntryItemData
{
public:
	EntryItemData() : role(-1) {}
	EntryItemData(int role, const string &v) : role(role), value(v) {}
	EntryItemData(const EntryItemData &other){ role = other.role; value = other.value; }
	inline EntryItemData &operator=(const EntryItemData &other)
	{
		if( this != &other)
		{
			role = other.role;
			value = other.value;
		}
		return  *this;
	}
	inline bool operator==(const EntryItemData &other) const
	{ return role == other.role && value == other.value; }

	int role;		// property id
	string value;	// property value(better set variant type)
};
inline istream& operator>>(istream &in, EntryItemData &data)
{
	in >> data.role;
	in >> data.value;
	return in;
}

inline ostream& operator<<(ostream &out, const EntryItemData &data)
{
	//out << data.role;
	out << data.value;
	return out;
}

/************************************************************************/
/*@brief The EntryItemPrivate class provides operator for a contact.
 *@author Yuri Yang <yuri.yang@liteon.com>
/************************************************************************/
class EntryItemPrivate
{
	friend class EntryItem;
public:
	inline EntryItemPrivate() : type(ABE::ContactType) {  }
	virtual ~EntryItemPrivate() { cout << "~EntryItemPrivate\n"; values.clear();childrenId.clear(); }

	EntryItemPrivate *copy() const
	{
		EntryItemPrivate *tmp = new EntryItemPrivate();
		tmp->type = this->type;
		tmp->values.assign(this->values.begin(), this->values.end());
		tmp->childrenId.assign(this->childrenId.begin(), this->childrenId.end());
		return tmp;
	}

	EntryItemPrivate &operator=(const EntryItemPrivate &other)
	{
		if (this == &other)
			return *this;
		this->type = other.type;
		this->values.assign(other.values.begin(), other.values.end());
		this->childrenId.assign(other.childrenId.begin(), other.childrenId.end());
		return *this;
	}

	int type;
	vector<EntryItemData> values;
	//vector<EntryItem*> children;
	vector<int> childrenId;
};

//////////////////////////////////////////////////////////////////////////
//
// EntryItem class implement
//
//////////////////////////////////////////////////////////////////////////
EntryItem::EntryItem(ABE::EntryType type /*= ABE::ContactType*/)
	: d( new EntryItemPrivate )
{
	setType(type);
}

EntryItem::EntryItem(const string &name, ABE::EntryType type /*= ABE::ContactType*/)
	: d( new EntryItemPrivate )
{
	setType(type);
	setData( name, ABE::Role_Name );
}

EntryItem::EntryItem(const string &name, const string fax)
	: d( new EntryItemPrivate )
{
	setType(ABE::ContactType);
	setData( name, ABE::Role_Name );
	setData( fax, ABE::Role_Fax );
}

EntryItem::EntryItem(const string &name, const string fax, const string &company)
	: d( new EntryItemPrivate )
{
	setType(ABE::ContactType);
	setData( name, ABE::Role_Name );
	setData( fax, ABE::Role_Fax );
	setData( company, ABE::Role_Company );
}

EntryItem::EntryItem(const EntryItem &other)
	:d(new EntryItemPrivate)
{
	d = other.d->copy();
}

EntryItem::~EntryItem(void)
{
	cout << "~EntryItem\n";
	delete d;
}

int EntryItem::dataCount() const
{
	return d->values.size();
}

int EntryItem::type() const
{
	return d->type;
}

void EntryItem::setType(int type)
{
	string strType = Util::ToString(type);
	setData( strType, ABE::Role_Type );
	d->type = type;
}

void EntryItem::setData(const string &value, int role)
{
	if( d->type == ABE::GroupType )
	{
		if( role != ABE::Role_Name && role != ABE::Role_Id)
		{
			return;
		}
	}
	bool found = false;
	vector<EntryItemData>::iterator it;
	for (it = d->values.begin(); it != d->values.end(); ++it) 
	{
		if ((*it).role == role)
		{
			if ( (*it).value == value )
				return;
			(*it).value = value;
			// send message to model update data changed.
			//...
			found = true;
			break;
		}
	}
	if( !found )
	{
		d->values.push_back(EntryItemData(role, value));
		// send message to model update data changed.
		//...
	}
}

string EntryItem::data(int role) const
{
// 	if (d->type == ABE::GroupType && role != ABE::Role_Name)
// 		return string();

	vector<EntryItemData>::iterator it;
	for (it = d->values.begin(); it != d->values.end(); ++it) 
	{
		if ((*it).role == role )
			return (*it).value;
	}
	return string();
}

EntryItem & EntryItem::operator=(const EntryItem &other)
{
	cout << "EntryItem::operator=\n";
	if (this != &other)
	{
		d = other.d->copy();
	}
	return *this;
}

bool EntryItem::operator==(const EntryItem &other)
{
	cout << "EntryItem::operator==\n";
	if (this == &other)
		return true;

	if (d->type == other.d->type)
	{
		if (d->type == ABE::ContactType)
		{
			// just compare name and fax
			string other_name = other.data(ABE::Role_Name);
			string this_name = this->data(ABE::Role_Name);
			string other_fax = other.data(ABE::Role_Fax);
			string this_fax = this->data(ABE::Role_Fax);

			return (other_name.compare(this_name) == 0 && other_fax.compare(this_fax) == 0);
		}
		else if (d->type == ABE::GroupType)
		{
			string other_name = other.data(ABE::Role_Name);
			string this_name = this->data(ABE::Role_Name);
			return other_name.compare(this_name) == 0;
		}
	}

	return false;
	//return (this->d->values == other.d->values);
}

bool EntryItem::addChild(int childId)
{
	if ( !hasChild(childId))
	{
		d->childrenId.push_back(childId);
		return true;
	}

	return false;
}

bool EntryItem::removeChild(int childId)
{
	bool ok = false;
	vector <int>::iterator iter = d->childrenId.begin();
	for ( ; iter != d->childrenId.end(); ++iter)
	{
		if ( (*iter) == childId )
		{
			d->childrenId.erase(iter);
			ok = true;
			break;
		}
	}
	return ok;
}

bool EntryItem::hasChild(int childId) const
{
	bool exist = false;
	vector<int>::const_iterator it = d->childrenId.begin();
	for (; it != d->childrenId.end(); ++it)
	{
		if ( (*it) == childId )
		{
			exist = true;
			break;;
		}
	}

	return exist;
}

vector<int> EntryItem::children() const
{
	if ( d->type != ABE::GroupType )
		return vector<int>();

	return d->childrenId;
}

/*
bool EntryItem::addChild(EntryItem *child)
{
	if ( !hasChild(child))
	{
		d->children.push_back(child);
		return true;
	}

	return false;
}

void EntryItem::removeChild(EntryItem *child)
{
	vector <EntryItem*>::iterator iter = d->children.begin();
	for ( ; iter != d->children.end();)
	{
		if ( *iter == child )
		{
			// note don't delete item
			// delete *iter;
			d->children.erase(iter);
			iter = d->children.begin();
			break;
		}
	}
}

void EntryItem::removeChild(int index)
{
	vector <EntryItem*>::iterator iter = d->children.begin();
	for ( int idx = 0; iter != d->children.end(); ++iter, ++idx)
	{
		if ( idx == index )
		{
			d->children.erase(iter);
			iter = d->children.begin();
			break;
		}
	}
}

vector<EntryItem*> EntryItem::children() const
{
	if ( d->type != ABE::GroupType )
		return vector<EntryItem*>();

	return d->children;
}

bool EntryItem::hasChild(EntryItem *item)
{
	bool exist = false;
	vector<EntryItem*>::const_iterator it = d->children.begin();
	for (; it != d->children.end(); ++it)
	{
		if ( (*it) == item )
		{
			exist = true;
			break;;
		}
	}

	return exist;
}
*/

EntryItem & EntryItem::operator<<(const string &value)
{
	setData(value, ABE::Role_User);
	return *this;
}

ostream& operator<<(ostream &out, const EntryItem &entry)
{
	/*
	out << entry.data(ABE::Role_Id) << " "
		<< entry.data(ABE::Role_Name)  << " "
		<< entry.data(ABE::Role_Fax) << " "
		<< entry.data(ABE::Role_Company) << " "
		<< entry.data(ABE::Role_Department) << " "
		<< endl;
	*/
	for (unsigned int i = 0; i < entry.d->values.size(); ++i)
	{
		out << entry.d->values.at(i) << " ";
	}
	return out;
}

ostream& operator<<(ostream &out, const EntryItem *entry)
{
	/*
	out << entry->data(ABE::Role_Id) << " "
		<< entry->data(ABE::Role_Name) << " "
		<< entry->data(ABE::Role_Fax) << " "
		<< entry->data(ABE::Role_Company) << " "
		<< entry->data(ABE::Role_Department) << " "
		<< endl;
	*/
	for (unsigned int i = 0; i < entry->d->values.size(); ++i)
	{
		out << entry->d->values.at(i) << " ";
	}
	return out;
}

/*
istream& operator>>(istream &in, EntryItem &entry)
{
	EntryItemData data;
	in >> data;
	entry.setData(data.value, data.role);
	return in;
}

istream& operator>>(istream &in, EntryItem *entry)
{
	EntryItemData data;
	in >> data;
	entry->setData(data.value, data.role);
	return in;
}
*/
