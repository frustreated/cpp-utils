//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _thread_pool_.cpp - implementation file for the thread pool
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_thread_pool_.h"

namespace soige {

_thread_pool_::_thread_pool_()
{
	InitializeCriticalSection( &_csLock );
	_maxThreads = _currThreads = _keepAliveThreads = 0;
	_threadIDs = NULL;
	_jobList = NULL;
	_jobTail = NULL;
	_hReleaseEvent = CreateEvent( NULL,		// lpSecAttr
								  FALSE,	// manual reset
								  TRUE,		// initially signaled
								  NULL		// name
								);

	// Set the initial max thread count to (num_processors * 2)
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	setMaxThreads( si.dwNumberOfProcessors * 2 );
}

_thread_pool_::~_thread_pool_()
{
	// Wait for all the threads to finish what they're doing and exit;
	// keep-alives will keep them from dying unless set to 0
	_keepAliveThreads = 0;
	while( _currThreads > 0 )
		Sleep(0);  // if( WaitForSingleObject( _hReleaseEvent, INFINITE ) != WAIT_FAILED ) SetEvent( _hReleaseEvent );

	if( _hReleaseEvent )
		CloseHandle( _hReleaseEvent );
	_hReleaseEvent = NULL;
	DeleteCriticalSection( &_csLock );
	_maxThreads = 0;
	free( _threadIDs );
	_threadIDs = NULL;
	_jobList = NULL;
	_jobTail = NULL;
}

// Setting max threads to 0 will put job processing on hold.
void _thread_pool_::setMaxThreads(int maxThreadCount)
{
	if( maxThreadCount == _maxThreads || maxThreadCount < 0 )
		return;
	
	// If the new number of threads is greater than the current,
	// change the maxThreads variable and increase the thread array.
	// If the new number is less than the current,
	// make sure that whatever jobs are pending are processed
	// first before freeing any threads that may be busy.
	
	if( maxThreadCount > _maxThreads )
	{
		_lock();
		_maxThreads = maxThreadCount;
		_threadIDs = (DWORD*) realloc( _threadIDs, _maxThreads*sizeof(DWORD) );
		for(int i=_currThreads; i<_maxThreads; i++)
			_threadIDs[i] = 0;
		// if there are jobs pending, spawn new threads up to max;
		// if more than needed get created, the extra ones will
		// die when they find out there is nothing for them to do
		while( _jobList && (_currThreads < _maxThreads) )
			_createAndAddThreadToPool();
		_unlock();
	}
	else // ( maxThreadCount < _maxThreads )
	{
		_maxThreads = maxThreadCount;
		if( _keepAliveThreads > _maxThreads )
			_keepAliveThreads = _maxThreads;
		// wait for the extra threads to die
		while( _currThreads > _maxThreads )
			Sleep(0);
	}
}

// Keep-alive threads are kept alive only after being created;
// we don't specifically create threads when keep-alives are set.
// If there are more keep alives than the new value, the extra
// ones will die off as soon as there are no jobs queued.
void _thread_pool_::setKeepAliveThreads( int keepAliveThreadCount )
{
	if( keepAliveThreadCount >= 0 && keepAliveThreadCount <= _maxThreads )
		_keepAliveThreads = keepAliveThreadCount;
}

void _thread_pool_::queueJob( JOB_PROCESSING_FUNC pProcessingFunc, void* pJob )
{
	QueuedJob* pNewJob = new QueuedJob( pProcessingFunc, pJob );
	
	_lock();
	if( !_jobList )
		_jobList = pNewJob;
	else
		_jobTail->pNextJob = pNewJob;

	_jobTail = pNewJob;

	// If all threads are busy and didn't reach the max thread count,
	// add a new thread and make it work for its livelihood;
	// otherwise the job will have to wait to be processed
	// by the next thread that gets released.
	if( (_currThreads < _maxThreads) && _allThreadsBusy() )
		_createAndAddThreadToPool();

	_unlock();
}

void _thread_pool_::cancelPendingJobs( JOB_CLEANUP_FUNC pCleanupFunc /* = NULL */ )
{
	// To cancel all queued jobs, we simply remove them from
	// the list; the waiting threads will all automatically exit
	// when they are released and find out there are no jobs
	_lock();
	QueuedJob* it = _jobList;
	while(it)
	{
		_jobList = _jobList->pNextJob;
		if( pCleanupFunc )
			(pCleanupFunc)( it->pJob );
		delete it;
		it = _jobList;
	}
	_jobTail = NULL;
	_unlock();
}

int _thread_pool_::getMaxThreads() const
{
	return _maxThreads;
}

int _thread_pool_::getCurrThreads() const
{
	return _currThreads;
}

int _thread_pool_::getKeepAliveThreads() const
{
	return _keepAliveThreads;
}

bool _thread_pool_::havePendingJobs() const
{
	return (_jobList != NULL);
}

void _thread_pool_::_lock()
{
	EnterCriticalSection( &_csLock );
}

void _thread_pool_::_unlock()
{
	LeaveCriticalSection( &_csLock );
}

bool _thread_pool_::_createAndAddThreadToPool()
{
	// This func is called from queueJob() and setMaxThreads() which lock,
	// so no locking needed here
	DWORD tid = 0;
	HANDLE hThread = NULL;

	hThread = CreateThread( NULL,		 // security
							0,			 // stack size
							_threadFunc, // func
							this,		 // arg
							0,			 // create running,
							&tid		 // thread id
							);
	if( !hThread )
		return false;
	_threadIDs[_currThreads++] = tid;
	// _removeThreadFromPool() only removes the thread id from
	// the array; however, the handle also has to be closed
	CloseHandle( hThread );
	return true;
}

void _thread_pool_::_removeThreadFromPool(DWORD threadID)
{
	// This func is called from _threadFunc (which locks), so no locking needed
	for(int i=0; i<_currThreads; i++)
	{
		if( _threadIDs[i] == threadID )
		{
			for(int j=i; j<_currThreads-1; j++)
				_threadIDs[j] = _threadIDs[j+1];
			_threadIDs[--_currThreads] = 0;
			return;
		}
	}
}

bool _thread_pool_::_allThreadsBusy() const
{
	DWORD ret = WaitForSingleObject( _hReleaseEvent, 0 );
	if( ret == WAIT_OBJECT_0 || ret == WAIT_ABANDONED )
	{
		// event is signaled - i.e., no threads are waiting
		// (meaning all are busy or none were created yet)
		SetEvent( _hReleaseEvent );
		return true;
	}
	else if( ret == WAIT_TIMEOUT )
		// event not signaled - i.e. at least one thread is available
		return false;

	// WAIT_FAIL ?
	return true;
}

/* static */
DWORD WINAPI _thread_pool_::_threadFunc( void* pParam )
{
	// pParam is a pointer to the thread pool class that created this thread
	_thread_pool_* pPool = (_thread_pool_*) pParam;
	QueuedJob* pNextJob = NULL;

	while (true)
	{
		if( WaitForSingleObject( pPool->_hReleaseEvent, INFINITE ) == WAIT_FAILED )
			break;

		// Release any other threads that may be waiting
		SetEvent( pPool->_hReleaseEvent );
		
		// If the max number of threads was revised down
		// while we were waiting, exit the thread and remove
		// it from the current thread pool
		if( pPool->_currThreads > pPool->_maxThreads )
			break;
		
		pPool->_lock();
		if( pPool->_jobList )
		{
			// Detach the next job from the list
			pNextJob = pPool->_jobList;
			pPool->_jobList = pPool->_jobList->pNextJob;
			if( !pPool->_jobList )
				pPool->_jobTail = NULL;
			
			pPool->_unlock();
		}
		else // !pPool->_jobList
		{
			pPool->_unlock();
			
			// If no jobs are queued, and the current number of threads
			// is greater than the keepalives needed, exit the thread;
			// however, if we are left one of the keepalives,
			// surrender the time slice and continue waiting
			if( pPool->_currThreads > pPool->_keepAliveThreads )
				break;
			
			Sleep(0);
			continue;
		}
		
		// call the user-provided func that runs the job
		if( pNextJob->pPrcsFunc )
			(pNextJob->pPrcsFunc)( pNextJob->pJob );

		// free the job
		delete pNextJob;
		pNextJob = NULL;
		// continue with next wait
	}
	
	pPool->_lock();
	pPool->_removeThreadFromPool( GetCurrentThreadId() );
	pPool->_unlock();
	
	return 0;
}


};	// namespace soige

