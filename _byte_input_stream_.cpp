//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _byte_input_stream_.cpp - implementation file
// for the byte input stream class.
//
// The stream is guaranteed to remain in consistent state
// (same as before) if an invalid/failed read occurs.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_byte_stream_.h"

namespace soige {

//------------------------------------------------------------
// _byte_input_stream_
//------------------------------------------------------------

// ctors/dtors
_byte_input_stream_::_byte_input_stream_()
{
	_stream = _curPos = NULL;
	_size = 0;
	_closed = false;
}

_byte_input_stream_::_byte_input_stream_(const void* p)
{
	setStreamPtr(p);
}

_byte_input_stream_::~_byte_input_stream_()
{
}

// basic_stream overrides
bool _byte_input_stream_::reset()
{
	_curPos = _stream + BYTE_STREAM_HEADER_SIZE;
	_closed = false;
	return true;
}

long _byte_input_stream_::peekPos() const
{
	return ( _curPos - (_stream+BYTE_STREAM_HEADER_SIZE) );
}

bool _byte_input_stream_::seekPos(int cb, seek_offset from)
{
	const byte* cur = _curPos;

	if(from == ofs_begin)
		_curPos = _stream + BYTE_STREAM_HEADER_SIZE + cb;
	else if(from == ofs_end)
		_curPos = (_stream+_size-BYTE_STREAM_END_MRKR_SIZE) + cb;
	else // ofs_current
		_curPos += cb;

	if( (_curPos > (_stream+_size-BYTE_STREAM_END_MRKR_SIZE)) || (_curPos < (_stream+BYTE_STREAM_HEADER_SIZE)) )
	{
		_curPos = cur;
		return false;
	}
	return true;
}

long _byte_input_stream_::sizeOfData() const
{
	return (_size - BYTE_STREAM_HEADER_SIZE - BYTE_STREAM_END_MRKR_SIZE);
}

long _byte_input_stream_::sizeOfStream() const
{
	return _size;
}

bool _byte_input_stream_::atBOF() const
{
	return ( _curPos <= (_stream+BYTE_STREAM_HEADER_SIZE) );
}

bool _byte_input_stream_::atEOF() const
{
	return ( _curPos == _stream+_size );
}

bool _byte_input_stream_::close()
{
	if(_closed) return true;
	reset();
	return (_closed = true);
}

// input_stream overrides
bool _byte_input_stream_::_readRaw(void* p, int cb)
{
	if( (cb < 0) || (_curPos-_stream+cb > _size) ) return false;
	memcpy(p, _curPos, cb);
	_curPos += cb;
	return true;
}

bool _byte_input_stream_::_readAtPos(long begOfs, void* p, int cb) const
{
	if( cb < 0 )
		return false;
	memcpy ( p, _stream + BYTE_STREAM_HEADER_SIZE + begOfs, cb );
	return true;
}

// _byte_input_stream_ specific stuff
bool _byte_input_stream_::setStreamPtr(const void* p)
{
	_curPos = _stream = (const byte*) p;
	if( p )
	{
		_closed = false;
		return _checkStream();
	}
	_closed = true;
	return false;
}

bool _byte_input_stream_::_checkStream()
{
	long magic;
	_size = 10;
	if( !_readRaw(&magic, sizeof(magic)) )
		return false;
	if( magic != BYTE_STREAM_MAGIC_0 )
		return false;
	return ( _readRaw(&_size, sizeof(_size)) && _checkType(_bof) );
}


};	// namespace soige

