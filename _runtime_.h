//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _runtime_.h - header file for runtime memory functions
// to avoid depending on C runtime lib/code.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __runtime_already_included_vasya__
#define __runtime_already_included_vasya__

#include <wtypes.h>
#include <winbase.h>

struct _MEM_STRUCT
{
	HANDLE	m_hHeap;	// The main heap
	DWORD	m_dwHeaps;	// Number of heaps we have (-1)
	HANDLE*	m_phHeaps;
	int		m_nHeap;	// Which heap to choose from
	
	_MEM_STRUCT();
	~_MEM_STRUCT();

	static const int snExtraAlloc;
	static const int snOffsetBlock;
};

extern _MEM_STRUCT _gMEM_STRUCT;

#ifdef __cplusplus
extern "C" {
#endif

// memmove
void* __cdecl memmove( void* dest, const void* src, size_t count );

//////////////////////////////////////////////////////////////
// Heap Allocation
void* __cdecl malloc	(size_t n);
void* __cdecl calloc	(size_t n, size_t s);
void* __cdecl realloc	(void* p, size_t n);
void  __cdecl free		(void* p);

#ifdef __cplusplus
}  // extern "C"
#endif

void* __cdecl operator new	 (size_t n);
void  __cdecl operator delete(void* p);


#endif  // __runtime_already_included_vasya__

