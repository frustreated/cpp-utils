//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _file_finder_.h - header file for file searching.
//
// Encapsulates recursive file/directory pattern searching.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __file_finder_already_included_vasya__
#define __file_finder_already_included_vasya__

#include "_common_.h"
#include "_string_array_.h"

namespace soige {

class _file_finder_
{
public:
	// ctor/dtor
	_file_finder_();
	virtual ~_file_finder_();

	//--------------------------------
	// public operations
	void initPattern(LPCTSTR pattern, bool bSearchSubdirs, int fileAttribs = allfiles);
	bool getNextFile(TCHAR* lpFile, bool bAbsFilePath = false);
	long fileCount();
	void reset();

protected:
	HANDLE			_hFind;					// Used by getNextFile(); has to be class member to preserve value & be able to reset()
	bool			_bSearchSubdirs;		// What could this be?
	DWORD			_fileAttribs;			// The file attributes
	_string_array_	_dirs;					// String array to hold subdir names
	TCHAR			_filePart[MAX_PATH];	// The file part (normalized) of the file spec
	TCHAR			_curDir[MAX_PATH];		// The directory currently being searched

protected:
	// The stupid Win32 file searching APIs don't give you a way
	// to filter the search by short or long filenames - they apply
	// the pattern to both. Which means that the FindXxxFile will
	// match if either the short or the long names of a file match.
	// However, what is *returned* is the long filename; so it can
	// be quite confusing searching for *.htm and getting back "index.html".
	// So each matching filename must go through our own verification
	// to make sure the *long* filename actually matches the pattern.
	// The pattern param is expected to be in upper case on entry.
	static bool _verifyMatch(LPCTSTR pattern, LPCTSTR name);

private:
	// no byval operations
	_file_finder_(const _file_finder_& rff) { }
	_file_finder_& operator=(const _file_finder_& rff) { }
};


};	// namespace soige

#endif  // __file_finder_already_included_vasya__

