//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// strings.cpp - checks the _cstring_ and _wstring_ classes
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// to compile WITHOUT the optional string stuff,
// #undefine ALL_STRING_STUFF in project settings

#include <crtdbg.h>

#include <_cstring_.h>
#include <_wstring_.h>
#include <_string_.h>

using namespace soige;

void check_strings();
void test_lstring();

int main(int argc, char* argv[])
{
	printf("Checking _cstring_ and _wstring_\n");
	check_strings();
	_CrtDumpMemoryLeaks();

	printf("Checking _string_ and _lwstring_\n");
	test_lstring();
	_CrtDumpMemoryLeaks();

	return 0;
}

void check_strings()
{
	_cstring_ s;
	s = _T("Using Assignment op");
	_cstring_ s1(s);
	s1 = s;

	_cstring_ s2(_T(" using LPCTSTR ctor "));
	_cstring_ s3(_T(""));

	s3 += _T("hello!");
	s3.insert(s3, 3);
	s2 += s;
	s = s1 + s2 + _T(" he-he ") + s3;
	s.insert(_T("-abrakadabra-"), 5);
	s.replace(s1, 2, 4);

	if(s==_T("duh!"))
		0;
	if(s==s2)
		0;
	int i = s.compare(s3, false);
	i = s.compare(s, false);
	int j = s1.find(_T("N"), 3, false);
	s2.upper();
	s2.lower();
	int k = s1.length();
	k = s1.capacity();
	s3 = s1.substring(2, 3);
	s2.clear();
	(s1 = _T("")).append(_T("he: "));
	s1.append(3.4);
	s1.append(_T(" ")).append((long)209).append(_cstring_(_T(" "))).append(65422L);

	if(!s.isNull())
		j = s[0];
	if(s2.isNull())
		0;
	if(s3)
		1;
	s.reverse();
	s1.reverse();
	s3.reverse();
	s.reverse();
	s1.reverse();
	s3.reverse();
	
	s = _T("  ab ba  ");
	s.trim();
	s = _T("  a");
	s.trim();
	s = _T("cda   ");
	s.trim();
	s = _T("     ");
	s.trim();

	s1.format(_T("%s, %d, 0x%08lx"), _T("shtring"), 789, 0x4abcdef2);
	s1 = _T("%s, %d, 0x%08lx");
	s1.format(s1, _T("shtring"), 789, 0x4abcdef2);
	s1 = _T("shtring");
	s1.format(_T("%s, %d, 0x%08lx"), (LPCTSTR)s1, 789, 0x4abcdef2);
	s1.format(_T("%s, %d, %f, %s, 0x%08lX"),
			  _T("long and boring shtring"), 789, 13237.9807,
			  _T("this one is too long to fit in 32 characters. yep."), 0x4abcdef2);
	k = s1.length();
	
	s1.replaceOccurences(_T("IN"), _T("YNN"), 10, 2, false);
	k = s1.length();

	if((i=s1.findReverse(_T("lonG"), MAX_INT, false)) >= 0)
		s = s1.substring(i);

	s.clear();
	s3 = s2 = s1 = s;
	s += 754L;
	s1 += (TCHAR)67;
	s2 += 13351L;
	s3 += 9.576453;

	s.clear();
	s = 9.576453;

	_cstring_ s4(_T('u'), 50);
	s4 = 83L + s1 + 8.23 + _T('Q');
	
	s = _T("abba baab dubba");
	i = s.find(_T("dub"));
	j = s.find(_T("DUB"), 3, false);
	s = s1 + s3;
	s = _T("abba") + _cstring_(_T('\0')) + _T("baab") + _T('\0') + _T("dubba");
	s.upper();
	s.reverse();
	s.lower();
	s.reverse();
	i = s.find(_T("dub"));
	j = s.find(_T("DUB"), 3, false);

	_cstring_ s5 = _T("come one, come all");
	s5 = (LPCTSTR) s5;
	s5 = s5;
	s5.append(s5);
	s5.append((LPCTSTR)s5);

	s = s5.left(3);
	s5.chopLeft(3);
	s = s5.right(5);
	s5.chopRight(5);
	s.padRight(14);
	s.clear();
	s.padLeft(4, '7');
	s = "vi";
	s.padLeft(17);

	_wstring_ ws = L" bla";
	s += ws;
	ws += _cstring_(" abba");

#ifdef ALL_STRING_STUFF
	// check split and join
	_cstring_ s6 = _T("dodo&sasa&masha&daz&");
	_array_<_cstring_> split_arr;
	s6.split(split_arr, _T("&"));
	s6 = s6.join(split_arr, _T("^"), 1, 3);

	SYSTEMTIME st;
	GetSystemTime(&st);
	s5.formatDate(_T("mm/dd/yy"), _T("HH:mm:ss"), st);
	_tprintf(_T("Current date/time: %s\n"), (LPCTSTR)s5);
#endif

	_cstring_ f = "searching in this string for something";
	_cstring_ t = "searching";
	i = f.find(t);
	t = "in THIS"; t.upper();
	i = f.find(t);
	i = f.find(t, 0, false);
	i = f.find(t, i, false);
	i = f.find(t, i+3, false);
	t = "thing";
	i = f.find(t, 33);
	i = f.find(t, 35);
	t = "things";
	i = f.find(t, 33);

	BYTE arr[12];
	f.replaceBytes(arr, 12, 3, 6);
	f.replaceBytes(arr, 0, 3, 12);
	f.insertBytes(arr, 12, 0);
	f.insertBytes(arr, 12, f.length());
}


void test_lstring()
{
	// lstring tests
	bool b;
	int n;
	long l;
	double d;
	char c;

	_string_ s1, s2, s3;
	s1 = "String 1";
	s2 = s1;
	s3 = s2;
	s3 = s1.substring(2);
	
	_string_ s4("String 1");
	_string_ s5(s4);
	_string_ s6('Q', 6); // -1, 0, 6
	_string_ s7(234360L);
	_string_ s8(343.5321);

	s4 = s4;
	s5 = 'T';
	s6 = 90132213L;
	s7 = 8989.2342093;

	s4 += s5;
	s5 += "Adding Another String.";
	s6 += 'L';
	s7 += 234399L;
	s8 += 7777.7777;

	if( !s1 )
		s1.clear();
	b = s4 < s8;
	b = s5 < "ABBA";
	b = s6 == s6;
	b = s7 == s7.c_str();
	b = s8 != s8;
	b = s3 != "tring 1";
	n = s3.compare( s3, false );
	n = s4.compare( "puppa", true );
	n = s5.startsWith( "stri", false );
	n = s5.startsWith( "" );
	n = s5.endsWith( "" );
	n = s5.endsWith( s5.right(4).c_str() );

	n = s5.find( _string_("rI"), -1 );
	n = s5.find( _string_("rI"), 0, false );
	n = s5.find( s1.c_str(), 5 );
	n = s8.findReverse( "343.5", 2 );
	n = s8.findReverse( "343.5" );

	b = s5.matchesPattern( "*ano?h* strIn??" );
	b = s5.matchesPattern( "*ano?h* strIn??", false );
	s1 = s5.substring( 9 );
	s1 = s5.substring( 1, 4 );
	s1 = s5;
	s1 = s1.left( 3 );
	s1 = s5.right( 4 );
	s1.clear();
	if( s1.isNull() )
		s1.append( "sdfsf" );
	n = s5.length();
	n = s5.capacity();
	b = s7.isNumeric();
	l = s7.longVal();
	d = s7.doubleVal();

	s6.swap( s7 );
	s6.swap( s6 );
	s6.upper();
	s7.lower();
	s8.reverse();
	s8.append( '\0' ).append( '\xD5' ).append( '\0' ).append( '\0' );
	s1 = s8;
	s8.reverse();
	s8.reverse();
	b = s1 == s8;

	n = s8.length();
	s8.padRight( n+4, '\0' );
	s8.padLeft( n+7, '\0' );
	n = s8.length();
	s8.trim( '\0' );
	n = s8.length();
	
	s2 = s8;
	s8.chopLeft( 2 );
	s8.chopRight( 4 );
	c = s1[2];
	s1[2] = s1[3] = s1[4];
	c = s1[2];
	printf("s5 value: %s\n", s5.c_str() );

	s2 = s8;
	s8.insert( "::Insert Str::", 2 );
	s8.replace( "::Replace Str::", 2, lstrlenA("::Insert Str::") );

	char car[5] = { '\0', '4', '\a', '\f', 'R' };
	s8.insertBytes( car, 5, s8.length() );
	s2.replaceBytes( car, 4, 3, 1 );
	s8.replaceOccurences( "::", "!!-!!", s8.length(), 1, false );
	s8.replaceOccurences( "::", "!!-!!", 1, 1, false );
	s8.replaceOccurences( NULL, NULL );
	s8.replaceOccurences( "9283yrwehfsdjf", "$" );
	s2.replaceOccurences( "\a", "" );

	s1 = "Trying %e, to 0x%08lx: %s format a string";
	s2 = s1;
	s1.format( s1.c_str(), 3124.30907, s2.c_str(), s1.c_str()+s1.find("format") );
	
	SYSTEMTIME st;
	GetSystemTime(&st);
	s5.formatDate( "mm/dd/yy", "HH:mm:ss", st );
	printf( "Current date/time: %s\n", s5.c_str() );
	
#ifdef ALL_STRING_STUFF
	// check split and join
	s6 = "dodo&sasa&masha&daz&";
	_array_<_string_> split_arr;
	s6.split( split_arr, "&" );
	s6 = s6.join( split_arr, "^", 1, 3 );
#endif

	
	s1 = s2 + s2 + s3;
	s1 = s2 + " :append";
	s1 = "prepend: " + s1;
	s2 = s1;
	s1 = s2 + '4' + 2343565L + 4.256;
	s1 = '5' + s2;
	s1 = 9230943. + s1;

	// wide strings
	// ctor, =, +=
	_string_ wstr( L"Unicode shtring" );
	s3 = wstr;
	wstr = L"bozo";
	wstr += L" and dozo ";
	wstr.append( L"went to school" );
}

