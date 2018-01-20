//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _wildcard_search_.cpp - impl file for the simple wildcard
// string search class.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_wildcard_search_.h"

namespace soige {

/* static */
bool _wildcard_search_::wildcardPatternMatch( LPCTSTR pattern, long pat_len,
											  LPCTSTR search_str, long str_len,
											  bool case_sensitive )
{
	TCHAR matchchar;

    // End of pattern, but not end of string?
    if( (pat_len == 0) && (str_len != 0) )
        return false;

    // If we hit a wild card, do recursion
    if( *pattern == _T('*') )
	{
	    // skip over multiple contiguous *'s
		while( *pattern == _T('*') ) { pattern++; pat_len--; }
        
		while( (str_len > 0) && (pat_len > 0) )
		{
			matchchar = *search_str;
			if( !case_sensitive && matchchar ) matchchar = totupper( matchchar );

            // See if this substring matches
		    if( *pattern == _T('?') || *pattern == matchchar )
				if( wildcardPatternMatch( pattern+1, pat_len-1, search_str+1, str_len-1, case_sensitive ) )
					return true;

            // Try the next substring
            search_str++; str_len--;
        }

		// Reached end of pattern and/or string.
		// Make sure that if it's not end of pattern,
		// it has a valid wild card ending,
		// so that it's really a match.
		return ( (pat_len == 0) || (*pattern == _T('*') && pat_len == 1) ) ? true : false;
    } 

    // Do straight compare until we hit a wild card
    while( str_len > 0 && *pattern != _T('*') )
	{
		matchchar = *search_str;
		if( !case_sensitive && matchchar ) matchchar = totupper( matchchar );

		if( *pattern == _T('?') || *pattern == matchchar )
		{
            pattern++; pat_len--;
            search_str++; str_len--;
        }
		else
            return false;
    }

    // If not done, recurse
    if( str_len )
        return wildcardPatternMatch( pattern, pat_len, search_str, str_len, case_sensitive );

    // Make sure it's a match
	return ( (pat_len == 0) || (*pattern == _T('*') && pat_len == 1) ) ? true : false;
}

_wildcard_search_::_wildcard_search_()
{
	_pattern = NULL;
	_caseSensitive = true;
	_wholeWord = false;
	_entireString = false;
}

_wildcard_search_::_wildcard_search_(const _wildcard_search_& other)
{
	_pattern = NULL;
	this->operator=(other);
}

_wildcard_search_::_wildcard_search_ (LPCTSTR pattern, bool caseSensitive,
									  bool matchWholeWord, bool matchEntireString)
{
	_pattern = NULL;
	initPattern(pattern, caseSensitive, matchWholeWord, matchEntireString);
}

_wildcard_search_& _wildcard_search_::operator= (const _wildcard_search_& other)
{
	if(_pattern)
		free(_pattern);
	_pattern = _tcsdup(other._pattern);
	_caseSensitive = other._caseSensitive;
	_wholeWord = other._wholeWord;
	_entireString = other._entireString;
	return *this;
}

_wildcard_search_::~_wildcard_search_()
{
	clear();
}

void _wildcard_search_::clear()
{
	if(_pattern)
		free(_pattern);
	_pattern = NULL;
	_caseSensitive = true;
	_wholeWord = false;
	_entireString = false;
}

void _wildcard_search_::initPattern( LPCTSTR pattern, bool caseSensitive,
									 bool matchWholeWord, bool matchEntireString )
{
	clear();
	_pattern = _tcsdup(pattern);
	_caseSensitive = caseSensitive;
	_wholeWord = matchWholeWord;
	_entireString = matchEntireString;
	if( !_caseSensitive )
		_tcsupr( _pattern );
}

// Returns true if match is found, false otherwise.
// The starting index of the match in @pString is returned in @pMatchStart,
// the length of the matching substring (in chars) - in @pMatchLength.
// match is done according to the three options:
// case sensitivity, whole word matching, and entire string matching.
// Length of the string is specified by @stringLen, so it can contain
// embedded null characters without the danger of stopping the matcher
// before the end of the string is reached.
// The first (shortest) matching substring is selected, so if this
// substring does not match whole word and/or entire string options,
// but a longer matching substring does, the result will be false.
bool _wildcard_search_::match ( LPCTSTR pString, long stringLen,
								long* pMatchStart, long* pMatchLength )
{
	if( (!pString || !pString[0]) && (!_pattern || !_pattern[0]) )
		return true;
	if( (!pString || !pString[0]) || !_pattern || !_pattern[0] )
		return false;
	
	if( *pMatchStart )
		*pMatchStart = -1;
	if( *pMatchLength )
		*pMatchLength = -1;
	return wildcardPatternMatch( _pattern, lstrlen(_pattern),
								 pString, stringLen, _caseSensitive );
}


};	// namespace soige

