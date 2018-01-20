//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _ipc_channel_.cpp - implementation of _ipc_channel_
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_ipc_channel_.h"

namespace soige {


_ipc_channel_::_ipc_channel_()
{
	_hPipeRead = _hPipeWrite = INVALID_HANDLE_VALUE;
	lstrcpy( _destMachine, _T(".") );
	lstrcpy( _channel, _T("defchannel") );
}

_ipc_channel_::_ipc_channel_(LPCTSTR destMachine, LPCTSTR channel)
{
	_hPipeRead = _hPipeWrite = INVALID_HANDLE_VALUE;
	reset(destMachine, channel);
}

_ipc_channel_::~_ipc_channel_()
{
	disconnect();
}

bool _ipc_channel_::isOtherSideConnected()
{
	if( !_ensureConnected() ) return false;
	if( incomingBytesAvail() != -1 )
		return true;
	return false;
}

DWORD _ipc_channel_::errNum()
{
	if( !_ensureConnected() ) return GetLastError();
	
	// If the other side is either not yet connected or already disconnected,
	// then we're just not connected; so channel's error is really 0; only
	// if it's something fatal, like unreachable net, etc., we return the error
	if( (incomingBytesAvail() == -1) &&
		(GetLastError() == ERROR_BAD_PIPE || GetLastError() == ERROR_BROKEN_PIPE)
	  )
		return 0;
	return GetLastError();
}

long _ipc_channel_::read(void* pBuf, DWORD bytesToRead)
{
	DWORD nBytesRead = 0;
	BOOL result;
	
	if( !_ensureConnected() ) return -1;
	result = ReadFile(_hPipeRead, pBuf, bytesToRead, &nBytesRead, NULL);
	return ( result? (long)nBytesRead : -1 );
}

long _ipc_channel_::incomingBytesAvail()
{
	DWORD nBytesAvail = 0;
	BOOL result;
	
	if( !_ensureConnected() ) return -1;
	result = PeekNamedPipe(_hPipeRead, NULL, 0, NULL, &nBytesAvail, NULL);
	return ( result? (long)nBytesAvail : -1 );
}

long _ipc_channel_::write(const void* pBuf, DWORD bytesToWrite)
{
	DWORD nBytesWritten = 0;
	BOOL result = TRUE;
	
	if( !_ensureConnected() ) return -1;
	
	// If pipe buffer gets full, a single write may not write all the data;
	// but what are we if not persistent?!
	while(bytesToWrite > 0 && result)
	{
		result = WriteFile(_hPipeWrite, pBuf, bytesToWrite, &nBytesWritten, NULL);
		bytesToWrite -= nBytesWritten;
	}
	return ( result? (long)bytesToWrite : -1 );
}

bool _ipc_channel_::connect(bool blocking)
{
	if(_hPipeRead != INVALID_HANDLE_VALUE)
		return true;	// already connected
	
	TCHAR pipeName[MAX_PATH];
	
	// Determine what side we are: first or second.
	// If the first side was already open, it can only be
	// on the first caller's machine,
	// so first try to open the pipe on dest machine.
	// Also, pipe ends are different for the two cases:
	// if we are first, pipe ending in 'R' is our read pipe,
	// the one ending in 'W' is our write pipe;
	// if we are not the first, pipe ending in 'W' is our
	// read pipe, and the one ending in 'R' is our write pipe
	lstrcpy( pipeName, _T("\\\\") );
	lstrcat( pipeName, _destMachine );
	lstrcat( pipeName, _T("\\pipe\\") );
	lstrcat( pipeName, _channel );
	lstrcat( pipeName, _T("$~$R") );
	_hPipeWrite =  CreateFile ( pipeName, GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL, OPEN_EXISTING, 0, NULL );
	if(_hPipeWrite != INVALID_HANDLE_VALUE)
	{
		// read end
		pipeName[lstrlen(pipeName)-1] = _T('W');
		_hPipeRead = CreateFile ( pipeName, GENERIC_READ,
								  FILE_SHARE_READ | FILE_SHARE_WRITE,
								  NULL, OPEN_EXISTING, 0, NULL );
		if(_hPipeRead == INVALID_HANDLE_VALUE)
		{
			CloseHandle(_hPipeWrite);
			_hPipeWrite = INVALID_HANDLE_VALUE;
			return false;
		}
	}
	else
	{
		// See if the reason is unreachable host/network
		if( GetLastError() == ERROR_BAD_NET_NAME || GetLastError() == ERROR_NETWORK_ACCESS_DENIED )
			return false;
		// The pipe does not exist; have to create new one.
		// See if already have two sides connected to this pipe and we are not welcome
		if( GetLastError() == ERROR_PIPE_BUSY )
			return false;
		// We are first, create the pipe on this machine.
		// read end
		lstrcpy( pipeName, _T("\\\\.\\pipe\\") );
		lstrcat( pipeName, _channel );
		lstrcat( pipeName, _T("$~$R") );
		_hPipeRead =  CreateNamedPipe ( pipeName, PIPE_ACCESS_INBOUND,
										PIPE_TYPE_BYTE | PIPE_WAIT,
										2, 4096*4, 4096*4, 0, NULL );
		if(_hPipeRead == INVALID_HANDLE_VALUE)
			return false;
		// write end
		pipeName[lstrlen(pipeName)-1] = _T('W');
		_hPipeWrite = CreateNamedPipe ( pipeName, PIPE_ACCESS_OUTBOUND,
										PIPE_TYPE_BYTE | PIPE_WAIT,
										2, 4096*4, 4096*4, 0, NULL );
		if(_hPipeWrite == INVALID_HANDLE_VALUE)
		{
			CloseHandle(_hPipeRead);
			_hPipeRead = INVALID_HANDLE_VALUE;
			return false;
		}
		if(!blocking)
			return true;
		// Wait for the client to connect
		BOOL rv = ConnectNamedPipe(_hPipeRead, NULL);
		rv = rv && ConnectNamedPipe(_hPipeWrite, NULL);
		if( rv || (!rv && GetLastError() == ERROR_PIPE_CONNECTED) )
			return true;
		return false;
	}
	
	return true;
}

bool _ipc_channel_::disconnect()
{
	if(_hPipeRead != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_hPipeRead);
		CloseHandle(_hPipeWrite);
	}
	_hPipeRead = _hPipeWrite = INVALID_HANDLE_VALUE;
	return true;
}

bool _ipc_channel_::reset(LPCTSTR destMachine, LPCTSTR channel)
{
	disconnect();
	lstrcpy(_destMachine, destMachine);
	lstrcpy(_channel, channel);
	return true;
}

bool _ipc_channel_::_ensureConnected()
{
	// if connect() wasn't called explicitly,
	// assume blocking connect call
	return connect(true);
}


};	// namespace soige

