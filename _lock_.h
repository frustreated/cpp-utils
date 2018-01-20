//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _lock_.h - header/impl file for the locking classes.
//
// Provides an exclusive lock and also a lock that is
// optimized for heavy read usage with infrequent writes.
// Stolen from the MS's LookupTable object.
//
// Platform SDK's UtSem.h also includes a few field-proven
// implementations of locks, esp. the UTSemReadWrite class.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __lock_already_included_vasya__
#define __lock_already_included_vasya__

#include "_common_.h"

// #define USESPINLOCK

namespace soige {


//------------------------------------------------------------
// exclusive lock implementation
//------------------------------------------------------------
class _exclusive_lock_
{
#ifdef USESPINLOCK

private:
	long _lLock;
	long _cSpin;

	_waitAcquire()
	{
		while (1)
		{
			for (int i = 0; i <= _cSpin; i++) // at least once
			{
				if (_lLock == 0)
				{
					if (InterlockedExchange(&_lLock, 1) == 0)
						return;
				}
			}

			// still locked after spin - swap out the current thread
			Sleep(0);
		}
	}

public:

	_exclusive_lock_()
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		_cSpin = (si.dwNumberOfProcessors > 1) ? 4096 : 0;
		_lLock = 0;
	}

	virtual ~_exclusive_lock_()
	{
	}

	void acquire()
	{
		if (InterlockedExchange(&_lLock, 1) != 0)
			_waitAcquire();
	}

	void release()
	{
		_lLock = 0;
	}

#else

private:
	CRITICAL_SECTION _csLock;

public:
	_exclusive_lock_()
	{
		InitializeCriticalSection(&_csLock);
	}

	virtual ~_exclusive_lock_()
	{
		DeleteCriticalSection(&_csLock);
	}

	void acquire()
	{
		EnterCriticalSection(&_csLock);
	}

	void release()
	{
		LeaveCriticalSection(&_csLock);
	}

#endif

private:
	// no byval operations
	_exclusive_lock_(const _exclusive_lock_& rel) { }
	_exclusive_lock_& operator=(const _exclusive_lock_& rel) { }
};


//------------------------------------------------------------
// Simple R/W lock implementation.
// This implementation should only be used when write locks
// are rare. UTSemReadWrite (see comments above) uses an event
// to let the waiting writer know when all readers drained;
// we use Sleep() for this purpose. Otherwise, the two
// implementations are very similar.
//------------------------------------------------------------
class _rw_lock_
{
private:
	_exclusive_lock_  _exLock;	// write lock holds it
	long  _cReadLocks;			// count of current read locks

public:
	_rw_lock_()
	{
		_cReadLocks = 0;
	}

	virtual ~_rw_lock_()
	{
	}

	void acquireReadLock()
	{
		_exLock.acquire();

		// increment (interlocked because release doesn't do CS) read lock count
		InterlockedIncrement(&_cReadLocks);

		_exLock.release();
	}

	void releaseReadLock()
	{
		InterlockedDecrement(&_cReadLocks); // interlocked because outside of CS
	}

	void acquireWriteLock()
	{
		_exLock.acquire();

		// wait for all read locks to drain
		while (_cReadLocks > 0)
			Sleep(0);

		// no read locks - write lock acquired
	}

	void releaseWriteLock()
	{
		_exLock.release();
	}

private:
	// no byval operations
	_rw_lock_(const _rw_lock_& rl) { }
	_rw_lock_& operator=(const _rw_lock_& rl) { }
};


};	// namespace soige

#endif  // __lock_already_included_vasya__

