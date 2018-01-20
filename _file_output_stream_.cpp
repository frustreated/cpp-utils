//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _file_output_stream_.cpp - implementation file
// for the file output stream class.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_file_stream_.h"

namespace soige {

//------------------------------------------------------------
// _file_output_stream_
//------------------------------------------------------------

// ctors/dtors
_file_output_stream_::_file_output_stream_(LPCTSTR fileName)
{
	_stmFile.reset(fileName);
	_initStream();
}

_file_output_stream_::~_file_output_stream_()
{
	close();
}

// basic_stream overrides
bool _file_output_stream_::reset()
{
	_stmFile.close();
	return _initStream();
}

long _file_output_stream_::peekPos() const
{
	if( !_stmFile.isOpen() ) return -1;
	return ( _stmFile.peekPos() - BYTE_STREAM_HEADER_SIZE );
}

bool _file_output_stream_::seekPos(int cb, seek_offset from)
{
	if(_stmFile.getFileName() == NULL) return false;
	
	long retval = 0;
	
	if(from == ofs_begin)
		retval = _stmFile.seekPos(cb + BYTE_STREAM_HEADER_SIZE, from);
	else // ofs_current || ofs_end
		retval = _stmFile.seekPos(cb, from);

	return (retval == -1) ? false : true;
}

long _file_output_stream_::sizeOfData() const
{
	// see if the stream file is closed or not
	if( _stmFile.isOpen() )
		return ( _stmFile.getFileSize() - BYTE_STREAM_HEADER_SIZE );
	else if(_stmFile.getFileHandle() == INVALID_HANDLE_VALUE && _stmFile.getFileName() != NULL)
		return ( _stmFile.getFileSize() - BYTE_STREAM_HEADER_SIZE - BYTE_STREAM_END_MRKR_SIZE );
	// not open and not closed: the file is not initialized yet
	return 0;
}

long _file_output_stream_::sizeOfStream() const
{
	if( _stmFile.isOpen() )
		return ( _stmFile.getFileSize() + BYTE_STREAM_END_MRKR_SIZE );
	else if(_stmFile.getFileHandle() == INVALID_HANDLE_VALUE && _stmFile.getFileName() != NULL)
		return _stmFile.getFileSize();
	// not open and not closed: the file is not initialized yet
	return 0;
}

bool _file_output_stream_::atBOF() const
{
	long retval = _stmFile.peekPos();
	return ( retval != -1 && retval <= BYTE_STREAM_HEADER_SIZE );
}

bool _file_output_stream_::atEOF() const
{
	return _stmFile.atEOF();
}

bool _file_output_stream_::close()
{
	if( !_stmFile.isOpen() && _stmFile.getFileName() != NULL ) return true;
	// write eof + magic at the end of stream
	_writeType(_eof);
	_writeRaw( &BYTE_STREAM_MAGIC_0, sizeof(BYTE_STREAM_MAGIC_0) );
	// write total stream size (headers and data) in the beginning,
	// after the magic and before the BOF marker
	int size = _stmFile.getFileSize();
	_writeAtPos( -sizeof(byte)-sizeof(size), &size, sizeof(size) );
	_stmFile.close();
	return true;
}

// output_stream overrides
bool _file_output_stream_::_writeRaw(const void* p, int cb)
{
	if( cb < 0 )
		return 0;
	if( _stmFile.write(p, cb) == -1 )
		return false;
	return true;
}

bool _file_output_stream_::_writeAtPos(int begOfs, const void* p, int cb)
{
	if( cb < 0 )
		return 0;
	long curpos = _stmFile.peekPos();
	_stmFile.seekPos(BYTE_STREAM_HEADER_SIZE + begOfs, ofs_begin);
	if( _stmFile.write(p, cb) == -1 )
		return false;
	// return the pointer back
	_stmFile.seekPos(curpos, ofs_begin);
	return true;
}

// byte_output_stream overrides
bool _file_output_stream_::_initStream()
{
	int streamLenReserve = 0;
	if( !_stmFile.open(access_all, share_read, create_always) ) return false;
	if( !_writeRaw(&BYTE_STREAM_MAGIC_0, sizeof(BYTE_STREAM_MAGIC_0)) ) return false;
	if( !_writeRaw(&streamLenReserve, sizeof(streamLenReserve)) ) return false;
	if( !_writeType(_bof) ) return false;
	return true;
}

// _file_output_stream_ - specific stuff
bool _file_output_stream_::reset(LPCTSTR fileName)
{
	_stmFile.reset(fileName);
	return _initStream();
}

LPCTSTR _file_output_stream_::getFileName()
{
	return _stmFile.getFileName();
}


};	// namespace soige

