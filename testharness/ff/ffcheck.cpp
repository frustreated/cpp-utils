//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ffcheck.cpp - checks the file_finder class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_file_finder_.h>

using namespace soige;

void check_file_finder(char* pattern);

int main(int argc, char* argv[])
{
	if(argc == 1)
		return printf("Usage: ffcheck file_pattern\n"), -1;

	printf("Checking _file_finder_\n");
	check_file_finder(argv[1]);
	_CrtDumpMemoryLeaks();
	return 0;
}

//-----------------------------------------------
// _file_finder_ test - recursive filename search
void check_file_finder(char* pattern)
{
	_file_finder_ ff;
	TCHAR file[MAX_PATH];
	int count = 0;

	ff.initPattern(pattern, true);
	while( ff.getNextFile(file, true) )
	{
		count++;
		printf("%s\n", file);
	}
	printf("-- Found %d files\n", count);
}

