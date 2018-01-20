//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _thread_pool_.h - header file for the thread pool class.
//
// Encapsulates simple job queueing to a pool of threads.
//
// Usage:
// 1. Create an object of the _thread_pool_ class.
// 2. Optionally set the maximum number of threads in the pool
//    (the default is num_of_processors * 2).
// 3. Call queueJob() providing a pointer to a callback
//    function that does the actual job processing.
//    If any application-level locking is needed to protect
//    data from simultaneous access by multiple threads,
//    the application is expected to take care of it.
// 4. If cancelPendingJobs() is called at any point in the
//    program and there is any application-specific cleanup
//    that needs to be done for each job, a pointer to the
//    appropriate function (both function signatures are
//    specified below) should be supplied to cancelPendingJobs.
//
// Note: If a thread pool object is created locally and then
// goes out of scope, the thread on which the object was
// created will not be freed until all pending jobs have
// been processed. If the application expects all jobs to
// be cancelled when the thread pool object is destroyed,
// it should call cancelPendingJobs() before the object
// goes out of scope.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __thread_pool_already_included_vasya__
#define __thread_pool_already_included_vasya__

#include "_common_.h"

namespace soige {

// A function with this signature has to be defined by the user and
// passed as the callback that will be called to process a queued job.
typedef int (__stdcall * JOB_PROCESSING_FUNC) ( void* pParam );

// A function with this signature can be (doesn't have to) defined
// by the user and passed as callback to cancelPendingJobs() which
// will call it for each job to do any app-specific job cleanup
// (for example, if a job points to dynamically allocated memory,
// the client app would free the contained structures, etc.).
// If no application-specific cleanup has to be done, the client
// can pass a null ptr to cancelPendingJobs().
typedef void (__stdcall * JOB_CLEANUP_FUNC) ( void* pParam );

class _thread_pool_
{
public:
	// ctor/dtor
	_thread_pool_();
	virtual ~_thread_pool_();

	//--------------------------------
	// operations
	void setMaxThreads( int maxThreadCount );
	void setKeepAliveThreads( int keepAliveThreadCount );
	void queueJob( JOB_PROCESSING_FUNC pProcessingFunc, void* pJob );
	void cancelPendingJobs( JOB_CLEANUP_FUNC pCleanupFunc = NULL );

	//--------------------------------
	// attributes
	int  getMaxThreads() const;
	int  getCurrThreads() const;
	int  getKeepAliveThreads() const;
	bool havePendingJobs() const;

protected:
	void _lock();
	void _unlock();
	bool _createAndAddThreadToPool();
	void _removeThreadFromPool(DWORD threadID);
	bool _allThreadsBusy() const;

	// the thread func
	static DWORD WINAPI _threadFunc( void* pParam );

protected:
	// The struct that stores queued job info
	struct QueuedJob {
		JOB_PROCESSING_FUNC	pPrcsFunc;
		void*				pJob;
		QueuedJob*			pNextJob;
		QueuedJob( JOB_PROCESSING_FUNC func, void* job ) :
			pPrcsFunc(func), pJob(job), pNextJob(NULL) {}
	};

	// members
	int					_maxThreads;
	int					_currThreads;		// current threads in pool
	int					_keepAliveThreads;	// number of threads to keep alive even if no jobs are pending
	DWORD*				_threadIDs;
	QueuedJob*			_jobList;			// linked list of pending jobs
	QueuedJob*			_jobTail;			// ptr to last job on list; for speed
	CRITICAL_SECTION	_csLock;
	HANDLE				_hReleaseEvent;		// thread signal event

private:
	// no byval operations
	_thread_pool_(const _thread_pool_& rtp) { }
	_thread_pool_& operator=(const _thread_pool_& rtp) { }
};


};	// namespace soige

#endif  // __thread_pool_already_included_vasya__

