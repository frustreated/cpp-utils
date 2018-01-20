//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _win32_file_.h - header file for the _win32_file_ class.
//
// Encapsulates file access in Win32.
//
// Some usage notes:
// read() usage:
// while( (lRead=read(buffer, 1)) > 0 )
// {
//     // ...
// }
// - or -
// while( lRead=read(buffer, 1), lRead != -1 && lRead != 0 )
// {
//     // ...
// }
//
// readLine() (the one where line is allocated by the
//				_win32_file_ itself) usage:
// LPSTR pCurLine;
// long nLineLen;
// while( (nLineLen = file.readLine(pCurLine)) != -1 )
// {
//     // ...
//     free(pCurLine);
// }
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __win32_file_already_included_vasya__
#define __win32_file_already_included_vasya__

#include "_common_.h"

namespace soige {

//------------------------------------------------------------
// The _win32_file_ class
//------------------------------------------------------------
class _win32_file_
{
public:
	static bool exists(LPCTSTR fileName);
	static bool ensureDirExists(LPCTSTR dirName);
	static long fileSize(LPCTSTR fileName);
	static bool wordAndLineCount(LPCTSTR fileName, int& wordCount, int& lineCount);

public:
	// enum for file types: regular (physical on-disk file),
	// temporary (deleted when closed), STDIN, STDOUT, STDERR
	enum file_type
	{
		ft_stdin		= 0,
		ft_stdout		= 1,
		ft_stderr		= 2,
		ft_temp			= 10,
		ft_ondisk		= 11	// should not be specified explicitly
	};

public:
	// file type will automatically be ft_ondisk
	_win32_file_ (LPCTSTR fileName = NULL);
	// for file types other than ft_ondisk
	_win32_file_ (file_type fileType);
	virtual ~_win32_file_();

	bool open (
				// GENERIC_READ | GENERIC_WRITE
				int accessMode = access_all,
				// FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE
				int shareMode = share_read,
				// CREATE_NEW | CREATE_ALWAYS | OPEN_EXISTING | OPEN_ALWAYS | TRUNCATE_EXISTING
				int howToOpen = open_always,
				// FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY
				int fileAttribs = normal,
				// FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH
				int miscFlags = seq_scan
			  );
	void close();
	// file type will automatically be ft_ondisk
	void reset(LPCTSTR fileName);
	// for file types other than ft_ondisk
	void reset(file_type fileType);
	
	file_type	getFileType  () const;
	LPCTSTR		getFileName  () const;
	HANDLE		getFileHandle() const;
	long		getFileSize  () const;
	bool		isOpen () const;
	long		seekPos(long byteOffset, seek_offset from = ofs_current);
	long		peekPos() const;
	bool		atBOF  () const;
	bool		atEOF  () const;

	// read() returns the number of bytes read or -1 on error/eof
	long read (void* pBuf, DWORD bytesToRead);
	// write() returns the number of bytes written or -1 on error
	long write(const void* pBuf, DWORD bytesToWrite);
	void flush();
	// this readLine() returns the number of bytes (not chars) read
	// (<CRLF> is removed from returned string) or -1 on error/eof.
	// Memory pointed to by pLine has to be free()'d by the caller
	long readLine(LPSTR& pLine);
	// this readLine() returns the number of bytes (not chars) read
	// (<CRLF> is removed from returned string) or -1 on error/eof.
	// pLine points to a buffer allocated by the caller,
	// size of which is bufSize; *pLfFound will be true if buffer
	// was large enough to read in a line with a LF on the end
	long readLine(LPSTR pLine, long bufSize, /* out */ bool* pLfFound = NULL);
	
	// writeLine() returns the number of bytes (not chars)
	// written (including the <CRLF>) or -1 on error
	long writeLine(LPCSTR str);
	// writeString() returns the number of bytes (not chars) written or -1 on error
	long writeString(LPCSTR str);
	// writeFormatted() returns the number of bytes written or -1 on error
	long writeFormatted(LPCSTR format_spec, ...);

protected:
	file_type	_fileType;
	LPTSTR		_fileName;
	HANDLE		_hFile;
	bool		_stdinEOF;  // reached eof of stdin?

protected:
	inline bool _ensureOpen();

private:
	// no byval operations
	_win32_file_(const _win32_file_& rwf) { }
	_win32_file_& operator=(const _win32_file_& rwf) { }
};


};	// namespace soige

#endif // __win32_file_already_included_vasya__

