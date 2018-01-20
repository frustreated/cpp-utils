//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _soundex_.h - header file for phonetic string search class
//
// Encapsulates the Soundex string matching algorithm.
//
// -------------------------------------------------------
//	The following changes can be made to improve accuracy
//	when these letters occur at the beginning of the word:
//	1. c -
//		followed by a, o, u - match with k also
//		followed by e, i, y - match with s also
//	2. g - followed by e, i, y - match with j also
//	3. k - followed by a, o, u - match with c
//	4. ph - match with f
//	5. ps - match with s
//	6. pn - match with n
//	7. z - match with x
//	8. x - match with z
//
// But then why the hell did I write the Word Sniffer?
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __soundex_already_included_vasya__
#define __soundex_already_included_vasya__

#include "_common_.h"

#if defined(UNICODE) || defined(_UNICODE)
	#error soige utils: _soundex_ does not support Unicode compilation
#endif

namespace soige {

class _soundex_
{
public:
	// Creates the soundex code from a word.
	// Assumes that @sdxVal has 4 chars and @pWord is modifiable
	static void soundexFromWord(char sdxVal[4], char* pWord);

public:
	_soundex_();
	_soundex_(const _soundex_& other);
	// the pattern must be a single word consisting of all alphabetic characters
	_soundex_(LPCTSTR pattern, bool matchEntireString = false);
	
	virtual ~_soundex_();
	
	_soundex_& operator=(const _soundex_& other);

	void clear();
	// the pattern must be a single word consisting of all alphabetic characters
	void initPattern(LPCTSTR pattern, bool matchEntireString = false);

	// returns the position of the pattern in @pstring, or -1 on no match.
	// match is done according to the entire string matching option.
	bool match (LPCTSTR pString, long  stringLen,
				long* pMatchStart = NULL, long* pMatchLength = NULL );
public:
	// array of soundex letter values
	static const char lettervals[26];

private:
	LPTSTR	_pattern;
	long	_patLen;		// length of the pattern
	// grep-specific options
	bool	_entireString;	// does entire string have to be matched by the pattern?
	
	// The soundex code for the pattern
	char	_patSdxCode[4];

	//-------------------------------------
	// helper functions

	// convert the pattern into its soundex representation (in _patSdxCode)
	void _prepareSdxForPattern();

	// Function to extract the next word from a line
	// returns -1 on end of line, count of skipped characters otherwise;
	// *pWordStart will contain the start of the word (relative to pString)
	long _nextWord(char* pWord, long maxWordSize, const char* pString, const char* pEnd, long* pWordStart);
};


};	// namespace soige

#endif  // __soundex_already_included_vasya__

