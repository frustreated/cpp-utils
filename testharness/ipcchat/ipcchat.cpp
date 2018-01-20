//------------------------------------------------------------
// ipcchat.cpp -
// A simple chat program for two machines based on the 
// _ipc_channel_ class from my utils classes.
//------------------------------------------------------------

#include <stdio.h>
#include <windows.h>
#include <lm.h>
#include <process.h>
#include <_win32_file_.h>
#include <_ipc_channel_.h>
using namespace soige;

void __cdecl readthread(void*);
void __cdecl writethread(void*);
void DisplayError(DWORD dwLastError);

bool timetodieR = false;
bool timetodieW = false;
const char* QUIT_MSG = "~!Die^Barney^Die!~";

int main(int argc, char** argv)
{
	_ipc_channel_ ch;
	int timeout = 0;
	DWORD ret = 0;

	if(argc < 2)
		return printf("Usage: %s hostname\n", argv[0]), 1;
	ch.reset(argv[1], _T("@IPCCHAT"));
	printf("Please wait... Trying to connect to %s...\n", argv[1]);
	ch.connect(false);
	while(!ch.isOtherSideConnected())
	{
		Sleep(100);
		timeout += 100;
		if( (ret=ch.errNum()) != 0 )
		{
			printf("Unable to connect. Error: 0x%08lx:\n", ret);
			DisplayError(ret);
			return -1;
		}
		if(timeout >= 60000)
			return printf("Timed out attempting to connect.\n"
						  "Make sure the other side is ready and try again\n"), -1;
	}
	
	printf("Connected to %s. When finished, type q! to quit\n", argv[1]);
	_beginthread(readthread, 0, &ch);
	_beginthread(writethread, 0, &ch);
	while(!(timetodieR || timetodieW))
		Sleep(500);
	
	return 0;
}

void __cdecl readthread(void* p)
{
	_ipc_channel_* pch = (_ipc_channel_*)p;
	char buf[1024];
	long nRead = 0;
	while( (nRead=pch->read(buf, sizeof(buf)-2)) != -1)
	{
		if(memcmp(buf, QUIT_MSG, lstrlenA(QUIT_MSG)) == 0)
		{
			printf("The other side disconnected\n");
			timetodieR = true;
			return;
		}
		buf[nRead++] = '\n';
		buf[nRead] = '\0';
		printf(buf);
	}
	timetodieR = true;
}

void __cdecl writethread(void* p)
{
	_ipc_channel_* pch = (_ipc_channel_*)p;
	char* line;
	long nRead = 0;
	long nWritten = 0;
	_win32_file_ in(_win32_file_::ft_stdin);
	while( (nRead=in.readLine(line)) > 0)
	{
		if((line[0] == 'q' || line[0] == 'Q') && line[1] == '!')
		{
			free(line);
			timetodieW = true;
			pch->write(QUIT_MSG, lstrlenA(QUIT_MSG));
			return;
		}
		if(pch->write(line, lstrlenA(line)) < 0)
		{
			if(!pch->isOtherSideConnected())
				printf("The other side disconnected\n");
			free(line);
			timetodieW = true;
			return;
		}
		free(line);
	}
	timetodieW = true;
}


void DisplayError(DWORD dwLastError)
{
	HMODULE hModule = NULL; // default to system source
	LPSTR MessageBuffer;
	DWORD dwFormatFlags;

	dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_IGNORE_INSERTS |
					FORMAT_MESSAGE_FROM_SYSTEM ;

	// if dwLastError is in the network range, load the message source
	if(dwLastError >= NERR_BASE && dwLastError <= MAX_NERR)
	{
		hModule = LoadLibraryExW(L"netmsg.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(hModule != NULL)
			dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	}

	// call FormatMessage() to allow for message text to be acquired
	// from the system or the supplied module handle.
	if(	FormatMessageA(dwFormatFlags,
						hModule, // module to get message from (NULL == system)
						dwLastError,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
						(LPSTR) &MessageBuffer, 0, NULL) )
	{
		printf("%s\n", MessageBuffer);
		// free the buffer allocated by the system
		LocalFree(MessageBuffer);
	}
	else
		printf("Unknown\n");
	
	// if you loaded a message source, unload it.
	if(hModule != NULL)
		FreeLibrary(hModule);
}
