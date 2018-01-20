//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _base64_.h - header file for the base64 class.
//
// Encapsulates Base64 (RFC 1113) encoding/decoding.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __base64_already_included_vasya__
#define __base64_already_included_vasya__

#include "_common_.h"

namespace soige {

class _base64_
{
public:
	static bool encode( /* in */  const BYTE* bufIn,
						/* in */  DWORD  nBytesIn,
						/* out */ char** pbufEncoded,
						/* out */ DWORD* pcbEncoded );
	
	static bool decode( /* in */  const char* bufCoded,
						/* out */ BYTE** pbufDecoded,
						/* out */ DWORD* pcbDecoded );
};


};	// namespace soige

#endif  // __base64_already_included_vasya__

