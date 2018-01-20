//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _cstring_.cpp - implementation of class _cstring_
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_cstring_.h"

namespace soige {

//------------------------------------------------
// Constructors
//------------------------------------------------
_cstring_::_cstring_()
{
	_p = NULL;
	_len = _buflen = 0;
}

_cstring_::_cstring_(const _cstring_& refstr)
{
	_p = NULL;
	_len = _buflen = 0;
	this->operator=(refstr);
}

_cstring_::_cstring_(LPCSTR lpstr)
{
	_p = NULL;
	_len = _buflen = 0;
	if(NULL == lpstr) return;

	int cch = lstrlenA(lpstr) + 1;
	if( !_alloc(cch) ) return;
	lstrcpyA(_p, lpstr);
	_len = (cch-1);
}

_cstring_::_cstring_(char chr, int count)
{
	_p = NULL;
	_len = _buflen = 0;
	if(count < 1) return;

	int cch = count+1;
	if( !_alloc(cch) ) return;
	//_strnset(_p, chr, count); doesn't work, have to do it manually
	cch = 0;
	while( _p[cch] = chr, ++cch < count ) ;
	_p[count] = '\0';
	_len = count;
}

_cstring_::_cstring_(long val)
{
	_p = NULL;
	_len = _buflen = 0;
	this->operator=(val);
}

_cstring_::_cstring_(double val)
{
	_p = NULL;
	_len = _buflen = 0;
	this->operator=(val);
}

_cstring_::~_cstring_()
{
	_free();
}

//------------------------------------------------
// Operations
//------------------------------------------------
_cstring_& _cstring_::operator=(const _cstring_& refstr)
{
	if(_p == refstr._p) return *this;
	if(NULL == refstr._p) return _free(), *this;

	int cch = refstr._len+1;
	if( !_alloc(cch) ) return *this;
	memmove(_p, refstr._p, cch);
	_len = refstr._len;
	return *this;
}

_cstring_& _cstring_::operator=(LPCSTR lpstr)
{
	if(NULL == lpstr) return _free(), *this;

	// to avoid problems if lpstr is part of this->_p, need a temp _cstring_
	_cstring_ s(lpstr);
	swap(s);
	return *this;
}

_cstring_& _cstring_::operator=(char chr)
{
	_free();
	return this->operator+=(chr);
}

_cstring_& _cstring_::operator=(long val)
{
	_free();
	return this->operator+=(val);
}

_cstring_& _cstring_::operator=(double val)
{
	_free();
	return this->operator+=(val);
}

_cstring_& _cstring_::operator=(const _wstring_& refwstr)
{
	if(NULL == refwstr._p) return _free(), *this;

	int cch = refwstr._len+1;
	if( !_alloc(cch) ) return *this;
	_len = refwstr._len;
	wcstombs(_p, refwstr._p, _len+1);
	return *this;
}

_cstring_& _cstring_::operator+=(const _cstring_& refstr)
{
	if(refstr._len <= 0) return *this;
	
	if(_len+refstr._len >= _buflen)
		if( !_realloc(_len+refstr._len+1) ) return *this;
	memmove ( &_p[_len], refstr._p, refstr._len+1 );
	_len += refstr._len;
	return *this;
}

_cstring_& _cstring_::operator+=(LPCSTR lpstr)
{
	if(NULL==lpstr || 0==lpstr[0]) return *this;
	
	int addsize = lstrlenA(lpstr);
	if(_len+addsize >= _buflen)
		if( !_realloc(_len+addsize+1) ) return *this;
	memmove ( &_p[_len], lpstr, addsize+1 );
	_len += addsize;
	return *this;
}

_cstring_& _cstring_::operator+=(char chr)
{
	int cch = _len+1;

	if(cch >= _buflen)
		if( !_realloc(cch) ) return *this;
	_p[_len] = chr;
	_p[++_len] = '\0';
	return *this;
}

_cstring_& _cstring_::operator+=(long val)
{
	char buf[32];
	_ltoa(val, buf, 10);
	return this->operator+=(buf);
}

_cstring_& _cstring_::operator+=(double val)
{
	char buf[64];
	_gcvt(val, 8, buf);
	return this->operator+=(buf);
}

_cstring_& _cstring_::operator+=(const _wstring_& refwstr)
{
	if(refwstr._len == 0) return *this;
	
	if(_len+refwstr._len >= _buflen)
		if( !_realloc(_len + refwstr._len + 1) ) return *this;
	wcstombs ( &_p[_len], refwstr._p, refwstr._len+1 );
	_len += refwstr._len;
	return *this;
}

// if(!s) { ... }
bool _cstring_::operator!() const
{
	return (!_p);
}

// comparison operators are case-sensitive
bool _cstring_::operator<(const _cstring_& refstr) const
{
	return (compare(refstr) < 0);
}

bool _cstring_::operator<(LPCSTR lpstr) const
{
	return (compare(lpstr) < 0);
}

bool _cstring_::operator==(const _cstring_& refstr) const
{
	if( _len == refstr._len && memcmp(_p, refstr._p, _len) == 0 )
		return true;
	return false;
}

bool _cstring_::operator==(LPCSTR lpstr) const
{
	return (compare(lpstr, false) == 0);
}

bool _cstring_::operator!=(const _cstring_& refstr) const
{
	if( _len == refstr._len && memcmp(_p, refstr._p, _len) == 0 )
		return false;
	return true;
}

bool _cstring_::operator!=(LPCSTR lpstr) const
{
	return (compare(lpstr, false) != 0);
}

int _cstring_::compare(const _cstring_& refstr, bool case_sensitive) const
{

	int result;
	// one or both strings can be null, have to guard against that
	if(case_sensitive)
		result = memcmp ( _p, refstr._p, (_len <= refstr._len)? _len : refstr._len );
	else
		result = memicmp( _p, refstr._p, (_len <= refstr._len)? _len : refstr._len );
	
	if(result == 0) return ( (_len == refstr._len) ? 0 : ((_len > refstr._len) ? 1 : -1) );
	else			return result;
}

int _cstring_::compare(LPCSTR lpstr, bool case_sensitive) const
{
	if(case_sensitive)	return lstrcmpA (_p, lpstr);
	else				return lstrcmpiA(_p, lpstr);
}

bool _cstring_::startsWith(LPCSTR lpstr, bool case_sensitive) const
{
	int len = lstrlenA(lpstr);
	if(len > _len) return false;

	if(case_sensitive)	return memcmp (_p, lpstr, len) == 0;
	else				return memicmp(_p, lpstr, len) == 0;
}

bool _cstring_::endsWith(LPCSTR lpstr, bool case_sensitive) const
{
	int len = lstrlenA(lpstr);
	if(len > _len) return false;

	if(case_sensitive)	return memcmp (&_p[_len-len], lpstr, len) == 0;
	else				return memicmp(&_p[_len-len], lpstr, len) == 0;
}

_cstring_& _cstring_::append(const _cstring_& refstr)
{
	return this->operator+=(refstr);
}

_cstring_& _cstring_::append(LPCSTR lpstr)
{
	return this->operator+=(lpstr);
}

_cstring_& _cstring_::append(char chr)
{
	return this->operator+=(chr);
}

_cstring_& _cstring_::append(long val)
{
	return this->operator+=(val);
}

_cstring_& _cstring_::append(double val)
{
	return this->operator+=(val);
}

void _cstring_::swap(_cstring_& refstr)
{
	LPSTR temp_p = _p;
	int temp_len = _len;
	int temp_buflen = _buflen;
	
	_p = refstr._p;
	_len = refstr._len;
	_buflen = refstr._buflen;

	refstr._p = temp_p;
	refstr._len = temp_len;
	refstr._buflen = temp_buflen;
}

_cstring_& _cstring_::insert(LPCSTR insert_str, int start)
{
	return replaceBytes ( insert_str, lstrlenA(insert_str), start, 0 );
}

// Removes @chars_to_del chars from the string starting at position @start,
// and inserts entire @replace_with string at that location
_cstring_& _cstring_::replace(LPCSTR replace_with, int start, int chars_to_del)
{
	return replaceBytes ( replace_with, lstrlenA(replace_with), start, chars_to_del );
}

// The offset specified by @start will depend on whether UNICODE
// is defined or not. Generally, if the string's purpose is to hold
// binary data, do not define UNICODE.
_cstring_& _cstring_::insertBytes(const void* pData, int byteCount, int start)
{
	return replaceBytes ( pData, byteCount, start, 0 );
}

// Removes @bytes_to_del bytes from the string starting at position @start,
// and inserts @byteCount bytes of pData at that location.
// The offset specified by @start will depend on whether UNICODE
// is defined or not. Generally, if the string's purpose is to hold
// binary data, do not define UNICODE.
_cstring_& _cstring_::replaceBytes(const void* pData, int byteCount, int start, int bytes_to_del)
{
	if(start < 0 || bytes_to_del < 0 || NULL == pData || byteCount < 0) return *this;
	if(start+bytes_to_del > _len) bytes_to_del = _len-start;
	int newlen = _len + byteCount - bytes_to_del;
	if( !_realloc(newlen) ) return *this;
	memmove ( &_p[start+byteCount], &_p[start+bytes_to_del], _len-start-bytes_to_del );
	memcpy ( &_p[start], pData, byteCount );
	_len = newlen;
	_p[_len] = '\0';
	return *this;
}

// @replace_count - how many occurences to replace.
// default - -1 (i.e., replace all occurences found)
_cstring_& _cstring_::replaceOccurences(LPCSTR str_to_find, LPCSTR replace_with,
									  int start_pos, int replace_count, bool case_sensitive)
{
	if( NULL == str_to_find || 0 == str_to_find[0] || NULL == replace_with ||
		0 == replace_count || start_pos >= _len || start_pos < 0 )
		return *this;
	
	int len_1 = lstrlenA(str_to_find);
	// to avoid problems if replace_with is part (or whole) of this->_p
	_cstring_ s(replace_with);
	int len_2 = s.length();
	int pos = start_pos;

	if(-1 == replace_count)
		replace_count = MAX_INT;  // maximum positive value of int (to replace all)

	pos = find(str_to_find, pos, case_sensitive);
	while( pos >= 0 && replace_count > 0 )
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
// to avoid making _cstring_ unnecessarily heavy by including
// the _array_ code in it. split returns the count of items
// returned in ret_arr
long _cstring_::split( _array_<_cstring_>& ret_arr,
					  LPCSTR delim, bool ignore_delim_case )
{
	ret_arr.clear();
	if( _len <= 0 || NULL == _p || NULL == delim || delim[0] == '\0' )
		return 0;
	
	// use @delim to split a string into string array
	int pos, prevpos;
	int delim_len = lstrlenA(delim);

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
_cstring_ _cstring_::join ( const _array_<_cstring_>& str_array,
						  LPCSTR delim, int first_index, int last_index )
{
	_cstring_ retval;
	bool empty_delim = (NULL == delim || delim[0] == '\0');
	if( str_array.length() == 0 ) return retval;
	
	// use @delim to join the values in the string array
	// from @first_index to @last_index into one string
	if(first_index < 0)
		first_index = 0;
	if( last_index == -1 || last_index > str_array.length()-1 )
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
_cstring_& _cstring_::formatDate( LPCSTR date_format, LPCSTR time_format,
								SYSTEMTIME& st, bool adjustForTZ )
{
	char bufdate[64];
	_cstring_ dfmt(date_format);
	_cstring_ tfmt(time_format);
	_cstring_ result;

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

	if( dfmt.length() == 0 )
		result = "";
	else
	{
		// adjust for month format mistakes
		dfmt.replaceOccurences("mmmm", "MMMM");
		dfmt.replaceOccurences("mmm", "MMM");
		dfmt.replaceOccurences("mm", "MM");
		dfmt.replaceOccurences("m", "M");
		// replace ' with ''''
		dfmt.replaceOccurences("\'", "\'\'\'\'");
		GetDateFormatA(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &st, (LPCSTR)dfmt, bufdate, sizeof(bufdate)/sizeof(bufdate[0]));
		result = bufdate;
	}

	if( tfmt.length() == 0 )
		result += "";
	else
	{
		// adjust for minute format mistakes
		tfmt.replaceOccurences("MM", "mm");
		tfmt.replaceOccurences("M", "m");
		// replace ' with ''''
		tfmt.replaceOccurences("\'", "\'\'\'\'");
		GetTimeFormatA(LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &st, (LPCSTR)tfmt, bufdate, sizeof(bufdate)/sizeof(bufdate[0]));
		if(result.length() == 0)
			result += bufdate;
		else
			result.append(" ").append(bufdate);
	}

	swap(result);
	return *this;
}

#endif // ALL_STRING_STUFF

_cstring_ _cstring_::substring(int start) const
{
	return substring(start, _len);
}

_cstring_ _cstring_::substring(int start, int count) const
{
	if( 0 == start && count >= _len ) return *this;
	_cstring_ newstr;
	if( start < 0 || start >= _len || count <= 0 )
		return ( newstr.operator=("") );
	int tocopy = (count>_len-start) ? (_len-start) : count;
	if( !newstr._alloc(tocopy) ) return newstr;
	memmove(newstr._p, &_p[start], tocopy);
	newstr._p[tocopy] = '\0';
	newstr._len = tocopy;
	return newstr;
}

_cstring_ _cstring_::left(int count) const
{
	return substring(0, count);
}

_cstring_ _cstring_::right(int count) const
{
	return substring(_len-count, count);
}

_cstring_& _cstring_::reverse()
{
	if( _len <= 0 || NULL == _p ) return *this;

	int i = -1, half = (int)(_len/2);
	char t;
	while(++i < half)
	{
		t = _p[_len-i-1];
		_p[_len-i-1] = _p[i];
		_p[i] = t;
	}
	return *this;
}

// The default for @char_to_trim is space
_cstring_& _cstring_::trim(char char_to_trim)
{
	if( _len <= 0 || NULL == _p ) return *this;
	
	// trim right side
	while(char_to_trim == _p[_len-1])
		_len--;
	_p[_len] = '\0';

	// trim left side
	int pos = 0;
	while(char_to_trim == _p[pos])
		pos++;
	if(pos)
	{
		memmove ( _p, &_p[pos], (_len+1-pos) );
		_len -= pos;
	}
	return *this;
}

_cstring_& _cstring_::padRight(int total_length, char char_to_pad)
{
	if(total_length <= _len) return *this;
	
	if(total_length >= _buflen)
		if( !_realloc(total_length+1) ) return *this;
	while ( _p[_len] = char_to_pad, ++_len < total_length ) ;
	_p[_len] = '\0';
	return *this;
}

_cstring_& _cstring_::padLeft(int total_length, char char_to_pad)
{
	if(total_length <= _len) return *this;

	_cstring_ s(char_to_pad, total_length-_len);
	s.operator += (*this);
	swap(s);
	return *this;
}

_cstring_& _cstring_::chopLeft(int chars_to_chop)
{
	if(_len <= 0) return *this;
	if(chars_to_chop > _len) chars_to_chop = _len;

	// remove the front chars_to_chop characters
	memmove ( _p, &_p[chars_to_chop], _len+1-chars_to_chop );
	_len -= chars_to_chop;
	return *this;
}

_cstring_& _cstring_::chopRight(int chars_to_chop)
{
	if(_len <= 0) return *this;
	if(chars_to_chop > _len) chars_to_chop = _len;

	// remove the last chars_to_chop characters
	_len -= chars_to_chop;
	_p[_len] = '\0';
	return *this;
}

_cstring_& _cstring_::format(LPCSTR format_spec, ...)
{
	va_list args;
	va_start(args, format_spec);
	
	// to avoid problems if format_spec or any other argument
	// is/are part (or whole) of this->_p
	_cstring_ result;
	int cch = 0;

#if defined(_USE_NO_CRT) && !defined(_DEBUG) && !defined(DEBUG)
	if( !result._alloc( 8192 ) )  // hopefully is enough
	{
		va_end(args);
		return *this;
	}

	if( (cch = wvsprintfA(result._p, format_spec, args)) <
		lstrlenA(format_spec) )
	{
		va_end(args);
		return *this;
	}
	// compact
	result._realloc( cch+1 );
#else
	// try increasing buffer size until all args are written
	int size = 16;
	do
	{
		size += 32;
		if(!result._alloc(size)) break;
	} while ( (cch = _vsnprintf(result._p, size, format_spec, args)) < 0 );
#endif

	va_end(args);

	result._len = cch;
	swap(result);
	return *this;
}

char& _cstring_::operator[](int pos)
{
	return _p[pos];
}

char _cstring_::getChar(int pos) const
{
	return _p[pos];
}

_cstring_::operator LPCSTR() const
{
	return _p;
}

void _cstring_::clear()
{
	_free();
}

void _cstring_::upper()
{
	if(!_p) return;
	int i = -1;
	while(++i < _len) _p[i] = (char)toupper(_p[i]);
}

void _cstring_::lower()
{
	if(!_p) return;
	int i = -1;
	while(++i < _len) _p[i] = (char)tolower(_p[i]);
}

int _cstring_::find(const _cstring_& refstr, int start, bool case_sensitive) const
{
	if(start < 0 || start >= _len || refstr._len > _len) return -1;
	if(refstr._len == 0) return start;

	int i;
	int until = _len - refstr._len;

	for(; start<=until; start++)
	{
		if( (_p[start] == refstr._p[0] ) ||
			(!case_sensitive && tolower(_p[start]) == tolower(refstr._p[0]))
		  )
		{
			i = 0;
			while(i < refstr._len)
			{
				if( (_p[start+i] == refstr._p[i] ) ||
					(!case_sensitive && tolower(_p[start+i]) == tolower(refstr._p[i]))
				  )
					i++;
				else break;
			}
			if(i == refstr._len) return start; // reached the end and matched so far - full match
		}
	}

	return -1;
}

int _cstring_::find(LPCSTR lpstr, int start, bool case_sensitive) const
{
	if(start < 0 || start >= _len) return -1;
	if(lpstr[0] == '\0') return start;
	int slen = lstrlenA(lpstr);
	if(slen > _len) return -1;
	int i;
	int until = _len - slen;

	// there can be embedded null chars in this->_p, so can't just use strstr
	for(; start<=until; start++)
	{
		if( (_p[start] == lpstr[0] ) ||
			(!case_sensitive && tolower(_p[start]) == tolower(lpstr[0]))
		  )
		{
			i = 0;
			while(i < slen)
			{
				if( (_p[start+i] == lpstr[i] ) ||
					(!case_sensitive && tolower(_p[start+i]) == tolower(lpstr[i]))
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
int _cstring_::findReverse(LPCSTR lpstr, int start, bool case_sensitive) const
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

bool _cstring_::isNumeric() const
{
	if(!_p || _len<1) return false;
	
	char* p = _p;
	while( isspace(*p) ) p++;
	if(*p == '-' || *p == '+') p++;
	if(*p == '.') p++;
	if(*p == 'd' || *p == 'D' || *p == 'e' || *p == 'E') p++;
	if(*p == '-' || *p == '+') p++;
	if( (p >= _p+_len) || (*p != '0' && strtod(_p, NULL) == 0) )
		return false;
	return true;
}

long _cstring_::longVal() const
{
	if(_len < 1) return 0;
	return atol(_p);
}

double _cstring_::doubleVal() const
{
	if(_len < 1) return 0.0;
	return strtod(_p, NULL);
}

bool _cstring_::isNull() const
{
	return (NULL == _p);
}

int _cstring_::length() const
{
	return _len;
}

int _cstring_::capacity() const
{
	return _buflen-1;
}

//------------------------------------------------
// Internal helper functions
//------------------------------------------------

// allocate the specified number of characters
bool _cstring_::_alloc(int cch)
{
	_free();
	return _realloc(cch);
}

// allocate the specified number of characters
// and move the existing string (if any) over to it
bool _cstring_::_realloc(int cch)
{
	cch = (cch>=160) ? ((int)(cch*1.25)) : (cch+16);

	if(_p) _p = (LPSTR) realloc(_p, cch);
	else {
		_p = (LPSTR) malloc(cch);
		if(_p) _p[0] = '\0';
	}
	
	if(!_p) return _buflen=0, false;
	if(_buflen > cch) _p[cch-1] = '\0';
	_buflen = cch;
	return true;
}

void _cstring_::_free()
{
	if(_p) free(_p);
	_p = NULL;
	_len = _buflen = 0;
}


//------------------------------------------------
// Non-member concatenation operations
//------------------------------------------------
_cstring_ operator+(const _cstring_& s1, const _cstring_& s2)
{
	_cstring_ s(s1);
	s.operator+=(s2);
	return s;
}

_cstring_ operator+(const _cstring_& refstr, LPCSTR lpstr)
{
	_cstring_ s(refstr);
	s.operator+=(lpstr);
	return s;
}

_cstring_ operator+(const _cstring_& refstr, char chr)
{
	_cstring_ s(refstr);
	s.operator+=(chr);
	return s;
}

_cstring_ operator+(const _cstring_& refstr, long val)
{
	_cstring_ s(refstr);
	s.operator+=(val);
	return s;
}

_cstring_ operator+(const _cstring_& refstr, double val)
{
	_cstring_ s(refstr);
	s.operator+=(val);
	return s;
}

_cstring_ operator+(LPCSTR lpstr, const _cstring_& refstr)
{
	_cstring_ s(lpstr);
	s.operator+=(refstr);
	return s;
}

_cstring_ operator+(char chr, const _cstring_& refstr)
{
	_cstring_ s(chr);
	s.operator+=(refstr);
	return s;
}

_cstring_ operator+(long val, const _cstring_& refstr)
{
	_cstring_ s(val);
	s.operator+=(refstr);
	return s;
}

_cstring_ operator+(double val, const _cstring_& refstr)
{
	_cstring_ s(val);
	s.operator+=(refstr);
	return s;
}

};	// namespace soige

