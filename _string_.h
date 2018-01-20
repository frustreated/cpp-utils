//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _string_.h - header file for class _string_.
//
// Lazy-copied string. Can contain null ('\0') chars.
// Operations are the same as in _cstring_.
// Obviously, using this class is preferable to _cstring_.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __string_already_included_vasya__
#define __string_already_included_vasya__

// to compile _string_ with the optional stuff
// #define ALL_STRING_STUFF 1

#include "_common_.h"
// some heavy operations are compiled only if requested
#ifdef ALL_STRING_STUFF
	#include "_array_.h"
#endif

namespace soige {


//------------------------------------------------------------
// The _string_ class
//------------------------------------------------------------
class _string_
{
private:
	// the string holder class itself;
	// _string_ is just a proxy to it
	class string_rep
	{
	public:
		// length == -1 means use lstrlenA() to find out the length;
		// otherwise specifies number of chars to copy into this new rep
		explicit string_rep (LPCSTR lpstr = NULL, int length = -1) :
		_p(NULL), _len(0), _buflen(0), _refcount(0) {
			if(NULL == lpstr) return;
			if( length == -1 ) length = lstrlenA(lpstr);
			if( !_realloc(length + 1) ) return;
			memcpy( _p, lpstr, length );
			_p[_len = length] = '\0';
		}
		~string_rep ()			{ free(_p); }

		// char& charAt(int index)	{ return _p[index]; } // needed for subscript_proxy
		LPSTR end()				{ return _p + _len; }
		int&  len()				{ return _len; }
		int   allocLen()		{ return _buflen; }
		bool  ensureLen(int cch){ if(cch <= _buflen) return true; return _realloc(cch); }
		int   ref()				{ InterlockedIncrement((LPLONG)&_refcount); return refs(); }
		int   deref()			{ InterlockedDecrement((LPLONG)&_refcount); return refs(); }
		int   refs()			{ return (_refcount & ~UNSHAREABLE); }

		void  markShareable()	{ _refcount &= ~UNSHAREABLE; }
		void  markUnshareable()	{ _refcount |= UNSHAREABLE; }
		bool  isShareable()		{ return !(_refcount & UNSHAREABLE); }

		LPSTR	_p;			// the C string represented by this rep
	
	private:
		int		_len;		// current length of the string, in characters
		int		_buflen;	// size of buffer, in characters
		int		_refcount;	// number of references to this representation

		// Memory management
		bool _realloc (int cch) {
			cch = (cch >= 120) ? ((int)(cch*1.25)) : (cch+16);
			if( !(_p = (LPSTR) realloc(_p, cch)) ) return _buflen = 0, false;
			if(_buflen > cch) _p[cch-1] = '\0'; // if decreasing size, terminate with NULL
			return _buflen = cch, true;
		}
		
		// No byval operations on the representation
		string_rep ( const string_rep& )		{ }
		void operator= ( const string_rep& )	{ }
	
		// The shareable flag: the highest bit of refcount
		static const int UNSHAREABLE; // 0x80000000
	};
	// end string_rep

	/*
	// To optimize access to the subscript operator, can use this
	// subscript proxy class; however, this creates subtle semantic
	// differences for the user who expects the result of op[] to be
	// char&; some complications may thus arise. So for now this is unused.
	
	// the subscript operator proxy class
	class subscript_proxy
	{
		void operator=(const subscript_proxy& ) { }

	public:
		_string_& s;
		int index;
		subscript_proxy(const subscript_proxy& p) : s(p.s), index(p.index)
			{ }
		subscript_proxy(_string_& str, int pos) : s(str), index(pos)
			{ }
		operator char() const
			{ return s._rep->charAt(index); }
		char& operator=(char c)
			{ s._ensureUnique(); return s._rep->charAt(index) = c; }
		char* operator & ()
			{ s._ensureUnique(); return &s._rep->charAt(index); }
	};

	friend class subscript_proxy;
	public:
	subscript_proxy operator[] (int pos);
	*/

public:
	_string_				( );
	_string_				( const _string_& refstr );
	explicit	_string_	( LPCSTR lpstr );
	explicit	_string_	( LPCWSTR lpwstr );
	explicit	_string_	( char chr, int count = 1 );
	explicit	_string_	( long val );
	explicit	_string_	( double val );
	virtual		~_string_	( );

	_string_&	operator=	( const _string_& refstr );
	_string_&	operator=	( LPCSTR lpstr );
	_string_&	operator=	( LPCWSTR lpwstr );
	_string_&	operator=	( char chr );
	_string_&	operator=	( long val );
	_string_&	operator=	( double val );

	_string_&	operator+=	( const _string_& refstr );
	_string_&	operator+=	( LPCSTR lpstr );
	_string_&	operator+=	( LPCWSTR lpwstr );
	_string_&	operator+=	( char chr );
	_string_&	operator+=	( long val );
	_string_&	operator+=	( double val );

	_string_&	append		( const _string_& refstr );
	_string_&	append		( LPCSTR lpstr );
	_string_&	append		( LPCWSTR lpwstr );
	_string_&	append		( char chr );
	_string_&	append		( long val );
	_string_&	append		( double val );

	bool		operator!	( ) const;
	bool		operator==	( const _string_& refstr ) const;
	bool		operator==	( LPCSTR lpstr ) const;
	bool		operator!=	( const _string_& refstr ) const;
	bool		operator!=	( LPCSTR lpstr ) const;
	bool		operator<	( const _string_& refstr ) const;
	bool		operator<	( LPCSTR lpstr ) const;
	int			compare		( const _string_& refstr, bool case_sensitive = true ) const;
	int			compare		( LPCSTR lpstr, bool case_sensitive = true ) const;
	bool		startsWith	( LPCSTR lpstr, bool case_sensitive = true ) const;
	bool		endsWith	( LPCSTR lpstr, bool case_sensitive = true ) const;

	int			find		( const _string_& refstr, int start = 0,
							  bool case_sensitive = true ) const;
	int			find		( LPCSTR lpstr, int start = 0,
							  bool case_sensitive = true ) const;
	int			findReverse ( LPCSTR lpstr, int start = MAX_INT,
							  bool case_sensitive = true ) const;
	// Wildcard pattern matching; supports * and ?
	bool		matchesPattern
							( LPCSTR pattern, bool case_sensitive = true ) const;

	_string_	substring	( int start ) const;
	_string_	substring	( int start, int count ) const;
	_string_	left		( int count ) const;
	_string_	right		( int count ) const;
	
	inline bool	isNull		( ) const	{ return ( _rep->_p == NULL ); }
	inline int	length		( ) const	{ return _rep->len(); }
	int			capacity	( ) const;

	bool		isNumeric	( ) const;
	long		longVal		( ) const;
	double		doubleVal	( ) const;

	void		swap		( _string_& refstr );
	void		clear		( );
	void		upper		( );
	void		lower		( );
	_string_&	reverse		( );
	
	_string_&	trim		( char char_to_trim = ' ' );
	_string_&	trimLeft	( char char_to_trim = ' ' );
	_string_&	trimRight	( char char_to_trim = ' ' );
	_string_&	padLeft		( int total_length, char char_to_pad = ' ' );
	_string_&	padRight	( int total_length, char char_to_pad = ' ' );
	_string_&	chopLeft	( int chars_to_chop );
	_string_&	chopRight	( int chars_to_chop );

	inline char& operator[]	( int pos ) {
		_ensureUnique();
		// Whoever invokes the [] operator may save a pointer to the
		// retrieved char and later dereference that ptr and assign to it,
		// thereby affecting all strings that are attached to that rep;
		// disallow attachment to such a rep by making it unshareable.
		_rep->markUnshareable();
		return _rep->_p[pos];
	}
	inline char  operator[]	( int pos ) const {
		return _rep->_p[pos];
	}
	inline LPCSTR	c_str	( ) const	{ return _rep->_p; }
	
	_string_&	insert		( LPCSTR insert_str, int start );
	_string_&	replace		( LPCSTR replace_with, int start, int chars_to_del );
	_string_&	insertBytes	( const void* pdata, int byte_count, int start );
	_string_&	replaceBytes( const void* pdata, int byte_count, int start, int bytes_to_del );
	_string_&	replaceOccurences
							( LPCSTR str_to_find, LPCSTR replace_with,
							  int start_pos = 0, int replace_count = -1,
							  bool case_sensitive = true );

	// sprintf-style formatting
	_string_&	format		( LPCSTR format_spec, ... );
	_string_&	formatDate	( LPCSTR date_format, LPCSTR time_format,
							  SYSTEMTIME& systime, bool adjust_for_tz = true );

#ifdef ALL_STRING_STUFF
	long		split		( _array_<_string_>& ret_arr,
							  LPCSTR delim = " ",
							  bool ignore_delim_case = false ) const;
	static _string_ join	( const _array_<_string_>& str_array,
							  LPCSTR delim = " ",
							  int first_index = 0, int last_index = -1 );
#endif

	// the NULL string rep, for consistency of operations
	static string_rep _null_rep;

protected:
	string_rep* _rep;
	
private:
	
	// internal helper functions
	bool _internalPatternMatch	( LPCSTR pattern, LPCSTR name, bool case_sensitive ) const;
	
	inline void _attach			( string_rep* rep );
	inline void _detach			( );
	inline void _reattach		( string_rep* rep );
	inline void _ensureUnique	( );
};

// global comparison func specialization
template<> inline int _compare<_string_>( const _string_& a, const _string_& b )
	{ return a.compare(b); }
// global swap func specialization
template<> inline void _swap<_string_>( _string_* a, _string_* b )
	{ a->swap(*b); }

// externals
_string_ operator+( const _string_&, const _string_& );
_string_ operator+( const _string_&, LPCSTR );
_string_ operator+( const _string_&, char chr );
_string_ operator+( const _string_&, long val );
_string_ operator+( const _string_&, double val );

_string_ operator+( LPCSTR, const _string_& );
_string_ operator+( char, const _string_& );
_string_ operator+( long, const _string_& );
_string_ operator+( double, const _string_& );


};	// namespace soige

#endif  // __string_already_included_vasya__
