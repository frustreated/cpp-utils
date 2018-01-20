//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _win32_file_.cpp - implementation file for _win32_file_
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_win32_file_.h"

namespace soige {

// Win32's newline - used in writeLine()
LPCSTR NEW_LINE = "\r\n";

/* static */
bool _win32_file_::exists(LPCTSTR fileName)
{
	HANDLE h = CreateFile(fileName, 0,
						  FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
						  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(h == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		if(err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND)
			return false;
	}
	else
		CloseHandle(h);
	
	return true;
}

// Create the requested directory and, if needed, all its parents
/* static */
bool _win32_file_::ensureDirExists(LPCTSTR dirName)
{
	// we could take unfair advantage of the little-known
	// imagehlp API MakeSureDirectoryPathExists, but linking
	// to a whole dll for one func? Nomethinks.
	bool backslash = true;
	TCHAR* p = _tcsrchr ( dirName, _T('\\') );
	if(!p) { backslash = false; p = _tcsrchr ( dirName, _T('/') ); }
	if(p)
	{
		*p = 0;
		ensureDirExists(dirName);
		*p = backslash? _T('\\') : _T('/');
	}
	// CreateDir will choke on drive letters, but it's ok;
	// what matters is the retval of the last CreateDir call.
	return CreateDirectory(dirName, NULL) ? true : false;
}

/* static */
long _win32_file_::fileSize(LPCTSTR fileName)
{
	DWORD size = 0;
	HANDLE h = CreateFile(fileName, 0,
						  FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
						  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(h != INVALID_HANDLE_VALUE)
	{
		size = GetFileSize(h, NULL);
		CloseHandle(h);
	}
	return (long)size;
}

/* static */
bool _win32_file_::wordAndLineCount(LPCTSTR fileName, int& wordCount, int& lineCount)
{
	DWORD nBytesRead = 0;
	char buf[4096];
	char* p;
	long lines = 1;			// for counting lines
	long words = 0;			// for counting words
	bool prev_sep = true;	// for counting words
	HANDLE hFile;

	wordCount = lineCount = 0;

	hFile = CreateFile( fileName,
						GENERIC_READ,
						FILE_SHARE_READ, NULL,
						OPEN_EXISTING,
						FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	while( ReadFile(hFile, buf, sizeof(buf), &nBytesRead, NULL) && nBytesRead > 0 )
	{
		p = buf;
		while(p-buf <= nBytesRead)
		{
			// line count
			if(*p == '\n') lines++;
			// word count
			if( !__iscsym(*p) )	// letter, underscore, or digit
				prev_sep = true;
			else if(prev_sep)
			{
				words++;
				prev_sep = false;
			}
			p++;
		}
	}
	CloseHandle(hFile);
	wordCount = words;
	lineCount = lines;
	return true;
}

_win32_file_::_win32_file_(LPCTSTR fileName)
{
	_fileName = NULL;
	_fileType = ft_ondisk;
	_hFile = INVALID_HANDLE_VALUE;
	_stdinEOF = false;
	// if the filename is NULL, change file type to temp
	if(fileName) _fileName = _tcsdup(fileName); else _fileType = ft_temp;
}

_win32_file_::_win32_file_(file_type fileType)
{
	_fileName = NULL;
	_fileType = fileType;
	_hFile = INVALID_HANDLE_VALUE;
	_stdinEOF = false;
	// ft_ondisk should not be passed, rather the first ctor
	// (with filename) should be used. However, if passed,
	// change the file type to temp
	if(fileType == ft_ondisk) _fileType = ft_temp;
}

_win32_file_::~_win32_file_()
{
	reset(NULL);
}

bool _win32_file_::open(int accessMode, int shareMode, int howToOpen,
						int fileAttribs, int miscFlags)
{
	close();

	DWORD dwDesiredAccess		= (DWORD) accessMode;
	DWORD dwShareMode			= (DWORD) shareMode;
	DWORD dwCreationDisposition	= (DWORD) howToOpen;
	DWORD dwFlagsAndAttributes	= (DWORD) fileAttribs | (DWORD) miscFlags;
	
	TCHAR tempPath[MAX_PATH] = _T(".");
	switch(_fileType)
	{
	case ft_stdin:
		_hFile = GetStdHandle(STD_INPUT_HANDLE);
		break;
	case ft_stdout:
		_hFile = GetStdHandle(STD_OUTPUT_HANDLE);
		break;
	case ft_stderr:
		_hFile = GetStdHandle(STD_ERROR_HANDLE);
		break;
	case ft_temp:
		GetTempPath(MAX_PATH-1, tempPath);
		if(_fileName) free(_fileName);
		_fileName = (LPTSTR) malloc(sizeof(TCHAR)*MAX_PATH);
		if(!GetTempFileName(tempPath, _T("$~$"), 0, _fileName))
			break;
		_hFile = CreateFile(_fileName,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ, NULL, // lpSecurityAttribs
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
							NULL // hTemplate
							);
		break;
	default: // ft_ondisk - physical file
		// if file is being open for writing, make sure the path exists
		if(dwDesiredAccess & GENERIC_WRITE)
		{
			TCHAR* p = _tcsrchr ( _fileName, _T('\\') );
			if(!p) p = _tcsrchr ( _fileName, _T('/') );
			if(p) { *p = 0; ensureDirExists(_fileName); *p = _T('\\'); }
		}
		_hFile = CreateFile(_fileName,
							dwDesiredAccess,
							dwShareMode, NULL, // lpSecurityAttribs
							dwCreationDisposition,
							dwFlagsAndAttributes, NULL // hTemplate
							);
	}
	
	if(_hFile == INVALID_HANDLE_VALUE) return false;
	return true;
}

void _win32_file_::close()
{
	if( _hFile != INVALID_HANDLE_VALUE &&
		!(_fileType == ft_stdin || _fileType == ft_stdout || _fileType == ft_stderr) )
		CloseHandle(_hFile);
	_hFile = INVALID_HANDLE_VALUE;
	_stdinEOF = false;
}

void _win32_file_::reset(LPCTSTR fileName)
{
	close();
	if(_fileName) free(_fileName);
	_fileName = NULL;
	_fileType = ft_ondisk;
	// if the filename is NULL, change file type to temp
	if(fileName) _fileName = _tcsdup(fileName); else _fileType = ft_temp;
}

void _win32_file_::reset(file_type fileType)
{
	close();
	if(_fileName) free(_fileName);
	_fileName = NULL;
	_fileType = fileType;
	// ft_ondisk should not be passed, rather the previous reset()
	// (with filename) should be used. However, if passed,
	// change the file type to temp
	if(fileType == ft_ondisk) _fileType = ft_temp;
}

_win32_file_::file_type _win32_file_::getFileType() const
{
	return _fileType;
}

LPCTSTR _win32_file_::getFileName() const
{
	if(ft_stdin == _fileType || ft_stdout == _fileType || ft_stderr == _fileType)
		return _T("");
	return _fileName;
}

HANDLE _win32_file_::getFileHandle() const
{
	return _hFile;
}

long _win32_file_::getFileSize() const
{
	if(_fileName == NULL) return 0;
	
	DWORD size = 0;
	if(_hFile == INVALID_HANDLE_VALUE)
	{
		HANDLE h = CreateFile(_fileName, 0,
							  FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(h != INVALID_HANDLE_VALUE)
		{
			size = GetFileSize(h, NULL);
			CloseHandle(h);
		}
	}
	else
		size = GetFileSize(_hFile, NULL);
	
	return (long)size;
}

bool _win32_file_::isOpen() const
{
	return (_hFile != INVALID_HANDLE_VALUE);
}

long _win32_file_::seekPos(long byteOffset, seek_offset from)
{
	if( !_ensureOpen() ) return -1;
	return SetFilePointer(_hFile, byteOffset, NULL, from);
}

long _win32_file_::peekPos() const
{
	if( !isOpen() ) return 0;
	return SetFilePointer(_hFile, 0, NULL, FILE_CURRENT);
}

bool _win32_file_::atBOF() const
{
	if( !isOpen() ) return false;
	// see if current file pointer is at 0
	return ( SetFilePointer(_hFile, 0, NULL, FILE_CURRENT) == 0 );
}

bool _win32_file_::atEOF() const
{
	if( !isOpen() ) return false;
	// get current file pointer and compare to size
	return ( SetFilePointer(_hFile, 0, NULL, FILE_CURRENT) == GetFileSize(_hFile, NULL) );
}

long _win32_file_::read(void* pBuf, DWORD bytesToRead)
{
	DWORD nBytesRead = 0;
	BOOL result;
	
	if( !_ensureOpen() ) return -1;
	result = ReadFile(_hFile, pBuf, bytesToRead, &nBytesRead, NULL);
	return ( result? (long)nBytesRead : -1 );
}

long _win32_file_::write(const void* pBuf, DWORD bytesToWrite)
{
	DWORD nBytesWritten = 0;
	BOOL result;
	
	if( !_ensureOpen() ) return -1;
	result = WriteFile(_hFile, pBuf, bytesToWrite, &nBytesWritten, NULL);
	return ( (result && bytesToWrite==nBytesWritten)? (long)nBytesWritten : -1 );
}

void _win32_file_::flush()
{
	if( _hFile != INVALID_HANDLE_VALUE && (_fileType == ft_ondisk || _fileType == ft_temp) )
		FlushFileBuffers(_hFile);
}

//----------------------------------------------------------------
// Alloc mem on pLine and copy the next line from the file to it.
// Memory pointed to by pLine has to be free()'d by the caller
//----------------------------------------------------------------
long _win32_file_::readLine(LPSTR& pLine)
{
	// buffer size has to be close enough to avg line length
	// to avoid the inefficiency of over- or under-reading,
	// but we want to read a little more to cache the data
	byte  buffer[128];
	DWORD dwRead = 0;
	long  len;				// total running length of current line
	char* pLf = NULL;		// pointer to the position of <LF> in buffer
	UINT_PTR lfOffset = 0;	// offset of <LF> from the beginning of the buffer
	BOOL  fRetVal = FALSE;

	if( !_ensureOpen() ) return -1;
	
	// stdin is different in the way that you can't set
	// the file pointer back after reading some bytes,
	// so for stdin, we read input one char at a time... ugh!
	if(ft_stdin == _fileType)
	{
		// see if have eof condition from previous read
		if(_stdinEOF)
		{
			_stdinEOF = false;
			return -1;
		}

		long curpos = 0;
		len = 32;
		pLine = (char*) malloc(len);
		while ( fRetVal=ReadFile(_hFile, buffer, 1, &dwRead, NULL), fRetVal && dwRead )
		{
			if(curpos >= len)
			{
				len += 32;
				pLine = (char*) realloc(pLine, len);
			}
			if( (pLine[curpos] = buffer[0]) == '\n' )
				break;
			curpos++;
		}
		
		if(0 == dwRead)  // stdin eof
		{
			// finish reading the <CRLF> after the ^Z
			if(fRetVal) ReadFile(_hFile, buffer, 2, &dwRead, NULL);
			if(0 == curpos)   // ^Z on a line by itself
				return free(pLine), -1;
			else					// a rare condition of ^Z at end of a line of chars,
				_stdinEOF = true;	// mark stdin eof condition for next read
		}
		len = curpos;
		pLine[len] = 0;
		if( pLine[len-1] == '\r' )
			pLine[--len] = 0;
		return len;
	}
	
	len = 0;
	pLine = (char*) malloc(len);
	fRetVal = ReadFile( _hFile, buffer, sizeof(buffer), &dwRead, NULL );
	while (fRetVal && dwRead)
	{
		// Find next newline
		if(dwRead) pLf = (char*) memchr( buffer, '\n', dwRead );
		// if found, cull it out; otherwise keep reading
		if(pLf)
		{
			lfOffset = (UINT_PTR)pLf - (UINT_PTR)&buffer[0];
			pLine = (char*) realloc(pLine, len + lfOffset + 1);
			memcpy( &pLine[len], buffer, lfOffset );
			len += lfOffset;
			pLine[len] = 0;
			if( pLine[len-1] == '\r' )
				pLine[--len] = 0;
			// set file pointer back to the byte just after the newline
			SetFilePointer(_hFile, -dwRead+lfOffset+1, NULL, FILE_CURRENT);
			return len;
		}
		else
		{
			pLine = (char*) realloc(pLine, len + dwRead + 1);
			memcpy( &pLine[len], buffer, dwRead );
			len += dwRead;
		}
		fRetVal = ReadFile( _hFile, buffer, sizeof(buffer), &dwRead, NULL );
	}

	if(0 == len)  // Nothing left
		return free(pLine), -1;
	
	// Finish up the last line in the file (no <LF> on the end)
	pLine[len] = 0;
	return len;
}

//----------------------------------------------------------------
// This version does not allocate any memory, since pLine points
// to a buffer already allocated by the caller. The size of that
// buffer (in characters, including the terminating NULL)
// is bufSize. The returned line can have embedded nulls.
// The function returns when either <LF> is found or the
// provided buffer is exhausted.
// Return values are same as for readLine() above.
//----------------------------------------------------------------
long _win32_file_::readLine(LPSTR pLine, long bufSize, bool* pLfFound)
{
	DWORD dwRead = 0;
	long  len;				// total running length of current line
	char* pLf = NULL;		// pointer to the position of <LF> in buffer
	BOOL  fRetVal = FALSE;
	const DWORD READ_SIZE = 128;  // chars to read in each iteration; more or less close to avg. line length

	if( !_ensureOpen() ) return -1;
	bufSize--;  // allow for terminating NULL
	len = 0;
	if(pLfFound) *pLfFound = false;

	// stdin is different in the way that you can't set
	// the file pointer back after reading some bytes,
	// so for stdin, we read input one char at a time... ugh!
	if(ft_stdin == _fileType)
	{
		// see if have eof condition from previous read
		if(_stdinEOF)
		{
			_stdinEOF = false;
			return -1;
		}

		while(len < bufSize)
		{
			fRetVal = ReadFile(_hFile, &pLine[len], 1, &dwRead, NULL);
			if( !(fRetVal && dwRead) )
				break;
			if( pLine[len] == '\n' )
			{
				if(pLfFound) *pLfFound = true;
				break;
			}
			len++;
		}
		
		if(0 == dwRead)  // stdin eof
		{
			// finish reading the <CRLF> after the ^Z, use fRetVal as a temp buffer
			if(fRetVal) ReadFile(_hFile, &fRetVal, 2, &dwRead, NULL);
			if(0 == len)   // ^Z on a line by itself
				return -1;
			else					// ^Z at end of a line of chars;
				_stdinEOF = true;	// mark stdin eof condition for next read
		}
		
		pLine[len] = 0;
		if( pLine[len-1] == '\r' )
			pLine[--len] = 0;
		return len;
	}
	
	while(len < bufSize)
	{
		fRetVal = ReadFile( _hFile, &pLine[len],
							(READ_SIZE<bufSize-len? READ_SIZE : bufSize-len),
							&dwRead, NULL );
		if( !(fRetVal && dwRead) )  // eof
			break;
		// Find next newline
		pLf = (char*) memchr( &pLine[len], '\n', dwRead );
		// if found, cull it out; otherwise keep reading
		if(pLf)
		{
			if(pLfFound) *pLfFound = true;
			// set file pointer back to the byte just after the newline
			len = (UINT_PTR)pLf - (UINT_PTR)&pLine[len];
			SetFilePointer( _hFile, -dwRead+len+1, NULL, FILE_CURRENT );
			len = (UINT_PTR)pLf - (UINT_PTR)&pLine[0];
			pLine[len] = 0;
			if( pLine[len-1] == '\r' )
				pLine[--len] = 0;
			return len;
		}
		else
			len += dwRead;
	}

	if( 0 == len )  // Nothing left
		return -1;
	else if( len >= bufSize )
		len = bufSize;
	// Finish up the last line in the file (no <LF> on the end)
	pLine[len] = 0;
	return len;
}

long _win32_file_::writeLine(LPCSTR str)
{
	long w1 = writeString(str);
	long w2 = writeString(NEW_LINE);
	if(w1 == -1 || w2 == -1)
		return -1;
	return w1+w2;
}

long _win32_file_::writeString(LPCSTR str)
{
	return write( str, lstrlenA(str) /* *sizeof(char) */ );
}

long _win32_file_::writeFormatted(LPCSTR format_spec, ...)
{
	va_list args;
	va_start(args, format_spec);
	
	char* write_str = NULL;
	int size = 32, cb = 0;
	long rv;

#if defined(_USE_NO_CRT) && !defined(_DEBUG) && !defined(DEBUG)
	if( (write_str = (char*) malloc(8192)) == NULL )  // hopefully is enough
		return -1;

	if( (cb = wvsprintfA(write_str, format_spec, args)) <
		lstrlenA(format_spec) )
		return -1;
#else
	// try increasing buffer size until all args are written
	do
	{
		size += 32;
		free(write_str);
		if( (write_str = (char*) malloc(size*sizeof(char))) == NULL ) return -1;
	} while ( (cb = _vsnprintf(write_str, size, format_spec, args)) < 0 );
#endif

	va_end(args);

	rv = write ( write_str, cb*sizeof(char) );
	free(write_str);
	return rv;
}

bool _win32_file_::_ensureOpen()
{
	if(_hFile != INVALID_HANDLE_VALUE)
		return true;
	return open();
}

};	// namespace soige

