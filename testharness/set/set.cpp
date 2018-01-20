//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// set.cpp - checks the set class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_set_.h>
#include <_cstring_.h>

using namespace soige;

void check_set();

int main(int argc, char* argv[])
{
	printf("Checking _set_\n");
	check_set();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// set tests
void check_set()
{
	_set_<_cstring_> str_set;
	_cstring_ s = _T("goh");
	str_set.insert(s);
	s = _T("w");
	str_set.insert(s);
	s = _T("www");
	str_set.insert(s);
	s = _T("ww");
	str_set.insert(s);
	s = _T("zz");
	str_set.insert(s);
	s = _T("a");
	str_set.insert(s);
	ulong c = str_set.find(_cstring_(_T("ww")));
	c = str_set.size();
	c = str_set.capacity();
	s = str_set.get(2);
	c = str_set.find(_cstring_(_T("zz")));
	c = (str_set.find(_cstring_(_T("ZZ"))) < 0);
	str_set.removeAt(0);
	str_set.removeAt(2);
	str_set.remove(_cstring_(_T("w")));
	str_set = str_set;
	c = (str_set == str_set);
}


