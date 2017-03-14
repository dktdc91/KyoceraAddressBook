#include "StdAfx.h"
#include "AbstractModel.h"
#include "AddressBook.h"

AbstractModel::AbstractModel(void)
{
}

AbstractModel::~AbstractModel(void)
{
}

bool AbstractModel::setData(int index, const string &value, int role)
{
	_UNUSED(index);
	_UNUSED(value);
	_UNUSED(role);

	return false;
}

std::string AbstractModel::headerData(int section, int role) const
{
	_UNUSED(role);

#if c11
	return std::to_string(section+1);
#endif
	char buf[10];
	sprintf_s(buf, "%d", section+1);

	return string(buf);
}

bool AbstractModel::setHeaderData(int section, const string &value, int role)
{
	_UNUSED(section);
	_UNUSED(value);
	_UNUSED(role);

	return false;
}

bool AbstractModel::insertRows(int index, int count)
{
	_UNUSED(index);
	_UNUSED(count);
	return false;
}

bool AbstractModel::removeRows(int index, int count)
{
	_UNUSED(index);
	_UNUSED(count);
	return false;
}

map<int, string> AbstractModel::itemDatas(int index) const
{
	map<int, string> roles;
	for (int i = 0; i < ABE::Role_User; ++i)
	{
		string strData = data(index, i);
		if (!strData.empty())
		{
			roles.insert( pair<int,string>(i,strData) );
		}
	}
	return roles;
}

bool AbstractModel::setItemDatas(int index, const map<int, string> &roles)
{
	bool b = true;
	for (map<int, string>::const_iterator it = roles.begin(); it != roles.end(); ++it)
		b = b && setData(index, it->second, it->first);
	return b;
}

void AbstractModel::removeAll()
{

}

int AbstractModel::removeAll(EntryItem *item)
{
	return 0;
}

int AbstractModel::removeDuplicates()
{
	return 0;
}

bool AbstractModel::isEmpty() const
{
	return true;
}

bool AbstractModel::hasId(int id, int type) const
{
	return false;
}

