

#include <_regex_.h>
#include <_win32_file_.h>

using namespace soige;


int main(int argc, char** argv)
{
	_win32_file_ file( _win32_file_::ft_stdin );
	_regex_ regex;
	bool full_regex = false;
	const char* pattern;
	char string_to_match[2048];
	int string_len = 0;

	if( argc < 2 )
		return printf( "Usage: regex <pattern> [-E]\n" ), -1;

	pattern = argv[1];

	if( argc >= 3 )
		full_regex = (!strcmpi(argv[2], "-E"))? true : false;
	
	if( !regex.initPattern( pattern, full_regex ) )
	{
		printf( "RE error in \'%s\': %s\n", pattern, regex.errstr() );
		return -1;
	}
	
#if defined(DEBUG) || defined(_DEBUG)
	regex.printDebugInfo();
#endif

	printf( "Type strings to match pattern against and press Enter:\n" );
	while( (string_len = file.readLine(string_to_match, sizeof(string_to_match))) != -1 )
	{
		if(	regex.match(string_to_match, string_len) )
			printf( "%s\n", string_to_match );
		else
			printf( ">> no match\n" );
	}
			
	return 0;
}

