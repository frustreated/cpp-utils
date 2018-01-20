//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sharedmem.cpp - checks the shared_memory class
//
// This test app has to be run as several simultaneous
// instances to see how they interact with the shared memory.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_shared_memory_.h>
#include <_win32_file_.h>

using namespace soige;

void check_shared_memory();

int main(int argc, char* argv[])
{
	printf("Checking _shared_memory_\n");
	check_shared_memory();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// shared memory tests
void check_shared_memory()
{
	_win32_file_ in(_win32_file_::ft_stdin);
	// set up a 32K shared memory block
	_shared_memory_ sm("shared1", 1024*32);
	long lRead;
	char buf[128];
	char nput[128];
	printf("Type w to write a line to shared memory, r to read from there,\n"
			"c for the block\'s refcount, v for version, l to check locking,\n"
			"q to quit. w has to be followed by a line to write:\n");
	while(lRead=in.readLine(buf, sizeof(buf)) > 0)
	{
		if(buf[0] == 'w')
		{
			lRead=in.readLine(nput, sizeof(nput));
			sm.lock();
			sm.write(0, &lRead, sizeof(lRead));
			sm.write(sizeof(lRead), nput, lRead+1);
			sm.unlock();
		}
		else if(buf[0] == 'r')
		{
			sm.read(0, &lRead, sizeof(lRead));
			sm.read(sizeof(lRead), nput, lRead+1);
			printf("%s\n", nput);
		}
		else if(buf[0] == 'c')
		{
			// ref count
			printf("Block Refcount: %d\n", sm.refCount());
		}
		else if(buf[0] == 'v')
		{
			// version
			printf("Block Version: %d\n", sm.version());
		}
		else if(buf[0] == 'l')
		{
			// locking
			printf("Block is %s\n", sm.isLocked()? "locked" : "not locked");
		}
		else if(buf[0] == 'q')
			return;
	}
}

