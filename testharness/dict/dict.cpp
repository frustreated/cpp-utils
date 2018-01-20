//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// dict.cpp - checks the dictionary class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_dictionary_.h>
#include <_ptr_.h>
#include <_cstring_.h>

using namespace soige;

void check_dict();

int main(int argc, char* argv[])
{
	printf("Checking _dictionary_\n");
	check_dict();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// dictionary tests
void check_dict()
{
	_dictionary_<_ptr_<int>, TCHAR> int_chr_dict;
	_ptr_<int> key = new int;
	int_chr_dict.put(key, _T('a'));
	key = new int;
	int_chr_dict.put(key, _T('b'));
	key = new int;
	int_chr_dict.put(key, _T('b'));
	try {
		int_chr_dict.put(key, _T('c'));
	} catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
	int_chr_dict[key] = _T('G');
	int_chr_dict.put(_ptr_<int>(new int), _T('c'));
	TCHAR c = int_chr_dict.get(key);
	bool b = int_chr_dict.containsKey(key);
	b = int_chr_dict.containsElement(_T('c'));
	b = int_chr_dict.containsKey(_ptr_<int>(new int));
	b = int_chr_dict.containsElement(_T('z'));
	ulong size = int_chr_dict.size();
	int_chr_dict.remove(key);
	key = new int;
	int_chr_dict.remove(key);
	size = int_chr_dict.size();
	const _array_<TCHAR>& els = int_chr_dict.elements();
	size = els.length();
	const _set_<_ptr_<int> >& keys = int_chr_dict.keys();
	b = (int_chr_dict==int_chr_dict);
	int_chr_dict = int_chr_dict;
	int_chr_dict.clear();

	_dictionary_<_cstring_, double> str_dbl_dict;
	_cstring_ skey = _T("first");
	str_dbl_dict.put(skey, 9.785);
	ulong count = str_dbl_dict.size();
	skey = _T("second");
	str_dbl_dict.put(skey, 19.785);
	count = str_dbl_dict.size();
	skey = _T("any");
	str_dbl_dict.put(skey, 349.785);
	count = str_dbl_dict.size();
	skey = _T("third");
	str_dbl_dict.put(skey, .785);
	count = str_dbl_dict.size();
	skey = _T("notlast");
	str_dbl_dict.put(skey, 89.785);
	count = str_dbl_dict.size();
	double val = str_dbl_dict.get(skey);
	val = str_dbl_dict.get(_T("first"));
	val = str_dbl_dict.get(_T("second"));
	val = str_dbl_dict.get(_T("third"));
	val = str_dbl_dict.get(_T("any"));
	str_dbl_dict.remove(_T("first"));
	count = str_dbl_dict.size();
	str_dbl_dict.remove(_T("notlast"));
	count = str_dbl_dict.size();
	str_dbl_dict.remove(_T("any"));
	count = str_dbl_dict.size();
	skey = _T("aaa");
	str_dbl_dict.put(skey, 85.98);
	skey = _T("zzz");
	str_dbl_dict.put(skey, 5.98);
	skey = _T("fizzy");
	str_dbl_dict.put(skey, 125.98);
	skey = _T("zaa");
	str_dbl_dict.put(skey, 7.98);
	try {
		str_dbl_dict.put(skey, 7.98);
	} catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
	val = str_dbl_dict.get(_T("aaa"));
	val = str_dbl_dict.get(_T("fizzy"));
	val = str_dbl_dict.get(_T("zaa"));
	try {
		val = str_dbl_dict.get(_T("Ty"));
	} catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
	str_dbl_dict.remove(_T("zzz"));
	str_dbl_dict.remove(_T("aaa"));
	str_dbl_dict.remove(_T("fizzy"));
	val = str_dbl_dict.get(_T("second"));
	val = str_dbl_dict.get(_T("third"));
	try {
		str_dbl_dict.remove(_T("Ty"));
	} catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
}

