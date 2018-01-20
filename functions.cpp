//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _functions_.cpp - implementation of common routines.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <_common_.h>

namespace soige {

/*
//------------------------------------------------------------
// Clear screen routine (Windows only)
//------------------------------------------------------------
void clearScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputCharacter(hConsole, _T(' '), (DWORD)(csbi.dwSize.X*csbi.dwSize.Y), coordScreen, &cCharsWritten );
	SetConsoleCursorPosition  (hConsole, coordScreen);
	return; 
}
*/

};	// namespace soige

