//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// threadpool.cpp - checks the thread_pool class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_thread_pool_.h>

using namespace soige;

_thread_pool_ tp;
int count = 0;

void check_thread_pool();
void check_thread_pool_2();

int main(int argc, char* argv[])
{
	printf("Checking _thread_pool_\n");
	check_thread_pool();
	while( tp.havePendingJobs() )
		Sleep(50);
	
	count = 0;
	check_thread_pool_2();

	// tp.~tp();
	// without the explicit dtor call above, dumpmemleaks
	// reports two blocks - one for the thread ids and the
	// the other for event-associated data (both in tp);
	// these are not leaks
	_CrtDumpMemoryLeaks();
	return 0;
}

//-----------------------------------------------
// thread pool test
struct JOB
{
	int n;
	char c;
	JOB(int nn, char cc) : n(nn), c(cc) {}
};

// the processing callback
int __stdcall prcs_func(void* pParam);
// the cleanup callback
void __stdcall cleanup_func(void* pParam);

void check_thread_pool()
{
	//tp.setMaxThreads(0);

	for(int i=0; i<500; i++)
	{
		printf( "Queueing job # %d\n", i+1 );
		tp.queueJob( prcs_func, new JOB(i, i%95+32) );
	}

	// tp.setMaxThreads(5);
}

void check_thread_pool_2()
{
	tp.setMaxThreads(0);
	for(int i=0; i<50; i++)
	{
		printf( "CheckPoolTest2: Queueing job # %d\n", i+1 );
		tp.queueJob( prcs_func, new JOB(i, i%95+32) );
	}
	tp.setMaxThreads(2);
	tp.setKeepAliveThreads(2);
	Sleep(5000);
}

int __stdcall prcs_func(void* pParam)
{
	JOB* job = (JOB*) pParam;
	
	count++;
	
	if( count == 100 )
	{
		printf( "Setting max threads to 3\n" );
		tp.setMaxThreads(3);
	}
	else if( count == 250 )
	{
		printf( "Setting max threads to 5\n" );
		tp.setMaxThreads(5);
	}
	else if( count == 350 )
	{
		printf( "Setting max threads to 2\n" );
		tp.setMaxThreads(2);
	}
	else if( count == 400 )
	{
		printf( "Setting max threads to 1\n" );
		tp.setMaxThreads(1);
	}
	else if( count == 425 )
	{
		for(int i=0; i<50; i++)
		{
			printf( "Queueing extra job # %d\n", i+1 );
			tp.queueJob( prcs_func, new JOB(i, i%95+32) );
			if(i % 25 == 0) Sleep(0);
		}
	}
	else if( count == 450 )
	{
		printf( "Cancelling all remaining jobs\n" );
		tp.cancelPendingJobs( cleanup_func );
	}

	// printf( "Processing job # %d on threadID %d\n", count, job->n );
	printf( "Processing job # %d on threadID %d\n", count, GetCurrentThreadId() );
	Sleep(50);
	job->c = job->n%127;
	if(job->c < 32)
		job->c += 32;
	Sleep(50);
	
	delete job;
	return 0;
}

void __stdcall cleanup_func(void* pParam)
{
	JOB* job = (JOB*) pParam;
	delete job;
}

