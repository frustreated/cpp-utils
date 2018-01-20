//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _queue_.h - header/impl file for the _queue_<> class.
//
// Defines list-based queue of objects.
// The queue operates on the idea of first in, first out,
// in case you wondered.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __queue_already_included_vasya__
#define __queue_already_included_vasya__

#include "_list_.h"

namespace soige {

//------------------------------------------------------------
// The queue class
//------------------------------------------------------------
template<typename elem_type> class _queue_
{
public:
	typedef elem_type elem_type;

	//-------------------------------------------------
	// constructors
	_queue_()
	{
	}
	_queue_(const _queue_& other)
	{
		_queue = other._queue;
	}

	virtual ~_queue_()
	{
		clear();
	}
	
	//-------------------------------------------------
	// operators
	virtual _queue_& operator=(const _queue_& other)
	{
		_queue = other._queue;
		return (*this);
	}
	bool operator==(const _queue_& other) const
	{
		if(this == &other) return true;
		return (_queue == other._queue);
	}
	bool operator!=(const _queue_& other) const
	{
		if(this == &other) return false;
		return (_queue != other._queue);
	}

	//-------------------------------------------------
	// attributes
	int length() const
	{
		return _queue.count();
	}

	//-------------------------------------------------
	// operations
	bool contains(const elem_type& elem) const
	{
		return (_queue.find(elem) >= 0);
	}

	void clear()
	{
		_queue.clear();
	}

	void put(const elem_type& elem)
	{
		_queue.append(elem);
	}
	bool get(elem_type& retval)
	{
		if(_queue.count() == 0) return false;
		retval = *_queue.first();
		_queue.removeFirst();
		return true;
	}
	elem_type& peek()
	{
		return *_queue.first();
	}
	const elem_type& peek() const
	{
		return *_queue.first();
	}

protected:
	_list_<elem_type> _queue;
};

};	// namespace soige

#endif  // __queue_already_included_vasya__

