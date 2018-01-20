//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _stack_list_.h - header/impl file
// for the _stack_list_<> class.
//
// Defines list-based stack of objects (LIFO structure).
// The main difference from the array-based one is the
// ability to limit the maximum depth of the stack to some
// value after which the values at the bottom of stack will
// be pushed out (discarded) of the stack to make room for
// new items being pushed onto the top of the stack.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __stack_list_already_included_vasya__
#define __stack_list_already_included_vasya__

#include "_list_.h"

namespace soige {

//------------------------------------------------------------
// The _stack_list_ class
//------------------------------------------------------------
template<typename elem_type> class _stack_list_
{
public:
	typedef elem_type elem_type;

	//-------------------------------------------------
	// constructors
	_stack_list_()
	{
		_maxDepth = 0;
	}
	_stack_list_(const _stack_list_& other)
	{
		_stack = other._stack;
		_maxDepth = other._maxDepth;
	}

	virtual ~_stack_list_()
	{
		rewind();
	}
	
	//-------------------------------------------------
	// operators
	virtual _stack_list_& operator=(const _stack_list_& other)
	{
		if(this == &other) return *this;
		_stack = other._stack;
		_maxDepth = other._maxDepth;
		return *this;
	}
	bool operator==(const _stack_list_& other) const
	{
		if(this == &other) return true;
		return (_maxDepth == other._maxDepth && _stack == other._stack);
	}
	bool operator!=(const _stack_list_& other) const
	{
		if(this == &other) return false;
		return (_maxDepth != other._maxDepth || _stack != other._stack);
	}

	//-------------------------------------------------
	// attributes
	int depth() const
	{
		return _stack.count();
	}
	int maxDepth() const
	{
		return _maxDepth;
	}

	//-------------------------------------------------
	// operations
	void setMaxDepth(int maxDepth)
	{
		_maxDepth = maxDepth;
		// if there are extra items on the stack right now,
		// discard the ones at the bottom
		if( _maxDepth > 0 )
		{
			while( _stack.count() > _maxDepth )
				_stack.removeFirst();
		}
	}

	int find(const elem_type& elem) const
	{
		int index = _stack.find(elem);
		if(index < 0) return -1;
		return (_stack.count() - index - 1);
	}

	// unpush @count items from stack; if 0, clear it
	void rewind(int count = 0)
	{
		if(count <= 0)
			_stack.clear();
		else
			for(int i=0; i<count && _stack.count(); i++)
				_stack.removeLast();
	}

	void push(const elem_type& elem)
	{
		// if the max depth is not 0 and we're about to have
		// more than allowed items, push the one at the bottom
		// off the stack to make room for the new item
		if( _maxDepth > 0 && _stack.count()+1 > _maxDepth )
			_stack.removeFirst();
		_stack.append(elem);
	}
	bool pop(elem_type& retval)
	{
		if(_stack.count() == 0)
			return false;
		retval = *_stack.last();
		_stack.removeLast();
		return true;
	}
	elem_type* top()
	{
		if(_stack.count() == 0)
			return NULL;
		return _stack.last();
	}

protected:
	_list_<elem_type>	_stack;
	int					_maxDepth;	// maximum depth of the stack; 0 means unlimited
};


};	// namespace soige

#endif  // __stack_list_already_included_vasya__

