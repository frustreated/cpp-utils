//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _input_stream_.cpp - implementation file
// for the base input stream class.
//
// The stream is guaranteed to remain in consistent state
// (same as before) if an invalid/failed read occurs.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_base_stream_.h"

namespace soige {

// read helper for built-ins
#define READ_TYPE(_t, _v)	\
		if(!_checkType(_t)) return false;\
		if(!_readRaw(&_v, sizeof(_v))) { REWIND(SZ_B); return false; }\
		return true;

//------------------------------------------------------------
// input_stream
//------------------------------------------------------------

bool input_stream::readChar(char& retval)
{
	READ_TYPE(dt_char, retval)
}

bool input_stream::readBool(bool& retval)
{
	READ_TYPE(dt_bool, retval)
}

bool input_stream::readShort(short& retval)
{
	READ_TYPE(dt_short, retval)
}

bool input_stream::readInt(int& retval)
{
	READ_TYPE(dt_int, retval)
}

bool input_stream::readLong(long& retval)
{
	READ_TYPE(dt_long, retval)
}

bool input_stream::readInt64(__int64& retval)
{
	READ_TYPE(dt_int64, retval)
}

bool input_stream::readFloat(float& retval)
{
	READ_TYPE(dt_float, retval)
}

bool input_stream::readDouble(double& retval)
{
	READ_TYPE(dt_double, retval)
}

bool input_stream::readUUID(UUID& rUUID)
{
	READ_TYPE(dt_uuid, rUUID)
}

bool input_stream::readArray(void* array, int elemSize, int elemCount)
{
	if( elemSize < 0 || elemCount < 0 || !array )
		return false;
	
	int size, count;
	if( !_checkType(dt_array) ) return false;
	if( !_readRaw(&size, sizeof(size)) ) { REWIND(SZ_B); return false; }
	if( !_readRaw(&count, sizeof(count)) ) { REWIND(SZ_BL); return false; }
	if(elemSize != size || elemCount != count)
	{
		REWIND(SZ_BLL);
		return false;
	}
	if( !_readRaw(array, size*count) )  { REWIND(SZ_BLL); return false; }
	return true;
}

// *pCharCount contains the number of chars in the buffer
// pointed to by lpStr on entry (must be large enough to
// include the terminating NULL), and the number of characters
// read (including the terminating NULL char) on return.
// If the buffer wasn't large enough, *pCharCount tells the number
// of chars required for successful read and returns false.
bool input_stream::readCString(char* lpStr, int* pCharCount)
{
	if( !pCharCount )
		return false;
	int len;
	if( !_checkType(dt_cstring) ) return false;
	if( !_readRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
	if( 0 == len && nextType() == dt_null )
	{
		_readType();
		*pCharCount = 0;
		return true;
	}

	if(*pCharCount < len+1)
	{
		*pCharCount = len+1;
		REWIND(SZ_BL);
		return false;
	}
	*pCharCount = len+1;
	if(NULL == lpStr)
	{
		REWIND(SZ_BL);
		return false;
	}
	if( !_readRaw(lpStr, (len+1)*sizeof(char)) ) { REWIND(SZ_BL); return false; }
	return true;
}

/*
bool input_stream::readCString(char*& lpStr, int* pCharCount, bool streamAllocd)
{
	if(NULL == lpStr && !streamAllocd)
		return false;
	int len;
	if( !_checkType(dt_cstring) ) return false;
	if( !_readRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
	if(0 == len && nextType() == dt_null)
	{
		_readType();
		if(streamAllocd) lpStr = NULL;
		*pCharCount = 0;
		return true;
	}

	if( !streamAllocd && *pCharCount<len+1 )
	{
		*pCharCount = len+1;
		REWIND(SZ_BL);
		return false;
	}
	*pCharCount = len+1;
	if(streamAllocd) lpStr = (char*) malloc((len+1)*sizeof(char));
	if( !_readRaw(lpStr, (len+1)*sizeof(char)) ) { REWIND(SZ_BL); return false; }
	return true;
}
*/

// *pCharCount contains the number of chars in the buffer
// pointed to by lpStr on entry (must be large enough to
// include the terminating NULL), and the number of characters
// read (including the terminating NULL char) on return.
// If the buffer wasn't large enough, *pCharCount tells the number
// of chars required for successful read and returns false.
bool input_stream::readWString(wchar_t* lpWstr, int* pCharCount)
{
	if( !pCharCount )
		return false;
	int len;
	if( !_checkType(dt_wstring) ) return false;
	if( !_readRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
	if( 0 == len && nextType() == dt_null )
	{
		_readType();
		*pCharCount = 0;
		return true;
	}

	if(*pCharCount < len+1)
	{
		*pCharCount = len+1;
		REWIND(SZ_BL);
		return false;
	}
	*pCharCount = len+1;
	if(NULL == lpWstr)
	{
		REWIND(SZ_BL);
		return false;
	}
	if( !_readRaw(lpWstr, (len+1)*sizeof(wchar_t)) ) { REWIND(SZ_BL); return false; }
	return true;
}

bool input_stream::readBSTR(BSTR* lpBSTR)
{
	if( !lpBSTR )
		return false;
	int len;
	if( !_checkType(dt_bstr) ) return false;
	if( !_readRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
	if( 0 == len && nextType() == dt_null )
	{
		_readType();
		*lpBSTR = NULL;
		return true;
	}

	wchar_t* buf = new wchar_t[len+1];
	if( !_readRaw(buf, (len+1)*sizeof(wchar_t)) )
	{
		REWIND(SZ_BL);
		return false;
	}
	*lpBSTR = SysAllocString(buf);
	delete[] buf;
	return (*lpBSTR != NULL);
}

bool input_stream::readObject(serializable*& pObj)
{
	long pos = peekPos();
	int len;
	if( !_checkType(dt_object) ) return false;
	if( !_readRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
	if( 0 == len && nextType() == dt_null )
	{
		_readType();
		pObj = NULL;
		return true;
	}

	if(NULL == pObj)
	{
		REWIND(SZ_BL);
		return false;
	}

	if( !pObj->readObject(this) ) { REWIND(SZ_BL); return false; }
	if( (peekPos() - pos - SZ_BL) != len )
	{
		REWIND(peekPos()-pos);
		return false;
	}

	return true;
}

bool input_stream::readObject(serializable& rObj)
{
	long pos = peekPos();
	serializable* p = &rObj;
	if( !readObject(p) ) return false;
	if(NULL == p)
	{
		REWIND(peekPos()-pos);
		return false;
	}
	return true;
}

bool input_stream::readBLOB(void* pBlob, int* pByteCount)
{
	if( !pBlob || !pByteCount )
		return false;

	int len;
	if( !_checkType(dt_blob) ) return false;
	if( !_readRaw(&len, sizeof(len)) ) { REWIND(SZ_B); return false; }
	if(*pByteCount < len)
	{
		*pByteCount = len;
		REWIND(SZ_BL);
		return false;
	}
	*pByteCount = len;
	if( !_readRaw(pBlob, len) ) { REWIND(SZ_BL); return false; }
	return true;
}

// input_stream overrides
basic_stream::data_type input_stream::nextType() const
{
	byte b = 0;
	_readAtPos( peekPos(), &b, sizeof(b) );
	return ((data_type) b);
}

int input_stream::nextSize() const
{
	int size = 0;
	int count = 0;
	byte dt;

	switch( nextType() )
	{
	case dt_char:		return sizeof(char);
	case dt_bool:		return sizeof(bool);
	case dt_short:		return sizeof(short);
	case dt_int:		return sizeof(int);
	case dt_long:		return sizeof(long);
	case dt_int64:		return sizeof(__int64);
	case dt_float:		return sizeof(float);
	case dt_double:		return sizeof(double);
	case dt_uuid:		return sizeof(UUID);
	case dt_array:
		_readAtPos ( peekPos() + SZ_B, &size, sizeof(size) );
		_readAtPos ( peekPos() + SZ_BL, &count, sizeof(count) );
		return size*count;
	case dt_cstring:
		_readAtPos ( peekPos() + SZ_B, &size, sizeof(size) );
		_readAtPos ( peekPos() + SZ_BL, &dt, sizeof(dt) );
		if( 0 == size && ((data_type)dt) == dt_null ) return 0;
		return (size+1)*sizeof(char);
	case dt_wstring:
	case dt_bstr:
		_readAtPos ( peekPos() + SZ_B, &size, sizeof(size) );
		_readAtPos ( peekPos() + SZ_BL, &dt, sizeof(dt) );
		if( 0 == size && ((data_type)dt) == dt_null ) return 0;
		return (size+1)*sizeof(wchar_t);
	case dt_object:
		_readAtPos ( peekPos() + SZ_B, &size, sizeof(size) );
		_readAtPos ( peekPos() + SZ_BL, &dt, sizeof(dt) );
		if( 0 == size && ((data_type)dt) == dt_null ) return 0;
		return size;
	case dt_blob:
		_readAtPos ( peekPos() + SZ_B, &size, sizeof(size) );
		return size;
	}

	return 0;
}

bool input_stream::_checkType(data_type dt)
{
	if(nextType() == dt)
	{
		_readType();
		return true;
	}
	else
		return false;
}

basic_stream::data_type input_stream::_readType()
{
	byte b = 0;
	_readRaw( &b, sizeof(b) );
	return ((data_type) b);
}


};	// namespace soige

