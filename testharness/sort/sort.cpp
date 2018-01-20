//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sort.cpp - checks the sort class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_sort_.h>

using namespace soige;

void check_sort();

int main(int argc, char* argv[])
{
	printf("Checking sorting\n");
	check_sort();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// sort tests
#if defined(UNICODE) || defined(_UNICODE)
	#define ltot(a, b, c)	_ltow(a, b, c)
#else
	#define ltot(a, b, c)	_ltoa(a, b, c)
#endif
 
int int_array[250000];
LPTSTR str_array[10000];

void check_sort()
{
	int indx;
	int int_array_size = sizeof(int_array)/sizeof(int);
	int str_array_size = sizeof(str_array)/sizeof(LPTSTR);

	// -- check integer sorting
	// fill
	for (indx=0; indx < int_array_size; ++indx)
		int_array[indx] = rand();

	// sort
	_sort_<int> intsort;
	intsort.sort(int_array, int_array_size);
	for (indx=1; indx < int_array_size; ++indx)
	{
		if (int_array[indx - 1] > int_array[indx])
		{
			_tprintf(_T("Bad int sort\n"));
			break;
		}
	}
 
	// -- check string sorting
	// fill
	TCHAR cc[2] = {0,0};
	for (indx=0; indx < str_array_size; ++indx)
	{
		str_array[indx] = new TCHAR[32];
		ltot(rand(), str_array[indx], 10);
		cc[0] = 65 + (rand()%57);
		lstrcat(str_array[indx], cc);
	}

	// sort
	_sort_<LPTSTR> strsort;
	strsort.sort(str_array, str_array_size);
	for (indx=1; indx < str_array_size; ++indx)
	{
		if (lstrcmp(str_array[indx - 1], str_array[indx]) > 0 )
		{
			_tprintf(_T("Bad LPTSTR sort\n"));
			break;
		}
		delete [] str_array[indx-1];
	}
	delete [] str_array[indx-1];
}

