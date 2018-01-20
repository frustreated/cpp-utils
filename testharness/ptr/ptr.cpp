//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ptr.cpp - checks the ptr class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>
#include <stdio.h>

#include <_ptr_.h>

using namespace soige;

void check_ptr();

int main(int argc, char* argv[])
{
	printf("Checking _ptr_\n");
	check_ptr();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// ptr tests
struct test_p
{
	int i;
	double j;
	char* l;
};

void check_ptr()
{
	_ptr_<double> pd;
	pd = new double;
	pd = new double;
	_ptr_<double> pd1 = pd;
	pd1 = new double;

	_ptr_<test_p> pt = new test_p();
	pt->i = 9;
	pt = null;
	if(pt==null)
		pd1 = pd1;
	_ptr_<char> pc(new char);
	char* ppc = pc;
	_ptr_<char> pc1 = pc;
}

