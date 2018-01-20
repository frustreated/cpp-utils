//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _common_.h - definitions of things common to all my classes
//
// Item comparison depends on operators == and < being
// defined for the item's class (used by _compare template).
// Also, items are passed to the _destroyN() func via their
// address, so be careful when overloading operator& in the
// item's class.
// Item copying relies on operator=.
//
// So for user-defined types you wish to use as template
// params to my classes, define, as a minimum, operators
// =, ==, and <.
// Also, as a general rule of GPP and to prevent mysterious
// errors, classes should implement copy constructors, and
// destructors should not just destroy dynamically allocated
// data, but set member variables to "clean" values (i.e.,
// pointer vars should be null'ed, etc.).
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __common_already_included_vasya__
#define __common_already_included_vasya__

//#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define VC_EXTRALEAN

#if defined(_USE_NO_CRT) && !defined(_DEBUG) && !defined(DEBUG)
// no C runtime wanted; import our own implementation
	#include "_runtime_.h"
#else
	#include <memory.h>
#endif

#include <new>
#include <exception>
#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>

namespace soige {

// maximum positive value of int - used in string classes
#define MAX_INT  0x7FFFFFFF

//------------------------------------------------------------
// common stuff
typedef unsigned char byte;
//------------------------------------------------------------

// A/W to T mappings
#if defined(UNICODE) || defined(_UNICODE)
	#define istalpha(a)		iswalpha(a)
	#define istdigit(a)		iswdigit(a)
	#define totlower(a)		towlower(a)
	#define totupper(a)		towupper(a)
#else
	#define istalpha(a)		isalpha(a)
	#define istdigit(a)		isdigit(a)
	#define totlower(a)		tolower(a)
	#define totupper(a)		toupper(a)
#endif

//------------------------------------------------------------
// Trace support in debugging
#if defined(_DEBUG) || defined(_DEBUG)
inline void STRACE(LPCTSTR lpszFormat, ...)
{
	va_list args;
	va_start(args, lpszFormat);
	TCHAR szBuffer[512];
	wvsprintf(szBuffer, lpszFormat, args);
	OutputDebugString(szBuffer);
	va_end(args);
}

#define STRACE_FULL \
	{TCHAR _s_z_[32]; \
	wsprintf(_s_z_, " (%d): ", __LINE__); \
	OutputDebugString(__FILE__); \
	OutputDebugString(_s_z_);} \
	STRACE

#endif
//------------------------------------------------------------

//------------------------------------------------------------
// Misc constants
// file and stream seek offsets
typedef int seek_offset;

#define ofs_begin		FILE_BEGIN
#define ofs_current		FILE_CURRENT
#define ofs_end			FILE_END


// file attributes;
// used by _file_finder_ and _win32_file_
#define attrib_none		0
#define archive			FILE_ATTRIBUTE_ARCHIVE
#define compressed		FILE_ATTRIBUTE_COMPRESSED
#define directory		FILE_ATTRIBUTE_DIRECTORY
#define encrypted		FILE_ATTRIBUTE_ENCRYPTED
#define hidden			FILE_ATTRIBUTE_HIDDEN
#define normal			FILE_ATTRIBUTE_NORMAL
#define readonly		FILE_ATTRIBUTE_READONLY
#define attrib_system	FILE_ATTRIBUTE_SYSTEM
#define attrib_temp		FILE_ATTRIBUTE_TEMPORARY
#define regfiles		(FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_NORMAL)
#define allfiles		((FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED |	\
							FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_HIDDEN |	\
							FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY |	\
							FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY) ^ \
							FILE_ATTRIBUTE_DIRECTORY)
#define everything		0xffffffff
		//(FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_COMPRESSED |
		//FILE_ATTRIBUTE_ENCRYPTED | FILE_ATTRIBUTE_HIDDEN |
		//FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY |
		//FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY |
		//FILE_ATTRIBUTE_DIRECTORY)

// file access mode;
// used by _win32_file_
#define access_query	0
#define access_read		GENERIC_READ
#define access_write	GENERIC_WRITE
#define access_all		(GENERIC_READ | GENERIC_WRITE)


// file share mode;
// used by _win32_file_
#define share_none		0
		// exclusive access
#define share_read		FILE_SHARE_READ
#define share_write		FILE_SHARE_WRITE
#define share_del		FILE_SHARE_DELETE
#define share_all		(FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE)


// file open/creation mode;
// used by _win32_file_
#define create_new		CREATE_NEW
#define create_always	CREATE_ALWAYS
#define open_existing	OPEN_EXISTING
#define open_always		OPEN_ALWAYS
#define trunc_existing	TRUNCATE_EXISTING


// misc file flags;
// used by _win32_file_
#define flags_none		0
#define del_on_close	FILE_FLAG_DELETE_ON_CLOSE
#define seq_scan		FILE_FLAG_SEQUENTIAL_SCAN
#define rand_access		FILE_FLAG_RANDOM_ACCESS
#define write_thru		FILE_FLAG_WRITE_THROUGH
//------------------------------------------------------------


//------------------------------------------------------------
// My quotes! the quotes are mine!
// Creating objects (uninitialized)
template<typename T> inline void _createN(T* dest, long elemCount)
	{ for(long i=0; i<elemCount; i++)  new(&dest[i]) T(); }
template<> inline void _createN<char>(char* dest, long elemCount)
	{ return; }
template<> inline void _createN<byte>(byte* dest, long elemCount)
	{ return; }
template<> inline void _createN<short>(short* dest, long elemCount)
	{ return; }
template<> inline void _createN<unsigned short>(unsigned short* dest, long elemCount)
	{ return; }
template<> inline void _createN<int>(int* dest, long elemCount)
	{ return; }
template<> inline void _createN<unsigned int>(unsigned int* dest, long elemCount)
	{ return; }
template<> inline void _createN<long>(long* dest, long elemCount)
	{ return; }
template<> inline void _createN<unsigned long>(unsigned long* dest, long elemCount)
	{ return; }
template<> inline void _createN<float>(float* dest, long elemCount)
	{ return; }
template<> inline void _createN<double>(double* dest, long elemCount)
	{ return; }
// Constructing objects; use in-place construction
template<typename T> inline void _constructN(T* dest, const T* src, long elemCount)
	{ for(long i=0; i<elemCount; i++)  new(&dest[i]) T(src[i]); }
template<> inline void _constructN<char>(char* dest, const char* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(char)); }
template<> inline void _constructN<byte>(byte* dest, const byte* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(byte)); }
template<> inline void _constructN<short>(short* dest, const short* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(short)); }
template<> inline void _constructN<unsigned short>(unsigned short* dest, const unsigned short* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(short)); }
template<> inline void _constructN<int>(int* dest, const int* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(int)); }
template<> inline void _constructN<unsigned int>(unsigned int* dest, const unsigned int* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(int)); }
template<> inline void _constructN<long>(long* dest, const long* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(long)); }
template<> inline void _constructN<unsigned long>(unsigned long* dest, const unsigned long* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(long)); }
template<> inline void _constructN<float>(float* dest, const float* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(float)); }
template<> inline void _constructN<double>(double* dest, const double* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(double)); }
// Destroying objects
template<typename T> inline void _destroyN(T* p, long elemCount)
	{ const T* const end = p + elemCount; for(; p != end; ++p) p->T::~T(); }
template<> inline void _destroyN<char>(char* p, long elemCount) { return; }
template<> inline void _destroyN<byte>(byte* p, long elemCount) { return; }
template<> inline void _destroyN<short>(short* p, long elemCount) { return; }
template<> inline void _destroyN<unsigned short>(unsigned short* p, long elemCount) { return; }
template<> inline void _destroyN<int>(int* p, long elemCount) { return; }
template<> inline void _destroyN<unsigned int>(unsigned int* p, long elemCount) { return; }
template<> inline void _destroyN<long>(long* p, long elemCount) { return; }
template<> inline void _destroyN<unsigned long>(unsigned long* p, long elemCount) { return; }
template<> inline void _destroyN<float>(float* p, long elemCount) { return; }
template<> inline void _destroyN<double>(double* p, long elemCount) { return; }
// Comparing objects
template<typename T> inline int _compare(const T& a, const T& b)
	{ if(a == b) return 0; else if(a < b) return -1; else return 1; }
template<> inline int _compare<short>(const short& a, const short& b)
	{ return (a - b); }
template<> inline int _compare<unsigned short>(const unsigned short& a, const unsigned short& b)
	{ return (int)(a - b); }
template<> inline int _compare<int>(const int& a, const int& b)
	{ return (a - b); }
template<> inline int _compare<unsigned int>(const unsigned int& a, const unsigned int& b)
	{ return (int)(a - b); }
template<> inline int _compare<long>(const long& a, const long& b)
	{ return (int)(a - b); }
template<> inline int _compare<unsigned long>(const unsigned long& a, const unsigned long& b)
	{ return (int)(a - b); }
template<> inline int _compare<float>(const float& a, const float& b)
	{ return (int)(a - b); }
template<> inline int _compare<double>(const double& a, const double& b)
	{ return (int)(a - b); }
template<> inline int _compare<LPCTSTR>(const LPTSTR& a, const LPTSTR& b)
	{ return lstrcmp(a, b); }
template<> inline int _compare<LPTSTR>(const LPTSTR& a, const LPTSTR& b)
	{ return lstrcmp(a, b); }
// Copying objects
template<typename T> inline void _copyN(T* dest, const T* src, long elemCount)
	{ for(long i=0; i<elemCount; i++) dest[i] = src[i]; }
template<> inline void _copyN<char>(char* dest, const char* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(char)); }
template<> inline void _copyN<byte>(byte* dest, const byte* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(byte)); }
template<> inline void _copyN<short>(short* dest, const short* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(short)); }
template<> inline void _copyN<unsigned short>(unsigned short* dest, const unsigned short* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(short)); }
template<> inline void _copyN<int>(int* dest, const int* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(int)); }
template<> inline void _copyN<unsigned int>(unsigned int* dest, const unsigned int* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(int)); }
template<> inline void _copyN<long>(long* dest, const long* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(long)); }
template<> inline void _copyN<unsigned long>(unsigned long* dest, const unsigned long* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(long)); }
template<> inline void _copyN<float>(float* dest, const float* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(float)); }
template<> inline void _copyN<double>(double* dest, const double* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(double)); }
// Reverse copying objects
template<typename T> inline void _reverseCopyN(T* dest, const T* src, long elemCount)
	{ for(long i=elemCount-1; i>=0; i--) dest[i] = src[i]; }
template<> inline void _reverseCopyN<char>(char* dest, const char* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(char)); }
template<> inline void _reverseCopyN<byte>(byte* dest, const byte* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(byte)); }
template<> inline void _reverseCopyN<short>(short* dest, const short* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(short)); }
template<> inline void _reverseCopyN<unsigned short>(unsigned short* dest, const unsigned short* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(short)); }
template<> inline void _reverseCopyN<int>(int* dest, const int* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(int)); }
template<> inline void _reverseCopyN<unsigned int>(unsigned int* dest, const unsigned int* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(int)); }
template<> inline void _reverseCopyN<long>(long* dest, const long* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(long)); }
template<> inline void _reverseCopyN<unsigned long>(unsigned long* dest, const unsigned long* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(long)); }
template<> inline void _reverseCopyN<float>(float* dest, const float* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(float)); }
template<> inline void _reverseCopyN<double>(double* dest, const double* src, long elemCount)
	{ memmove(dest, src, elemCount*sizeof(double)); }
// Swapping objects
template<typename T> inline void _swap(T* a, T* b)
	{ T t = *a; *a = *b; *b = t; }
//------------------------------------------------------------


};	// namespace soige

#endif // __common_already_included_vasya__

