//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// output_stream.cpp - implementation file
// for the base output stream class.
//
// The stream is not always guaranteed to remain in consistent
// state (same as before) if an invalid/failed write occurs;
// for example, for file streams, if stream source is stdout,
// you cannot position the stream pointer backwards in stream.
// However, if rewinding is possible, then stream consistency
// is guaranteed.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_base_stream_.h"

namespace soige {

// write helper for built-ins
#define WRITE_TYPE(_t, _v)	\
	if(!_writeType(_t)) return false;\
	if(!_writeRaw(&_v, sizeof(_v))) { REWIND(SZ_B); return false; }\
	return true;

//------------------------------------------------------------
// output_stream
//------------------------------------------------------------

// output_stream implementation
bool output_stream::writeChar(char charVal)
{
	WRITE_TYPE(dt_char, charVal)
}

bool output_stream::writeBool(bool boolVal)
{
	WRITE_TYPE(dt_bool, boolVal)
}

bool output_stream::writeShort(short shortVal)
{
	WRITE_TYPE(dt_short, shortVal)
}

bool output_stream::writeInt(int intVal)
{
	WRITE_TYPE(dt_int, intVal)
}

bool output_stream::writeLong(long longVal)
{
	WRITE_TYPE(dt_long, longVal)
}

bool output_stream::writeInt64(__int64 int64Val)
{
	WRITE_TYPE(dt_int64, int64Val)
}

bool output_stream::writeFloat(float fltVal)
{
	WRITE_TYPE(dt_float, fltVal)
}

bool output_stream::writeDouble(double dblVal)
{
	WRITE_TYPE(dt_double, dblVal)
}

bool output_stream::writeUUID(const UUID& rUUID)
{
	WRITE_TYPE(dt_uuid, rUUID)
}

bool output_stream::writeArray(const void* array, int elemSize, int elemCount)
{
	if( elemSize < 0 || elemCount < 0 || !array )
		return false;
	if( !_writeType(dt_array) ) return false;
	if( !_writeRaw(&elemSize, sizeof(elemSize)) ) { REWIND(SZ_B); return false; }
	if( !_writeRaw(&elemCount, sizeof(elemCount)) ) { REWIND(SZ_BL); return false; }
	if( !_writeRaw(array, elemSize*elemCount) ) { REWIND(SZ_BLL); return false; }
	return true;
}

bool output_stream::writeCString(const char* lpStr)
{
	int len = 0;
	if( !_writeType(dt_cstring) ) return false;
	if(lpStr == NULL)
	{
		if( !_writeRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
		if( !_writeType(dt_null) ) { REWIND(SZ_BL); return false; }
	}
	else
	{
		len = lstrlenA(lpStr);
		if( !_writeRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
		if( !_writeRaw(lpStr, (len+1)*sizeof(char)) ) { REWIND(SZ_BL); return false; }
	}
	return true;
}

bool output_stream::writeWString(const wchar_t* lpWstr)
{
	int len = 0;
	if( !_writeType(dt_wstring) ) return false;
	if(lpWstr == NULL)
	{
		if( !_writeRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
		if( !_writeType(dt_null) ) { REWIND(SZ_BL); return false; }
	}
	else
	{
		len = lstrlenW(lpWstr);
		if( !_writeRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
		if( !_writeRaw(lpWstr, (len+1)*sizeof(wchar_t)) ) { REWIND(SZ_BL); return false; }
	}
	return true;
}

bool output_stream::writeBSTR(const BSTR bstr)
{
	int len = 0;
	if( !_writeType(dt_bstr) ) return false;
	if(bstr == NULL)
	{
		if( !_writeRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
		if( !_writeType(dt_null) ) { REWIND(SZ_BL); return false; }
	}
	else
	{
		len = SysStringLen(bstr);
		if( !_writeRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
		if( !_writeRaw(bstr, (len+1)*sizeof(wchar_t)) ) { REWIND(SZ_BL); return false; }
	}
	return true;
}

bool output_stream::writeObject(serializable* pObj)
{
	int len = 0;
	if( !_writeType(dt_object) ) return false;
	long pos = peekPos();
	if( !_writeRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
	if(pObj == NULL)
	{
		if( !_writeType(dt_null) ) { REWIND(SZ_BL); return false; }
		return true;
	}
	else
	{
		if( !pObj->writeObject(this) ) { REWIND(peekPos()-pos+SZ_B); return false; }
		len = (peekPos() - pos) - sizeof(len);
		_writeAtPos( pos, &len, sizeof(len) );
	}
	return true;
}

bool output_stream::writeBLOB(const void* pBlob, int byteCount)
{
	if( byteCount < 0 || !pBlob )
		return false;
	if( !_writeType(dt_blob) ) return false;
	if( !_writeRaw(&byteCount, sizeof(byteCount)) ) { REWIND(SZ_B); return false; }
	if( !_writeRaw(pBlob, sizeof(byteCount)) ) { REWIND(SZ_BL); return false; }
	return true;
}

bool output_stream::_writeType(data_type dt)
{
	byte b = (byte)dt;
	return _writeRaw( &b, sizeof(b) );
}


};	// namespace soige

