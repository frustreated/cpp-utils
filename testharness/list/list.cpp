//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// list.cpp - checks the list class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_list_.h>

using namespace soige;

void check_list();

int main(int argc, char* argv[])
{
	printf("Checking _list_\n");
	check_list();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// list tests
void check_list()
{
	ulong i;
	_list_<int> int_list;
	int_list.append(25);
	int_list.append(30);
	int_list.append(35);
	int_list.append(40);
	int_list.prepend(10);
	int_list.prepend(5);
	int_list.prepend(0);
	int_list.insert(20, 3);
	int_list.insert(15, 3);
	i = int_list.find(20);
	i = int_list.find(1220);
	int_list.append(20);
	int_list.append(50);
	i = int_list.findLast(20);
	int_list = int_list;
	bool b = int_list == int_list;
	i = *int_list.first();
	i = *int_list.last();
	for(i=0; i<int_list.count(); i++)
		_tprintf(_T("Item #%u: %d\n"), i, *int_list.getAt(i));
	int_list.remove(10);
	int_list.removeAt(int_list.count()-2); // 35
	int_list.remove(40);
	int_list.removeFirst();
	int_list.removeLast();
	_tprintf(_T("\n"));
	for(i=0; i<int_list.count(); i++)
		_tprintf(_T("Item #%u: %d\n"), i, *int_list.getAt(i));
	_unique_list_<int> uq_list;
	for(i=0; i<int_list.count(); i++)
		if(uq_list.find(*int_list.getAt(i)) < 0)
			uq_list.append(*int_list.getAt(i));
	i = i;
	_list_<int> int_list1 = int_list;
	_unique_list_<int> uq_list1 = uq_list;
	int_list.clear();

	_list_<LPTSTR> char_list;
	TCHAR buf1[3] = _T("10");
	char_list.append(_T("25"));
	char_list.append(_T("30"));
	char_list.append(_T("35"));
	char_list.append(_T("40"));
	char_list.prepend(&buf1[0]);
	char_list.prepend(_T("5"));
	char_list.prepend(_T("0"));
	char_list.insert(_T("20"), 3);
	char_list.insert(_T("15"), 3);
	for(i=0; i<char_list.count(); i++)
		_tprintf(_T("Item #%u: %s\n"), i, *char_list.getAt(i));
	TCHAR buf2[3] = _T("10");
	char_list.remove(&buf2[0]);
	char_list.remove(_T("35"));
	char_list.remove(_T("40"));
	_tprintf(_T("\n"));
	for(i=0; i<char_list.count(); i++)
		_tprintf(_T("Item #%u: %s\n"), i, *char_list.getAt(i));
	char_list.append(_T("35"));
	char_list.append(_T("40"));
	_list_iterator_<LPTSTR> char_list_it(char_list);
	for(char_list_it.begin(); !char_list_it.isDone(); char_list_it.next())
		_tprintf(_T("Item: %s\n"), char_list_it.currentItem());

	_unique_list_<float> fl_list;
	fl_list.append(23.5);
	fl_list.append(24.5);
	fl_list.append(25.5);
	try {
		fl_list.append(24.5);
	} catch(soige_error& e) {
		printf("Caught exception adding 24.5: %s\n", e.what());
	}
}

