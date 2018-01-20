//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// w32f.cpp - checks the win32_file class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_win32_file_.h>

using namespace soige;

void check_win32_file(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	printf("Checking _win32_file_\n");
	check_win32_file(argc, argv);
	_CrtDumpMemoryLeaks();
	return 0;
}

//------------------------------------
// _win32_file_ tests
void check_win32_file(int argc, char* argv[])
{
	//-----------------------------------------------
	// checking reading std input with _win32_file_
	// fixed-buffer line
	TCHAR line[1024];
	_win32_file_ wf(_win32_file_::ft_stdin);
	if(argc==2) wf.reset(argv[1]);
	while(wf.readLine(line, sizeof(line))!=-1)
		printf("%s\n", line);
	wf.close();

	// _win32_file_ - allocated line
	LPSTR l;
	_win32_file_ f(_win32_file_::ft_stdin);
	if(argc==2) f.reset(argv[1]);
	while(f.readLine(l)!=-1)
	{
		printf("%s\n", l);
		free(l);
	}
	f.close();

	//-----------------------------------------------
	if(_win32_file_::exists(_T("c:\\winnt\\notepad.exe")))
		printf("notepad exists. size: %d\n",
				_win32_file_::fileSize(_T("c:\\winnt\\notepad.exe")));
	else
		printf("notepad doesn't exist\n");

	// test stdin, stdout, stderr, temp & regular files
	_win32_file_ stdout_f(_win32_file_::ft_stdout);
	// regular, no milk
	_win32_file_ file(_T("test.txt"));
	char* pLine = 0;
	long nLineLen = 0;
	bool ret;
	file.open(access_read);
	while( (nLineLen = file.readLine(pLine)) != -1)
	{
		stdout_f.writeLine(pLine);
		free(pLine);
	}
	ret = file.atEOF();
	ret = stdout_f.atEOF();

	// stdin
	printf("\nEnter lines. Press ^Z and Enter to end input\n");
	file.reset(_win32_file_::ft_stdin);
	while( (nLineLen = file.readLine(pLine)) != -1)
	{
		ret = file.atEOF();
		stdout_f.writeLine(pLine);
		free(pLine);
	}
	ret = file.atEOF();

	// stderr
	file.reset(_win32_file_::ft_stderr);
	file.writeFormatted(_T("Writing a formatted string\n%s, %d, %f, %s, 0x%08lX \n"),
						_T("long and boring shtring"), 789, 13237.9807,
						_T("this one is too long to fit in 32 characters. yep."), 0x4abcdef2);
	
	// temp
	file.reset(_win32_file_::ft_temp);
	file.writeFormatted(_T("Writing a formatted string\n%s, %d, %f, %s, 0x%08lX \n"),
						_T("long and boring shtring"), 789, 13237.9807,
						_T("this one is too long to fit in 32 characters. yep."), 0x4abcdef2);
	stdout_f.writeFormatted(_T("Temp filename: %s, size: %d\n"),
							file.getFileName(), file.getFileSize());
	stdout_f.writeFormatted(_T("Current pos: %d\n"), file.peekPos());
	file.seekPos(-20);
	file.writeString(_T("Inserting a string in the middle"));
	file.seekPos(0, ofs_begin);
	TCHAR buf[128];
	file.read(buf, sizeof(buf));
	file.close();

	// simultaneous work on one file
	_win32_file_ file1(_T("test.txt"));
	_win32_file_ file2(_T("test.txt"));
	file1.open(access_read);
	file2.open(access_read);
	while(!file1.atEOF())
	{
		nLineLen = file1.readLine(pLine);
		free(pLine);
	}
	nLineLen = file2.readLine(pLine);
	free(pLine);
	ret = file1.atEOF();
	ret = file2.atEOF();

	if(_win32_file_::exists(_T("c:\\winnt")))
		printf("c:\\winnt exists. size: %d\n",
				_win32_file_::fileSize(_T("c:\\winnt")));
}


