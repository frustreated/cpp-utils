//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// numeval.cpp - checks the num_eval class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

// num_eval includes string and dictionary
#include <_num_eval_.h>

using namespace soige;

void check_eval();

int main(int argc, char* argv[])
{
	printf("Checking _num_eval_\n");
	check_eval();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// _num_eval_ tests
void check_eval()
{
	_num_eval_ eval;
	TCHAR expr[4096];
	double result = 0.0;

	printf("\nEnter an expression and press Enter to evaluate. Enter q to end test\n");
	gets(expr);
	while(expr[0] != _T('q'))
	{
		eval.reset();
		try {
			result = eval.evaluate(expr);
			printf("Result is:   %f\n\n", result);
		} catch(soige_error& e) {
			printf("Caught exception while evaluating: %s\n", e.what());
		}
		gets(expr);
	}
}

