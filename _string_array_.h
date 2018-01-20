//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _string_array_.h - header/impl file for _string_array_.
//
// Provides a managed array of TCHAR strings.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __string_array_already_included_vasya__
#define __string_array_already_included_vasya__

#include "_common_.h"

namespace soige {

//------------------------------------------------------------
// The string array class
//------------------------------------------------------------
class _string_array_
{
public:
	_string_array_()
	{
		_array = NULL;
		_size = 0;
	}

	virtual ~_string_array_()
	{
		clear();
	}

	void clear()
	{
		int i;
		if(_size == 0)
			return;
		for(i=0; i<_size; i++)
			free(_array[i]);
		free(_array);
		_array = NULL;
		_size = 0;
	}

	int append(LPCTSTR newval)
	{
		return insert(newval, _size);
	}

	int insert(LPCTSTR newval, int index)
	{
		TCHAR** pv;
		if(index < 0)
			return -1;
		if(index >= _size)
			index = _size;
		// allocate new array
		pv = (TCHAR**) malloc( (_size+1)*sizeof(TCHAR*) );
		// copy the first part of array (up to the new one)
		memcpy( pv, _array, index*sizeof(TCHAR*) );
		// create the new element
		pv[index] = _tcsdup(newval);
		// now copy the part after the new element
		memcpy( &pv[index+1], &_array[index], (_size-index)*sizeof(TCHAR*) );
		// swap old and new pointers
		if(_array)
			free(_array);
		_array = pv;
		return ++_size;
	}

	long find(LPCTSTR val, bool ignore_case = false) const
	{
		long i;

		if(ignore_case)
			for(i=0; i<_size; i++)
				if(lstrcmpi(_array[i], val) == 0) return i;
		else
			for(i=0; i<_size; i++)
				if(lstrcmp (_array[i], val) == 0) return i;
		return -1;
	}

	LPTSTR get(int index)
	{
		if(index < 0 || index >= _size)
			return NULL;
		return _array[index];
	}

	LPTSTR operator[](int index)
	{
		if(index < 0 || index >= _size)
			return NULL;
		return _array[index];
	}

	int removeAt(int index)
	{
		TCHAR** pv;

		if(index < 0 || index >= _size)
			return -1;

		free(_array[index]);
		if(--_size == 0)
		{
			free(_array);
			_array = NULL;
			return _size;
		}
		pv = (TCHAR**) malloc( _size*sizeof(TCHAR*) );
		// copy from start up to the deleted item
		memcpy( pv, _array, index*sizeof(TCHAR*) );
		// now copy from after the deleted item to the end
		memcpy( &pv[index], &_array[index+1], (_size-index)*sizeof(TCHAR*) );
		// swap old and new pointers
		free(_array);
		_array = pv;
		return _size;
	}

	int length() const
	{
		return _size;
	}

private:
	LPTSTR*	_array;	// the array of values
	int		_size;	// element count

private:
	// no byval operations
	_string_array_(const _string_array_& rsa) { }
	_string_array_& operator=(const _string_array_& rsa) { }
};


};	// namespace soige

#endif  // __string_array_already_included_vasya__

