//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// que.cpp - checks the queue class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_queue_.h>

using namespace soige;

void check_queue();

int main(int argc, char* argv[])
{
	printf("Checking _queue_\n");
	check_queue();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// queue tests
void check_queue()
{
	_queue_<double> dbl_q;
	double d = 4.56;
	dbl_q.put(d);
	dbl_q.put(78.9);
	dbl_q.put(1.23);
	bool b = dbl_q.contains(3);
	b = dbl_q.contains(1.23);
	d = dbl_q.length();
	d = dbl_q.peek();
	b = dbl_q.get(d);
	b = dbl_q.get(d);
	b = dbl_q.get(d);
	dbl_q.clear();
}

