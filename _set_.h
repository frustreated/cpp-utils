//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _set_.h - header/impl file for the _set_<> class.
//
// Provides a sorted - contiguous - array of unique objects.
// Opposite to std::set, insertions are not constant time
// but rather linear time (increases with the set's size).
// std::set's storage is not contiguous, however, since
// it's based on a red-black tree.
// For a large quantity of items, if contiguous storage is
// not required, std::set provides superior performance.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __set_already_included_vasya__
#define __set_already_included_vasya__

#include "_common_.h"

namespace soige {

// the allocation slack
#ifndef ALLOC_SLACK
	#define ALLOC_SLACK  1.30
#endif

//------------------------------------------------------------
// The set class
//------------------------------------------------------------
template<typename elem_type> class _set_
{
public:
	typedef elem_type elem_type;

	//-------------------------------------------------
	// constructors
	_set_()
	{
		_array = NULL;
		_size = _allocSize = 0;
	}
	_set_(const _set_& other)
	{
		_array = NULL;
		_size = other._size;
		_allocSize = other._allocSize;
		if(_size == 0) return;
		_array = (elem_type*) malloc(_allocSize*sizeof(elem_type));
		_constructN<elem_type>(_array, other._array, _size);
	}
	virtual ~_set_()
	{
		clear();
	}

	//-------------------------------------------------
	// operators
	virtual _set_& operator=(const _set_& other);

	bool operator==(const _set_& other) const
	{
		if(this == &other) return true;
		if(_size != other._size) return false;
		for(int i=0; i<_size; i++)
			if(_compare(_array[i], other._array[i]) != 0) return false;
		return true;
	}
	bool operator!=(const _set_& other) const
	{
		return ( !this->operator==(other) );
	}
	bool operator<(const _set_& other) const
	{
		int this_less_count = 0, other_less_count = 0;
		int cmp;
		if(_size < other._size)
			return true;
		else if(_size > other._size)
			return false;
		// equal sizes; compare each element
		for(int i=0; i<_size; i++)
		{
			cmp = _compare(_array[i], other._array[i]);
			if(cmp < 0)
				this_less_count++;
			else if(cmp > 0)
				other_less_count++;
		}
		return (this_less_count > other_less_count);
	}

	const elem_type& get(int index) const
	{
		return _array[index];
	}


	//-------------------------------------------------
	// attributes
	int size() const
	{
		return _size;
	}
	int capacity() const
	{
		return _allocSize;
	}
	
	//-------------------------------------------------
	// operations
	int find(const elem_type& elem) const
	{
		bool exists;
		int index = _binSearch(elem, exists);
		return (exists ? index : -1);
	}
	
	int insert(const elem_type& elem);

	bool remove(const elem_type& elem)
	{
		bool exists;
		int index = _binSearch(elem, exists);
		if(!exists) return false;
		return removeAt(index);
	}
	bool removeAt(int index)
	{
		if(index >= _size) return false;
		_copyN<elem_type>(&_array[index], &_array[index+1], _size-index-1);
		_size -= 1;
		_destroyN<elem_type>(&_array[_size], 1);
		return true;
	}
	void clear()
	{
		if(_array)
		{
			_destroyN<elem_type>(_array, _size);
			free(_array);
			_array = NULL;
		}
		_size = _allocSize = 0;
	}
	void swap(_set_& other)
	{
		elem_type* temp_arr = _array;
		int temp_size = _size;
		int temp_alloc_size = _allocSize;
		_array = other._array;
		_size = other._size;
		_allocSize = other._allocSize;
		other._array = temp_arr;
		other._size = temp_size;
		other._allocSize = temp_alloc_size;
	}

protected:
	elem_type* _array;
	int _size;			// count of stored elements
	int _allocSize;		// allocated memory in number of elements, not bytes

private:
	int _binSearch(const elem_type& elem, bool& exists) const;
};


//------------------------------------------------------------
// Assignment to this set from another one
template<typename elem_type>
	_set_<elem_type> &
	_set_<elem_type>::operator= ( const _set_& other )
{
	if(_array == other._array) return *this;
	clear();
	if(other._size == 0) return *this;
	_size = other._size;
	_allocSize = other._allocSize;
	_array = (elem_type*) malloc(_allocSize*sizeof(elem_type));
	_constructN<elem_type>(_array, other._array, _size);
	return *this;
}

//------------------------------------------------------------
// Insert the item into its sorted position in the array
template<typename elem_type>
	int _set_<elem_type>::insert ( const elem_type& elem )
{
	bool exists;
	int index = _binSearch(elem, exists);
	if(exists)
		return -1;

	// see if no more slack left
	if(_size >= _allocSize)
	{
		_allocSize = (int)((_size+1)*ALLOC_SLACK) + 1;
		elem_type* newarray = (elem_type*) malloc(_allocSize*sizeof(elem_type));
		_constructN<elem_type>(newarray, _array, index);
		// the new element
		_constructN<elem_type>(&newarray[index], &elem, 1);
		_constructN<elem_type>(&newarray[index+1], &_array[index], _size-index);
		if(_array)
		{
			_destroyN<elem_type>(_array, _size);
			free(_array);
		}
		_array = newarray;
	}
	else
	{
		// construct the last element because it will be assigned to by _reverseCopyN
		_createN<elem_type>(&_array[_size], 1);
		if(index < _size)
			_reverseCopyN<elem_type>(&_array[index+1], &_array[index], _size-index);
		_copyN<elem_type>(&_array[index], &elem, 1);
	}
	_size += 1;

	return index;
}

//------------------------------------------------------------
// Do a binary search on the set and return the index of the
// item where it was found (in which case @exists will be
// set to true), or where it can be inserted in its sorted
// position (in which case @exists will be set to false)
template<typename elem_type>
	int _set_<elem_type>::_binSearch ( const elem_type& elem, bool& exists ) const
{
	exists = false;
	
	if(_size == 0) return 0;

	int low = 0;
	int high = _size - 1;
	int mid;
	int cmp;

	while (low <= high)
	{
	    mid = (low + high) >> 1;
	    cmp = _compare( _array[mid], elem );
	    if(cmp < 0)
			low = mid + 1;
	    else if(cmp > 0)
			high = mid - 1;
	    else
			return (exists = true), mid; // found
	}
	
	// not found, return index where can insert this element
	return low;
}


};	// namespace soige

#endif  // __set_already_included_vasya__

