//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _byte_stream_.h - header file for byte stream classes
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __byte_stream_already_included_vasya__
#define __byte_stream_already_included_vasya__

#include "_base_stream_.h"

namespace soige {

//------------------------------------------------------------
// BYTE STREAMS
//------------------------------------------------------------
static const int BYTE_STREAM_MAGIC_0		= 0x2f35e8b9;
static const int BYTE_STREAM_HEADER_SIZE	= sizeof(BYTE_STREAM_MAGIC_0) + sizeof(long) + sizeof(byte);
static const int BYTE_STREAM_END_MRKR_SIZE	= sizeof(byte) + sizeof(BYTE_STREAM_MAGIC_0);

//------------------------------------------------------------
// Byte stream format:
// BYTE_STREAM_MAGIC STREAM_SIZE BOF_MARKER ... data ... EOF_MARKER BYTE_STREAM_MAGIC
// |<---  long --->| |<-long ->| |<-byte->| ............ |<-byte->| |<---  long --->|
//------------------------------------------------------------

//------------------------------------------------------------
// Byte output stream
//------------------------------------------------------------
class _byte_output_stream_ : public output_stream
{
public:
	// ctors/dtors
	_byte_output_stream_();
	virtual ~_byte_output_stream_();

	// basic_stream overrides
	virtual bool reset  ();
	virtual long peekPos() const;
	virtual bool seekPos(int cb, seek_offset from = ofs_current);
	virtual long sizeOfData  () const;
	virtual long sizeOfStream() const;
	virtual bool atBOF  () const;
	virtual bool atEOF  () const;
	virtual bool close  ();

	// writeXXX funcs are handled by output_stream

protected:
	// input_stream overrides
	virtual bool _writeRaw	(const void* p, int cb);
	// this func does not affect stream position or size
	// but simply writes bytes to the specified offset from stream beginning
	virtual bool _writeAtPos(int begOfs, const void* p, int cb);

public:
	// _byte_output_stream_ specific stuff
	const void* getStreamPtr() const;
protected:
	virtual bool _initStream();
	bool _resizeStream(long newSize);

protected:
	// data
	byte* _stream;
	byte* _curPos;
	long  _size;
	long  _allocSize;
	bool  _closed;
};


//------------------------------------------------------------
// Byte input stream
//------------------------------------------------------------
class _byte_input_stream_ : public input_stream
{
public:
	// ctors/dtors
	_byte_input_stream_();
	_byte_input_stream_(const void* p);
	virtual ~_byte_input_stream_();

	// basic_stream overrides
	virtual bool  reset  ();
	virtual long  peekPos() const;
	virtual bool  seekPos(int cb, seek_offset from = ofs_current);
	virtual long  sizeOfData  () const;
	virtual long  sizeOfStream() const;
	virtual bool  atBOF  () const;
	virtual bool  atEOF  () const;
	virtual bool  close  ();

	// readXXX funcs are handled by input_stream

protected:
	// input_stream overrides
	virtual bool _readRaw  (void* p, int cb);
	virtual bool _readAtPos(long begOfs, void* p, int cb) const;

public:
	// _byte_input_stream_ specific stuff
	bool setStreamPtr(const void* p);
protected:
	virtual bool _checkStream();

protected:
	// data
	byte const*	_stream;
	byte const*	_curPos;
	long		_size;
	bool		_closed;
};


};	// namespace soige

#endif  // __byte_stream_already_included_vasya__

