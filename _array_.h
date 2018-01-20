//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _array_.h - header/impl file for the _array_<> class.
//
// Provides a managed array of objects stored in contiguous
// memory locations (like STL's vector).
// Preservation of values depends on object's copy
// constructor and operator= being correctly defined.
// For comparisons, user-defined types have to define
// operators == and < (which are used by the _compare
// template routine in _common_.h).
//
// Some notes.
// Reallocation is done only on inserts and resizes
// (similar to STL's); removals simply free elements.
// Resize() initializes the new extra elements as empty.
// When reallocation happens, existing elements are moved
// using either assignment (that's why for user-defined
// objects, it's important to have operator= for proper
// copying semantics), or the placement new operator to
// copy-construct objects.
// And, of course, we never use memmove (ugh!) to relocate
// objects because if they contain pointers pointing to
// other members of the same array (or a similar situation),
// the objects will end up pointing to wrong locations.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __array_already_included_vasya__
#define __array_already_included_vasya__

#include "_common_.h"
#include "_sort_.h"


namespace soige {

// the allocation slack
#ifndef ALLOC_SLACK
	#define ALLOC_SLACK  1.30
#endif

//------------------------------------------------------------
// The array class
//------------------------------------------------------------
template<typename elem_type> class _array_
{
public:
	typedef elem_type elem_type;

	//-------------------------------------------------
	// constructors
	_array_()
	{
		_array = NULL;
		_length = _allocSize = 0;
	}
	_array_(const _array_& other)
	{
		_array = NULL;
		_length = other._length;
		_allocSize = other._allocSize;
		if(_length == 0) return;
		_array = (elem_type*) malloc(_allocSize*sizeof(elem_type));
		_constructN<elem_type>(_array, other._array, _length);
	}
	virtual ~_array_()
	{
		clear();
	}

	//-------------------------------------------------
	// operators
	virtual _array_& operator=(const _array_& other);
	
	bool operator==(const _array_& other) const
	{
		if(this == &other) return true;
		if(_length != other._length) return false;
		for(int i=0; i<_length; i++)
			if(_compare(_array[i], other._array[i]) != 0) return false;
		return true;
	}
	bool operator!=(const _array_& other) const
	{
		return ( !this->operator==(other) );
	}

	elem_type& operator[](int index)
	{
		return _array[index];
	}
	const elem_type& operator[](int index) const
	{
		return _array[index];
	}
	
	elem_type& get(int index)
	{
		return _array[index];
	}
	const elem_type& get(int index) const
	{
		return _array[index];
	}


	//-------------------------------------------------
	// attributes
	int length() const
	{
		return _length;
	}
	int capacity() const
	{
		return _allocSize;
	}
	
	//-------------------------------------------------
	// operations
	
	void resize(int newLength);

	int find(const elem_type& elem) const
	{
		for(int i=0; i<_length; i++)
			if(_compare(_array[i], elem) == 0) return i;
		return -1;
	}
	void append(const elem_type& elem)
	{
		insertNAt(_length, &elem, 1);
	}
	void prepend(const elem_type& elem)
	{
		insertNAt(0, &elem, 1);
	}
	void insert(const elem_type& elem, int index)
	{
		insertNAt(index, &elem, 1);
	}

	void insertNAt(int index, const elem_type srcArray[], int elemCount);
	
	void sort(bool descending = false)
	{
		if(_length < 2) return;
		
		_sort_<elem_type> sorter;
		sorter.sort(_array, _length);
		if(descending) reverse();
	}

	void reverse()
	{
		if(_length < 2) return;
		
		int i = -1, half = (int)(_length/2);
		elem_type t;
		while(++i < half)
		{
			t = _array[_length-i-1];
			_array[_length-i-1] = _array[i];
			_array[i] = t;
		}
	}

	void remove(const elem_type& elem)
	{
		removeNAt(find(elem), 1);
	}
	void removeAt(int index)
	{
		removeNAt(index, 1);
	}
	
	bool removeNAt(int index, int count);
	
	void clear()
	{
		if(_array)
		{
			_destroyN<elem_type>(_array, _length);
			free(_array);
			_array = NULL;
		}
		_length = _allocSize = 0;
	}

protected:
	elem_type* _array;
	int _length;		// count of stored elements
	int _allocSize;		// allocated memory in number of elements, not bytes
};


//------------------------------------------------------------
// Assignment to this array from another one
template<typename elem_type>
	_array_<elem_type> &
	_array_<elem_type>::operator= ( const _array_& other )
{
	if(_array == other._array) return *this;
	clear();
	if(other._length == 0) return *this;
	_length = other._length;
	_allocSize = other._allocSize;
	_array = (elem_type*) malloc(_allocSize*sizeof(elem_type));
	_constructN<elem_type>(_array, other._array, _length);
	return *this;
}

//------------------------------------------------------------
// Resize the array to the specified number of elements,
// preserving as many elements as makes sense with new size.
// Resizing always reallocates (to provide a backdoor)
template<typename elem_type>
	void _array_<elem_type>::resize ( int newLength )
{
	if(newLength == _length) return;
	int elemsToCopy = (newLength > _length) ? _length : newLength;
	_allocSize = (int)(newLength*ALLOC_SLACK) + 1;
	elem_type* newarray = (elem_type*) malloc(_allocSize*sizeof(elem_type));
	// copy over existing ones by constructing them in place
	_constructN<elem_type>(newarray, _array, elemsToCopy);
	// default-construct the ones after the copied ones
	_createN<elem_type>(&newarray[elemsToCopy], newLength-elemsToCopy);
	// destroy the old ones
	if(_array)
	{
		_destroyN<elem_type>(_array, _length);
		free(_array);
	}
	_array = newarray;
	_length = newLength;
}

//------------------------------------------------------------
// Insert the specified number of elements from a normal C
// array of same-typed elements into this array starting at
// the specified index, moving the previous elements at that
// position up (to the end of the array).
template<typename elem_type>
	void _array_<elem_type>::insertNAt ( int index,
										 const elem_type srcArray[],
										 int elemCount )
{
	if(index > _length) index = _length;
	// see if no more slack left
	if(_length+elemCount > _allocSize)
	{
		// have to reallocate
		_allocSize = (int)((_length+elemCount)*ALLOC_SLACK) + 1;
		elem_type* destarray = (elem_type*) malloc(_allocSize*sizeof(elem_type));
		// copy-construct the existing ones before the index
		_constructN<elem_type>(destarray, _array, index);
		// copy-construct the new ones
		_constructN<elem_type>(&destarray[index], srcArray, elemCount);
		// copy-construct the existing ones after the new ones
		_constructN<elem_type>(&destarray[index+elemCount], &_array[index], _length-index);
		// destroy the old array and point to the new one
		if(_array)
		{
			_destroyN<elem_type>(_array, _length);
			free(_array);
		}
		_array = destarray;
		_length += elemCount;
	}
	else
	{
		// default-construct empty elements in the uninitialized slots
		// after current array end, because they will be copied to below
		_createN<elem_type>(&_array[_length], elemCount);
		_length += elemCount;
		// move up (to the end of the array) the ones after the new ones.
		// do it backwards, so that not to overwrite the previous els by copying.
		_reverseCopyN<elem_type>(&_array[index+elemCount], &_array[index], _length-index-elemCount);
		// copy the new ones
		_copyN(&_array[index], srcArray, elemCount);
	}
}

//------------------------------------------------------------
// Remove the specified number of elements, compacting
// the array to fill the gap.
template<typename elem_type>
	bool _array_<elem_type>::removeNAt ( int index, int count )
{
	if(index >= _length) return false;
	if(index+count > _length) count = _length-index;
	// move the items down, starting from the deleted ones, into their place
	_copyN<elem_type>(&_array[index], &_array[index+count], _length-count-index);
	// delete the items after the ones that were moved
	_destroyN<elem_type>(&_array[_length-count], count);
	_length -= count;
	return true;
}


};	// namespace soige


#endif  // __array_already_included_vasya__

