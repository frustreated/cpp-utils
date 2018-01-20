//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _stack_array_.h - header/impl file
// for the _stack_array_<> class.
//
// Defines array-based stack of objects (LIFO structure).
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __stack_array_already_included_vasya__
#define __stack_array_already_included_vasya__

#include "_array_.h"

namespace soige {

//------------------------------------------------------------
// The _stack_array_ class
//------------------------------------------------------------
template<typename elem_type> class _stack_array_
{
public:
	typedef elem_type elem_type;

	//-------------------------------------------------
	// constructors
	_stack_array_()
	{
	}
	_stack_array_(const _stack_array_& other)
	{
		_stack = other._stack;
	}

	virtual ~_stack_array_()
	{
		rewind();
	}
	
	//-------------------------------------------------
	// operators
	virtual _stack_array_& operator=(const _stack_array_& other)
	{
		if(this == &other) return *this;
		_stack = other._stack;
		return *this;
	}
	bool operator==(const _stack_array_& other) const
	{
		if(this == &other) return true;
		return (_stack == other._stack);
	}
	bool operator!=(const _stack_array_& other) const
	{
		if(this == &other) return false;
		return (_stack != other._stack);
	}

	//-------------------------------------------------
	// attributes
	int depth() const
	{
		return _stack.length();
	}

	//-------------------------------------------------
	// operations
	int find(const elem_type& elem) const
	{
		int index = _stack.find(elem);
		if(index < 0) return -1;
		return (_stack.length() - index - 1);
	}

	// unpush @count items from stack; if 0, clear it
	void rewind(int count = 0)
	{
		if(count <= 0)
			_stack.clear();
		else
			for(int i=0; i<count && _stack.length(); i++)
				_stack.removeAt(_stack.length()-1);
	}

	void push(const elem_type& elem)
	{
		_stack.append(elem);
	}
	bool pop(elem_type& retval)
	{
		if(_stack.length() == 0)
			return false;
		retval = _stack.get( _stack.length()-1 );
		_stack.removeAt( _stack.length()-1 );
		return true;
	}
	elem_type* top()
	{
		if(_stack.length() == 0)
			return NULL;
		return &_stack.get( _stack.length()-1 );
	}

protected:
	_array_<elem_type> _stack;
};


};	// namespace soige

#endif  // __stack_array_already_included_vasya__

