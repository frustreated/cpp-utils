//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _wildcard_search_.h - header file for a simple wildcard
// string search class, with just two wildcards: * and ?.
// * means 'match 0 or more of any characters', and ? means
// 'match exactly one - any - character'.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __wildcard_search_already_included_vasya__
#define __wildcard_search_already_included_vasya__

#include "_common_.h"

namespace soige {

class _wildcard_search_
{
public:
	static bool wildcardPatternMatch( LPCTSTR pattern, long pat_len,
									  LPCTSTR search_str, long str_len,
									  bool case_sensitive );
	
	_wildcard_search_();
	_wildcard_search_(const _wildcard_search_& other);
	_wildcard_search_(LPCTSTR pattern, bool caseSensitive = true,
					  bool matchWholeWord = false, bool matchEntireString = false);
	
	virtual ~_wildcard_search_();
	
	virtual _wildcard_search_& operator=(const _wildcard_search_& other);

	void clear();
	void initPattern(LPCTSTR pattern, bool caseSensitive = true,
					 bool matchWholeWord = false, bool matchEntireString = false);

	bool match (LPCTSTR pString, long  stringLen,
				long* pMatchStart = NULL, long* pMatchLength = NULL);

private:
	LPTSTR	_pattern;
	bool	_caseSensitive;
	bool	_wholeWord;		// does the match have to be delimited by word boundaries?
	bool	_entireString;	// does entire string have to be matched by the pattern?
};

};	// namespace soige

#endif  // __wildcard_search_already_included_vasya__

