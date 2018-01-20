//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _base_stream_.cpp - implementation file
// for the base stream classes.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "_base_stream_.h"

namespace soige {

//------------------------------------------------------------
// basic_stream
//------------------------------------------------------------
const char* basic_stream::data_type_desc[] =
{
	"MIN TYPE CODE",
	"BOF Marker",
	"Null pointer",
	"Single Char",
	"Boolean",
	"Short",
	"Int",
	"Long",
	"Int64",
	"Float",
	"Double",
	"UUID",
	"Array of any type",
	"ANSI string",
	"Unicode string",
	"BSTR",
	"Serializable Object",
	"BLOB",
	// May add something here in the future.
	// The indexes in the enum for the following two do not match
	// with their position in this array, but since they will never
	// need to be read by clients, clients never care or see
	// these two descriptions (as well as the first two)
	"EOF Marker",
	"MAX TYPE CODE"
};


};	// namespace soige

