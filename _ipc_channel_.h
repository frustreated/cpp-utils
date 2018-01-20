//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _ipc_channel_.h - header for the _ipc_channel_ class
//
// Defines an inter-process communication channel, usable
// by two processes on either the same machine or different
// machines; if on different machines, the processes must
// specify their mutual hostnames as end-point machines,
// NOT the name of one ("server") machine.
//
// The read and write actions on one end are not dependent
// on each other to complete, but do depend on the other
// side's opposite action, so that one process's write
// will block until the other process is ready to read.
//
// Usage: either create two threads in each process,
// one that reads and one that writes, or intersperse reads
// and writes on one thread according to your own protocol.
// To connect, first construct or use reset(), then do either:
// // 1. -- blocking connect -- //
// if(!channel.connect(true))
// {
//     printf("Unable to connect. Error: 0x%08lx:\n", channel.errNum());
//     DisplayError(channel.errNum());
//     return -1;
// }
// - or -
// // 2. -- async connect -- //
// channel.connect(false);
// while(!channel.isOtherSideConnected())
// {
//     Sleep(x);
//     if( (ret=channel.errNum()) != 0 )
//     {
//			printf("Unable to connect. Error: 0x%08lx:\n", ret);
//			DisplayError(ret);
//			return -1;
//     }
// }
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __ipc_channel_already_included_vasya__
#define __ipc_channel_already_included_vasya__

#include "_common_.h"

namespace soige {

//------------------------------------------------------------
// The inter-process communication channel class
//------------------------------------------------------------
class _ipc_channel_
{
public:
	_ipc_channel_();
	_ipc_channel_(LPCTSTR machine, LPCTSTR channel);
	virtual ~_ipc_channel_();

	bool isOtherSideConnected();
	// if isOtherSideConnected returns false, check errNum()
	// to get the GetLastError(). if 0, then just not connected,
	// otherwise something else, like unreachable network, etc.
	DWORD errNum();

	// read() returns the number of bytes read, or -1 on error
	long read(void* pBuf, DWORD bytesToRead);
	// returns the number of bytes in the read buffer, or -1 on error
	long incomingBytesAvail();
	// write() returns the number of bytes written or -1 on error
	long write(const void* pBuf, DWORD bytesToWrite);

	// see usage comment above
	bool connect(bool blocking);
	bool disconnect();
	bool reset(LPCTSTR machine, LPCTSTR channel);

private:
	HANDLE _hPipeRead;
	HANDLE _hPipeWrite;
	TCHAR  _destMachine[MAX_PATH];	// dest machine name
	TCHAR  _channel[MAX_PATH];		// channel name

private:
	bool _ensureConnected();

	// no byval operations
	_ipc_channel_(const _ipc_channel_& ric) { }
	_ipc_channel_& operator=(const _ipc_channel_& ric) { }
};


};	// namespace soige

#endif  // __ipc_channel_already_included_vasya__

