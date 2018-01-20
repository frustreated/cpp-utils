//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _shared_memory_.h - header for the _shared_memory_ class
//
// Defines an object to allow processes on a single machine
// to use shared, synchronized (if desired) memory block.
//
// The block is a pagefile-backed in-memory file mapping.
//
// Usage: use a common name to create the block in each
// process that wants to use shared memory and just use it.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __shared_memory_already_included_vasya__
#define __shared_memory_already_included_vasya__

#include "_common_.h"

namespace soige {

//------------------------------------------------------------
// The inter-process shared memory class
//------------------------------------------------------------
class _shared_memory_
{
public:
	_shared_memory_();
	virtual ~_shared_memory_();

	// read() returns the number of bytes read or -1 on error.
	// It does not fail if the memory is locked; when locked
	// reads/writes are important, check isLocked() and/or
	// use lock()/unlock() or version() when reading/writing
	long  read(long offset, void* pBuf, DWORD bytesToRead);
	// write() returns the number of bytes written or -1 on error.
	// It does not fail if the memory is locked; when locked
	// reads/writes are important, check isLocked() and/or
	// use lock()/unlock() or version() when reading/writing
	long  write(long offset, const void* pBuf, DWORD bytesToWrite);
	
	// number of objects using this memory block;
	// may not be correct if a process was shut down abnormally
	long  refCount();
	bool  isLocked();
	DWORD size();
	// the version of the data in memory; incremented on each write
	DWORD version();
	void  lock();
	void  unlock();

	// disconnect from the previous block and establish a new one
	bool  reset(LPCTSTR memName, DWORD maxSize);
	void  detach();

private:
	HANDLE _hFileMapping;	// file mapping handle
	HANDLE _hMutex;			// mutex for synchronization
	DWORD  _memBlockSize;	// user-usable block size
	void*  _memBlockStart;	// the start of the memory block
	void*  _userMemStart;	// the start of the user-usable portion of the memory block

private:
	bool  _connectToExistingBlock(LPCTSTR memName);
	bool  _createNewBlock(LPCTSTR memName, DWORD maxSize);

	// size of the bookkeeping block
	static const long BOOKKEEP_BLOCK_SIZE;
	// offsets of values in the bookkeeping block, in bytes
	static const long OFFSET_BLOCK_USERSIZE;
	static const long OFFSET_BLOCK_REFCOUNT;
	static const long OFFSET_BLOCK_VERSION;
	// to make the names of the file mapping object and the mutex unique
	static const TCHAR SHARED_MEM_MAGIC[];

private:
	// no byval operations
	_shared_memory_(const _shared_memory_& rsm) { }
	_shared_memory_& operator=(const _shared_memory_& rsm) { }
};


};	// namespace soige

#endif  // __shared_memory_already_included_vasya__

