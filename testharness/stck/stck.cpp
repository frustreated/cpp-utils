//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// stck.cpp - checks the stack classes
// (_stack_array_ & _stack_list_).
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_stack_array_.h>
#include <_stack_list_.h>
#include <_cstring_.h>

using namespace soige;

void check_stack_array();
void check_stack_list();

int main(int argc, char* argv[])
{
	printf("Checking _stack_array_\n");
	check_stack_array();
	check_stack_list();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// stack tests
void check_stack_array()
{
	_stack_array_<_cstring_> str_stk;
	_cstring_ s;
	str_stk.push(_cstring_(_T("100.00")));
	str_stk.push(_cstring_(_T("200.00")));
	str_stk.push(_cstring_(_T("300.00")));
	s = _T("200.00");
	ulong c = str_stk.find(s);
	c = str_stk.depth();
	str_stk = str_stk;
	s = *str_stk.top();
	str_stk.pop(s);
	str_stk.pop(s);
	str_stk.pop(s);
	str_stk.push(_cstring_(_T("300.00")));
	str_stk.rewind();
}


void check_stack_list()
{
	_stack_list_<_cstring_> str_stk;
	_cstring_ s;
	str_stk.push(_cstring_(_T("100.00")));
	str_stk.push(_cstring_(_T("200.00")));
	str_stk.push(_cstring_(_T("300.00")));
	s = _T("200.00");
	ulong c = str_stk.find(s);
	c = str_stk.depth();
	ulong l = str_stk.maxDepth();
	str_stk.setMaxDepth(3);
	str_stk.push(_cstring_(_T("300.00")));
	str_stk.push(_cstring_(_T("300.00")));
	str_stk.push(_cstring_(_T("300.00")));
	c = str_stk.depth();
	str_stk = str_stk;
	s = *str_stk.top();
	str_stk.pop(s);
	str_stk.pop(s);
	str_stk.pop(s);
	str_stk.push(_cstring_(_T("300.00")));
	str_stk.rewind();
}

