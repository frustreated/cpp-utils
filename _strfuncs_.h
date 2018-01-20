//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _strfuncs_.h - custom string functions declarations.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __strfuncs_already_included_vasya__
#define __strfuncs_already_included_vasya__

#include <ctype.h>
#include <tchar.h>

namespace soige {

//------------------------------------------------------------
// asm routines for case-insensitive string stuff;
// defined in _strfuncs_.cpp; all strings must be non-null.
char*    __cdecl strichr (const char*    string,  int c);
char*    __cdecl stristr (const char*    string1, const char*    string2);
wchar_t* __cdecl wstrichr(const wchar_t* string,  int c);
wchar_t* __cdecl wstristr(const wchar_t* string1, const wchar_t* string2);

extern TCHAR* (*tcsichr) (const TCHAR*, int);
extern TCHAR* (*tcsistr) (const TCHAR*, const TCHAR*);

/*
// quick string testing
inline int streq(LPCTSTR s1, LPCTSTR s2) {return (!lstrcmp(s1, s2));}
*/

// Checks if matched substring is a word
bool isWord(const char* pString, const char* pSubstring, long substrLen);

int ishexdigit(char c);

// Converts hex-encoded 2-char string to the corresponding char
char hextochar(const char* pHex);

};	// namespace soige

#endif  // __strfuncs_already_included_vasya__

