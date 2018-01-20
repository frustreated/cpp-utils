//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _boyer_moore_.cpp - impl file for exact string search class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_boyer_moore_.h"

namespace soige {

_boyer_moore_::_boyer_moore_()
{
	_pattern = NULL;
	_patLen = 0;
	_caseSensitive = true;
	_wholeWord = false;
	_entireString = false;
	memset( _charOffsets, 0, sizeof(_charOffsets) );
}

_boyer_moore_::_boyer_moore_(const _boyer_moore_& other)
{
	_pattern = NULL;
	this->operator=(other);
}

_boyer_moore_::_boyer_moore_(LPCTSTR pattern, bool caseSensitive,
							 bool matchWholeWord, bool matchEntireString)
{
	_pattern = NULL;
	initPattern( pattern, caseSensitive, matchWholeWord, matchEntireString );
}

_boyer_moore_& _boyer_moore_::operator=(const _boyer_moore_& other)
{
	if(_pattern)
		free(_pattern);
	_pattern = NULL;
	_patLen = other._patLen;
	if(_patLen)
		_pattern = _tcsdup(other._pattern);
	_caseSensitive = other._caseSensitive;
	_wholeWord = other._wholeWord;
	_entireString = other._entireString;
	memcpy( _charOffsets, other._charOffsets, sizeof(_charOffsets) );
	return *this;
}

_boyer_moore_::~_boyer_moore_()
{
	clear();
}

void _boyer_moore_::clear()
{
	if(_pattern)
		free(_pattern);
	_pattern = NULL;
	_patLen = 0;
	_caseSensitive = true;
	_wholeWord = false;
	_entireString = false;
	memset( _charOffsets, 0, sizeof(_charOffsets) );
}

void _boyer_moore_::initPattern(LPCTSTR pattern, bool caseSensitive,
								bool matchWholeWord, bool matchEntireString)
{
	clear();
	if(pattern)
	{
		_patLen = lstrlen(pattern);
		_pattern = _tcsdup(pattern);
	}
	_caseSensitive = caseSensitive;
	_wholeWord = matchWholeWord;
	_entireString = matchEntireString;
	_prepareOffsets();
}

//------------------------------------------------------------
// Returns true if match is found, false otherwise.
// The starting index of the match in @pString is returned
// in @pMatchStart, the length of the matching substring
// (in chars) - in @pMatchLength.
// Match is done according to the three options:
// case sensitivity, whole word, and entire string matching.
// Length of the string is specified by @stringLen, so it
// can contain embedded null characters without the danger
// of stopping the matcher before end of string is reached.
//------------------------------------------------------------
bool _boyer_moore_::match ( LPCTSTR pString, long stringLen,
							long* pMatchStart, long* pMatchLength )
{
	const char* pc;		// current character in pString
	long i;
	bool bMatch;		// matches so far or not

	if(pMatchStart)
		*pMatchStart = 0;
	if(pMatchLength)
		*pMatchLength = _patLen; // match length won't change in exact match
	
	// Null expression matches every line except when
	// the Match Entire String option is also specified
	if(_patLen == 0)
		return (_entireString ? (stringLen == 0) : true);

	if(_patLen > stringLen)
		return false;

	if(_entireString)
	{
		if(stringLen != _patLen)
			return false;
		return (_caseSensitive) ?
			(memcmp (pString, _pattern, _patLen) == 0) :
			(memicmp(pString, _pattern, _patLen) == 0) ;
	}

	// Do the search
	pc = &pString[_patLen-1];
	while(pc < &pString[stringLen])
	{
		bMatch = true;
		for(i = 0; i > -_patLen; i--)
		{
			if( ( pc[i] == _pattern[_patLen+i-1] ) ||
				( !_caseSensitive && isupper(pc[i]) && (pc[i]+32 == _pattern[_patLen+i-1]) )
			  )
				; // matches so far
			else
			{
				// no match, advance pc pointer according to the
				// offset of the character in charOffset array
				pc += _charOffsets[(BYTE)pc[i]];
				bMatch = false;
				break;
			}
		}
		
		if(bMatch)
		{
			// matched
			if(pMatchStart)
				*pMatchStart = &pc[-_patLen+1] - pString;
			
			// see if the found string has to be a word;
			// if has to be, but isn't, continue searching
			if(!_wholeWord)
				return true;
			else if( isWord(pString, &pc[-_patLen+1], _patLen) )
				return true;
			else
				{ pc++; continue; }
		}
	}

	return false;
}

//------------------------------------------------------------
// Fill in the _charOffsets array with the character offsets
// in the pattern.
//------------------------------------------------------------
void _boyer_moore_::_prepareOffsets()
{
	int i;
	if(_patLen == 0) return;

	// All the indexes were zeroed out by clear() or ctor;
	// fill the indexes of characters that are in the pattern
	// making sure that if a char occurs more than once,
	// the smallest offset is used.

	// Find the first occurence of the current character
	// in the search string and set its offset in array
	for(i = _patLen-1; i >= 0; i--)
		if(_charOffsets[_pattern[i]] == 0) // not yet set
			_charOffsets[_pattern[i]] = ((_patLen - i - 2) <= 0) ? 1 : (_patLen - i - 2);
	
	// fix up the chars that do not occur in the pattern
	for(i=0; i<sizeof(_charOffsets)/sizeof(_charOffsets[0]); i++)
		if(_charOffsets[i] == 0)
			_charOffsets[i] = _patLen;
	
	// adjust for case-insensitive search
	if(!_caseSensitive)
	{
		_strlwr(_pattern);
		for(i=65; i<=92; i++)
		{
			if(_charOffsets[i] < _charOffsets[i+32])
				_charOffsets[i+32] = _charOffsets[i];
			else
				_charOffsets[i] = _charOffsets[i+32];
		}
	}
}


};	// namespace soige

