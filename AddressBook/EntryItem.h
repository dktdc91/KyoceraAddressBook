#pragma once
#include "stdafx.h"
#include <string>
#include <iostream>
#include <vector>

#include "AddressBook.h"

using namespace std;

class EntryItemPrivate;

/************************************************************************/
/*@brief The EntryItem class provides the address book contact each property.
 *@author Yuri Yang <yuri.yang@liteon.com> 2016-6-17
/************************************************************************/
class ADDRESS_BOOK_EXPORT EntryItem
{
public:
	EntryItem(ABE::EntryType type = ABE::ContactType);
	EntryItem(const string &name, ABE::EntryType type = ABE::ContactType);
	EntryItem(const string &name, const string fax);
	EntryItem(const string &name, const string fax, const string &company);
	EntryItem(const EntryItem &other);
	virtual ~EntryItem();

	/*! Returns its data counts, that column counts in list view*/
	virtual int dataCount() const;

	/*! returned/set entry type(contact or group)*/
	virtual int type() const;
	virtual void setType(int type);

	/*! returned/set role data for the entry*/
	virtual string data(int role = ABE::Role_Name) const;
	virtual void setData(const string &value, int role = ABE::Role_Name);

	//Variant data(ABE::EntryRole role) const;
	//void setData(const Variant &value, ABE::EntryRole role);

	// just save child's id
	bool addChild(int childId );
	bool removeChild(int childId);
	bool hasChild(int childId) const;
	vector<int> children() const;
	/*
	bool addChild(EntryItem *child);
	void removeChild(EntryItem *child);
	void removeChild(int index);
	vector<EntryItem*> children() const;
	bool hasChild(EntryItem *item);
	*/

	EntryItem &operator=(const EntryItem &other);
	bool operator==(const EntryItem &other);

	inline  EntryItem &operator<< (const string &value);

	friend ADDRESS_BOOK_EXPORT ostream& operator<<(ostream &out, const EntryItem &entry);
	friend ADDRESS_BOOK_EXPORT ostream& operator<<(ostream &in, const EntryItem *entry);

	//friend ADDRESS_BOOK_EXPORT istream& operator>>(istream &in, EntryItem &entry);
	//friend ADDRESS_BOOK_EXPORT istream& operator>>(istream &in, EntryItem *entry);

private:
	EntryItemPrivate *d;
};


/************************************************************************/
/*@brief The Variant class provides a conventional data.
 *@author Yuri Yang <yuri.yang@liteon.com>
 *@issue some bug, abandon it
/************************************************************************/
class Variant
{
public:
	Variant(): d() { d.data.ptr = 0; }
	Variant(bool b) : d(Bool){ d.data.b = b; }
	Variant(int i) : d(Int){ d.data.i = i; }
// 	Variant(const char *c) : d(Char) { strcpy(d.data.c, c) ; }// char *c = new char[?];
	Variant(const string &str) :d(String) { d.data.c = const_cast<char*>(str.c_str()); }
	Variant(void *ptr) { d.data.ptr = ptr; }
	Variant(const Variant &other)
	{
		cout << "3.[Copy Variant]\n";
		//d.data = other.d.data;

		switch( other.d.type )
		{
		case Bool:
			d.data.b = other.d.data.b;
			break;
		case Int:
			d.data.i = other.d.data.i;
			break;
		case Char:
		case String:
			d.data.c = other.d.data.c;
			break;
		default:
			d.data.ptr = other.d.data.ptr;
			break;
		}
	}

	~Variant() { cout << "3.[~Variant]\n"; }

	bool toBool() const { return d.data.b; }
	int toInt() const { return d.data.i; }
	char *toChar() const { if (d.data.c == 0) return "\0"; return d.data.c; }
	string toString() const { if (d.data.c == 0) return string(""); return string(d.data.c); }

	inline Variant &operator=(const Variant &v)
	{
		cout << "3.[Variant]: operator=\n";

		if( this == &v )
		{
			return *this;
		}

		d.data = v.d.data;

		/*

		switch( d.type )
		{
		case Bool:
			d.data.b = v.d.data.b;
			break;
		case Int:
			d.data.i = v.d.data.i;
			break;
		case Char:
		case String:
			d.data.c = v.d.data.c;
			break;
		default:
			d.data.ptr = v.d.data.ptr;
			break;
		}
	*/
		return *this;
	}

	inline bool operator==(const Variant &v) const
	{
		if( d.type != v.d.type )
		{
			return false;
		}

		switch( d.type )
		{
		case Bool:
			return d.data.b == v.d.data.b;
		case Int:
			return d.data.i == v.d.data.i;
		case Char:
			return d.data.c == v.d.data.c;
		default:
			return d.data.ptr == v.d.data.ptr;
		}

		return false;
	}
	enum Type
	{
		Invalid,
		Bool,
		Int,
		Char,
		String
	};

	struct Private
	{
		inline Private() : type(Invalid){ data.ptr = 0;}
		explicit inline Private(unsigned int variantType) : type(variantType){ cout << "4.[Private] \n"; }
		inline Private(const Private &other) : data(other.data), type(other.type) { cout << "4.[Copy Private] \n";}
		inline Private &operator=(const Private &other) { data = other.data; cout << "4.[Private] operator=\n"; return *this; }
		~Private() { cout << "4.[~Private]\n"; }

		union Data
		{
			bool b;
			int i;
			char *c;
			void *ptr;
		}data;
		unsigned int type : 1;
	};

/*
	template <typename T>
	inline T variantTo(const Variant::Private &d, const T& val)
	{
		//if (d.type == typeid(T))
		return val;
	}
*/

private:
	Private d;
};

