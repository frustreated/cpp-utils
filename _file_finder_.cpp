//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _file_finder_.cpp - implementation file for the file finder
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_file_finder_.h"

namespace soige {

_file_finder_::_file_finder_() : _hFind(INVALID_HANDLE_VALUE)
{
	reset();
}

_file_finder_::~_file_finder_()
{
	reset();
}

void _file_finder_::initPattern(LPCTSTR pattern, bool bSearchSubdirs, int fileAttribs)
{
	long len;
	TCHAR szBuffer[MAX_PATH];

	reset();
	_bSearchSubdirs = bSearchSubdirs;
	lstrcpy ( szBuffer, pattern );
	len = lstrlen ( szBuffer );
	
	// If pattern refers to dir, append * to end
	if( szBuffer[len-1] == _T('\\') || szBuffer[len-1] == _T('/') )
	{
		lstrcat ( szBuffer, _T("*") );
	}
	else
	{
		// See if pattern specifies a dir, but user neglected to append \ to the end;
		// if so, append \* to end so that we'll search this entire dir
		HANDLE hDir = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA fdDir;

		hDir = FindFirstFile( szBuffer, &fdDir );
		if( hDir != INVALID_HANDLE_VALUE )
		{
			if(
				( fdDir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) &&
				// make sure there are no wildcards in specified dir name
				( lstrcmpi( fdDir.cFileName, &szBuffer[len-lstrlen(fdDir.cFileName)] ) == 0)
			  )
				lstrcat ( szBuffer, _T("\\*") );
			
			FindClose(hDir);
		}
	}
	
	// Extract the pattern after the path part
	len = lstrlen( szBuffer );
	while( --len, (szBuffer[len] != _T('\\') && szBuffer[len] != _T('/') && len > 0) )
		;
	lstrcpy ( _filePart, &szBuffer[ len?len+1:len ] );
	_tcsupr( _filePart );
	
	// Add the first directory to the dir list
	szBuffer[ len?len+1:len ] = _T('\0');
	_dirs.insert( szBuffer, 0 );
	_fileAttribs = (DWORD) (fileAttribs | regfiles);
}


//--------------------------------------------------------------
// Find the next file matching the wildcard pattern,
// and place it in the lpFile argument (which must be
// large enough to hold it; MAX_PATH should suffice).
// The bAbsFilePath arg specifies that the caller wants
// the returned filename to contain the absolute file path.
// Return false on no more match.
//--------------------------------------------------------------
bool _file_finder_::getNextFile(TCHAR* lpFile, bool bAbsFilePath /* = false */)
{
	WIN32_FIND_DATA fd;	// used both for building subdirs and for file searching
	BOOL bNextFile = TRUE;
	int posToAdd;
	TCHAR szBuffer[MAX_PATH*2];

	while( (_hFind != INVALID_HANDLE_VALUE) || (_dirs.length() > 0) )
	{
		if( _hFind == INVALID_HANDLE_VALUE )
		{
			// This is the first run in this recursion (directory) -
			// initialize currently searched directory and add its
			// subdirs - if search subdirs specified - to _dirs list
			lstrcpy ( _curDir, _dirs.get(0) );
			_dirs.removeAt(0);
			
			// first fill the subdirectories, if requested
			if( _bSearchSubdirs )
			{
				HANDLE hDirs = INVALID_HANDLE_VALUE;
				
				lstrcpy ( szBuffer, _curDir );
				lstrcat ( szBuffer, _T("*") );
				hDirs = FindFirstFile( szBuffer, &fd );
				bNextFile = TRUE;
				if( hDirs != INVALID_HANDLE_VALUE )
				{
					posToAdd = 0;
					while( bNextFile )
					{
						if(
							( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )	&& 
							lstrcmp ( fd.cFileName, _T(".") )					&&
							lstrcmp ( fd.cFileName, _T("..") )
						  )
						{
							// Make up the new directory's full name and add to array
							lstrcpy ( szBuffer, _curDir );
							lstrcat ( szBuffer, fd.cFileName );
							lstrcat ( szBuffer, _T("\\") );
							_dirs.insert( szBuffer, posToAdd );
							posToAdd++;
						}
						bNextFile = FindNextFile( hDirs, &fd );
					}
					FindClose(hDirs);
				}
			}
			
			// Now search for the actually requested files.
			// Create the pattern for this directory
			lstrcpy ( szBuffer, _curDir );
			// Instead of appending the actual searched file pattern,
			// append * so that we don't have to do triple matching
			// (one done by OS on short name, one on long name, and
			// one by us in _verifyMatch; see comments in .h file).
			// This way the OS will return all files without the
			// overhead of matching, and the only matching is done
			// by us, which we'll do anyway.
			lstrcat ( szBuffer, _T("*") );
			
			// Find the first file in this directory
			_hFind = FindFirstFile( szBuffer, &fd );
			// if nothing, then try the next directory
			if( _hFind == INVALID_HANDLE_VALUE )
				continue;
			bNextFile = TRUE;
		}
		else // _hFind != INVALID_HANDLE_VALUE
			bNextFile = FindNextFile( _hFind, &fd );

		// Skip the directories that happen to also match
		// the specified pattern (we're looking only for files);
		// and skip all files that don't match (see comments in .h).
		// _verifyMatch is the wildcard pattern matching routine.
		while (
				bNextFile &&
				// ( fd.dwFileAttributes & _fileAttribs ) &&
				(
				  ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ||
				  ( !_verifyMatch(_filePart, fd.cFileName) )
				)
			  )
			bNextFile = FindNextFile( _hFind, &fd );
		
		if( bNextFile )
		{
			// Found our file
			lstrcpy ( lpFile, _curDir );
			lstrcat ( lpFile, fd.cFileName );
			// See if the caller wants the absolute file path
			if( bAbsFilePath )
				_tfullpath( lpFile, lpFile, _MAX_PATH );
			return true;
		}
		
		// All files have been enumerated in this dir.
		// Try the next one
		FindClose( _hFind );
		_hFind = INVALID_HANDLE_VALUE;
		// continue;

	} // while( _dirs.length() > 0 )

	return false;
}


/* static */
bool _file_finder_::_verifyMatch(LPCTSTR pattern, LPCTSTR name)
{
	TCHAR matchchar;

    // End of pattern, but not end of string?
    if( !*pattern && *name )
        return false;

    // If we hit a wild card, do recursion
    if( *pattern == _T('*') )
	{
	    // skip over multiple contiguous *'s
		while( *pattern == _T('*') ) pattern++;
        
		while( *name && *pattern )
		{
			matchchar = totupper( *name );

            // See if this substring matches
		    if( *pattern == _T('?') || *pattern == matchchar )
  		        if( _verifyMatch( pattern+1, name+1 ) )
                    return true;

            // Try the next substring
            name++;
        }

		// Reached end of pattern and/or string.
		// Make sure that if it's not end of pattern,
		// it has a valid wild card ending,
		// so that it's really a match.
		return ( !*pattern || (*pattern == _T('*') && !*(pattern+1)) ) ? true : false;
    } 

    // Do straight compare until we hit a wild card
    while( *name && *pattern != _T('*') )
	{
		matchchar = totupper( *name );

		if( *pattern == _T('?') || *pattern == matchchar )
		{
            pattern++;
            name++;
        }
		else
            return false;
    }

    // If not done, recurse
    if( *name )
        return _verifyMatch( pattern, name );

    // Make sure it's a match
	return ( !*pattern || (*pattern == _T('*') && !*(pattern+1)) ) ? true : false;
}


long _file_finder_::fileCount()
{
	if(
		_bSearchSubdirs ||
		_tcschr( _filePart, _T('*') ) ||
		_tcschr( _filePart, _T('?') )
	  )
		return -1;
	return 1;
}


void _file_finder_::reset()
{
	if( _hFind != INVALID_HANDLE_VALUE )
		FindClose(_hFind);
	_hFind			= INVALID_HANDLE_VALUE;
	_bSearchSubdirs	= false;
	_fileAttribs	= (DWORD) allfiles;
	_dirs.clear();
	_filePart[0]	= _T('\0');
	_curDir[0]		= _T('\0');
}

};	// namespace soige

