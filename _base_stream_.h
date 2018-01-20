//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _base_stream_.h - header file for base stream classes
// and interfaces.
//
// Defines interfaces for:
// Input stream (from file, unmarshaling, etc.);
// Output stream (to file, marshaling, etc.);
// Serialization interface (for objects to implement).
//
// A couple of things to keep in mind:
// 1. If a serializable object is a subclass of other object(s),
//    make sure to call the parent class(es)'s readObject/
//    writeObject in the object's respective function (i.e.,
//	  to be serialized correctly, superclasses must also
//    implement serializable); should probably be called
//    before object's own serialization.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __base_stream_already_included_vasya__
#define __base_stream_already_included_vasya__

#include "_common_.h"

namespace soige {

//------------------------------------------------------------
// The stream and serialization interfaces
//------------------------------------------------------------
// forward declare interfaces
class basic_stream;
class output_stream;
class input_stream;
class serializable;

// for rewinding
static const int SZ_B		= sizeof(byte);
static const int SZ_BL		= sizeof(byte) + sizeof(int);
static const int SZ_BLL		= sizeof(byte) + sizeof(int) + sizeof(int);
#define REWIND(cb)	seekPos(-(cb))

//------------------------------------------------------------
// The serializable interface - to be implemented by objects
// desiring serialization support from our streams.
//------------------------------------------------------------
class serializable
{
public:
	virtual bool writeObject(output_stream*) = 0;
	virtual bool readObject (input_stream* ) = 0;
};

//------------------------------------------------------------
// The basic_stream interface -
// must be implemented by all types of streams
//------------------------------------------------------------
class basic_stream
{
public:
	enum data_type
	{
		dt_start = 0,
		_bof,
		dt_null,
		dt_char,
		dt_bool,
		dt_short,
		dt_int,
		dt_long,
		dt_int64,
		dt_float,
		dt_double,
		dt_uuid,
		dt_array,
		dt_cstring,
		dt_wstring,
		dt_bstr,
		dt_object,
		dt_blob,
		// may add something in the future here
		_eof = 254,
		dt_end = 255
	};

	static const char* data_type_desc[];

public:
	//----------------------------------------------------------
	// These functions are specific to each stream implementation
	// and thus have to be overridden separately by each class
	//----------------------------------------------------------
	virtual ~basic_stream()					{  }
	virtual bool  reset  ()					= 0;
	// Current position in stream relative to beginning of its user portion,
	// NOT including any implementation-specific headers
	virtual long peekPos()	const			= 0;
	// Seek position cb bytes from offset;
	// ofs_begin is from the usable portion of the stream's start, not including headers,
	// ofs_end is from the usable portion of the stream's end, not including end markers
	virtual bool seekPos(int cb, seek_offset from = ofs_current)	= 0;
	virtual long sizeOfData () const		= 0;
	virtual long sizeOfStream () const		= 0;
	virtual bool atBOF  () const			= 0;
	virtual bool atEOF  () const			= 0;
	virtual bool close  ()					= 0;
};


//------------------------------------------------------------
// The output_stream class/interface -
// must be implemented by all output streams
//------------------------------------------------------------
class output_stream : public basic_stream
{
public:
	//----------------------------------------------------------
	// These functions are implemented by output_stream itself
	// since they do not change their low-level functionality
	// depends on the two functions below (_writeRaw & _writeAtPos)
	//----------------------------------------------------------
	virtual bool writeChar	 (char charVal);
	virtual bool writeBool	 (bool boolBal);
	virtual bool writeShort	 (short shortVal);
	virtual bool writeInt	 (int intVal);
	virtual bool writeLong	 (long longVal);
	virtual bool writeInt64	 (__int64 int64Val);
	virtual bool writeFloat	 (float fltVal);
	virtual bool writeDouble (double dblVal);
	virtual bool writeUUID	 (const UUID& rUUID);
	virtual bool writeArray	 (const void* arr, int elSize, int elCnt);
	virtual bool writeCString(const char* lpStr);
	virtual bool writeWString(const wchar_t* lpWstr);
	virtual bool writeBSTR	 (const BSTR bstr);
	virtual bool writeObject (serializable* pObj);
	virtual bool writeBLOB	 (const void* pBlob, int byteCount);

protected:
	virtual bool _writeType	 (data_type dt);

	//----------------------------------------------------------
	// These functions are the ones on which all the writeXXX
	// functions rely to do their work and have to be overridden
	// by each class since they are implementation-dependent
	//----------------------------------------------------------
	
	// This func writes and adjusts current position in the stream
	virtual bool _writeRaw	 (const void* p, int cb)					= 0;
	// This func does not affect stream position or size
	// but simply writes bytes to the specified offset from
	// stream beginning (i.e., from user data portion of stream,
	// NOT including any implementation-specific headers)
	virtual bool _writeAtPos (int begOfs, const void* p, int cb)	= 0;
};


//------------------------------------------------------------
// The input_stream interface -
// must be implemented by all input streams
//------------------------------------------------------------
class input_stream : public basic_stream
{
public:
	virtual bool readChar	(char& rChar);
	virtual bool readBool	(bool& rBool);
	virtual bool readShort	(short& rShort);
	virtual bool readInt	(int& rInt);
	virtual bool readLong	(long& rLng);
	virtual bool readInt64	(__int64& rInt64);
	virtual bool readFloat	(float& rFlt);
	virtual bool readDouble	(double& rDbl);
	virtual bool readUUID	(UUID& rUUID);
	virtual bool readArray	(void* arr, int elSize, int elCnt);
	virtual bool readCString(char* lpStr, int* pCharCount);
	virtual bool readWString(wchar_t* lpWstr, int* pCharCount);
	virtual bool readBSTR	(BSTR* lpBSTR);
	virtual bool readObject	(serializable*& pObj);
	virtual bool readObject	(serializable&  rObj);
	virtual bool readBLOB	(void* pBlob, int* pByteCount);
	// These functions are used to look ahead
	// by the readXXX ones and by users
	virtual data_type nextType() const;
	// Returns the size (in bytes) of the next read; for strings includes the terminating null
	virtual int		  nextSize() const;

protected:
	virtual bool	  _checkType(data_type dt);
	virtual data_type _readType ();
	
	//----------------------------------------------------------
	// These functions are the ones on which all the readXXX
	// functions rely to do their work and have to be overridden
	// by each class since they are implementation-dependent
	//----------------------------------------------------------
	
	// This func reads and adjusts current position in the stream
	virtual bool _readRaw  (void* p, int cb)					= 0;
	// This func does not affect stream position or size
	// but simply reads bytes at the specified offset from
	// stream beginning (i.e., from user data portion of stream,
	// NOT including any implementation-specific headers)
	virtual bool _readAtPos(long begOfs, void* p, int cb) const	= 0;
};


};	// namespace soige

#endif  // __base_stream_already_included_vasya__

