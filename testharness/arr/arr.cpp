//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// arr.cpp - checks the array class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_array_.h>
#include <_cstring_.h>

using namespace soige;

void check_array();

int main(int argc, char* argv[])
{
	printf("Checking _array_\n");
	check_array();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// array tests
struct test
{
	int p;
	test(){p=(56);}
	test(const test& t){p=(t.p);}
	test& operator=(const test& t) {
		p=t.p;
		return *this;
	}
	bool operator==(const test& t) const {
		return p==t.p;
	}
	bool operator<(const test& t) const {
		return p<t.p;
	}
	~test() {;}
};

void check_array()
{
	_array_<double> dbl_arr;
	try {
		dbl_arr.get(34);
	}
	catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
	_array_<double> dbl_arr1;
	dbl_arr.insert(3.0, 0);
	dbl_arr.insert(2.0, 0);
	dbl_arr.insert(1.0, 0);
	dbl_arr.insert(5.0, dbl_arr.length());
	dbl_arr.insert(4.0, dbl_arr.length()-1);
	dbl_arr1.insert(25.0, 0);
	dbl_arr1.insert(30.0, 1);
	const _array_<double> dbl_arr2(dbl_arr1);
	//dbl_arr2[0] = 2.89; // can't assign to const
	dbl_arr = dbl_arr1;
	dbl_arr = dbl_arr;
	dbl_arr.clear();
	dbl_arr = dbl_arr;
	
	dbl_arr.append(1.986);
	dbl_arr.append(2.986);
	dbl_arr.prepend(0.986);
	ulong c;
	c = dbl_arr.find(1.986);
	double da[] = { 7.89, 4.123, 13.212, .67, .767, 2.133 };
	dbl_arr.insertNAt(0, da, sizeof(da)/sizeof(da[0]));
	c = dbl_arr.capacity();
	c = dbl_arr.length();
	dbl_arr.removeAt(0);
	dbl_arr.removeAt(2);
	dbl_arr.reverse();
	dbl_arr.remove(4.123);
	double d = dbl_arr.get(1);
	d = dbl_arr[2];
	dbl_arr.sort();
	dbl_arr.resize(34);
	dbl_arr = dbl_arr;


	test t;
	_array_<test> tst_arr;
	tst_arr.insert(test(), 0);
	t.p = 60;
	tst_arr.insert(t, tst_arr.length());
	tst_arr.insert(t, tst_arr.length());
	tst_arr.insert(tst_arr.get(0), tst_arr.length());
	tst_arr.insert(tst_arr[0], tst_arr.length());
	for(ulong i=0; i<tst_arr.length(); i++)
		_tprintf(_T("i = %u, arr[i].p = %d\n"), i, tst_arr[i].p);
	tst_arr.insert(test(), 0);
	tst_arr.insert(test(), 0);
	tst_arr.insert(test(), 0);
	tst_arr.insert(test(), 0);
	tst_arr.insert(test(), 0);
	tst_arr.insert(test(), 0);
	tst_arr.insert(test(), 0);
	tst_arr.remove(test());
	tst_arr.remove(test());
	tst_arr.remove(test());
	tst_arr.removeAt(tst_arr.length()-1);
	tst_arr.removeAt(0);
	tst_arr.removeAt(0);
	tst_arr.removeAt(2);
	test* pt = &tst_arr[0] - 1;
	while(++pt < &tst_arr[0]+tst_arr.length())
		_tprintf(_T("pt->p = %d\n"), pt->p);

	_array_<_cstring_> str_arr;
	_cstring_ s = _T("goh");
	str_arr.insert(s, 0);
	s = _T("bah");
	str_arr.insert(s, 0);
	s = _T("aah");
	str_arr.insert(s, 0);
	_cstring_ s1 = _T("goh");
	str_arr.remove(s1);
	str_arr.sort();

	str_arr.clear();
	str_arr.append(_cstring_(1.986));
	str_arr.append(_cstring_(2.986));
	str_arr.prepend(_cstring_(_T("0.986")));
	c = str_arr.find(_cstring_(_T("1.986")));
	_cstring_ sa[] = { _cstring_(_T("7.89")), _cstring_(_T("4.123")),
					  _cstring_(_T("13.212")), _cstring_(_T(".67")),
					  _cstring_(_T(".767")), _cstring_(_T("2.133")) };
	str_arr.insertNAt(2, sa, sizeof(sa)/sizeof(sa[0]));
	c = str_arr.capacity();
	c = str_arr.length();
	str_arr.removeAt(0);
	str_arr.removeAt(3);
	str_arr.reverse();
	str_arr.remove(_cstring_(_T("4.123")));
	str_arr.removeNAt(2, 2);
	s = str_arr.get(1);
	s = str_arr[2];
	str_arr.sort();
	str_arr.resize(34);
	str_arr = str_arr;
}

