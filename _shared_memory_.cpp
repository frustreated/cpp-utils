//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _shared_memory_.cpp - implementation of _shared_memory_
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_shared_memory_.h"

namespace soige {

//------------------------------------------------------------
// Layout of the memory area:
//
// |------------------------|-----------------------------~~
// | bookkeeping block      | user data block             ~~
// |------------------------|-----------------------------~~
//
// Layout of the bookkeeping block:
// 1. Size of the user-accessible shared memory block	- DWORD
// 2. Number of references to this block				- long
// 3. Current version of the data in the block
//    (incremented on each write)						- DWORD
// 4. Free space up to the beginning of the user block.
//
// All this data is initialized by the first process
// creating the memory block and then updated by the other
// objects as needed (i.e., on attach, detach, and write).
//------------------------------------------------------------

// Size the bookkeeping block with an eye to the future
const long _shared_memory_::BOOKKEEP_BLOCK_SIZE		= 64;
const long _shared_memory_::OFFSET_BLOCK_USERSIZE	= 0;
const long _shared_memory_::OFFSET_BLOCK_REFCOUNT	= sizeof(DWORD) * 2;
const long _shared_memory_::OFFSET_BLOCK_VERSION	= sizeof(DWORD) * 2 + sizeof(long);
const TCHAR _shared_memory_::SHARED_MEM_MAGIC[]		= _T("6C2A-6D80-11D4-8146-");

// the size of the user block - maxSize for the first creating object,
// and for all other objects taken from the bookkeeping block's 1st slot
#define BLOCK_USER_SIZE	(*((LPDWORD)(((LPBYTE)_memBlockStart) + OFFSET_BLOCK_USERSIZE)))
// the reference count to this shared block; may be incorrect if a
// process holding a reference to the block was shut down abnormally
#define BLOCK_REFCOUNT	(*((long*)(((LPBYTE)_memBlockStart) + OFFSET_BLOCK_REFCOUNT)))
// the version of the data in the block, incremented on each write();
// a user may save the version, do some work and then compare the current
// version with the saved one to see if anyone changed the data during that period
#define BLOCK_VERSION	(*((LPDWORD)(((LPBYTE)_memBlockStart) + OFFSET_BLOCK_VERSION)))

_shared_memory_::_shared_memory_()
{
	_hFileMapping = NULL;
	_hMutex = NULL;
	_memBlockSize = 0;
	_memBlockStart = NULL;
	_userMemStart = NULL;
}

_shared_memory_::~_shared_memory_()
{
	detach();
}

long _shared_memory_::read(long offset, void* pBuf, DWORD bytesToRead)
{
	if( (_userMemStart == NULL) || ((offset + bytesToRead) > _memBlockSize) )
		return -1;

	// all accesses have to be wrapped in seh
	try {
		memcpy(pBuf, ((LPBYTE)_userMemStart) + offset, bytesToRead);
	} catch(...) {
		return -1;
	}
	return (long)bytesToRead;
}

long _shared_memory_::write(long offset, const void* pBuf, DWORD bytesToWrite)
{
	if( (_userMemStart == NULL) || ((offset + bytesToWrite) > _memBlockSize) )
		return -1;

	try {
		memcpy(((LPBYTE)_userMemStart) + offset, pBuf, bytesToWrite);
		++BLOCK_VERSION;
	} catch(...) {
		return -1;
	}
	return (long)bytesToWrite;
}

long _shared_memory_::refCount()
{
	return BLOCK_REFCOUNT;
}

bool _shared_memory_::isLocked()
{
	if( WaitForSingleObject(_hMutex, 0) != WAIT_OBJECT_0 )
		return true;
	ReleaseMutex(_hMutex);
	return false;
}

DWORD _shared_memory_::size()
{
	return _memBlockSize;
}

DWORD _shared_memory_::version()
{
	return BLOCK_VERSION;
}

void _shared_memory_::lock()
{
	WaitForSingleObject(_hMutex, INFINITE);
}

void _shared_memory_::unlock()
{
	ReleaseMutex(_hMutex);
}

bool _shared_memory_::reset(LPCTSTR memName, DWORD maxSize)
{
	detach();
	if( _connectToExistingBlock(memName) )
		return true;
	if( !_createNewBlock(memName, maxSize) )
		return false;
	return true;
}

void _shared_memory_::detach()
{
	// update the number of references
	if(_memBlockStart)
		try { --BLOCK_REFCOUNT; } catch(...) { }
	if(_hMutex)
		CloseHandle(_hMutex);
	if(_hFileMapping)
	{
		if(_memBlockStart)
			UnmapViewOfFile(_memBlockStart);
		CloseHandle(_hFileMapping);
	}
	_hFileMapping = NULL;
	_hMutex = NULL;
	_memBlockSize = 0;
	_memBlockStart = NULL;
	_userMemStart = NULL;
}

bool _shared_memory_::_connectToExistingBlock(LPCTSTR memName)
{
	TCHAR objName[MAX_PATH];
	// build the file mapping name from the user-specified name
	lstrcpy(objName, SHARED_MEM_MAGIC);
	if(memName == NULL || memName[0] == 0)
		lstrcat ( objName, _T("@DEF_MEM") );
	else
		lstrcpyn (  &objName[lstrlen(SHARED_MEM_MAGIC)],
					memName,
					MAX_PATH-lstrlen(SHARED_MEM_MAGIC)-1 );
	_tcsupr(objName);
	
	// try to open existing file mapping object
	_hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, objName);
	if(_hFileMapping == NULL)
		return false;	// does not exist
	
	// the name of the mutex used for synch'ing is the same
	// as the name of the memory but with first char changed to '$'
	objName[0] = _T('$');
	// open the mutex
	_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, objName);
	if(_hMutex == NULL)
	{
		CloseHandle(_hFileMapping);
		_hFileMapping = NULL;
		return false;
	}
	// map view of the block
	_memBlockStart = MapViewOfFile(_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(_memBlockStart == NULL)
	{
		CloseHandle(_hMutex);
		CloseHandle(_hFileMapping);
		_hMutex = _hFileMapping = NULL;
		return false;
	}
	// get and update bookkeeping info - no need for locking here
	_memBlockSize = BLOCK_USER_SIZE;
	_userMemStart = ((LPBYTE)_memBlockStart) + BOOKKEEP_BLOCK_SIZE;
	// update the number of references
	try {
		++BLOCK_REFCOUNT;
	} catch(...) {
	}
	return true;
}

bool _shared_memory_::_createNewBlock(LPCTSTR memName, DWORD maxSize)
{
	TCHAR objName[MAX_PATH];
	
	// build the file mapping name from the user-specified name
	lstrcpy(objName, SHARED_MEM_MAGIC);
	if(memName == NULL || memName[0] == 0)
		lstrcat ( objName, _T("@DEF_MEM") );
	else
		lstrcpyn (  &objName[lstrlen(SHARED_MEM_MAGIC)],
					memName,
					MAX_PATH-lstrlen(SHARED_MEM_MAGIC)-1 );
	// file mapping names are case-sensitive; account for that
	_tcsupr(objName);
	
	// create a new file mapping object
	_memBlockSize = maxSize;
	_hFileMapping = CreateFileMapping( (HANDLE)0xFFFFFFFF,
										NULL, PAGE_READWRITE,
										0, _memBlockSize + BOOKKEEP_BLOCK_SIZE,
										objName );
	if(_hFileMapping == NULL)
		return (GetLastError() == ERROR_ALREADY_EXISTS) ?
					_connectToExistingBlock(memName) : false;
	
	// the name of the mutex used for synch'ing is the same
	// as the name of the memory but with first char changed to '$'
	objName[0] = _T('$');
	// create the mutex;
	// TRUE locks the mutex on creation so that we can peacefully init the block
	_hMutex = CreateMutex(NULL, TRUE, objName);
	if(_hMutex == NULL)
	{
		CloseHandle(_hFileMapping);
		_hFileMapping = NULL;
		return false;
	}
	// map view of the block
	_memBlockStart = MapViewOfFile(_hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(_memBlockStart == NULL)
	{
		unlock();
		CloseHandle(_hMutex);
		CloseHandle(_hFileMapping);
		_hMutex = _hFileMapping = NULL;
		return false;
	}
	// we are the first - write all necessary bookkeeping info
	_userMemStart = ((LPBYTE)_memBlockStart) + BOOKKEEP_BLOCK_SIZE;
	try {
		BLOCK_USER_SIZE = _memBlockSize;
		BLOCK_REFCOUNT  = 1; // one user
		BLOCK_VERSION   = 0; // just started
	} catch(...) {
	}
	unlock();
	return true;
}


};	// namespace soige

