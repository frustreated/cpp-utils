//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _wstring_.cpp - implementation of class _wstring_
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_wstring_.h"

namespace soige {

//------------------------------------------------
// Constructors
//------------------------------------------------
_wstring_::_wstring_()
{
	_p = NULL;
	_len = _buflen = 0;
}

_wstring_::_wstring_(const _wstring_& refstr)
{
	_p = NULL;
	_len = _buflen = 0;
	this->operator=(refstr);
}

_wstring_::_wstring_(LPCWSTR lpstr)
{
	_p = NULL;
	_len = _buflen = 0;
	if(NULL == lpstr) return;

	int cch = lstrlenW(lpstr) + 1;
	if( !_alloc(cch) ) return;
	lstrcpyW(_p, lpstr);
	_len = cch-1;
}

_wstring_::_wstring_(WCHAR chr, int count)
{
	_p = NULL;
	_len = _buflen = 0;
	if(count < 1) return;

	int cch = count+1;
	if( !_alloc(cch) ) return;
	//wcsnset(_p, chr, count); doesn't work, have to do it manually
	cch = 0;
	while( _p[cch] = chr, ++cch < count ) ;
	_p[count] = L'\0';
	_len = count;
}

_wstring_::_wstring_(long val)
{
	_p = NULL;
	_len = _buflen = 0;
	this->operator=(val);
}

_wstring_::_wstring_(double val)
{
	_p = NULL;
	_len = _buflen = 0;
	this->operator=(val);
}

_wstring_::~_wstring_()
{
	_free();
}

//------------------------------------------------
// Operations
//------------------------------------------------
_wstring_& _wstring_::operator=(const _wstring_& refstr)
{
	if(_p == refstr._p) return *this;
	if(NULL == refstr._p) return _free(), *this;

	int cch = refstr._len + 1;
	if( !_alloc(cch) ) return *this;
	memmove ( _p, refstr._p, cch*sizeof(WCHAR) );
	_len = refstr._len;
	return *this;
}

_wstring_& _wstring_::operator=(LPCWSTR lpstr)
{
	if(NULL == lpstr) return _free(), *this;

	// to avoid problems if lpstr is part of this->_p, need a temp _wstring_
	_wstring_ s(lpstr);
	swap(s);
	return *this;
}

_wstring_& _wstring_::operator=(WCHAR chr)
{
	_free();
	return this->operator+=(chr);
}

_wstring_& _wstring_::operator=(long val)
{
	_free();
	return this->operator+=(val);
}

_wstring_& _wstring_::operator=(double val)
{
	_free();
	return this->operator+=(val);
}

_wstring_& _wstring_::operator=(const _cstring_& refcstr)
{
	if(NULL == refcstr._p) return _free(), *this;

	int cch = refcstr._len + 1;
	if( !_alloc(cch) ) return *this;
	_len = refcstr._len;
	mbstowcs(_p, refcstr._p, _len+1);
	return *this;
}

_wstring_& _wstring_::operator+=(const _wstring_& refstr)
{
	if(refstr._len <= 0) return *this;
	
	if(_len+refstr._len >= _buflen)
		if( !_realloc(_len+refstr._len+1) ) return *this;
	memmove ( &_p[_len], refstr._p, (refstr._len+1)*sizeof(WCHAR) );
	_len += refstr._len;
	return *this;
}

_wstring_& _wstring_::operator+=(LPCWSTR lpstr)
{
	if( NULL == lpstr || 0 == lpstr[0] ) return *this;
	
	int addsize = lstrlenW(lpstr);
	if(_len+addsize >= _buflen)
		if( !_realloc(_len+addsize+1) ) return *this;
	memmove ( &_p[_len], lpstr, (addsize+1)*sizeof(WCHAR) );
	_len += addsize;
	return *this;
}

_wstring_& _wstring_::operator+=(WCHAR chr)
{
	int cch = _len+1;

	if(cch >= _buflen)
		if( !_realloc(cch) ) return *this;
	_p[_len] = chr;
	_p[++_len] = L'\0';
	return *this;
}

_wstring_& _wstring_::operator+=(long val)
{
	WCHAR buf[32];
	_ltow(val, buf, 10);
	return this->operator+=(buf);
}

_wstring_& _wstring_::operator+=(double val)
{
	char buf[64];
	_gcvt(val, 8, buf);
	WCHAR wbuf[64];
	mbstowcs ( wbuf, buf, sizeof(wbuf)/sizeof(wbuf[0]) );
	return this->operator+=(wbuf);
}

_wstring_& _wstring_::operator+=(const _cstring_& refcstr)
{
	if(refcstr._len == 0) return *this;
	
	if(_len+refcstr._len >= _buflen)
		if( !_realloc(_len + refcstr._len + 1) ) return *this;
	mbstowcs ( &_p[_len], refcstr._p, refcstr._len+1 );
	_len += refcstr._len;
	return *this;
}

/*
// if(s) { ... }
_wstring_::operator int() const
{
	return (_p?1:0);
}
*/

// if(!s) { ... }
bool _wstring_::operator!() const
{
	return (!_p);
}

// comparison operators are case-sensitive
bool _wstring_::operator<(const _wstring_& refstr) const
{
	return (compare(refstr) < 0);
}

bool _wstring_::operator<(LPCWSTR lpstr) const
{
	return (compare(lpstr) < 0);
}

bool _wstring_::operator==(const _wstring_& refstr) const
{
	if( _len == refstr._len && memcmp(_p, refstr._p, _len*sizeof(WCHAR)) == 0 )
		return true;
	return false;
}

bool _wstring_::operator==(LPCWSTR lpstr) const
{
	return (compare(lpstr, false) == 0);
}

bool _wstring_::operator!=(const _wstring_& refstr) const
{
	if( _len == refstr._len && memcmp(_p, refstr._p, _len*sizeof(WCHAR)) == 0 )
		return false;
	return true;
}

bool _wstring_::operator!=(LPCWSTR lpstr) const
{
	return (compare(lpstr, false) != 0);
}

int _wstring_::compare(const _wstring_& refstr, bool case_sensitive) const
{

	int result;
	// one or both strings can be null, have to guard against that
	if(case_sensitive)
		result = memcmp (_p, refstr._p, (_len <= refstr._len)? _len*sizeof(WCHAR) : refstr._len*sizeof(WCHAR));
	else
		result = memicmp(_p, refstr._p, (_len <= refstr._len)? _len*sizeof(WCHAR) : refstr._len*sizeof(WCHAR));
	
	if(result == 0)	return ( (_len == refstr._len) ? 0 : ((_len > refstr._len) ? 1 : -1) );
	else			return result;
}

int _wstring_::compare(LPCWSTR lpstr, bool case_sensitive) const
{
	if(case_sensitive)	return lstrcmpW (_p, lpstr);
	else				return lstrcmpiW(_p, lpstr);
}

bool _wstring_::startsWith(LPCWSTR lpstr, bool case_sensitive) const
{
	int len = lstrlenW(lpstr);
	if(len > _len) return false;

	if(case_sensitive)	return memcmp (_p, lpstr, len*sizeof(WCHAR)) == 0;
	else				return memicmp(_p, lpstr, len*sizeof(WCHAR)) == 0;
}

bool _wstring_::endsWith(LPCWSTR lpstr, bool case_sensitive) const
{
	int len = lstrlenW(lpstr);
	if(len > _len) return false;

	if(case_sensitive)	return memcmp (&_p[_len-len], lpstr, len*sizeof(WCHAR)) == 0;
	else				return memicmp(&_p[_len-len], lpstr, len*sizeof(WCHAR)) == 0;
}

_wstring_& _wstring_::append(const _wstring_& refstr)
{
	return this->operator+=(refstr);
}

_wstring_& _wstring_::append(LPCWSTR lpstr)
{
	return this->operator+=(lpstr);
}

_wstring_& _wstring_::append(WCHAR chr)
{
	return this->operator+=(chr);
}

_wstring_& _wstring_::append(long val)
{
	return this->operator+=(val);
}

_wstring_& _wstring_::append(double val)
{
	return this->operator+=(val);
}

void _wstring_::swap(_wstring_& refstr)
{
	LPWSTR temp_p = _p;
	int temp_len = _len;
	int temp_buflen = _buflen;
	
	_p = refstr._p;
	_len = refstr._len;
	_buflen = refstr._buflen;

	refstr._p = temp_p;
	refstr._len = temp_len;
	refstr._buflen = temp_buflen;
}

_wstring_& _wstring_::insert(LPCWSTR insert_str, int start)
{
	return replace(insert_str, start, 0);
}

// Removes @chars_to_del chars from the string starting at position @start,
// and inserts entire @replace_with string at that location
_wstring_& _wstring_::replace(LPCWSTR replace_with, int start, int chars_to_del)
{
	if(start < 0 || chars_to_del < 0 || NULL == replace_with) return *this;
	if(start+chars_to_del > _len) chars_to_del = _len-start;
	int rlen = lstrlenW(replace_with);
	int newlen = _len + rlen - chars_to_del;
	if( !_realloc(newlen) ) return *this;
	memmove ( &_p[start+rlen], &_p[start+chars_to_del], (_len-start-chars_to_del)*sizeof(WCHAR) );
	memcpy ( &_p[start], replace_with, rlen*sizeof(WCHAR) );
	_len = newlen;
	_p[_len] = L'\0';
	return *this;
}

// @replace_count - how many occurences to replace.
// default - -1 (i.e., replace all occurences found)
_wstring_& _wstring_::replaceOccurences(LPCWSTR str_to_find, LPCWSTR replace_with,
										int start_pos, int replace_count, bool case_sensitive)
{
	if( NULL == str_to_find || 0 == str_to_find[0] || NULL == replace_with ||
		0 == replace_count || start_pos >= _len || start_pos < 0 )
		return *this;
	
	int len_1 = lstrlenW(str_to_find);
	// to avoid problems if replace_with is part (or whole) of this->_p
	_wstring_ s(replace_with);
	int len_2 = s.length();
	int pos = start_pos;

	if(-1 == replace_count)
		replace_count = MAX_INT;  // maximum positive value of int (to replace all)

	pos = find(str_to_find, pos, case_sensitive);
	while(pos>=0 && replace_count>0)
	{
		replace(s._p, pos, len_1);
		pos += len_2;
		replace_count--;
		pos = find(str_to_find, pos, case_sensitive);
	}

	return *this;
}

#ifdef ALL_STRING_STUFF
// split and join are defined only if explicitly requested
// to avoid making _wstring_ unnecessarily heavy by including
// the _array_ code in it. split returns the count of items
// returned in ret_arr
long _wstring_::split( _array_<_wstring_>& ret_arr,
					  LPCWSTR delim, bool ignore_delim_case )
{
	ret_arr.clear();
	if( _len <= 0 || NULL == _p || NULL == delim || delim[0] == L'\0' )
		return 0;
	
	// use @delim to split a string into string array
	int pos, prevpos;
	int delim_len = lstrlenW(delim);

	prevpos = 0;
	pos = find(delim, 0, ignore_delim_case);
	while(pos != -1)
	{
		ret_arr.append( substring(prevpos, pos-prevpos) );
		prevpos = pos + delim_len;
		pos = find(delim, prevpos, ignore_delim_case);
	}
	ret_arr.append( substring(prevpos) );
		
	return ret_arr.length();
}

// default for @delim = " "; @first_index = 0; @last_index = -1 (until last elem)
/* static */
_wstring_ _wstring_::join(const _array_<_wstring_>& str_array,
						  LPCWSTR delim, int first_index, int last_index)
{
	_wstring_ retval;
	bool empty_delim = ( NULL == delim || delim[0] == L'\0' );
	if(str_array.length() == 0) return retval;
	
	// use @delim to join the values in the string array
	// from @first_index to @last_index into one string
	if(first_index < 0)
		first_index = 0;
	if(last_index == -1 || last_index > str_array.length()-1)
		last_index = str_array.length() - 1;
	for(int i=first_index; i<=last_index; i++)
	{
		retval.append(str_array[i]);
		if( !empty_delim && i != last_index )
			retval.append(delim);
	}
	
	return retval;
}

/*
+++ Date Format Specifiers +++
d		Day of month as digits with no leading zero for single-digit days. 
dd		Day of month as digits with leading zero for single-digit days. 
ddd		Day of week as a three-letter abbreviation. The function uses the LOCALE_SABBREVDAYNAME value associated with the specified locale. 
dddd	Day of week as its full name. The function uses the LOCALE_SDAYNAME value associated with the specified locale. 
M		Month as digits with no leading zero for single-digit months. 
MM		Month as digits with leading zero for single-digit months. 
MMM		Month as a three-letter abbreviation. The function uses the LOCALE_SABBREVMONTHNAME value associated with the specified locale. 
MMMM	Month as its full name. The function uses the LOCALE_SMONTHNAME value associated with the specified locale. 
y		Year as last two digits, but with no leading zero for years less than 10. 
yy		Year as last two digits, but with leading zero for years less than 10. 
yyyy	Year represented by full four digits. 
gg		Period/era string. The function uses the CAL_SERASTRING value associated with the specified locale. This element is ignored if the date to be formatted does not have an associated era or period string. 

+++ Time Format Specifiers +++
h		Hours with no leading zero for single-digit hours; 12-hour clock. 
hh		Hours with leading zero for single-digit hours; 12-hour clock. 
H		Hours with no leading zero for single-digit hours; 24-hour clock. 
HH		Hours with leading zero for single-digit hours; 24-hour clock. 
m		Minutes with no leading zero for single-digit minutes. 
mm		Minutes with leading zero for single-digit minutes. 
s		Seconds with no leading zero for single-digit seconds. 
ss		Seconds with leading zero for single-digit seconds. 
t		One character time-marker string, such as A or P. 
tt		Multicharacter time-marker string, such as AM or PM. 
*/
_wstring_& _wstring_::formatDate(LPCWSTR date_format, LPCWSTR time_format, SYSTEMTIME& st, bool adjustForTZ)
{
	WCHAR bufdate[64];
	_wstring_ dfmt(date_format);
	_wstring_ tfmt(time_format);
	_wstring_ result;

	// adjust for the timezone
	if(adjustForTZ)
	{
		TIME_ZONE_INFORMATION tzi;
		unsigned __int64 tzBias = 0;
		if(GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_DAYLIGHT)
			tzBias = tzi.Bias + tzi.DaylightBias;
		else
			tzBias = tzi.Bias;
		tzBias *= 600000000; // FILETIME is in 100 nanoseconds intervals, 600000000 == 1 minute
		// localtime = utc - bias
		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);
		unsigned __int64 t = 0;
		memcpy ( &t, &ft, sizeof(t) );
		t -= tzBias;
		memcpy ( &ft, &t, sizeof(t) );
		FileTimeToSystemTime(&ft, &st);
	}

	if( dfmt.isNull() || dfmt.length() == 0 )
		result = L"";
	else
	{
		// adjust for month format mistakes
		dfmt.replaceOccurences(L"mmmm", L"MMMM");
		dfmt.replaceOccurences(L"mmm", L"MMM");
		dfmt.replaceOccurences(L"mm", L"MM");
		dfmt.replaceOccurences(L"m", L"M");
		// replace ' with ''''
		dfmt.replaceOccurences(L"\'", L"\'\'\'\'");
		GetDateFormatW(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &st, (LPCWSTR)dfmt, bufdate, sizeof(bufdate)/sizeof(bufdate[0]));
		result = bufdate;
	}

	if( tfmt.isNull() || tfmt.length() == 0 )
		result += L"";
	else
	{
		// adjust for minute format mistakes
		tfmt.replaceOccurences(L"MM", L"mm");
		tfmt.replaceOccurences(L"M", L"m");
		// replace ' with ''''
		tfmt.replaceOccurences(L"\'", L"\'\'\'\'");
		GetTimeFormatW(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &st, (LPCWSTR)tfmt, bufdate, sizeof(bufdate)/sizeof(bufdate[0]));
		if(result.length() == 0)
			result += bufdate;
		else
			result.append(L" ").append(bufdate);
	}

	swap(result);
	return *this;
}

#endif // ALL_STRING_STUFF

_wstring_ _wstring_::substring(int start) const
{
	return substring(start, _len);
}

_wstring_ _wstring_::substring(int start, int count) const
{
	if(0 == start && count >= _len) return *this;
	_wstring_ newstr;
	if(start < 0 || start >= _len || count <= 0)
		return ( newstr.operator=(L"") );
	int tocopy = (count>_len-start) ? (_len-start) : count;
	if( !newstr._alloc(tocopy) ) return newstr;
	memmove ( newstr._p, &_p[start], tocopy*sizeof(WCHAR) );
	newstr._p[tocopy] = L'\0';
	newstr._len = tocopy;
	return newstr;
}

_wstring_ _wstring_::left(int count) const
{
	return substring(0, count);
}

_wstring_ _wstring_::right(int count) const
{
	return substring(_len-count, count);
}

_wstring_& _wstring_::reverse()
{
	if(_len<=0 || NULL==_p) return *this;

	int i = -1, half = (int)(_len/2);
	WCHAR t;
	while(++i < half)
	{
		t = _p[_len-i-1];
		_p[_len-i-1] = _p[i];
		_p[i] = t;
	}
	return *this;
}

// The default for @char_to_trim is space
_wstring_& _wstring_::trim(WCHAR char_to_trim)
{
	if(_len <= 0 || NULL == _p) return *this;
	
	// trim right side
	while(char_to_trim == _p[_len-1])
		_len--;
	_p[_len] = L'\0';

	// trim left side
	int pos = 0;
	while(char_to_trim == _p[pos])
		pos++;
	if(pos)
	{
		memmove ( _p, &_p[pos], (_len+1-pos)*sizeof(WCHAR) );
		_len -= pos;
	}
	return *this;
}

_wstring_& _wstring_::padRight(int total_length, WCHAR char_to_pad)
{
	if(total_length <= _len) return *this;
	
	if(total_length >= _buflen)
		if( !_realloc(total_length+1) ) return *this;
	while( _p[_len] = char_to_pad, ++_len < total_length ) ;
	_p[_len] = '\0';
	return *this;
}

_wstring_& _wstring_::padLeft(int total_length, WCHAR char_to_pad)
{
	if(total_length <= _len) return *this;

	_wstring_ s(char_to_pad, total_length-_len);
	s.operator += (*this);
	swap(s);
	return *this;
}

_wstring_& _wstring_::chopLeft(int chars_to_chop)
{
	if(_len <= 0) return *this;
	if(chars_to_chop > _len) chars_to_chop = _len;
	
	// remove the front chars_to_chop characters
	memmove ( _p, &_p[chars_to_chop], (_len+1-chars_to_chop)*sizeof(WCHAR) );
	_len -= chars_to_chop;
	return *this;
}

_wstring_& _wstring_::chopRight(int chars_to_chop)
{
	if(_len <= 0) return *this;
	if(chars_to_chop > _len) chars_to_chop = _len;

	// remove the last chars_to_chop characters
	_len -= chars_to_chop;
	_p[_len] = L'\0';
	return *this;
}

_wstring_& _wstring_::format(LPCWSTR format_spec, ...)
{
	va_list args;
	va_start(args, format_spec);
	
	// to avoid problems if format_spec or any other argument
	// is/are part (or whole) of this->_p
	_wstring_ result;
	int cch = 0;

#if defined(_USE_NO_CRT) && !defined(_DEBUG) && !defined(DEBUG)
	if( !result._alloc( 8192 ) )  // hopefully is enough
	{
		va_end(args);
		return *this;
	}

	if( (cch = wvsprintfW(result._p, format_spec, args)) <
		lstrlenW(format_spec) )
	{
		va_end(args);
		return *this;
	}
	// compact
	result._realloc( cch+1 );
#else
	// try increasing buffer size until all args are written
	int size = 32;
	do
	{
		size += 32;
		if( !result._alloc(size) ) break;
	} while ( (cch = _vsnwprintf(result._p, size, format_spec, args)) < 0 );
#endif

	va_end(args);

	result._len = cch;
	swap(result);
	return *this;
}

WCHAR& _wstring_::operator[](int pos)
{
	return _p[pos];
}

WCHAR _wstring_::getChar(int pos) const
{
	return _p[pos];
}

_wstring_::operator LPCWSTR() const
{
	return _p;
}

void _wstring_::clear()
{
	_free();
}

void _wstring_::upper()
{
	if(!_p) return;
	int i = -1;
	while(++i < _len) _p[i] = (WCHAR)towupper(_p[i]);
}

void _wstring_::lower()
{
	if(!_p) return;
	int i = -1;
	while(++i < _len) _p[i] = (WCHAR)towlower(_p[i]);
}

int _wstring_::find(const _wstring_& refstr, int start, bool case_sensitive) const
{
	if(start < 0 || start >= _len || refstr._len > _len) return -1;
	if(refstr._len == 0) return start;

	int i;
	int until = _len - refstr._len;

	for(; start<=until; start++)
	{
		if( (_p[start] == refstr._p[0] ) ||
			(!case_sensitive && towlower(_p[start]) == towlower(refstr._p[0]))
		  )
		{
			i = 0;
			while(i < refstr._len)
			{
				if( (_p[start+i] == refstr._p[i] ) ||
					(!case_sensitive && towlower(_p[start+i]) == towlower(refstr._p[i]))
				  )
					i++;
				else break;
			}
			if(i == refstr._len) return start; // reached the end and matched so far - full match
		}
	}

	return -1;
}

int _wstring_::find(LPCWSTR lpstr, int start, bool case_sensitive) const
{
	if(start < 0 || start >= _len) return -1;
	if(lpstr[0] == L'\0') return start;
	int slen = lstrlenW(lpstr);
	if(slen > _len) return -1;
	int i;
	int until = _len - slen;

	// there can be embedded null chars in this->_p, so can't just use strstr
	for(; start<=until; start++)
	{
		if( (_p[start] == lpstr[0] ) ||
			(!case_sensitive && towlower(_p[start]) == towlower(lpstr[0]))
		  )
		{
			i = 0;
			while(i < slen)
			{
				if( (_p[start+i] == lpstr[i] ) ||
					(!case_sensitive && towlower(_p[start+i]) == towlower(lpstr[i]))
				  )
					i++;
				else break;
			}
			if(i == slen) return start; // reached the end and matched so far - full match
		}
	}

	return -1;
}

// The arg @start is from the beginning of the string, not from the end.
// If not provided, search starts from the end of the string
int _wstring_::findReverse(LPCWSTR lpstr, int start, bool case_sensitive) const
{
	if(start < 0 || NULL == lpstr) return -1;
	if(0 == lpstr[0]) return _len-1;

	if(start >= _len) start = _len-1;
	int match_pos = -1;
	while(start >= 0)
	{
		if( (match_pos = find(lpstr, start, case_sensitive)) >= 0 )
			return match_pos;
		start--;
	}
	return -1;
}

bool _wstring_::isNumeric() const
{
	if(!_p || _len < 1) return false;
	
	WCHAR* pw = _p;
	while( iswspace(*pw) ) pw++;
	if(*pw == L'-' || *pw == L'+') pw++;
	if(*pw == L'.') pw++;
	if(*pw == L'd' || *pw == L'D' || *pw == L'e' || *pw == L'E') pw++;
	if(*pw == L'-' || *pw == L'+') pw++;
	if( (pw >= _p+_len) || (*pw != L'0' && wcstod(_p, NULL) == 0) )
		return false;
	return true;
}

long _wstring_::longVal() const
{
	if(_len < 1) return 0;
	return _wtol(_p);
}

double _wstring_::doubleVal() const
{
	if(_len < 1) return 0.0;
	return wcstod(_p, NULL);
}

bool _wstring_::isNull() const
{
	return (NULL == _p);
}

int _wstring_::length() const
{
	return _len;
}

int _wstring_::capacity() const
{
	return _buflen-1;
}

//------------------------------------------------
// Internal helper functions
//------------------------------------------------

// allocate the specified number of characters
bool _wstring_::_alloc(int cch)
{
	_free();
	return _realloc(cch);
}

// allocate the specified number of characters
// and move the existing string (if any) over to it
bool _wstring_::_realloc(int cch)
{
	cch = (cch >= 160) ? ((int)(cch*1.25)) : (cch+16);

	if(_p) _p = (LPWSTR) realloc(_p, cch * sizeof(WCHAR));
	else {
		_p = (LPWSTR) malloc(cch * sizeof(WCHAR));
		if(_p) _p[0] = L'\0';
	}
	
	if(!_p) return _buflen=0, false;
	if(_buflen > cch) _p[cch-1] = L'\0';
	_buflen = cch;
	return true;
}

void _wstring_::_free()
{
	if(_p) free(_p);
	_p = NULL;
	_len = _buflen = 0;
}


//------------------------------------------------
// Non-member concatenation operations
//------------------------------------------------
_wstring_ operator+(const _wstring_& s1, const _wstring_& s2)
{
	_wstring_ s(s1);
	s.operator+=(s2);
	return s;
}

_wstring_ operator+(const _wstring_& refstr, LPCWSTR lpstr)
{
	_wstring_ s(refstr);
	s.operator+=(lpstr);
	return s;
}

_wstring_ operator+(const _wstring_& refstr, WCHAR chr)
{
	_wstring_ s(refstr);
	s.operator+=(chr);
	return s;
}

_wstring_ operator+(const _wstring_& refstr, long val)
{
	_wstring_ s(refstr);
	s.operator+=(val);
	return s;
}

_wstring_ operator+(const _wstring_& refstr, double val)
{
	_wstring_ s(refstr);
	s.operator+=(val);
	return s;
}

_wstring_ operator+(LPCWSTR lpstr, const _wstring_& refstr)
{
	_wstring_ s(lpstr);
	s.operator+=(refstr);
	return s;
}

_wstring_ operator+(WCHAR chr, const _wstring_& refstr)
{
	_wstring_ s(chr);
	s.operator+=(refstr);
	return s;
}

_wstring_ operator+(long val, const _wstring_& refstr)
{
	_wstring_ s(val);
	s.operator+=(refstr);
	return s;
}

_wstring_ operator+(double val, const _wstring_& refstr)
{
	_wstring_ s(val);
	s.operator+=(refstr);
	return s;
}

};	// namespace soige

