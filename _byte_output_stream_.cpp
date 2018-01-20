//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _byte_output_stream_.cpp - implementation file
// for the byte output stream class.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_byte_stream_.h"

namespace soige {

//------------------------------------------------------------
// _byte_output_stream_
//------------------------------------------------------------

// ctors/dtors
_byte_output_stream_::_byte_output_stream_()
{
	_stream = _curPos = NULL;
	_initStream();
}

_byte_output_stream_::~_byte_output_stream_()
{
	if(_stream) free(_stream);
	_stream = _curPos = NULL;
}

// basic_stream overrides
bool _byte_output_stream_::reset()
{
	return _initStream();
}

long _byte_output_stream_::peekPos() const
{
	return ( _curPos - (_stream+BYTE_STREAM_HEADER_SIZE) );
}

bool _byte_output_stream_::seekPos(int cb, seek_offset from)
{
	if(_closed) return false;

	byte* cur = _curPos;

	if(from == ofs_begin)
		_curPos = _stream + BYTE_STREAM_HEADER_SIZE + cb;
	else if(from == ofs_end)
		// the end marker size is not deducted, since it's not there yet
		_curPos = (_stream+_size) + cb;
	else // ofs_current
		_curPos += cb;

	if( (_curPos > (_stream+_size)) || (_curPos < (_stream+BYTE_STREAM_HEADER_SIZE)) )
	{
		_curPos = cur;
		return false;
	}
	return true;
}

long _byte_output_stream_::sizeOfData() const
{
	if(_closed) return (_size - BYTE_STREAM_HEADER_SIZE - BYTE_STREAM_END_MRKR_SIZE);
	return (_size - BYTE_STREAM_HEADER_SIZE);
}

long _byte_output_stream_::sizeOfStream() const
{
	if(_closed) return _size;
	return (_size + BYTE_STREAM_END_MRKR_SIZE);
}

bool _byte_output_stream_::atBOF() const
{
	return ( _size <= BYTE_STREAM_HEADER_SIZE );
}

bool _byte_output_stream_::atEOF() const
{
	return ( _curPos == _stream+_size );
}

bool _byte_output_stream_::close()
{
	if(_closed) return true;
	// write eof + magic at the end of stream
	_writeType(_eof);
	_writeRaw( &BYTE_STREAM_MAGIC_0, sizeof(BYTE_STREAM_MAGIC_0) );
	// write total stream size (headers and data) in the beginning,
	// after the magic and before the BOF marker
	_writeAtPos( -sizeof(byte)-sizeof(_size), &_size, sizeof(_size) );
	_closed = true;
	return true;
}

// output_stream overrides
bool _byte_output_stream_::_writeRaw(const void* p, int cb)
{
	if( cb < 0 || _closed )
		return false;
	if(_size+cb >= _allocSize)
		_resizeStream( (_size+cb)*1.35 );
	memcpy(_curPos, p, cb);
	_curPos += cb;
	_size += cb;
	return true;
}

bool _byte_output_stream_::_writeAtPos(int begOfs, const void* p, int cb)
{
	if( begOfs < 0 || cb < 0 )
		return false;
	memcpy ( _stream + BYTE_STREAM_HEADER_SIZE + begOfs, p, cb );
	return true;
}

// _byte_output_stream_ specific stuff
const void* _byte_output_stream_::getStreamPtr() const
{
	return _stream;
}

bool _byte_output_stream_::_initStream()
{
	_closed = false;
	int streamLenReserve = 0;
	if(_stream) free(_stream);
	_stream = _curPos = NULL;
	if( !_resizeStream(128) ) return false;
	if( !_writeRaw(&BYTE_STREAM_MAGIC_0, sizeof(BYTE_STREAM_MAGIC_0)) ) return false;
	if( !_writeRaw(&streamLenReserve, sizeof(streamLenReserve)) ) return false;
	if( !_writeType(_bof) ) return false;
	_size = _curPos - _stream;
	return true;
}

bool _byte_output_stream_::_resizeStream(long newSize)
{
	if( newSize < 0 )
		return false;

	_allocSize = newSize;
	if(_stream == NULL)
	{
		_stream = (byte*) malloc(newSize);
		_curPos = _stream;
	}
	else
	{
		int offset = _curPos - _stream;
		_stream = (byte*) realloc(_stream, newSize);
		_curPos = _stream + offset;
	}
	
	if(_size > newSize) _size = newSize;
	return (_stream != NULL);
}


};	// namespace soige

