//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _file_stream_.h - header file for file stream classes
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __file_stream_already_included_vasya__
#define __file_stream_already_included_vasya__

#include "_byte_stream_.h"
#include "_win32_file_.h"

namespace soige {

//------------------------------------------------------------
// FILE STREAMS
//------------------------------------------------------------

// These derive from byte streams, and so use their magic
// static long const FILE_STREAM_MAGIC_0 = 0x6a90c7d2;

//------------------------------------------------------------
// File output stream
//------------------------------------------------------------
class _file_output_stream_ : public _byte_output_stream_
{
public:
	// ctor/dtor
	_file_output_stream_(LPCTSTR fileName = NULL);
	virtual ~_file_output_stream_();

	// basic_stream overrides
	virtual bool  reset();
	virtual long  peekPos() const;
	virtual bool  seekPos(int cb, seek_offset from = ofs_current);
	virtual long  sizeOfData() const;
	virtual long  sizeOfStream() const;
	virtual bool  atBOF() const;
	virtual bool  atEOF() const;
	virtual bool  close();

protected:
	// output_stream overrides
	virtual bool _writeRaw(const void* p, int cb);
	virtual bool _writeAtPos(int begOfs, const void* p, int cb);

protected:
	// byte_input_stream overrides
	virtual bool _initStream();

public:
	// _file_output_stream_ - specific stuff
	bool	reset(LPCTSTR fileName);
	LPCTSTR getFileName();

protected:
	_win32_file_  _stmFile;
};

//------------------------------------------------------------
// File input stream
//------------------------------------------------------------
class _file_input_stream_ : public _byte_input_stream_
{
public:
	// ctors/dtors
	_file_input_stream_(LPCTSTR fileName = NULL);
	virtual ~_file_input_stream_();

	// basic_stream overrides
	virtual bool reset();
	virtual long peekPos() const;
	virtual bool seekPos(int cb, seek_offset from = ofs_current);
	virtual long sizeOfData() const;
	virtual long sizeOfStream() const;
	virtual bool atBOF() const;
	virtual bool atEOF() const;
	virtual bool close();

protected:
	// input_stream overrides
	virtual bool _readRaw(void* p, int cb);
	virtual bool _readAtPos(long begOfs, void* p, int cb) const;

protected:
	// byte_input_stream overrides
	virtual bool _checkStream();

public:
	// _file_input_stream_ - specific stuff
	bool	reset(LPCTSTR fileName);
	LPCTSTR getFileName();

protected:
	mutable _win32_file_  _stmFile;
};

};	// namespace soige

#endif  // __file_stream_already_included_vasya__

