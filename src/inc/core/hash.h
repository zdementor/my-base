//|-------------------------------------------------------------------------
//| File:        hash.h
//|
//| Descr:       This file is a part of the 'MyEngine'
//| Author:      Zhuk 'zdementor' Dmitry (aka ZDimitor)
//| Email:       zdimitor@pochta.ru, sibergames@nm.ru
//|
//|     Copyright (c) 2004-2009 by Zhuk Dmitry, Krasnoyarsk - Moscow
//|                      All Rights Reserved.
//|-------------------------------------------------------------------------
#ifndef hashHPP
#define hashHPP

#include "list.h"
#include "array.h"

//----------------------------------------------------------------------------
namespace my {
namespace core {
//----------------------------------------------------------------------------

template <class T>
u32 hash_key(T key, u8 bits, u32 size)
{
	u32 _bits = sizeof(key) << 3;
	T _key = key;
	while (_bits > bits)
	{
		_bits >>= 1;
		_key ^= _key >> _bits;		
	}
	return u32(_key % size);
}

//----------------------------------------------------------------------------

union ptr_to_u64 { void * ptr; u64 num; };
union ptr_to_u32 { void * ptr; u32 num; };

//----------------------------------------------------------------------------

template <class T>
u32 hash_key(T *key, u8 bits, u32 size)
{
#if MY_CPU_ARCH == MY_X64
	ptr_to_u64 conv;
	conv.ptr = (void*)key;
	u64 _key = conv.num;
#elif MY_CPU_ARCH == MY_X32
	ptr_to_u32 conv;
	conv.ptr = (void*)key;
	u32 _key = conv.num;
#endif
	u32 _bits = sizeof ( key ) << 3;
	while (_bits > bits)
	{
		_bits >>= 1;
		_key ^= _key >> _bits;		
	}
	return u32(_key % size);
}

//---------------------------------------------------------------------------

template <class T>
u32 hash_string(const T *key, u8 bits, u32 size)
{
	const T *p = key;
	u32 _key = 0;
	while (*p)
	{
		_key = 31*_key + (*p);
		p++;
	}
	return hash_key(_key, bits, size);
}

//---------------------------------------------------------------------------

template <>
inline u32 hash_key(const c8 *key, u8 bits, u32 size)
{
	return hash_string(key, bits, size);
}

//---------------------------------------------------------------------------

template <>
inline u32 hash_key(const wchar_t *key, u8 bits, u32 size)
{
	return hash_string(key, bits, size);
}

//---------------------------------------------------------------------------

template <class T>
u32 hash_key(const const_string<T> &key, u8 bits, u32 size)
{
	return hash_string(key.c_str(), bits, size);
}

//---------------------------------------------------------------------------

template <class T>
u32 hash_key(const string<T> &key, u8 bits, u32 size)
{
	return hash_string(key.c_str(), bits, size);
}

//---------------------------------------------------------------------------

template <class Type, u32 Size = 255>
class hash_array
{
public:

	hash_array() : used(0), modified(false)
	{
		hash_bits = 0;

		u32 sz = 0;
		while (sz < Size - 1)
		{
			sz = (sz << 1) | 1;
			hash_bits++;
		}
	}

	u32 size()
	{ return used; }

	template <class Key>
	u32 size (const Key & key)
	{
		u32 i = hash_key(key, hash_bits, Size);
		return hash_tbl[i].size ();
	}

	template <class Key>
	u32 push_back(const Key & key, const Type & e)
	{
		modified = true, used++;
		u32 i = hash_key(key, hash_bits, Size);
		return hash_tbl[i].push_back(e);
	}

	template <class Key>
	void erase(const Key & key, u32 idx)
	{
		modified = true, used--;
		u32 i = hash_key(key, hash_bits, Size);
		hash_tbl[i].erase(idx);
	}

	template <class TypeFind, class Key>
	s32 find(const Key & key, const TypeFind & e)
	{
		u32 i = hash_key(key, hash_bits, Size);
        return hash_tbl[i].linear_search(e);
	}

	template <class TypeFind, class Key>
	s32 find_ptr(const Key & key, const TypeFind & e)
	{
		u32 i = hash_key(key, hash_bits, Size);
        return hash_tbl[i].linear_search_ptr(e);
	}

	array<Type>& get_as_array()
	{
		if (!modified)
			return as_array;
		as_array.set_used(used);
		as_array.set_sorted(false);
		u32 a = 0;
		for (u32 i = 0; i < Size; i++)
		{
			u32 sz = hash_tbl[i].size();
			for (u32 j = 0; j < sz; j++)
				as_array[a++] = hash_tbl[i][j];
		}
		modified = false;
		return as_array;
	}

	template <class Key>
	const Type& operator() (const Key &key, u32 idx) const
	{
		u32 i = hash_key(key, hash_bits, Size);
		return hash_tbl[i][idx];
	}

	template <class Key>
	const array<Type>& operator()(const Key &key) const
	{
		u32 i = hash_key(key, hash_bits, Size);
        return hash_tbl[i];
	}

	void operator = (const hash_array < Type > & other)
	{
		clear();
		used = 0;
		for (u32 i = 0; i < Size && i < other.get_table_size(); i++)
		{
			hash_tbl[i] = other.hash_tbl[i];
			used += hash_tbl[i].size();
		}
		modified = true;
	}

	void clear()
	{
		for (u32 i = 0; i < Size; i++)
			hash_tbl[i].clear();
		used = 0;
		modified = true;
	}

	void clear_ptr()
	{
		for (u32 i = 0; i < Size; i++)
		{
			u32 sz = hash_tbl[i].size();
			for (u32 j = 0; j < sz; j++)
				delete hash_tbl[i][j];
			hash_tbl[i].clear ();
		}
		used = 0;
		modified = true;
	}

	u32 get_table_size() const
	{ return Size; }

private:

	array <Type>	as_array;
	array <Type>	hash_tbl [Size];
	u32				hash_bits;
	u32				used;
	bool			modified;
};

//---------------------------------------------------------------------------

template <class TK, class TV>
struct HashElement
{
	HashElement(TK k, TV v, bool own)
		: key(k), value(v), next(0) {}
	void set_value(TV v, bool own)
	{ value = v; }
	TK						key;
	TV						value;
	HashElement <TK, TV>	*next;
};

//---------------------------------------------------------------------------

template <class TK, class TV>
struct HashElement <TK, TV*>
{
	HashElement(TK k, TV* v, bool own)
		: key(k), value(v), next(0), own_data(own) {}
	virtual ~HashElement()
	{
		if (own_data)
			delete value;
	}
	void set_value(TV *v, bool own)
	{
		if (value == v)
			return;
		if (own_data && value)
			delete value;
		own_data = own;
		value = v;
	}
	TK						key;
	TV						*value;
	HashElement <TK, TV*>	*next;
	bool					own_data;
};

//---------------------------------------------------------------------------

template <class TK>
struct HashElement <TK, IUnknown*>
{
	HashElement(TK k, IUnknown* v, bool own)
		: key(k), value(v), next(0)
	{
		value->grab();
	}
	virtual ~HashElement()
	{
		value->drop();
	}
	void set_value(IUnknown *v, bool own)
	{
		if (value == v)
			return;
		SAFE_DROP(value);
		value = v;
		SAFE_GRAB(value);
	}
	TK							key;
	IUnknown					*value;
	HashElement <TK, IUnknown*>	*next;
};

//---------------------------------------------------------------------------

template <class TKey, class TValue, u32 Size = 255>
class hash_table
{
public:

	hash_table()
	{
		hash_bits = 0;
		u32 sz = 0;
		while (sz < Size - 1)
		{
			sz = (sz << 1) | 1;
			hash_bits++;
		}
		memset(hash_tbl, 0, sizeof(hash_tbl));
		iterator_end = iterator(this, Size, NULL);
	}

	~hash_table()
	{ clear(); }

	class iterator
	{
		friend class hash_table;
	public:

		iterator() : table(NULL), cell(0), elem(NULL) {}

		iterator& operator ++ ()
		{
			if (!elem)
				return *this;
			elem = elem->next;
			while (!elem)
			{
				if (++cell < Size)
					elem = table->hash_tbl[cell];
				else
					break;
			}
			return *this;
		}

        bool operator ==(const iterator& other) const 
		{ 
			return elem == other.elem && cell == other.cell; 
		}

        bool operator !=(const iterator& other) const 
		{ 
			return elem != other.elem || cell != other.cell; 
		}

        const TValue& operator *() 
		{ return elem->value; }

		const TKey& key()
		{ return elem->key; }

		const TValue& value()
		{ return elem->value; }

	private:

		iterator(hash_table <TKey, TValue, Size> *_table, u32 _cell,
			HashElement <TKey, TValue> *_elem)
			: table(_table), cell(_cell), elem(_elem) {}

		iterator(hash_table <TKey, TValue, Size> *_table, u32 _cell)
			: table(_table), cell(_cell), elem(NULL)
		{
			if (cell < table->get_table_size())
			{
				elem = table->hash_tbl[cell];
				while (!elem)
				{
					cell++;
					if (cell < Size)
						elem = table->hash_tbl[cell];
					else
						break;
				};
			}
		}

		hash_table <TKey, TValue, Size> *table;
		HashElement <TKey, TValue> *elem;
		u32 cell;
	};

	iterator begin()
	{
		return iterator(this, 0);
	}

	const iterator& end()
	{
		return iterator_end;
	}

	template <class TKey2>
	TValue& set_value(const TKey2 &key, const TValue &val, bool own_data = true)
	{
		u32 i = core::hash_key(key, hash_bits, Size);
		HashElement <TKey, TValue> **elem = &hash_tbl[i];
		while (*elem)
		{
			if ((*elem)->key == key)
			{
				(*elem)->set_value(val, own_data);
				return (*elem)->value;
			}
			elem = &(*elem)->next;
		}
		*elem = new HashElement <TKey, TValue> (key, val, own_data);
		return (*elem)->value;
	}

	template <class TKey2>
	TValue& get_value(const TKey2 &key)
	{
		u32 i = core::hash_key(key, hash_bits, Size);
		HashElement <TKey, TValue> * elem = hash_tbl[i];
		while (elem)
		{
			if (elem->key == key)
				return elem->value;
			elem = elem->next;
		}
        empty_value = (TValue)0;
		return empty_value;
	}

	template <class TKey2>
	bool erase_value(const TKey2 &key)
	{
		u32 i = hash_key(key, hash_bits, Size);
		HashElement <TKey, TValue> **elem = &hash_tbl[i];
		while (*elem)
		{
			if ((*elem)->key == key)
			{
				HashElement <TKey, TValue> *_e = *elem;
				*elem = (*elem)->next;
				delete _e;
				return true;
			}
			elem = &(*elem)->next;
		}
		return false;
	}

	void clear()
	{
		for (u32 i = 0; i < Size; i++)
		{
			HashElement <TKey, TValue> **elem = &hash_tbl[i];
			while (*elem)
			{
				HashElement <TKey, TValue> * _e = *elem;
				*elem = (*elem)->next;
				delete _e;
			}
		}
	}

	u32 get_table_size() { return Size; }
	u32 get_hash_bits() { return hash_bits; }

private:

	HashElement <TKey, TValue>	*hash_tbl[Size];
	u32							hash_bits;
	TValue						empty_value;
	iterator					iterator_end;
};

//---------------------------------------------------------------------------
} // end namespace core
} // end namespace my
//---------------------------------------------------------------------------

#endif // #ifndef hashHPP
