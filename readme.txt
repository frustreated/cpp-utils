This project provides a library of miscellaneous classes
and functions that are pretty useful for a C++ developer.

---------------------------------------------------------------------
Some of the included functions
---------------------------------------------------------------------
strichr, stristr, wstrichr, wstristr -
	Case-insensitive additions to the equivalent C
	runtime routines. Written in assembly.
cls -
	Clear screen

---------------------------------------------------------------------
Some of the included classes
---------------------------------------------------------------------
_base64_	-	Base64 (RFC 1113) encoding/decoding.
_file_finder_	-	Searches for files given a wildcard pattern.
_win32_file_	-	Provides convenient file access on Win32.
_array_<>	-	Manageable contiguous array of items.
_set_<>		-	Sorted array of unique elements.
_dictionary_<>	-	Collection of pairs, where each pair consists
			of a key and a corresponding value. Keys have
			to be unique throughout the entire collection.
_list_<>	-	Doubly-linked list.
_stack_<>	-	Stack of items.
_queue_<>	-	Queue.
_ptr_<>		-	Smart pointer; automatically destroys objects
			to which it points whenever necessary.
_cstring_	-	Non-lazy copied string of ascii/binary chars.
_wstring_	-	Non-lazy copied string of Unicode chars.
_sort_<>	-	Optimized sorting algorithm.
_table_<>	-	Table consisting of rows and columns.
streams		-	Byte- and file- input and output streams.
_num_eval_	-	Numeric expression evaluator.
_boyer_moore_	-	Exact string matching algorithm.
_soundex_	-	Sound-alike (soundex) string matching alg.
_regex_		-	Regular expression string matching algorithm.
_shared_memory_	-	A chunk of memory that can be easily shared
			by several programs running on the same box.
_ipc_channel_	-	A two-way interprocess communication channel
			that can be used by two (and only two) processes
			running on the same or different machines.

