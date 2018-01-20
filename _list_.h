//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _list_.h - header/impl file for the _list_<> class.
//
// Defines doubly-linked list of objects, a unique list
// derived from list, and a list iterator.
// Comparison relies on object's operator==.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __list_already_included_vasya__
#define __list_already_included_vasya__

#include "_common_.h"

namespace soige {

template<typename elem_type> class _list_iterator_;

//------------------------------------------------------------
// The list class
//------------------------------------------------------------
template<typename elem_type> class _list_
{
public:
	typedef elem_type elem_type;

	//-------------------------------------------------
	// constructors
	_list_()
	{
		_term = new _node();
		_term->_prev = _term;
		_term->_next = _term;
		_count = 0;
	}
	_list_(const _list_& other)
	{
		_term = new _node();
		_term->_prev = _term;
		_term->_next = _term;
		_count = 0;
		_node* current = other._term->_next;
		while(current != other._term)
		{
			append(current->_value);
			current = current->_next;
		}
	}

	virtual ~_list_()
	{
		clear();
		delete _term;
		_term = NULL;
	}
	
	//-------------------------------------------------
	// operators
	_list_& operator=(const _list_& other);
	bool operator==(const _list_& other) const;
	bool operator!=(const _list_& other) const
	{
		return ( !this->operator==(other) );
	}

	//-------------------------------------------------
	// attributes
	int count() const
	{
		return _count;
	}

	//-------------------------------------------------
	// operations
	elem_type* getAt(int index)
	{
		if(index < 0 || index >= _count)
			return NULL;
		_node* current = _traverseTo(index);
		return &current->_value;
	}
	elem_type* first()
	{
		if(_count == 0)
			return NULL;
		return &_term->_next->_value;
	}
	elem_type* last()
	{
		if(_count == 0)
			return NULL;
		return &_term->_prev->_value;
	}
	
	int find(const elem_type& elem) const;
	int findLast(const elem_type& elem) const;

	bool append(const elem_type& elem)
	{
		return insert(elem, _count);
	}
	bool prepend(const elem_type& elem)
	{
		return insert(elem, 0);
	}
	bool insert(const elem_type& elem, int index);

	bool remove(const elem_type& elem);
	bool removeAt(int index);
	bool removeLast()
	{
		return removeAt(_count-1);
	}
	bool removeFirst()
	{
		return removeAt(0);
	}
	void clear()
	{
		while(_count > 0) removeAt(0);
	}

	//-------------------------------------------------
	// iteration support
	friend class _list_iterator_<elem_type>;

protected:
	struct _node
	{
		_node* _prev;
		_node* _next;
		elem_type _value;
		_node( _node* prev = NULL, _node* next = NULL, const elem_type& val = elem_type() ) :
			_prev(prev), _next(next), _value(val) { }
	};

	// the ring terminator node; _next points to first item, _prev - to last
	_node* _term;
	int    _count;

protected:
	_node* _traverseTo(int index)
	{
		if(index >= _count) return _term;

		int i;
		_node* current = _term;
		if(index > _count/2)
			for(i=0; i<=(_count-index-1); i++)
				current = current->_prev;
		else
			for(i=0; i<=index; i++)
				current = current->_next;
		return current;
	}
};

template<typename elem_type>
	_list_<elem_type> & 
	_list_<elem_type>::operator= ( const _list_& other )
{
	if(this == &other) return *this;

	clear();
	_node* current = other._term->_next;
	while(current != other._term)
	{
		append(current->_value);
		current = current->_next;
	}
	
	return (*this);
}

template<typename elem_type>
	bool _list_<elem_type>::operator== ( const _list_& other ) const
{
	if(this == &other)
		return true;
	if(_count != other._count)
		return false;
	_node* this_current = _term->_next;
	_node* other_current = other._term->_next;
	while(this_current != _term && other_current != other._term)
	{
		if( _compare(this_current->_value, other_current->_value) != 0 ) return false;
		this_current = this_current->_next;
		other_current = other_current->_next;
	}
	return true;
}

template<typename elem_type>
	int _list_<elem_type>::find ( const elem_type& elem ) const
{
	int i = 0;
	_node* current = _term->_next;
	while(current != _term)
	{
		if( _compare(current->_value, elem) == 0 ) return i;
		current = current->_next;
		i++;
	}
	return -1;
}

template<typename elem_type>
	int _list_<elem_type>::findLast ( const elem_type& elem ) const
{
	int i = 0;
	_node* current = _term->_prev;
	while(current != _term)
	{
		if( _compare(current->_value, elem) == 0 ) return (_count - i - 1);
		current = current->_prev;
		i++;
	}
	return -1;
}

template<typename elem_type>
	bool _list_<elem_type>::insert ( const elem_type& elem, int index )
{
	if(index < 0)
		return false;
	_node* current = _traverseTo(index);
	_node* new_node = new _node(current->_prev, current, elem);
	new_node->_prev->_next = new_node;
	new_node->_next->_prev = new_node;
	_count++;
	return true;
}

template<typename elem_type>
	bool _list_<elem_type>::remove ( const elem_type& elem )
{
	_node* current = _term->_next;
	while(current != _term)
	{
		if(_compare(current->_value, elem) == 0)
		{
			current->_prev->_next = current->_next;
			current->_next->_prev = current->_prev;
			delete current;
			_count--;
			return true;
		}
		current = current->_next;
	}
	return false;
}

template<typename elem_type>
	bool _list_<elem_type>::removeAt ( int index )
{
	if(index < 0 || index >= _count)
		return false;
	_node* current = _traverseTo(index);
	current->_prev->_next = current->_next;
	current->_next->_prev = current->_prev;
	delete current;
	_count--;
	return true;
}


//------------------------------------------------------------
// Subclass of list for collections of unique elements
//------------------------------------------------------------
template<typename elem_type> class _unique_list_ : public _list_<elem_type>
{
public:
	typedef elem_type elem_type;
	
	bool append(const elem_type& elem)
	{
		if(find(elem) >= 0) return false;
		return _list_<elem_type>::append(elem);
	}
	bool prepend(const elem_type& elem)
	{
		if(find(elem) >= 0) return false;
		return _list_<elem_type>::prepend(elem);
	}
	bool insert(const elem_type& elem, int index)
	{
		if(find(elem) >= 0) return false;
		return _list_<elem_type>::insert(elem, index);
	}
	const elem_type& getAt(int index) const
	{
		return _list_<elem_type>::getAt(index);
	}
	const elem_type& first() const
	{
		return _list_<elem_type>::first();
	}
	const elem_type& last() const
	{
		return _list_<elem_type>::last();
	}
};


//------------------------------------------------------------
// The list iterator class - not robust
//------------------------------------------------------------
template<typename elem_type> class _list_iterator_
{
public:
	typedef elem_type elem_type;
	
	_list_iterator_(_list_<elem_type>& aList, bool reverseIter = false)
	{
		reset(aList, reverseIter);
	}
	virtual ~_list_iterator_()
	{
		_list = NULL;
		_current = NULL;
	}

	virtual void reset(_list_<elem_type>& aList, bool reverseIter = false)
	{
		_list = &aList;
		_isReverse = reverseIter;
		begin();
	}
	
	virtual void begin()
	{
		_current = _isReverse ? _list->_term->_prev : _list->_term->_next;
	}
	virtual void next()
	{
		_current = _isReverse ? _current->_prev : _current->_next;
	}
	virtual bool isDone() const
	{
		return (_current == _list->_term);
	}
	virtual elem_type& currentItem()
	{
		return _current->_value;
	}
	virtual const elem_type& currentItem() const
	{
		return _current->_value;
	}

protected:
	_list_<elem_type>*			_list;
	_list_<elem_type>::_node*	_current;
	bool						_isReverse;
};


};	// namespace soige

#endif  // __list_already_included_vasya__

