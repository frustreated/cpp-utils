//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _cstring_.h - header file for class _cstring_
//
// Includes frequently used operations, and also
// allows having '\0' chars in the middle of the string
// (which, by the way, considerably slows down many operations)
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-------------------------------------------------------------
// "Every programmer sooner or later writes a string class"
// - Alex Rozenbaum
//-------------------------------------------------------------

#ifndef __cstring_already_included_vasya__
#define __cstring_already_included_vasya__

// to compile _cstring_ with the optional stuff
// #define ALL_STRING_STUFF 1

#include "_common_.h"
#include "_wstring_.h"
// some operations are compiled only if requested
#ifdef ALL_STRING_STUFF
	#include "_array_.h"
#endif

namespace soige {

//------------------------------------------------------------
// The _cstring_ class
//------------------------------------------------------------
class _cstring_
{
friend class _wstring_;
public:
	_cstring_();
	_cstring_(const _cstring_& refstr);
	_cstring_(LPCSTR lpstr);
	_cstring_(char chr, int count = 1);
	_cstring_(long val);
	_cstring_(double val);
	virtual ~_cstring_();

	_cstring_&	operator= (const _cstring_&);
	_cstring_&	operator= (LPCSTR);
	_cstring_&	operator= (char chr);
	_cstring_&	operator= (long val);
	_cstring_&	operator= (double val);
	_cstring_&	operator= (const _wstring_&);

	_cstring_&	operator+=(const _cstring_&);
	_cstring_&	operator+=(LPCSTR);
	_cstring_&	operator+=(char chr);
	_cstring_&	operator+=(long val);
	_cstring_&	operator+=(double val);
	_cstring_&	operator+=(const _wstring_&);

	bool		operator! () const;
	bool		operator< (const _cstring_&) const;
	bool		operator< (LPCSTR) const;
	bool		operator==(const _cstring_&) const;
	bool		operator==(LPCSTR) const;
	bool		operator!=(const _cstring_&) const;
	bool		operator!=(LPCSTR) const;
	int			compare (const _cstring_& refstr, bool case_sensitive = true) const;
	int			compare (LPCSTR lpstr, bool case_sensitive = true) const;
	bool		startsWith(LPCSTR lpstr, bool case_sensitive = true) const;
	bool		endsWith  (LPCSTR lpstr, bool case_sensitive = true) const;

	_cstring_&	append(const _cstring_& refstr);
	_cstring_&	append(LPCSTR lpstr);
	_cstring_&	append(char chr);
	_cstring_&	append(long val);
	_cstring_&	append(double val);

	// more efficient swap from another string
	// than just copying byval back and forth
	void		swap(_cstring_& refstr);
	
	_cstring_&	insert (LPCSTR insert_str, int start);
	_cstring_&	replace(LPCSTR replace_with, int start, int chars_to_del);
	_cstring_&	insertBytes (const void* pData, int byteCount, int start);
	_cstring_&	replaceBytes(const void* pData, int byteCount, int start, int bytes_to_del);
	_cstring_&	replaceOccurences(LPCSTR str_to_find, LPCSTR replace_with,
								  int start_pos = 0, int replace_count = -1,
								  bool case_sensitive = true);

#ifdef ALL_STRING_STUFF
	long				split(_array_<_cstring_>& ret_arr,
							  LPCSTR delim = " ",
							  bool ignore_delim_case = false);
	static _cstring_	join (const _array_<_cstring_>& str_array,
							  LPCSTR delim = " ",
							  int first_index = 0, int last_index = -1);
	_cstring_&		formatDate (LPCSTR date_format, LPCSTR time_format,
								SYSTEMTIME& sysTime, bool adjustForTZ = true);
#endif

	_cstring_	substring(int start) const;
	_cstring_	substring(int start, int count) const;
	_cstring_	left (int count) const;
	_cstring_	right(int count) const;
	
	_cstring_&	reverse();
	_cstring_&	trim(char char_to_trim = ' ');
	_cstring_&	padRight(int total_length, char char_to_pad = ' ');
	_cstring_&	padLeft (int total_length, char char_to_pad = ' ');
	// remove characters off the head or tail
	_cstring_&	chopLeft(int chars_to_chop);
	_cstring_&	chopRight(int chars_to_chop);
	// sprintf-style formatting
	_cstring_&	format(LPCSTR format_spec, ...);

	char&		operator[](int pos);
	char		getChar(int pos) const;
	
	operator	LPCSTR() const;
	void		clear();
	
	void		upper();
	void		lower();

	int			find(const _cstring_& refstr, int start = 0, bool case_sensitive = true) const;
	int			find(LPCSTR lpstr, int start = 0, bool case_sensitive = true) const;
	int			findReverse(LPCSTR lpstr, int start = MAX_INT, bool case_sensitive = true) const;

	bool		isNumeric() const;
	long		longVal  () const;
	double		doubleVal() const;

	bool		isNull  () const;
	int			length  () const;
	int			capacity() const;

protected:
	LPSTR	_p;
	int		_len;		// current length of the string, in characters
	int		_buflen;	// size of buffer, in characters
	
private:
	// memory operations
	bool _alloc  (int);
	bool _realloc(int);
	void _free   ();
};

// global comparison func specialization
template<> inline int _compare<_cstring_>(const _cstring_& a, const _cstring_& b)
	{ return a.compare(b); }
// global swap func specialization
template<> inline void _swap<_cstring_>(_cstring_* a, _cstring_* b)
	{ a->swap(*b); }

// externals
_cstring_ operator+(const _cstring_&, const _cstring_&);
_cstring_ operator+(const _cstring_&, LPCSTR);
_cstring_ operator+(const _cstring_&, char chr);
_cstring_ operator+(const _cstring_&, long val);
_cstring_ operator+(const _cstring_&, double val);

_cstring_ operator+(LPCSTR, const _cstring_&);
_cstring_ operator+(char, const _cstring_&);
_cstring_ operator+(long, const _cstring_&);
_cstring_ operator+(double, const _cstring_&);


};	// namespace soige

#endif  // __cstring_already_included_vasya__
