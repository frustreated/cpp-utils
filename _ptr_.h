//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _ptr_.h - header file for _ptr_<>, the smart pointer class.
//
// Defines the smart pointer class that manages references
// to an object created with new or malloc(), and delete's
// the object when it's no longer referenced. Should NEVER
// be used to point to objects not created with new.
// Never delete via a plain pointer an object whose address
// was assigned to a smart ptr. If this is needed, work with
// that object _only_ through plain pointers and do not mix
// in any smart pointers (for that particular instance, i.e.).
// You can potentially define a detach() function to detach
// an object pointed to, so that its lifetime is no longer
// under the smart pointer's control. But if the object is
// detached and it has other outstanding refs, you'll have
// a problem. Up to you, kid. Don't call me when they come
// for you with a transporter and a narrow band force field.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __ptr_already_included_vasya__
#define __ptr_already_included_vasya__

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define VC_EXTRALEAN
#include <windows.h>

// disable operator-> warning for scalar types
#pragma warning(disable:4284)

namespace soige {

template<typename obj_type> class _ptr_
{
public:
	typedef obj_type obj_type;
	
	//-------------------------------------------------
	// constructors
	_ptr_(obj_type* ptr = NULL)
	{
		_p = ptr;
		_pRefCount = NULL;
		if(_p != NULL) _pRefCount = new long(1);
	}
	_ptr_(const _ptr_<obj_type>& other)
	{
		_p = NULL;
		_pRefCount = NULL;
		_attach(other);
	}
	virtual ~_ptr_()
	{
		_release();
	}

	//-------------------------------------------------
	// operators
	
	// assignment
	obj_type*& operator=(const _ptr_<obj_type>& other)
	{
		if(_p == other._p  &&  _p != NULL) return _p;
		_release();
		_attach(other);
		return _p;
	}

	obj_type*& operator=(obj_type* ptr)
	{
		if(_p == ptr && ptr != NULL) return _p;
		_release();
		if(ptr != NULL) { _p = ptr; _pRefCount = new long(1); }
		return _p;
	}

	// comparison
	/*
	bool operator>(const _ptr_<obj_type>& other) const
	{
		return ((UINT_PTR)_p > (UINT_PTR)other._p);
	}
	*/
	bool operator<(const _ptr_<obj_type>& other) const
	{
		return ((UINT_PTR)_p < (UINT_PTR)other._p);
	}

	bool operator==(const _ptr_<obj_type>& other) const
	{
		return ((UINT_PTR)_p == (UINT_PTR)other._p);
	}

	bool operator==(obj_type* ptr) const
	{
		return ((UINT_PTR)_p == (UINT_PTR)ptr);
	}

	bool operator!=(const _ptr_<obj_type>& other) const
	{
		return ((UINT_PTR)_p != (UINT_PTR)other._p);
	}

	bool operator!=(obj_type* ptr) const
	{
		return ((UINT_PTR)_p != (UINT_PTR)ptr);
	}

	bool operator!() const	// if(!ptr)
	{
		return (_p == NULL);
	}

	// pointer operators
	obj_type*& p() // access the pointer itself (for its address, etc.)
	{
		return _p;
	}
	
	/*
	// reference to the pointer... not sure should have it
	operator obj_type*&() // reference to the pointer
	{
		return _p;
	}
	operator const obj_type*&() const // reference to the pointer
	{
		return _p;
	}
	
	// same for address-of operator
	obj_type** operator&()
	{
		return (&_p);
	}
	*/

	obj_type* operator->() const
	{
		return _p;
	}

	obj_type& operator*()
	{
		return (*_p);
	}
	const obj_type& operator*() const
	{
		return (*_p);
	}

protected:
	obj_type* _p;
	// _pRefCount is either created by the current instance of the pointer class,
	// or points to refcount created by another pointer class (the source).
	// It is NULL if _p is NULL, otherwise will point to a valid value.
	long* _pRefCount;
	
	// helper attacher
	void _attach(const _ptr_& other)
	{
		if(other._p == NULL) return;
		_p = other._p;
		_pRefCount = other._pRefCount;
		InterlockedIncrement(_pRefCount);
	}

	// helper releaser
	void _release()
	{
		if(_pRefCount == NULL)  { _p = NULL; return; }
		InterlockedDecrement(_pRefCount);
		// see if the object is no longer referenced
		// by any other ptrs and destroy it if this is so
		if(*_pRefCount <= 0)
		{
			delete _pRefCount;
			delete _p;
		}
		_p = NULL;
		_pRefCount = NULL;
	}
};

};	// namespace soige

#pragma warning(default:4284)

#endif // __ptr_already_included_vasya__

