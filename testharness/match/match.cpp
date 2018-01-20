//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// match.cpp - checks the string matching alg classes
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_soundex_.h>
#include <_boyer_moore_.h>
#include <_win32_file_.h>

using namespace soige;

void check_match_algs();

int main(int argc, char* argv[])
{
	printf("Checking string matching algorithms\n");
	check_match_algs();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// string matching algorithms tests -
//_boyer_moore_ and _soundex_
void check_match_algs()
{
	LPSTR line;
	long lineLen, matchPos, matchLen;

	_win32_file_ f_in(_win32_file_::ft_stdin);
	_win32_file_ f_out(_win32_file_::ft_stdout);
	_boyer_moore_ bm;
	_soundex_ sx;
	// check boyer-moore with default options
	printf("Checking boyer-moore (sa n8), case-sens, whole word = false, entire string = false\n");
	bm.initPattern("sa n8");
	while((lineLen=f_in.readLine(line)) != -1)
	{
		if(bm.match(line, lineLen, &matchPos, &matchLen))
			f_out.writeFormatted("Match position: %d, length: %d\n", matchPos, matchLen);
		else
			f_out.writeLine("No match");
		free(line);
	}

	// now check boyer-moore with the options reversed
	printf("Checking boyer-moore (sa n8), case-insens, whole word = true, entire string = false\n");
	bm.initPattern("sa n8", false, true, false);
	while((lineLen=f_in.readLine(line)) != -1)
	{
		if(bm.match(line, lineLen, &matchPos, &matchLen))
			f_out.writeFormatted("Match position: %d, length: %d\n", matchPos, matchLen);
		else
			f_out.writeLine("No match");
		free(line);
	}

	// now check boyer-moore with the options reversed
	printf("Checking boyer-moore (sa n8), case-insens, whole word = true, entire string = true\n");
	bm.initPattern("sa n8", false, true, true);
	while((lineLen=f_in.readLine(line)) != -1)
	{
		if(bm.match(line, lineLen, &matchPos, &matchLen))
			f_out.writeFormatted("Match position: %d, length: %d\n", matchPos, matchLen);
		else
			f_out.writeLine("No match");
		free(line);
	}

	// check soundex
	printf("\n");
	printf("Checking soundex (blabber), entire string = false\n");
	sx.initPattern("blabber");
	while((lineLen=f_in.readLine(line)) != -1)
	{
		if(sx.match(line, lineLen, &matchPos, &matchLen))
			f_out.writeFormatted("Match position: %d, length: %d\n", matchPos, matchLen);
		else
			f_out.writeLine("No match");
		free(line);
	}

	// check soundex with entire line = true
	printf("Checking soundex (blabber), entire string = true\n");
	sx.initPattern("blabber", true);
	while((lineLen=f_in.readLine(line)) != -1)
	{
		if(sx.match(line, lineLen, &matchPos, &matchLen))
			f_out.writeFormatted("Match position: %d, length: %d\n", matchPos, matchLen);
		else
			f_out.writeLine("No match");
		free(line);
	}
}

