//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// utils - master project for my utility classes/functions.
//
// All of them are defined to be in the 'soige' namespace.
//		Locations are:
// strichr, stristr, wstrichr, wstristr are in _strfuncs_.cpp
// _base64_			-	_base64_.h and _base64_.cpp
// _file_finder_	-	_file_finder_.h and _file_finder_.cpp
// _win32_file_		-	_win32_file_.h and _win32_file_.cpp
// _array_<>		-	_array_.h
// _set_<>			-	_set_.h
// _dictionary_<>	-	_dictionary_.h
// _list_<>			-	_list_.h
// _stack_array_<>	-	_stack_array_.h
// _stack_list_<>	-	_stack_list_.h
// _queue_<>		-	_queue_.h
// _ptr_<>			-	_ptr_.h
// _cstring_		-	_cstring_.h and _cstring_.cpp
// _wstring_		-	_wstring_.h and _wstring_.cpp
// _string_			-	_string_.h and _string_.cpp
// _sort_<>			-	_sort_.h
// _table_<>		-	_table_.h
// _tree_<>			-	_tree_.h
// streams			-	_base_stream_.h, _byte_stream_.h,
//						_file_stream_.h,
//						_base_stream_.cpp,
//						_byte_input_stream_.cpp,
//						_byte_output_stream_.cpp,
//						_file_input_stream_.cpp,
//						_file_output_stream_.cpp
// _num_eval_		-	_num_eval_.h and _num_eval_.cpp
// _boyer_moore_	-	_boyer_moore_.h
// _soundex_		-	_soundex_.h
// _shared_memory_	-	_shared_memory_.h and _shared_memory_.cpp
// _ipc_channel_	-	_ipc_channel_.h and _ipc_channel_.cpp
// locks			-	_lock_.h
// _thread_pool_	-	_thread_pool_.h and _thread_pool_.cpp
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//-----------------------------------------------------
// Performance statistics (PIII 500MHz, average, ms),
// using _array_ based on assigment operator copying:
//	my _array_<int>		insert(): 420
//	std::vector<int>	insert(): 1320
//	my _array_<int>		remove(): 160
//	std::vector<int>	remove(): 520
//	my _array_<string>	insert(): 1200
//	std::vector<string> insert(): 650
//	my _array_<string>	remove(): 130
//	std::vector<string> remove(): 170
//	my _array_<string>	insert() after remove all: 145
//	std::vector<string> insert() after remove all: 260
//-----------------------------------------------------
// Performance statistics (PIII 450MHz, average, ms),
// using _array_ based on a mix of assignment operator
// and in-place construction copying:
//  my _array_<int>		insert(): 840
//  std::vector<int>	insert(): 1730
//  my _array_<int>		remove(): 430
//  std::vector<int>	remove(): 680
//  my _array_<string>	insert(): 1550
//  std::vector<string>	insert(): 830
//  my _array_<string>	remove(): 190
//  std::vector<string>	remove(): 220
//  my _array_<string>	insert() after remove all: 260
//  std::vector<string> insert() after remove all: 310
//-----------------------------------------------------
// Performance statistics (PIII 450MHz, average, ms),
// using _set_ based on a mix of assignment operator
// and in-place construction copying:
//  my set<int>			insert(): 1030	- ordered insertions, 500,000 items
//								  5030	- random insertions, 50,000 items
//  std::set<int>		insert(): 30000	- ordered, 500,000 items
//								  1830	- random, 50,000 items
//-----------------------------------------------------


// identifier was truncated to 256 chars in debug info
#pragma warning (disable:4786)

#include <stdio.h>
#include <conio.h>
#include <crtdbg.h>

#include <vector>
#include <set>
using namespace std;

// my includes
#include <_file_finder_.h>
#include <_win32_file_.h>
#include <_array_.h>
#include <_set_.h>
#include <_dictionary_.h>
#include <_list_.h>
#include <_stack_array_.h>
#include <_stack_list_.h>
#include <_queue_.h>
#include <_ptr_.h>
#include <_cstring_.h>
#include <_wstring_.h>
#include <_string_.h>
#include <_sort_.h>
#include <_table_.h>
#include <_num_eval_.h>
#include <_boyer_moore_.h>
#include <_soundex_.h>
#include <_shared_memory_.h>
#include <_ipc_channel_.h>
#include <_byte_stream_.h>
#include <_file_stream_.h>
#include <_wildcard_search_.h>
#include <_thread_pool_.h>

using namespace soige;

void performance();

int main(int argc, char* argv[])
{
	TCHAR file[512];
	_file_finder_ ff;
	ff.initPattern(argv[1], true);
	while( ff.getNextFile( file ) )
		printf("%s\n", file);
	return 0;

	_wildcard_search_ s;
	char* pattern = "t*a*?a-ta "; // match == 0
//	char* pattern = "t*?????a-";
//	char* pattern = "??????a-";
//	char* pattern = "?????a-d";

	char* search  = "tra ta-ta ta-da";
//	char* search  = "tra ta-ta-dta-da";
	char p[128] = "";
	bool b;
	long start = 0, length = 0;

	if(argc >= 3)
	{
		pattern = argv[1];
		search = argv[2];
	}
	s.initPattern(pattern);
	b = s.match(search, lstrlen(search), &start, &length);
	if(b)
	{
		lstrcpyn(p, &search[start], length+1);
		printf("Match found: %s\n", p);
	}
	else
		printf("No match\n");

	return 0;
	
	performance();
	_CrtDumpMemoryLeaks();
	/*
	_set_<int> seti;
	int ar[] = { 4, 234, 23, 90909, 29384, 3, 23432, 45,
				 909, 983402, 984732, 2341, 543, 354,
				 20938, 3452, 9873, 2837, 987, 9877, 8, 0 };
	for(int i=0; ar[i]; i++)
		seti.insert(ar[i]);
	for(i=1; i<seti.size(); i++)
		if(seti.get(i) < seti.get(i-1))
			printf("Bad dog: %d, %d\n", seti.get(i), seti.get(i-1));
	*/
	return 0;
}


//------------------------------------
// performance tests
void performance()
{
	vector<int> vi1;
	_array_<int> vi2;
	vector<_string_> vs1;
	_array_<_string_> vs2;
	_string_ s( "sadjbsfdskjf" );
	
	set<int> set1;
	_set_<int> set2;
	int curr = 0;
	
//goto sets_bebe;

	int i;
	unsigned long c;
	c = GetTickCount();
	for(i=0; i<500000; i++)
		vi2.append(i);
	c = GetTickCount()-c;
	printf("my _array_<int> insert(): %u\n", c);
	c = GetTickCount();
	for(i=0; i<500000; i++)
		vi1.insert(vi1.end(), i);
	c = GetTickCount()-c;
	printf("std::vector<int> insert(): %u\n", c);
	
	c = GetTickCount();
	for(i=500000-1; i>=0; i--)
		vi2.removeAt(i);
	c = GetTickCount()-c;
	printf("my _array_<int> remove(): %u\n", c);
	c = GetTickCount();
	for(i=500000-1; i>=0; i--)
		vi1.erase(vi1.end()-1);
	c = GetTickCount()-c;
	printf("std::vector<int> remove(): %u\n", c);
	
	c = GetTickCount();
	for(i=0; i<50000; i++)
		vs2.append(s);
	c = GetTickCount()-c;
	printf("my _array_<string> insert(): %u\n", c);
	c = GetTickCount();
	for(i=0; i<50000; i++)
		vs1.insert(vs1.end(), s);
	c = GetTickCount()-c;
	printf("std::vector<string> insert(): %u\n", c);

	c = GetTickCount();
	for(i=50000-1; i>=0; i--)
		vs2.removeAt(i);
	c = GetTickCount()-c;
	printf("my _array_<string> remove(): %u\n", c);
	c = GetTickCount();
	for(i=50000-1; i>=0; i--)
		vs1.erase(vs1.end()-1);
	c = GetTickCount()-c;
	printf("std::vector<string> remove(): %u\n", c);

	c = GetTickCount();
	for(i=0; i<50000; i++)
		vs2.append(s);
	c = GetTickCount()-c;
	printf("my _array_<string> insert() after remove all: %u\n", c);
	c = GetTickCount();
	for(i=0; i<50000; i++)
		vs1.insert(vs1.end(), s);
	c = GetTickCount()-c;
	printf("std::vector<string> insert() after remove all: %u\n", c);

	//---------------------------------------
	//---------------------------------------
//sets_bebe:

	printf("\nTesting sets\n");

	// my set
	srand(2345);
	c = GetTickCount();
//	// sorted (in-order) insertions
//	for(i=0; i<500000; i++)
//		try { set2.insert(i); } catch(soige_error&) { }

	// random (not ordered) insertions
	for(i=50000-1; i>=0; i--)
	{
		curr = ((50000-i) * rand()) + i;
		set2.insert(curr);
	}
	
	c = GetTickCount()-c;
	printf("my set<int> insert(): %u\n", c);

	// check integrity
	for(i=1; i<set2.size(); i++)
		if(set2.get(i-1) == set2.get(i))
			printf("Bad uniqueness test: i-1 == %d, i == %d\n", set2.get(i-1), set2.get(i));
		else if(set2.get(i-1) > set2.get(i))
			printf("Bad sort: i-1 == %d, i == %d\n", set2.get(i-1), set2.get(i));

	// std::set
	srand(2345);
	c = GetTickCount();
//	// sorted (in-order) insertions
//	for(i=0; i<500000; i++)
//		try { set1.insert(i); } catch(exception&) { }
	
	// random (not ordered) insertions
	for(i=50000-1; i>=0; i--)
	{
		curr = ((50000-i) * rand()) + i;
		try { set1.insert(curr); } catch(exception&) { }
	}
	
	c = GetTickCount()-c;
	printf("std::set<int> insert(): %u\n", c);
	
	c = GetTickCount();
return;

	for(i=50000-1; i>=0; i--)
		set2.removeAt(i);
	c = GetTickCount()-c;
	printf("my _set_<int> remove(): %u\n", c);
	c = GetTickCount();
	set<int>::iterator it;
	for(i=50000-1; i>=0; i--)
	{
		it = set1.end();
		set1.erase(--it);
	}
	c = GetTickCount()-c;
	printf("std::set<int> remove(): %u\n", c);
	
	// _string_ sets
	_set_<_string_> set3;
	set<_string_> set4;
	s = "sadjbsfdskjf";
	c = GetTickCount();
	for(i=0; i<50000; i++)
		set3.insert(s);
	c = GetTickCount()-c;
	printf("my _set_<string> insert(): %u\n", c);
	c = GetTickCount();
	for(i=0; i<50000; i++)
		set4.insert(s);
	c = GetTickCount()-c;
	printf("std::set<string> insert(): %u\n", c);

	c = GetTickCount();
	for(i=50000-1; i>=0; i--)
		vs2.removeAt(i);
	c = GetTickCount()-c;
	printf("my _array_<string> remove(): %u\n", c);
	c = GetTickCount();
	for(i=50000-1; i>=0; i--)
		vs1.erase(vs1.end()-1);
	c = GetTickCount()-c;
	printf("std::vector<string> remove(): %u\n", c);
}

