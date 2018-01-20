//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _soundex_.cpp - impl file for phonetic string search class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_soundex_.h"

namespace soige {

// array of soundex letter values
const char _soundex_::lettervals[26] =
{
	0x00, 0x01, 0x02, 0x03,  0x00, 0x01, 0x02, 0x00,
	0x00, 0x02, 0x02, 0x04,  0x05, 0x05, 0x00, 0x01,
	0x02, 0x06,	0x02, 0x03,  0x00, 0x01, 0x00, 0x02,
	0x00, 0x02
};

/* static */
void _soundex_::soundexFromWord(char sdxVal[4], char* pWord)
{
	long i, j, len;

	strupr(pWord);
	sdxVal[0] = pWord[0];
	sdxVal[1] = sdxVal[2] = sdxVal[3] = 0x00;
	len = lstrlenA(pWord);
	if(len == 1) return;
	
	// Replace the letters in the string with code numbers
	for(i=1; i<len; i++)
		pWord[i] = lettervals[pWord[i]-0x41];

	// Build the code from the string, eliminating 0xff
	// codes and consecutive codes with the same value
	i = j = 1;
	while( i < 4 && j < len )
	{
		if( pWord[j] != 0x00 && pWord[j] != pWord[j-1] )
			sdxVal[i++] = pWord[j];
		j++;
	}
}

_soundex_::_soundex_()
{
	_pattern = NULL;
	_patLen = 0;
	_entireString = false;
	memset( _patSdxCode, 0, sizeof(_patSdxCode) );
}

_soundex_::_soundex_(const _soundex_& other)
{
	_pattern = NULL;
	this->operator=(other);
}

//------------------------------------------------------------
// The pattern must be a single word consisting of all
// alphabetic characters.
//------------------------------------------------------------
_soundex_::_soundex_(LPCTSTR pattern, bool matchEntireString)
{
	_pattern = NULL;
	initPattern(pattern, matchEntireString);
}

_soundex_::~_soundex_()
{
	clear();
}

_soundex_& _soundex_::operator=(const _soundex_& other)
{
	if(_pattern)
		free(_pattern);
	_pattern = NULL;
	_patLen = other._patLen;
	if(_patLen)
		_pattern = _tcsdup(other._pattern);
	_entireString = other._entireString;
	memcpy( _patSdxCode, other._patSdxCode, sizeof(_patSdxCode) );
	return *this;
}

void _soundex_::clear()
{
	if(_pattern)
		free(_pattern);
	_pattern = NULL;
	_patLen = 0;
	memset( _patSdxCode, 0, sizeof(_patSdxCode) );
}

//------------------------------------------------------------
// The pattern must be a single word consisting of all
// alphabetic characters.
//------------------------------------------------------------
void _soundex_::initPattern(LPCTSTR pattern, bool matchEntireString)
{
	clear();
	if(pattern)
	{
		_patLen = lstrlen(pattern);
		_pattern = _tcsdup(pattern);
	}
	_entireString = matchEntireString;
	_prepareSdxForPattern();
}

//------------------------------------------------------------
// Returns true if match is found, false otherwise.
// The starting index of the match in @pString is returned
// in @pMatchStart, the length of the matching substring
// (in chars) - in @pMatchLength.
// Match is validated by the entire string matching option.
// Length of the string is specified by @stringLen, so it
// can contain embedded null characters without the danger
// of stopping the matcher before end of string is reached.
//------------------------------------------------------------
bool _soundex_::match ( LPCTSTR pString, long stringLen,
						long* pMatchStart, long* pMatchLength )
{
	char pWord[64];
	char sdxVal[4];
	long i, count;
	long wordStart;

	if(pMatchStart)
		*pMatchStart = 0;
	if(pMatchLength)
		*pMatchLength = _patLen;
	
	// Null expression matches every line except when
	// the Match Entire String option is also specified
	if(_patLen == 0)
		return (_entireString ? (stringLen == 0) : true);

	i = 0;
	while( (count = _nextWord(pWord, sizeof(pWord)-1,
							  &pString[i], &pString[stringLen], &wordStart)) != -1 )
	{
		soundexFromWord(sdxVal, pWord);
		if (sdxVal[0] == _patSdxCode[0] && sdxVal[1] == _patSdxCode[1] &&
			sdxVal[2] == _patSdxCode[2] && sdxVal[3] == _patSdxCode[3] )
		{
			// match
			if(pMatchStart) *pMatchStart = i+wordStart;
			if(pMatchLength) *pMatchLength = count-wordStart;
			return (_entireString)? ((i+wordStart == 0) && (i+count == stringLen)) : true;
		}
		i += count;
	}

	return false;
}

//------------------------------------------------------------
// Convert the pattern into its soundex representation
// and store it in _patSdxCode.
//------------------------------------------------------------
void _soundex_::_prepareSdxForPattern()
{
	if(_patLen==0) return;
	
	LPTSTR buf = _tcsdup(_pattern);

	for(int i=0; i<_patLen; i++)
	{
		if( !isalpha(buf[i]) )
		{
			buf[i] = 0;
			break;
		}
	}
	
	soundexFromWord(_patSdxCode, buf);
	free(buf);
}

//------------------------------------------------------------
// Function to extract the next word from a string.
// Returns -1 on end of string, count of skipped
// characters otherwise.
// *pWordStart will contain the start of the word
// (relative to pString).
//------------------------------------------------------------
long _soundex_::_nextWord(char* pWord, long maxWordSize, const char* pString, const char* pEnd, long* pWordStart)
{
	long i, j;
	i = j = 0;
	
	while( !isalpha(pString[i]) )
	{
		if( &pString[i] >= pEnd )
			return -1;
		i++;
	}

	*pWordStart = i;
	while( isalpha(pString[i]) && j < maxWordSize )
		pWord[j++] = pString[i++];

	pWord[j] = 0;
	
	return i;
}


};	// namespace soige

