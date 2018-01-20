//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _dictionary_.h - header file for the _dictionary_<> class.
//
// Provides a collection of objects of one type indexed
// by unique keys of a (optionally) different type.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __dictionary_already_included_vasya__
#define __dictionary_already_included_vasya__

#include "_set_.h"
#include "_array_.h"

namespace soige {

//------------------------------------------------------------
// The dictionary class
//------------------------------------------------------------
template<typename key_type, typename elem_type> class _dictionary_
{
public:
	typedef key_type key_type;
	typedef elem_type elem_type;

	//-------------------------------------------------
	// ctor/dtor
	_dictionary_()
	{
	}
	_dictionary_(const _dictionary_& other)
	{
		_keys = other._keys;
		_elems = other._elems;
	}
	virtual ~_dictionary_()
	{
		clear();
	}

	//-------------------------------------------------
	// operators
	virtual _dictionary_& operator=(const _dictionary_& other)
	{
		if(this == &other) return *this;
		_keys = other._keys;
		_elems = other._elems;
		return *this;
	}
	bool operator==(const _dictionary_& other) const
	{
		if(this == &other) return true;
		return ( (_keys == other._keys) && (_elems == other._elems) );
	}
	bool operator!=(const _dictionary_& other) const
	{
		return !(this->operator==(other));
	}

	//-------------------------------------------------
	// attributes
	int size() const
	{
		return _keys.size();
	}
	bool containsKey(const key_type& key) const
	{
		return (_keys.find(key) >= 0);
	}
	bool containsElement(const elem_type& elem) const
	{
		return (_elems.find(elem) >= 0);
	}

	//-------------------------------------------------
	// operations
	bool put(const key_type& key, const elem_type& elem)
	{
		int index;
		if( (index=_keys.insert(key)) < 0 )
			return false;
		_elems.insert(elem, index);
		return true;
	}
	elem_type& operator[](const key_type& key)
	{
		int index = _keys.find(key);
		if(index < 0) throw exception( "Specified key does not exist" );
		return _elems.get(index);
	}
	const elem_type& operator[](const key_type& key) const
	{
		int index = _keys.find(key);
		if(index < 0) throw exception( "Specified key does not exist" );
		return _elems.get(index);
	}
	elem_type& get(const key_type& key)
	{
		int index = _keys.find(key);
		if(index < 0) throw exception( "Specified key does not exist" );
		return _elems.get(index);
	}
	const elem_type& get(const key_type& key) const
	{
		int index = _keys.find(key);
		if(index < 0) throw exception( "Specified key does not exist" );
		return _elems.get(index);
	}

	bool remove(const key_type& key)
	{
		int index = _keys.find(key);
		if(index < 0)
			return false;
		_keys.removeAt(index);
		_elems.removeAt(index);
		return true;
	}
	void clear()
	{
		_keys.clear();
		_elems.clear();
	}

	// access to the respective arrays
	const _set_<key_type>& keys() const
	{
		return _keys;
	}
	const _array_<elem_type>& elements() const
	{
		return _elems;
	}

protected:
	_set_	<key_type>  _keys;
	_array_ <elem_type> _elems;
};

};	// namespace soige

#endif // __dictionary_already_included_vasya__

