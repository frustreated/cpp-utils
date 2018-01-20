//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _base64_.cpp - implementation file for the rfc 1113.
//
// Source taken from Valery Pryamikov's utils:
//  http://home.online.no/~valery/
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_base64_.h"

namespace soige {

//------------------------------------------------------------
//  Algorithm taken from NCSA HTTP and wwwlib.
//
//  NOTE: These conform to RFC1113, which is slightly
//        from the Unix uuencode and uudecode.
//------------------------------------------------------------

const int pr2six[256] = {
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
	52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,64,0,1,2,3,4,5,6,7,8,9,
	10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,64,26,27,
	28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
	64,64,64,64,64,64,64,64,64,64,64,64,64
};

const char six2pr[64] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m',
	'n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9','+','/'
};


//------------------------------------------------------------
// Base64-encode a [binary] string of data.
//
// Arguments:
//    bufIn			pointer to data to encode.
//    nBytesIn		number of bytes to encode.
//    pbufEncoded	pointer to pointer to a char.
//					Must be free()'d by the caller.
//    pcbEncoded	number of bytes alloc'ed in *pbufEncoded.
//
// Returns true if successful; otherwise false.
//------------------------------------------------------------
/* static */
bool _base64_::encode ( /* in */  const BYTE* bufIn,
						/* in */  DWORD  nBytesIn,
						/* out */ char** pbufEncoded,
						/* out */ DWORD* pcbEncoded )
{
	BYTE* outptr;
	unsigned int i;

	// Resize the buffer to 133% of the incoming data
	*pcbEncoded = nBytesIn + ((nBytesIn + 3) / 3) + 4;
	*pbufEncoded = (char*) malloc( *pcbEncoded );
	if( !*pbufEncoded )
		return false;

	outptr = (BYTE*) *pbufEncoded;

	for(i=0; i<nBytesIn; i += 3)
	{
		*(outptr++) = six2pr[*bufIn >> 2];		/* c1 */
		*(outptr++) = six2pr[((*bufIn << 4) & 060) | ((bufIn[1] >> 4) & 017)];  /*c2*/
		*(outptr++) = six2pr[((bufIn[1] << 2) & 074) | ((bufIn[2] >> 6) & 03)]; /*c3*/
		*(outptr++) = six2pr[bufIn[2] & 077];	/* c4 */

		bufIn += 3;
	}

	// If nBytesIn was not a multiple of 3, then we have encoded too
	// many characters. Adjust appropriately.
	if( i == nBytesIn + 1 )
		outptr[-1] = '='; // only 2 bytes in the last group
	else if( i == nBytesIn + 2 )
		outptr[-2] = outptr[-1] = '='; //only 1 byte in the last group

	*outptr = '\0';
	return true;
}

//------------------------------------------------------------
// Base64-decode a string of data.
//
// Arguments:
//    bufCoded		pointer to base64-encoded string.
//    pbufDecoded	pointer to pointer to output bytes.
//					Must be free()'d by the caller.
//    pcbDecoded	number of decoded bytes.
//
// Returns true if successful; otherwise false.
//------------------------------------------------------------
/* static */
bool _base64_::decode ( /* in */  const char* bufCoded,
						/* out */ BYTE** pbufDecoded,
						/* out */ DWORD* pcbDecoded )
{
	int nbytesdecoded;
	const char* bufin = bufCoded;
	BYTE* bufout;
	int nprbytes;

	// Strip leading whitespace
	while( *bufCoded == ' ' || *bufCoded == '\t' )
		bufCoded++;

	// Figure out how many characters are in the input buffer.
	// If this would decode into more bytes than would fit into
	// the output buffer, adjust the number of input bytes downwards.
	bufin = bufCoded;
	while( pr2six[*(bufin++)] <= 63 )
		; // sic!
	nprbytes = bufin - bufCoded - 1;
	nbytesdecoded = ((nprbytes+3)/4) * 3;

	*pbufDecoded = (BYTE*) malloc( nbytesdecoded + 4 );
	if( !*pbufDecoded )
		return false;

	bufout = *pbufDecoded;
	bufin = bufCoded;

	while( nprbytes > 0 )
	{
		*(bufout++) = (BYTE) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
		*(bufout++) = (BYTE) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
		*(bufout++) = (BYTE) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
		bufin += 4;
		nprbytes -= 4;
	}

	if( nprbytes & 03 )
	{
		if( pr2six[bufin[-2]] > 63 )
			nbytesdecoded -= 2;
		else
			nbytesdecoded -= 1;
	}

	(*pbufDecoded)[nbytesdecoded] = '\0';
	*pcbDecoded = nbytesdecoded;

	return true;
}


};	// namespace soige

