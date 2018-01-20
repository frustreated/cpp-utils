//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _boyer_moore_.h - header file for exact string search class
//
// Encapsulates the Boyer-Moore string matching algorithm.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __boyer_moore_already_included_vasya__
#define __boyer_moore_already_included_vasya__

#include "_common_.h"
#include "_strfuncs_.h"

#if defined(UNICODE) || defined(_UNICODE)
	#error soige utils: _boyer_moore_ does not support Unicode compilation
#endif

namespace soige {

class _boyer_moore_
{
public:
	_boyer_moore_();
	_boyer_moore_(const _boyer_moore_& other);
	_boyer_moore_(LPCTSTR pattern, bool caseSensitive = true,
				  bool matchWholeWord = false, bool matchEntireString = false);
	
	virtual ~_boyer_moore_();
	
	virtual _boyer_moore_& operator=(const _boyer_moore_& other);

	void clear();
	void initPattern(LPCTSTR pattern, bool caseSensitive = true,
					 bool matchWholeWord = false, bool matchEntireString = false);

	bool match (LPCTSTR pString, long  stringLen,
				long* pMatchStart = NULL, long* pMatchLength = NULL);

private:
	LPTSTR	_pattern;
	long	_patLen;		// length of the pattern
	bool	_caseSensitive;
	// grep-specific options
	bool	_wholeWord;		// does the match have to be delimited by word boundaries?
	bool	_entireString;	// does entire string have to be matched by the pattern?
	
	// The array of indexes into positions of letters inside the pattern;
	// indexed by the letters themselves (offset of 'A' is in [65], etc.).
	// accomodates patterns as large as 65K characters in length.
	unsigned short _charOffsets[256];

private:
	
	// fill in the _charOffsets array with the character offsets
	void _prepareOffsets();
};

};	// namespace soige

#endif  // __boyer_moore_already_included_vasya__

