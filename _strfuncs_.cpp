//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _strfuncs_.cpp - implementation of asm and other custom
// string routines.
//
// No efficiency claims are made for assembly routines,
// as I am not an asm magician, but a humble student.
//
// Un caveat: in ANSI routines, I make an assumption that the
// charset being used is English, and do not use tolower() to
// convert individual chars, but rather the manual "or ch,20h".
// If the charset is other than English, the routines will
// result in case-sensitive comparisons.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_strfuncs_.h"

namespace soige {

#if defined(UNICODE) || defined(_UNICODE)
TCHAR* (*tcsichr) (const TCHAR*, int) = wstrichr;
TCHAR* (*tcsistr) (const TCHAR*, const TCHAR*) = wstristr;
#else
TCHAR* (*tcsichr) (const TCHAR*, int) = strichr;
TCHAR* (*tcsistr) (const TCHAR*, const TCHAR*) = stristr;
#endif

//------------------------------------------------------------
// ANSI strings
//------------------------------------------------------------
__declspec(naked) char* __cdecl strichr(const char* string, int c)
{
	__asm
	{
	    // foreplay
		push	ebp
		mov		ebp, esp

		// body
		// after the above manipulations the first arg is at [ebp+08h],
		// second - at [ebp+0Ch], third - at [ebp+010h], etc.
		mov		edx, [ebp+8]			// move the string address to edx
		movsx	ecx, byte ptr [ebp+0Ch]	// move the char to ecx
		//
		mov		ch, cl					// convert char to lower case if needed
		cmp		ch, 'Z'
		ja		ScanLoop
		cmp		ch, 'A'
		jb		ScanLoop
		or		ch, 20h

ScanLoop:
		mov		al, byte ptr [edx]
		cmp		al, 0h
		je		NotFound
		cmp		al, 'Z'
		ja		ScanTest
		cmp		al, 'A'
		jb		ScanTest
		or		al, 20h
ScanTest:
		cmp		al, ch
		je		ScanFound
		add		edx, 01h				// move to the next char in string
		jmp		ScanLoop

ScanFound:
		mov		eax, edx
		jmp		Return
NotFound:
		xor		eax, eax
		jmp		Return

Return:
		// epilog
		mov		esp, ebp
		pop		ebp
		ret
	}
}


__declspec(naked) char* __cdecl stristr(const char* string1, const char* string2)
{
	__asm
	{
	    // foreplay
		push	ebp
		mov		ebp, esp

		// body
		// after the above manipulations the first arg is at [ebp+08h],
		// second - at [ebp+0Ch], third - at [ebp+010h], etc.
		mov		ecx, [ebp+8]	// move string1 address to ecx
		mov		edx, [ebp+0Ch]	// move string2 address to edx
		//
		push	ecx
		push	edx
		// load first char of string2 in ah
		// characters of string1 being compared are in al, of string2 - in ah
StartScan:
		pop		edx
		pop		ecx
		mov		ah, byte ptr [edx]
		cmp		ah, 0h
		je		EmptyString2	// string2 is empty, return entire string1
		cmp		ah, 'Z'
		ja		ScanLoop
		cmp		ah, 'A'
		jb		ScanLoop
		or		ah, 20h
ScanLoop:
		mov		al, byte ptr [ecx]
		cmp		al, 0h
		je		NotFound
		cmp		al, 'Z'
		ja		ScanTest
		cmp		al, 'A'
		jb		ScanTest
		or		al, 20h
ScanTest:
		cmp		al, ah
		je		PrepMatchLoop
		inc		ecx
		jmp		ScanLoop

PrepMatchLoop:
		inc		ecx
		push	ecx			// save current pos of string1 & string2 ptrs
		push	edx
		inc		edx
MatchLoop:
		// load next char of string2 in ah
		mov		ah, byte ptr [edx]
		cmp		ah, 0h
		je		GotIt
		cmp		ah, 'Z'
		ja		String1Low
		cmp		ah, 'A'
		jb		String1Low
		or		ah, 20h
String1Low:
		mov		al, byte ptr [ecx]
		cmp		al, 0h
		je		NotFound
		cmp		al, 'Z'
		ja		MatchTest
		cmp		al, 'A'
		jb		MatchTest
		or		al, 20h
MatchTest:
		cmp		al, ah
		jne		StartScan
		inc		ecx
		inc		edx
		jmp		MatchLoop

EmptyString2:
		inc		ecx			// to be adjusted back in GotIt
		push	ecx
		push	edx

GotIt:
		pop		edx
		pop		eax			// pop the saved ecx (which is [the start of match]+1)
		dec		eax			// into eax as retval, and adjust it back 1 char
		jmp		Return

NotFound:
		xor		eax, eax	// no match, return NULL
		jmp		Return

Return:
		// epilog
		mov		esp, ebp
		pop		ebp
		ret
	}
}


//------------------------------------------------------------
// Unicode strings. The language can be other than English,
// so we rely on towlower() to convert chars to lower case.
//------------------------------------------------------------
__declspec(naked) wchar_t* __cdecl wstrichr(const wchar_t* string, int c)
{
	__asm
	{
	    // foreplay
		push	ebp
		mov		ebp, esp

		// body
		// after the above manipulations the first arg is at [ebp+08h],
		// second - at [ebp+0Ch], third - at [ebp+010h], etc.
		
		// we'll keep string address in ecx, char to find (c) - in edx
		movsx	edx, word ptr [ebp+0Ch]	// move the char to edx
		push	edx						// convert char to lower case
		call	towlower
		add		esp, 4					// adjust the stack pointer back after call
		mov		edx, eax
		and		edx, 0FFFFh				// mask out the high two bytes of return val
		
		mov		ecx, [ebp+8]			// move the string address to ecx

ScanLoop:
		movsx	eax, word ptr [ecx]
		cmp		eax, 0h					// see if reached end of string
		je		NotFound
		
		push	ecx						// save registers before calling towlower
		push	edx
		// call
		push	eax						// convert current char to lower case
		call	towlower
		add		esp, 4
		and		eax, 0FFFFh				// mask out the high two bytes of return
		
		pop		edx						// restore the registers after call
		pop		ecx

		cmp		eax, edx				// compare the chars
		je		ScanFound
		add		ecx, 02h				// move to the next char in string
		jmp		ScanLoop

ScanFound:
		mov		eax, ecx
		jmp		Return

NotFound:
		xor		eax, eax
		jmp		Return

Return:
		// epilog
		mov		esp, ebp
		pop		ebp
		ret
	}
}


__declspec(naked) wchar_t* __cdecl wstristr(const wchar_t* string1, const wchar_t* string2)
{
	__asm
	{
	    // foreplay
		push	ebp
		mov		ebp, esp

		push	ebx
		push	edi
		push	esi

		// body
		// at this point the first arg is at [ebp+08h], second - at [ebp+0Ch]
		mov		esi, [ebp+8]	// move string1 address to esi
		mov		edi, [ebp+0Ch]	// move string2 address to edi
		//
		push	esi
		push	edi
		// load first char of string2 in cx
		// characters of string1 being compared are in bx, of string2 - in cx
StartScan:
		pop		edi
		pop		esi
		mov		cx, word ptr [edi]
		cmp		cx, 0h
		je		EmptyString2	// string2 is empty, return entire string1

		// convert 1st char of string2 to lower
		push	ecx
		call	towlower
		add		esp, 4
		mov		cx, ax

ScanLoop:
		mov		bx, word ptr [esi]
		cmp		bx, 0h
		je		NotFound

		// convert current char of string1 to lower
		push	ecx				// save registers before calling towlower
		// call
		push	ebx				// convert current char to lower case
		call	towlower
		add		esp, 4
		mov		bx, ax
		pop		ecx				// restore the registers after call

		cmp		bx, cx
		je		PrepMatchLoop
		add		esi, 02h
		jmp		ScanLoop

PrepMatchLoop:
		add		esi, 02h
		push	esi				// save current pos of string1 & string2 ptrs
		push	edi
		add		edi, 02h
MatchLoop:
		// load next char of string2 in cx
		mov		cx, word ptr [edi]
		cmp		cx, 0h
		je		GotIt			// reached end of string2 and matched so far - got a match
		// convert current char of string2 to lower
		push	ecx
		call	towlower
		add		esp, 4
		mov		cx, ax

		mov		bx, word ptr [esi]
		cmp		bx, 0h
		je		NotFound		// reached end of string1 before end of string2 - no match

		// convert current char of string1 to lower
		push	ecx				// save registers before calling towlower
		push	ebx				// convert current char to lower case
		call	towlower
		add		esp, 4
		mov		bx, ax
		pop		ecx				// restore the registers after call

		cmp		bx, cx
		jne		StartScan
		add		esi, 02h
		add		edi, 02h
		jmp		MatchLoop

EmptyString2:
		add		esi, 02h		// to be adjusted back in GotIt
		push	esi
		push	edi

GotIt:
		pop		edi
		pop		eax				// pop the saved esi (which is [the start of match]+1)
		sub		eax, 02h		// into eax as retval, and adjust it back 1 wchar_t
		jmp		Return

NotFound:
		xor		eax, eax		// no match, return NULL
		jmp		Return

Return:
		pop		esi
		pop		edi
		pop		ebx

		// epilog
		mov		esp, ebp
		pop		ebp
		ret
	}
}


//----------------------------------------------------------------
// Function to check whether the matched string is a word.
// Used by matching routines when an entire word has to be matched.
// pString		- pointer to the entire string,
// pSubstring	- pointer to the start of the matching
//				  portion of the string,
// substrLen	- length of the matching substring
//				  (pattern length for fixed-string searches).
//----------------------------------------------------------------
bool isWord(const char* pString, const char* pSubstring, long substrLen)
{
	if( (pSubstring == pString) ||
		(pSubstring > pString && !__iscsym(pSubstring[-1])) )
	{
		// word beginning looks good, check word ending
		if( pSubstring[substrLen] == 0 || !__iscsym(pSubstring[substrLen]) )
		   return true;
	}
	return false;
}

//----------------------------------------------------------------
// Hex support
int ishexdigit(char c)
{
	c = toupper(c);
	if( isdigit(c) || c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' )
		return true;
	return false;
}

// pHex points to a string with at least two hex digits
char hextochar(const char* pHex)
{
	if( !ishexdigit(*pHex) || !ishexdigit(*(pHex+1)) )
		return 0;
	char c, c1 = toupper(*pHex), c2 = toupper(*(pHex+1));
	c = isdigit(c1)? (c1 - '0')*16 : (c1 - 'A' + 10)*16;
	c += isdigit(c2)? (c2 - '0') : (c2 - 'A' + 10);
	return c;
}

};	// namespace soige

