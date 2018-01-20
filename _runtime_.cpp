//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _runtime_.cpp - impl file for runtime memory functions
// to avoid depending on C runtime lib/code.
//
// malloc/realloc/calloc/free stolen from ATL.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_runtime_.h"

#if defined(_USE_NO_CRT) && !defined(_DEBUG) && !defined(DEBUG)

_MEM_STRUCT _gMEM_STRUCT;

const int _MEM_STRUCT::snExtraAlloc = 8;
const int _MEM_STRUCT::snOffsetBlock = snExtraAlloc/sizeof(HANDLE);

_MEM_STRUCT::_MEM_STRUCT()
{
	m_dwHeaps = 0;
	m_nHeap = 0;
	m_phHeaps = NULL;
	m_hHeap = HeapCreate(0, 0, 0);

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	if(si.dwNumberOfProcessors > 1)
	{
		DWORD dwHeaps = si.dwNumberOfProcessors * 2;
		m_dwHeaps = 0xFFFFFFFF;
		for(int bits = 0; bits < 32; bits++)
		{
			if(dwHeaps & 0x80000000)
				break;
			dwHeaps <<= 1;
			m_dwHeaps >>= 1;
		}
		m_dwHeaps >>= 1;

		// Allocate more heaps for each processor
		m_phHeaps = (HANDLE*) HeapAlloc(m_hHeap, 0, sizeof(HANDLE) * (m_dwHeaps + 1));
		for(DWORD i = 0; i <= m_dwHeaps; i++)
			m_phHeaps[i] = HeapCreate(0, 0, 0);
	}
	else
	{
		m_phHeaps = NULL;
		m_dwHeaps = 0;
	}
}

_MEM_STRUCT::~_MEM_STRUCT()
{
	if(m_hHeap)
	{
		if(m_phHeaps != NULL)
		{
			for(DWORD i = 0; i <= m_dwHeaps; i++)
				HeapDestroy(m_phHeaps[i]);
		}
		HeapDestroy(m_hHeap);
	}
}

#ifdef __cplusplus
extern "C" {
#endif

extern inline void* __cdecl memmove( void *dest, const void *src, size_t count )
	{ MoveMemory( dest, src, count ); }

void* __cdecl malloc(size_t n)
{
	void* pv = NULL;
	if(_gMEM_STRUCT.m_phHeaps == NULL)
	{
		pv = (HANDLE*) HeapAlloc(_gMEM_STRUCT.m_hHeap, 0, n);
	}
	else
	{
		// overallocate to remember the heap handle
		int nHeap = _gMEM_STRUCT.m_nHeap++;
		HANDLE hHeap = _gMEM_STRUCT.m_phHeaps[nHeap & _gMEM_STRUCT.m_dwHeaps];
		HANDLE* pBlock = (HANDLE*) HeapAlloc(hHeap, 0, n + _MEM_STRUCT::snExtraAlloc);
		if(pBlock != NULL)
		{
			*pBlock = hHeap;
			pv = (void*)(pBlock + _MEM_STRUCT::snOffsetBlock);
		}
		else
			pv = NULL;
	}
	return pv;
}

void* __cdecl calloc(size_t n, size_t s)
{
	return malloc(n*s);
}

void* __cdecl realloc(void* p, size_t n)
{
	if(p == NULL)
		return malloc(n);
	if(_gMEM_STRUCT.m_phHeaps == NULL)
		return HeapReAlloc(_gMEM_STRUCT.m_hHeap, 0, p, n);
	else
	{
		HANDLE* pHeap = ((HANDLE*)p)-_MEM_STRUCT::snOffsetBlock;
		pHeap = (HANDLE*) HeapReAlloc(*pHeap, 0, pHeap, n + _MEM_STRUCT::snExtraAlloc);
		return (pHeap != NULL) ? pHeap + _MEM_STRUCT::snOffsetBlock : NULL;
	}
}

void __cdecl free(void* p)
{
	if(_gMEM_STRUCT.m_phHeaps == NULL)
		HeapFree(_gMEM_STRUCT.m_hHeap, 0, p);
	else
	{
		HANDLE* pHeap = ((HANDLE*)p)-_MEM_STRUCT::snOffsetBlock;
		HeapFree(*pHeap, 0, pHeap);
	}
}

#ifdef __cplusplus
}  // extern "C"
#endif

extern inline void* __cdecl operator new(size_t n)		{ return malloc(n); }
extern inline void __cdecl operator delete(void* p)		{ free(p); }

#endif	// _USE_NO_CRT

