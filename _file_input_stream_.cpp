//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _file_input_stream_.cpp - implementation file
// for the file input stream class.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_file_stream_.h"

namespace soige {

//------------------------------------------------------------
// _file_input_stream_
//------------------------------------------------------------

// ctors/dtors
_file_input_stream_::_file_input_stream_(LPCTSTR fileName)
{
	if(fileName)
	{
		_stmFile.reset(fileName);
		_checkStream();
	}
}

_file_input_stream_::~_file_input_stream_()
{
	close();
}

// basic_stream overrides
bool _file_input_stream_::reset()
{
	return ( _stmFile.seekPos(BYTE_STREAM_HEADER_SIZE, ofs_begin) != -1 );
}

long _file_input_stream_::peekPos() const
{
	if( !_stmFile.isOpen() ) return -1;
	return ( _stmFile.peekPos() - BYTE_STREAM_HEADER_SIZE );
}

bool _file_input_stream_::seekPos(int cb, seek_offset from)
{
	if(_stmFile.getFileName() == NULL) return false;
	
	long retval = 0;
	
	if(from == ofs_begin)
		retval = _stmFile.seekPos(cb + BYTE_STREAM_HEADER_SIZE, from);
	else if(from == ofs_end)
		retval = _stmFile.seekPos(cb - BYTE_STREAM_HEADER_SIZE, from);
	else // ofs_current
		retval = _stmFile.seekPos(cb, from);

	return (retval == -1) ? false : true;
}

long _file_input_stream_::sizeOfData() const
{
	return (_stmFile.getFileSize() - BYTE_STREAM_HEADER_SIZE - BYTE_STREAM_END_MRKR_SIZE);
}

long _file_input_stream_::sizeOfStream() const
{
	return _stmFile.getFileSize();
}

bool _file_input_stream_::atBOF() const
{
	long retval = _stmFile.peekPos();
	return ( retval != -1 && retval <= BYTE_STREAM_HEADER_SIZE );
}

bool _file_input_stream_::atEOF() const
{
	long retval = _stmFile.peekPos();
	return ( retval != -1 && retval >= (_stmFile.getFileSize()-BYTE_STREAM_HEADER_SIZE) );
}

bool _file_input_stream_::close()
{
	_stmFile.close();
	return true;
}

// input_stream overrides
bool _file_input_stream_::_readRaw(void* p, int cb)
{
	if( cb < 0 )
		return false;
	if( _stmFile.read(p, cb) < 0 )
		return false;
	return true;
}

bool _file_input_stream_::_readAtPos(long begOfs, void* p, int cb) const
{
	if( cb < 0 )
		return 0;
	long curpos = _stmFile.peekPos();
	_stmFile.seekPos(BYTE_STREAM_HEADER_SIZE + begOfs, ofs_begin);
	if( _stmFile.read(p, cb) < 0 )
		return false;
	// return the pointer back
	_stmFile.seekPos(curpos, ofs_begin);
	return true;
}

// byte_input_stream overrides
bool _file_input_stream_::_checkStream()
{
	int magic;
	int size = 10;
	
	if( !_stmFile.open(access_read, share_read, open_existing) )
		return false;
	if( !_readRaw(&magic, sizeof(magic)) )
		return false;
	if( magic != BYTE_STREAM_MAGIC_0 )
		return false;
	return ( _readRaw(&size, sizeof(size)) && _checkType(_bof) );
}

// _file_input_stream_ - specific stuff
bool _file_input_stream_::reset(LPCTSTR fileName)
{
	_stmFile.reset(fileName);
	return _checkStream();
}

LPCTSTR _file_input_stream_::getFileName()
{
	return _stmFile.getFileName();
}


};	// namespace soige

