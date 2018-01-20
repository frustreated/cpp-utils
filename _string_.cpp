//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _string_.cpp - implementation of lazy-copied string.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_string_.h"

namespace soige {

const int _string_::string_rep::UNSHAREABLE = 0x80000000;
_string_::string_rep _string_::_null_rep;

//------------------------------------------------
// Constructors
//------------------------------------------------
_string_::_string_()
{
	_attach( &_null_rep );
}

_string_::_string_(const _string_& refstr)
{
	_attach( refstr._rep );
}

_string_::_string_(LPCSTR lpstr)
{
	_attach( lpstr? new string_rep(lpstr) : &_null_rep );
}

_string_::_string_(LPCWSTR lpwstr)
{
	if( !lpwstr ) {
		_attach( &_null_rep ); return;
	}

	int wlen = lstrlenW(lpwstr);
	string_rep* rep = new string_rep();
	if( !rep || !rep->ensureLen(wlen+1) ) {
		delete rep; _attach( &_null_rep ); return;
	}
	_attach(rep);
	wcstombs( _rep->_p, lpwstr, wlen + 1 );
	_rep->len() = wlen;
}

_string_::_string_(char chr, int count)
{
	if(count < 1) {
		_attach( &_null_rep ); return;
	}

	int cch = count + 1;
	string_rep* rep = new string_rep();
	if( !rep || !rep->ensureLen(cch) ) {
		delete rep; _attach( &_null_rep ); return;
	}
	_attach(rep);
	//_strnset(_p, chr, count); doesn't work, have to do it manually
	cch = 0;
	while( _rep->_p[cch] = chr, ++cch < count ) ;
	_rep->_p[_rep->len() = count] = '\0';
}

_string_::_string_(long val)
{
	char buf[32];
	_ltoa(val, buf, 10);
	_attach( new string_rep(buf) );
}

_string_::_string_(double val)
{
	char buf[64];
	_gcvt(val, 16, buf);
	_attach( new string_rep(buf) );
}

_string_::~_string_()
{
	_detach();
}

//------------------------------------------------
// Operations
//------------------------------------------------
_string_& _string_::operator=(const _string_& refstr)
{
	_reattach( refstr._rep );
	return *this;
}

_string_& _string_::operator=(LPCSTR lpstr)
{
	_reattach( lpstr? new string_rep(lpstr) : &_null_rep );
	return *this;
}

_string_& _string_::operator=(LPCWSTR lpwstr)
{
	if( !lpwstr ) return _reattach( NULL ), *this;

	_ensureUnique();
	int wlen = lstrlenW(lpwstr);
	if( !_rep->ensureLen( wlen+1 ) ) return *this;
	wcstombs( _rep->_p, lpwstr, wlen + 1 );
	_rep->len() = wlen;
	return *this;
}

_string_& _string_::operator=(char chr)
{
	char buf[] = { chr, 0 };
	_reattach( new string_rep(buf) );
	return *this;
}

_string_& _string_::operator=(long val)
{
	char buf[32];
	_ltoa(val, buf, 10);
	_reattach( new string_rep(buf) );
	return *this;
}

_string_& _string_::operator=(double val)
{
	char buf[64];
	_gcvt(val, 16, buf);
	_reattach( new string_rep(buf) );
	return *this;
}

_string_& _string_::operator+=(const _string_& refstr)
{
	if( refstr._rep->len() <= 0 ) return *this;
	
	_ensureUnique();
	if( !_rep->ensureLen( _rep->len() + refstr._rep->len() + 1 ) ) return *this;
	memmove ( _rep->end(), refstr._rep->_p, refstr._rep->len() + 1 );
	_rep->len() += refstr._rep->len();
	return *this;
}

_string_& _string_::operator+=(LPCSTR lpstr)
{
	if( !lpstr || !lpstr[0] ) return *this;
	
	_ensureUnique();
	int addsize = lstrlenA(lpstr);
	if( !_rep->ensureLen( _rep->len() + addsize + 1 ) ) return *this;
	memmove ( _rep->end(), lpstr, addsize + 1 );
	_rep->len() += addsize;
	return *this;
}

_string_& _string_::operator+=(LPCWSTR lpwstr)
{
	if( !lpwstr || !lpwstr[0] ) return *this;
	
	_ensureUnique();
	int wlen = lstrlenW(lpwstr);
	if( !_rep->ensureLen( _rep->len() + wlen + 1 ) ) return *this;
	wcstombs( _rep->end(), lpwstr, wlen+1 );
	_rep->len() += wlen;
	return *this;
}

_string_& _string_::operator+=(char chr)
{
	_ensureUnique();
	if( !_rep->ensureLen( _rep->len() + 1 ) ) return *this;
	_rep->_p[_rep->len()++] = chr;
	_rep->_p[_rep->len()] = '\0';
	return *this;
}

_string_& _string_::operator+=(long val)
{
	char buf[32];
	_ltoa(val, buf, 10);
	return this->operator += ( buf );
}

_string_& _string_::operator+=(double val)
{
	char buf[64];
	_gcvt(val, 16, buf);
	return this->operator += ( buf );
}

_string_& _string_::append(const _string_& refstr)
{
	return this->operator+=(refstr);
}

_string_& _string_::append(LPCSTR lpstr)
{
	return this->operator+=(lpstr);
}

_string_& _string_::append(LPCWSTR lpwstr)
{
	return this->operator+=(lpwstr);
}

_string_& _string_::append(char chr)
{
	return this->operator+=(chr);
}

_string_& _string_::append(long val)
{
	return this->operator+=(val);
}

_string_& _string_::append(double val)
{
	return this->operator+=(val);
}

// if(!s) { ... }
bool _string_::operator!() const
{
	return ( !_rep->_p );
}

// comparison operators are case-sensitive
bool _string_::operator==(const _string_& refstr) const
{
	if( (_rep == refstr._rep) ||
		(_rep->len() == refstr._rep->len() && memcmp( _rep->_p, refstr._rep->_p, _rep->len() ) == 0) )
		return true;
	return false;
}

bool _string_::operator==(LPCSTR lpstr) const
{
	return ( compare(lpstr, false) == 0 );
}

bool _string_::operator!=(const _string_& refstr) const
{
	return !( this->operator==(refstr) );
}

bool _string_::operator!=(LPCSTR lpstr) const
{
	return ( compare(lpstr, false) != 0 );
}

bool _string_::operator<(const _string_& refstr) const
{
	return ( compare(refstr) < 0 );
}

bool _string_::operator<(LPCSTR lpstr) const
{
	return ( compare(lpstr) < 0 );
}

int _string_::compare(const _string_& refstr, bool case_sensitive) const
{
	if( (_rep == refstr._rep) || (!_rep->_p && !refstr._rep->_p) )
		return 0;

	int result;
	// one or both strings may have embedded NULL chars in them,
	// so can't use strcmp to compare
	if(case_sensitive)
		result = memcmp ( _rep->_p, refstr._rep->_p,
						  min(_rep->len(), refstr._rep->len()) );
	else
		result = memicmp( _rep->_p, refstr._rep->_p,
						  min(_rep->len(), refstr._rep->len()) );
	
	if(result == 0)
		return ( (_rep->len() == refstr._rep->len()) ?
					0 :
					((_rep->len() > refstr._rep->len()) ? 1 : -1) );
	else
		return result;
}

int _string_::compare(LPCSTR lpstr, bool case_sensitive) const
{
	if(case_sensitive)	return lstrcmpA (_rep->_p, lpstr);
	else				return lstrcmpiA(_rep->_p, lpstr);
}

bool _string_::startsWith(LPCSTR lpstr, bool case_sensitive) const
{
	int len = lstrlenA(lpstr);
	if( len > _rep->len() ) return false;

	if(case_sensitive)	return memcmp ( _rep->_p, lpstr, len ) == 0;
	else				return memicmp( _rep->_p, lpstr, len ) == 0;
}

bool _string_::endsWith(LPCSTR lpstr, bool case_sensitive) const
{
	int len = lstrlenA(lpstr);
	if( len > _rep->len() ) return false;

	if(case_sensitive)	return memcmp ( &_rep->_p[_rep->len()-len], lpstr, len ) == 0;
	else				return memicmp( &_rep->_p[_rep->len()-len], lpstr, len ) == 0;
}

int _string_::find(const _string_& refstr, int start, bool case_sensitive) const
{
	if( start < 0 || start >= _rep->len() || refstr._rep->len() > _rep->len() )
		return -1;
	if( refstr._rep->len() == 0 )
		return start;

	int i;
	int until = _rep->len() - refstr._rep->len();

	for(; start <= until; start++)
	{
		if(
			(_rep->_p[start] == refstr._rep->_p[0] ) ||
			(!case_sensitive && tolower(_rep->_p[start]) == tolower(refstr._rep->_p[0]))
		  )
		{
			i = 0;
			while(i < refstr._rep->len())
			{
				if(
					(_rep->_p[start+i] == refstr._rep->_p[i] ) ||
					(!case_sensitive && tolower(_rep->_p[start+i]) == tolower(refstr._rep->_p[i]))
				  )
					i++;
				else break;
			}
			if( i == refstr._rep->len() ) return start; // reached the end and matched so far - full match
		}
	}

	return -1;
}

// If you feel really ambitious and don't care about weight,
// just use _boyer_moore_ and forget about char-by-char comparisons.
int _string_::find(LPCSTR lpstr, int start, bool case_sensitive) const
{
	if( start < 0 || start >= _rep->len() || !lpstr ) return -1;
	if( !lpstr[0] ) return start;
	int slen = lstrlenA(lpstr);
	if( slen > _rep->len() ) return -1;
	int i;
	int until = _rep->len() - slen;

	// there can be embedded NULL chars in this->_p, so can't just use strstr
	for(; start <= until; start++)
	{
		if(
			(_rep->_p[start] == lpstr[0] ) ||
			(!case_sensitive && tolower(_rep->_p[start]) == tolower(lpstr[0]))
		  )
		{
			i = 0;
			while(i < slen)
			{
				if(
					(_rep->_p[start+i] == lpstr[i] ) ||
					(!case_sensitive && tolower(_rep->_p[start+i]) == tolower(lpstr[i]))
				  )
					i++;
				else break;
			}
			if( i == slen ) return start; // reached the end and matched so far - full match
		}
	}

	return -1;
}

// The arg @start is from the beginning of the string, not from the end.
// If not provided, search starts from the end of the string
int _string_::findReverse(LPCSTR lpstr, int start, bool case_sensitive) const
{
	if( start < 0 || !lpstr ) return -1;
	if( !lpstr[0] ) return _rep->len() - 1;

	if( start >= _rep->len() ) start = _rep->len() - 1;
	int match_pos = -1;
	while(start >= 0)
	{
		if( (match_pos = find(lpstr, start, case_sensitive)) >= 0 )
			return match_pos;
		start--;
	}
	return -1;
}

// Stolen almost entirely from Mark [Russinovich] and
// Bryce's [Cogswell] utilities.
bool _string_::matchesPattern(LPCSTR pattern, bool case_sensitive) const
{
	if( _rep->len() <= 0 && (!pattern || !pattern[0]) ) return true;
	if( _rep->len() <= 0 || !pattern || !pattern[0] ) return false;
	
	LPSTR internal_pat = _strdup(pattern);
	if( !case_sensitive ) strupr( internal_pat );
	bool matched = _internalPatternMatch( internal_pat, _rep->_p, case_sensitive );
	free( internal_pat );
	return matched;
}

_string_ _string_::substring(int start) const
{
	return substring(start, _rep->len());
}

_string_ _string_::substring(int start, int count) const
{
	if( 0 == start && count >= _rep->len() ) return *this;
	if( start < 0 || start >= _rep->len() || count <= 0 )
		return ( _string_("") );
	int chars_to_copy = (count > _rep->len() - start) ? (_rep->len() - start) : count;
	
	_string_ newstr;
	newstr._reattach( new string_rep( _rep->_p + start, chars_to_copy ) );
	return newstr;
}

_string_ _string_::left(int count) const
{
	return substring(0, count);
}

_string_ _string_::right(int count) const
{
	return substring(_rep->len() - count, count);
}

// The following two are defined inline in the .H file
// bool _string_::isNull() const
// int _string_::length() const

int _string_::capacity() const
{
	return (_rep->allocLen() - 1); // 1 for term NULL
}

bool _string_::isNumeric() const
{
	if( _rep->len() <= 0 ) return false;
	
	char* p = _rep->_p;
	while( isspace(*p) ) p++;
	if(*p == '-' || *p == '+') p++;
	if(*p == '.') p++;
	if(*p == 'd' || *p == 'D' || *p == 'e' || *p == 'E') p++;
	if(*p == '-' || *p == '+') p++;
	if( (p >= _rep->_p + _rep->len()) || (*p != '0' && strtod(_rep->_p, NULL) == 0) )
		return false;
	return true;
}

long _string_::longVal() const
{
	if( _rep->len() <= 0 ) return 0;
	return atol(_rep->_p);
}

double _string_::doubleVal() const
{
	if( _rep->len() <= 0 ) return 0.0;
	return strtod(_rep->_p, NULL);
}

void _string_::swap(_string_& refstr)
{
	bool refstr_shareable  = refstr._rep->isShareable();
	bool thisstr_shareable = _rep->isShareable();
	string_rep* temp_rep = _rep;
	_rep = refstr._rep;
	refstr._rep = temp_rep;
	// the quality of being unshareable always stays with the string
	if( refstr_shareable ) refstr._rep->markShareable(); else refstr._rep->markUnshareable();
	if( thisstr_shareable ) this->_rep->markShareable(); else this->_rep->markUnshareable();
}

void _string_::clear()
{
	_detach();
	_attach( &_null_rep );
}

void _string_::upper()
{
	if( _rep->len() <= 0 ) return;
	_ensureUnique();
	int i = -1;
	while( ++i < _rep->len() ) _rep->_p[i] = (char) toupper(_rep->_p[i]);
}

void _string_::lower()
{
	if( _rep->len() <= 0 ) return;
	_ensureUnique();
	int i = -1;
	while( ++i < _rep->len() ) _rep->_p[i] = (char) tolower(_rep->_p[i]);
}

_string_& _string_::reverse()
{
	if( _rep->len() <= 0 ) return *this;

	_ensureUnique();
	int i = -1, half = (int)(_rep->len()/2);
	char t;
	while(++i < half)
	{
		t = _rep->_p[_rep->len() - i - 1];
		_rep->_p[_rep->len() - i - 1] = _rep->_p[i];
		_rep->_p[i] = t;
	}
	return *this;
}

// The default for @char_to_trim is space
_string_& _string_::trim(char char_to_trim)
{
	trimRight();
	trimLeft();
	return *this;
}

_string_& _string_::trimLeft(char char_to_trim)
{
	if( _rep->len() <= 0 ) return *this;

	_ensureUnique();
	
	int pos = 0;
	while( char_to_trim == _rep->_p[pos] )
		pos++;
	if(pos)
	{
		memmove ( _rep->_p, &_rep->_p[pos], (_rep->len() - pos + 1) );
		_rep->len() -= pos;
	}
	return *this;
}

_string_& _string_::trimRight(char char_to_trim)
{
	if( _rep->len() <= 0 ) return *this;

	_ensureUnique();
	
	while( char_to_trim == _rep->_p[_rep->len() - 1] )
		_rep->len()--;
	_rep->_p[_rep->len()] = '\0';
	return *this;
}

_string_& _string_::padLeft(int total_length, char char_to_pad)
{
	if( total_length <= _rep->len() ) return *this;

	_ensureUnique();
	if( !_rep->ensureLen(total_length + 1) ) return *this;
	memmove( &_rep->_p[total_length - _rep->len()], _rep->_p, _rep->len() + 1 );
	int chars = total_length - _rep->len() - 1;
	while ( _rep->_p[chars] = char_to_pad, --chars >= 0 ) ;
	_rep->len() = total_length;
	return *this;
}

_string_& _string_::padRight(int total_length, char char_to_pad)
{
	if( total_length <= _rep->len() ) return *this;
	
	_ensureUnique();
	if( !_rep->ensureLen(total_length + 1) ) return *this;
	while ( _rep->_p[_rep->len()] = char_to_pad, ++(_rep->len()) < total_length ) ;
	_rep->_p[_rep->len()] = '\0';
	return *this;
}

// Removes the front chars_to_chop characters from this string
_string_& _string_::chopLeft(int chars_to_chop)
{
	if( _rep->len() <= 0 ) return *this;
	if( chars_to_chop > _rep->len() ) chars_to_chop = _rep->len();

	_ensureUnique();
	memmove ( _rep->_p, &_rep->_p[chars_to_chop], _rep->len() - chars_to_chop + 1 );
	_rep->len() -= chars_to_chop;
	return *this;
}

// Remove the last chars_to_chop characters from this string
_string_& _string_::chopRight(int chars_to_chop)
{
	if( _rep->len() <= 0 ) return *this;
	if( chars_to_chop > _rep->len() ) chars_to_chop = _rep->len();

	_ensureUnique();
	_rep->len() -= chars_to_chop;
	_rep->_p[_rep->len()] = '\0';
	return *this;
}

/*
_string_::subscript_proxy _string_::operator[](int pos)
{
	return subscript_proxy(*this, pos);
}
*/

/*
char& _string_::operator[] (int pos)
{
	_ensureUnique();
	return _rep->_p[pos];
}

char _string_::operator[] (int pos) const
{
	return _rep->_p[pos];
}

LPCSTR _string_::c_str() const
{
	return _rep->_p;
}
*/

_string_& _string_::insert(LPCSTR insert_str, int start)
{
	return replaceBytes ( insert_str, lstrlenA(insert_str), start, 0 );
}

// Removes @chars_to_del chars from the string starting at position @start,
// and inserts entire @replace_with string at that location
_string_& _string_::replace(LPCSTR replace_with, int start, int chars_to_del)
{
	return replaceBytes ( replace_with, lstrlenA(replace_with), start, chars_to_del );
}

_string_& _string_::insertBytes(const void* pdata, int byte_count, int start)
{
	return replaceBytes ( pdata, byte_count, start, 0 );
}

// Removes @bytes_to_del bytes from the string starting at position @start,
// and inserts @byte_count bytes of @pdata at that location.
_string_& _string_::replaceBytes(const void* pdata, int byte_count, int start, int bytes_to_del)
{
	if( start < 0 || bytes_to_del < 0 || NULL == pdata || byte_count < 0 )
		return *this;

	_ensureUnique();
	if( start + bytes_to_del > _rep->len() )
		bytes_to_del = _rep->len() - start;
	int newlen = _rep->len() + byte_count - bytes_to_del;
	if( !_rep->ensureLen(newlen) ) return *this;
	memmove ( &_rep->_p[start + byte_count],
			  &_rep->_p[start + bytes_to_del],
			  _rep->len() - start - bytes_to_del );
	memcpy ( &_rep->_p[start], pdata, byte_count );
	_rep->len() = newlen;
	_rep->_p[_rep->len()] = '\0';
	return *this;
}

// @replace_count - how many occurences to replace.
// default - -1 (i.e., replace all occurences found)
_string_& _string_::replaceOccurences( LPCSTR str_to_find,
										 LPCSTR replace_with,
										 int start_pos, int replace_count,
										 bool case_sensitive )
{
	if( NULL == str_to_find || 0 == str_to_find[0] || NULL == replace_with ||
		0 == replace_count || start_pos >= _rep->len() || start_pos < 0 )
		return *this;
	
	_ensureUnique();
	int len_1 = lstrlenA(str_to_find);
	// to avoid problems if replace_with is part (or whole) of this->_rep->_p,
	// use a separate replacement lstring
	_string_ s(replace_with);
	int len_2 = s.length();
	int pos = start_pos;

	if( -1 == replace_count )
		replace_count = MAX_INT;  // maximum positive value of int (to replace all)

	pos = find( str_to_find, pos, case_sensitive );
	while( pos >= 0 && replace_count > 0 )
	{
		replace( s._rep->_p, pos, len_1 );
		pos += len_2;
		replace_count--;
		pos = find( str_to_find, pos, case_sensitive );
	}

	return *this;
}

_string_& _string_::format(LPCSTR format_spec, ...)
{
	va_list args;
	va_start(args, format_spec);
	
	// to avoid problems if format_spec or any other argument
	// is/are part (or whole) of this->_rep->_p
	_string_ result;
	int cch = 0;
	// force pointing to other than _null_rep, since below we manipulate its _rep directly
	result._ensureUnique();
	
#if defined(_USE_NO_CRT) && !defined(_DEBUG) && !defined(DEBUG)
	if( !result._rep->ensureLen( 8192 ) )  // hopefully is enough
	{
		va_end(args);
		return *this;
	}
	
	if( (cch = wvsprintfA(result._rep->_p, format_spec, args)) <
		lstrlenA(format_spec) )
	{
		va_end(args);
		return *this;
	}
	// compact
	result._rep->ensureLen( cch+1 );
#else
	// try increasing buffer size until all args are written
	int size = 16;
	do
	{
		size += 32;
		if( !result._rep->ensureLen(size) ) break;
	} while ( (cch = _vsnprintf(result._rep->_p, size, format_spec, args)) < 0 );
#endif

	va_end(args);

	result._rep->len() = cch;
	swap(result);
	return *this;
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
_string_& _string_::formatDate( LPCSTR date_format, LPCSTR time_format,
								  SYSTEMTIME& systime, bool adjust_for_tz )
{
	char bufdate[64];
	_string_ dfmt(date_format);
	_string_ tfmt(time_format);
	_string_ result;

	// adjust for the timezone
	if(adjust_for_tz)
	{
		TIME_ZONE_INFORMATION tzi;
		unsigned __int64 tzBias = 0;
		if( GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_DAYLIGHT )
			tzBias = tzi.Bias + tzi.DaylightBias;
		else
			tzBias = tzi.Bias;
		tzBias *= 600000000; // FILETIME is in 100 nanoseconds intervals, 600000000 == 1 minute
		// localtime = utc - bias
		FILETIME filetime;
		SystemTimeToFileTime( &systime, &filetime );
		unsigned __int64 t = 0;
		memcpy ( &t, &filetime, sizeof(t) );
		t -= tzBias;
		memcpy ( &filetime, &t, sizeof(t) );
		FileTimeToSystemTime( &filetime, &systime );
	}

	if( dfmt.length() == 0 )
		result = "";
	else
	{
		// adjust for month format mistakes
		dfmt.replaceOccurences( "mmmm", "MMMM" );
		dfmt.replaceOccurences( "mmm", "MMM" );
		dfmt.replaceOccurences( "mm", "MM" );
		dfmt.replaceOccurences( "m", "M" );
		// replace ' with ''''
		dfmt.replaceOccurences( "\'", "\'\'\'\'" );
		GetDateFormatA( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP,
						&systime, dfmt.c_str(), bufdate,
						sizeof(bufdate)/sizeof(bufdate[0]) );
		result = bufdate;
	}

	if( tfmt.length() == 0 )
		result += "";
	else
	{
		// adjust for minute format mistakes
		tfmt.replaceOccurences( "MM", "mm" );
		tfmt.replaceOccurences( "M", "m" );
		// replace ' with ''''
		tfmt.replaceOccurences( "\'", "\'\'\'\'" );
		GetTimeFormatA( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP,
						&systime, tfmt.c_str(), bufdate,
						sizeof(bufdate)/sizeof(bufdate[0]) );
		if( result.length() == 0 )
			result += bufdate;
		else
			result.append(" ").append(bufdate);
	}

	swap(result);
	return *this;
}

#ifdef ALL_STRING_STUFF
// split() and join() are defined only if explicitly requested
// to avoid making _string_ unnecessarily heavy by including
// the _array_ code in it.
// split() returns the count of items returned in ret_arr
long _string_::split ( _array_<_string_>& ret_arr,
						LPCSTR delim, bool ignore_delim_case ) const
{
	ret_arr.clear();
	if( _rep->len() <= 0 || NULL == delim || delim[0] == '\0' )
		return 0;
	
	// use @delim to split a string into string array
	int pos, prevpos;
	int delim_len = lstrlenA(delim);

	prevpos = 0;
	pos = find( delim, 0, ignore_delim_case );
	while(pos != -1)
	{
		ret_arr.append( substring( prevpos, pos - prevpos ) );
		prevpos = pos + delim_len;
		pos = find( delim, prevpos, ignore_delim_case );
	}
	ret_arr.append( substring(prevpos) );
		
	return ret_arr.length();
}

// default for @delim = " "; @first_index = 0; @last_index = -1 (until last elem)
/* static */
_string_  _string_::join ( const _array_<_string_>& str_array,
							 LPCSTR delim, int first_index, int last_index )
{
	_string_ retval;
	bool empty_delim = (NULL == delim || delim[0] == '\0');
	if( str_array.length() == 0 ) return retval;
	
	// use @delim to join the values in the string array
	// from @first_index to @last_index into one string
	if( first_index < 0 )
		first_index = 0;
	if( last_index < 0 || last_index > str_array.length()-1 )
		last_index = str_array.length() - 1;
	for(int i=first_index; i<=last_index; i++)
	{
		retval.append( str_array[i] );
		if( !empty_delim && i != last_index )
			retval.append( delim );
	}
	
	return retval;
}

#endif // ALL_STRING_STUFF

//------------------------------------------------
// Internal helper functions
//------------------------------------------------

bool _string_::_internalPatternMatch(LPCSTR pattern, LPCSTR name, bool case_sensitive) const
{
	char matchchar;

    // End of pattern, but not end of string?
    if( !*pattern && *name )
        return false;

    // If we hit a wild card, do recursion
    if( *pattern == '*' )
	{
	    // skip over multiple contiguous *'s
		while( *pattern == '*' ) pattern++;
        
		while( *name && *pattern )
		{
			matchchar = *name;
			if( !case_sensitive && matchchar >= 'a' && matchchar <= 'z' )
				matchchar -= 'a' - 'A';

            // See if this substring matches
		    if( *pattern == '?' || *pattern == matchchar )
  		        if( _internalPatternMatch( pattern+1, name+1, case_sensitive ) )
                    return true;

            // Try the next substring
            name++;
        }

		// Reached end of pattern and/or string.
		// Make sure that if it's not end of pattern,
		// it has a valid wild card ending,
		// so that it's really a match.
		return ( !*pattern || (*pattern == '*' && !*(pattern+1)) ) ? true : false;
    } 

    // Do straight compare until we hit a wild card
    while( *name && *pattern != '*' )
	{
		matchchar = *name;
		if( !case_sensitive && matchchar >= 'a' && matchchar <= 'z' )
			matchchar -= 'a' - 'A';

		if( *pattern == '?' || *pattern == matchchar )
		{
            pattern++;
            name++;
        }
		else
            return false;
    }

    // If not done, recurse
    if( *name )
        return _internalPatternMatch( pattern, name, case_sensitive );

    // Make sure it's a match
	return ( !*pattern || (*pattern == '*' && !*(pattern+1)) ) ? true : false;
}

void _string_::_attach ( string_rep* rep )
{
	if( !(_rep = rep) ) _rep = &_null_rep;
	// if rep is unshareable, need to clone a new one from it rather than attach to it
	if( !_rep->isShareable() )
		_rep = new string_rep(_rep->_p, _rep->len());
	_rep->ref();
}

void _string_::_detach ()
{
	if( _rep->deref() <= 0 && _rep != &_null_rep ) delete _rep;
}

void _string_::_reattach ( string_rep* rep )
{
	if( _rep == rep ) return;
	bool shareable = _rep->isShareable();
	_detach();
	_attach( rep );
	if( !shareable ) _rep->markUnshareable();
}

void _string_::_ensureUnique()
{
	if( _rep == &_null_rep )
		_reattach( new string_rep() );
	else if( _rep->refs() > 1 )
		_reattach( new string_rep(_rep->_p, _rep->len()) );
}


//------------------------------------------------
// Non-member concatenation operations
//------------------------------------------------
_string_ operator+(const _string_& s1, const _string_& s2)
{
	_string_ s(s1);
	s.operator+=(s2);
	return s;
}

_string_ operator+(const _string_& refstr, LPCSTR lpstr)
{
	_string_ s(refstr);
	s.operator+=(lpstr);
	return s;
}

_string_ operator+(const _string_& refstr, char chr)
{
	_string_ s(refstr);
	s.operator+=(chr);
	return s;
}

_string_ operator+(const _string_& refstr, long val)
{
	_string_ s(refstr);
	s.operator+=(val);
	return s;
}

_string_ operator+(const _string_& refstr, double val)
{
	_string_ s(refstr);
	s.operator+=(val);
	return s;
}

_string_ operator+(LPCSTR lpstr, const _string_& refstr)
{
	_string_ s(lpstr);
	s.operator+=(refstr);
	return s;
}

_string_ operator+(char chr, const _string_& refstr)
{
	_string_ s(chr);
	s.operator+=(refstr);
	return s;
}

_string_ operator+(long val, const _string_& refstr)
{
	_string_ s(val);
	s.operator+=(refstr);
	return s;
}

_string_ operator+(double val, const _string_& refstr)
{
	_string_ s(val);
	s.operator+=(refstr);
	return s;
}

};	// namespace soige

