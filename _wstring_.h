//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _wstring_.h - header file for class _wstring_ (Unicode)
//
// Includes frequently used operations, and also
// allows having '\0' chars in the middle of the string
// (which, by the way, considerably slows down many operations)
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __wstring_already_included_vasya__
#define __wstring_already_included_vasya__

// to compile _wstring_ with the optional stuff
// #define ALL_STRING_STUFF 1

#include "_common_.h"
#include "_cstring_.h"
// some operations are compiled only if requested
#ifdef ALL_STRING_STUFF
	#include "_array_.h"
#endif

namespace soige {

//------------------------------------------------------------
// The _wstring_ class
//------------------------------------------------------------
class _wstring_
{
friend class _cstring_;
public:
	_wstring_();
	_wstring_(const _wstring_& refstr);
	_wstring_(LPCWSTR lpstr);
	_wstring_(WCHAR chr, int count = 1);
	_wstring_(long val);
	_wstring_(double val);
	virtual ~_wstring_();

	_wstring_& operator= (const _wstring_&);
	_wstring_& operator= (LPCWSTR);
	_wstring_& operator= (WCHAR chr);
	_wstring_& operator= (long val);
	_wstring_& operator= (double val);
	_wstring_& operator= (const _cstring_&);

	_wstring_& operator+=(const _wstring_&);
	_wstring_& operator+=(LPCWSTR);
	_wstring_& operator+=(WCHAR chr);
	_wstring_& operator+=(long val);
	_wstring_& operator+=(double val);
	_wstring_& operator+=(const _cstring_&);

	// operator  int() const;
	bool	  operator! () const;
	bool	  operator< (const _wstring_&) const;
	bool	  operator< (LPCWSTR) const;
	bool	  operator==(const _wstring_&) const;
	bool	  operator==(LPCWSTR) const;
	bool	  operator!=(const _wstring_&) const;
	bool	  operator!=(LPCWSTR) const;
	int		  compare (const _wstring_& refstr, bool case_sensitive = true) const;
	int		  compare (LPCWSTR lpstr, bool case_sensitive = true) const;
	bool	  startsWith(LPCWSTR lpstr, bool case_sensitive = true) const;
	bool	  endsWith  (LPCWSTR lpstr, bool case_sensitive = true) const;

	_wstring_& append(const _wstring_& refstr);
	_wstring_& append(LPCWSTR lpstr);
	_wstring_& append(WCHAR chr);
	_wstring_& append(long val);
	_wstring_& append(double val);

	// more efficient swap from another string
	// than just copying byval back and forth
	void	  swap(_wstring_& refstr);
	
	_wstring_& insert (LPCWSTR insert_str, int start);
	_wstring_& replace(LPCWSTR replace_with, int start, int chars_to_del);
	_wstring_& replaceOccurences(LPCWSTR str_to_find, LPCWSTR replace_with,
								int start_pos = 0, int replace_count = -1,
								bool case_sensitive = true);

#ifdef ALL_STRING_STUFF
	long			split(_array_<_wstring_>& ret_arr,
						  LPCWSTR delim = L" ",
						  bool ignore_delim_case = false);
	static _wstring_	join (const _array_<_wstring_>& str_array,
						  LPCWSTR delim = L" ",
						  int first_index = 0, int last_index = -1);
	_wstring_&		formatDate (LPCWSTR date_format, LPCWSTR time_format,
								SYSTEMTIME& sysTime, bool adjustForTZ = true);
#endif

	_wstring_  substring(int start) const;
	_wstring_  substring(int start, int count) const;
	_wstring_  left (int count) const;
	_wstring_  right(int count) const;
	
	_wstring_& reverse();
	_wstring_& trim(WCHAR char_to_trim = L' ');
	_wstring_& padRight(int total_length, WCHAR char_to_pad = L' ');
	_wstring_& padLeft (int total_length, WCHAR char_to_pad = L' ');
	// remove characters off the head or tail
	_wstring_& chopLeft(int chars_to_chop);
	_wstring_& chopRight(int chars_to_chop);
	// sprintf-style formatting
	_wstring_& format(LPCWSTR format_spec, ...);

	WCHAR&	  operator[](int pos);
	WCHAR	  getChar(int pos) const;
	
	operator  LPCWSTR() const;
	void	  clear();
	
	void	  upper();
	void	  lower();

	int		  find(const _wstring_& refstr, int start = 0, bool case_sensitive = true) const;
	int		  find(LPCWSTR lpstr, int start = 0, bool case_sensitive = true) const;
	int		  findReverse(LPCWSTR lpstr, int start = MAX_INT, bool case_sensitive = true) const;

	bool	  isNumeric() const;
	long	  longVal  () const;
	double	  doubleVal() const;

	bool	  isNull  () const;
	int		  length  () const;
	int		  capacity() const;

protected:
	LPWSTR	_p;
	int		_len;		// current length of the string, in characters
	int		_buflen;	// size of buffer, in characters
	
private:
	// memory operations
	bool _alloc  (int);
	bool _realloc(int);
	void _free   ();
};

// global comparison func specialization
template<> inline int _compare<_wstring_>(const _wstring_& a, const _wstring_& b)
	{ return a.compare(b); }
// global swap func specialization
template<> inline void _swap<_wstring_>(_wstring_* a, _wstring_* b)
	{ a->swap(*b); }

// externals
_wstring_ operator+(const _wstring_&, const _wstring_&);
_wstring_ operator+(const _wstring_&, LPCWSTR);
_wstring_ operator+(const _wstring_&, WCHAR chr);
_wstring_ operator+(const _wstring_&, long val);
_wstring_ operator+(const _wstring_&, double val);

_wstring_ operator+(LPCWSTR, const _wstring_&);
_wstring_ operator+(WCHAR, const _wstring_&);
_wstring_ operator+(long, const _wstring_&);
_wstring_ operator+(double, const _wstring_&);


};	// namespace soige

#endif  // __wstring_already_included_vasya__
