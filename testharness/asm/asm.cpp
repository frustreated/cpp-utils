//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// asm.cpp - checks the win32_file class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_common_.h>
#include <_strfuncs_.h>

using namespace soige;

void check_asm();

int main(int argc, char* argv[])
{
	printf("Checking asm routines\n");
	check_asm();
	_CrtDumpMemoryLeaks();
	return 0;
}

//------------------------------------
// assembly functions tests
void check_asm()
{
	char* s1 = "Hello Buddy! Johnny got a case of bloody NoSe.";
	char* s2 = "bLO";
	char  b  = 'j';
	char* p;

	p = strichr(s1, b);
	p = strichr(s1, 'Z');
	p = stristr(s1, s2);
	p = stristr(s1, "cUpId");
	
	wchar_t* ws1 = L"Hello Buddy! Johnny got a case of bloody NoSe.";
	wchar_t* ws2 = L"bLO";
	wchar_t  wc  = L'j';
	wchar_t* pw;
	pw = wstrichr(ws1, wc);
	pw = wstrichr(ws1, L'Z');
	pw = wstristr(ws1, ws2);
	pw = wstristr(ws1, L"cUpId");
	pw = wstristr(ws1, L"");
	pw = wstristr(L"", ws2);
	
	//p = tcsichr(s1, b);
	//p = tcsistr(s1, s2);
}

