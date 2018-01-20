//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _sort_.h - header/impl file for the _sort_<> class.
//
// Defines a sorting algorithm based on a mixture of three
// algorithms: insertion, heap, and quick.
// For comparisons, user-defined types have to define
// operators == and < (which are used by the _compare
// template routine in _common_.h).
// Copying of item values relies on operator=.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __sort_already_included_vasya__
#define __sort_already_included_vasya__

#include "_common_.h"

namespace soige {

//------------------------------------------------------------
// The sort class
//------------------------------------------------------------
template<typename item_type> class _sort_
{
public:
	_sort_(int insertBound = 16) : INSERTION_SORT_BOUND(insertBound)
	{ }

	void sort(item_type Array[], int cElems)
	{
		if( cElems <= 1 )
			return;
		_quickSort(Array, 0, cElems - 1);
	}

	virtual ~_sort_()
	{ }

protected:
	// these two things are too large to make them inline
	void _quickSort(item_type Array[], int first, int last);
	void _heapSort(item_type Array[], int first, int cElems);

protected:
	int const INSERTION_SORT_BOUND;	// boundary point to use insertion sort
	static size_t const item_size;
};


template<typename item_type> const size_t _sort_<item_type>::item_size = sizeof(item_type);


template<typename item_type>
	void _sort_<item_type>::_quickSort (item_type Array[], int first, int last)
{
	int stack_pointer = 0;
	int first_stack[32];
	int last_stack[32];

	for (;;)
	{
		if (last - first <= INSERTION_SORT_BOUND)
		{
			// for small sort (or subsort), use insertion sort
			int indx;
			item_type prev_val;
			item_type cur_val;
			prev_val = Array[first];

			for (indx = first + 1; indx <= last; ++indx)
			{
				cur_val = Array[indx];
				if ( _compare(prev_val, cur_val) > 0 )
				{
					int indx2;
					// out of order
					Array[indx] = prev_val;

					for (indx2 = indx - 1; indx2 > first; --indx2)
					{
						item_type temp_val;
						temp_val = Array[indx2 - 1];
						if ( _compare(temp_val, cur_val) > 0 )
							Array[indx2] = temp_val;
						else
							break;
					}
					Array[indx2] = cur_val;
				}
				else
				{
					// in order, advance to next element
					prev_val = cur_val;
				}
			}
		} // end if (insertion sort)
		else
		{
			// try quick sort
			item_type pivot;
			int med = (first + last) >> 1;

			// Choose pivot from first, last, and median position.
			// Sort the three elements.
			if ( _compare(Array[first], Array[last]) > 0 )
				_swap<item_type>(&Array[first], &Array[last]);

			if ( _compare(Array[first], Array[med]) > 0 )
				_swap<item_type>(&Array[med], &Array[first]);

			if ( _compare(Array[med], Array[last]) > 0 )
				_swap<item_type>(&Array[last], &Array[med]);

			pivot = Array[med];

			int up;
			int down;
			// First and last element will be loop stopper.
			// Split array into two partitions.
			down = first;
			up = last;
			for (;;)
			{
				do
				{
					++down;
				} while ( _compare(pivot, Array[down]) > 0 );
				// while ( down <= last && _compare(pivot, Array[down]) > 0 );

				do
				{
					--up;
				} while ( _compare(Array[up], pivot) > 0 );
				// while ( up >= first && _compare(Array[up], pivot) > 0 );

				if (up > down)
					_swap<item_type>(&Array[down], &Array[up]);  // interchange L[down] and L[up]
				else
					break;
			}

			int len1; // length of first segment
			int len2; // length of second segment
			len1 = up - first + 1;
			len2 = last - up;
			if (len1 >= len2)
			{
				if ((len1 >> 5) > len2)
				{
					// badly balanced partitions, heap sort first segment
					_heapSort(Array, first, len1);
				}
				else
				{
					first_stack[stack_pointer] = first; // stack first segment
					last_stack[stack_pointer++] = up;
				} 
				first = up + 1;
				// tail recursion elimination of
				// _quickSort(Array,fun_ptr,up + 1,last)
			}
			else
			{
				if ( (len2 >> 5) > len1 )
				{
					// badly balanced partitions, heap sort second segment
					_heapSort(Array, up + 1, len2);
				}
				else
				{
					first_stack[stack_pointer] = up + 1; // stack second segment
					last_stack[stack_pointer++] = last;
				}
				last = up;
				// tail recursion elimination of
				// _quickSort(Array,fun_ptr,first,up)
			}

			continue;
		
			// end of quick sort //
		
		} // end else

		if (stack_pointer > 0)
		{
			// Sort segment from stack.
			first = first_stack[--stack_pointer];
			last = last_stack[stack_pointer];
		}
		else
			break;
	} // end for
}

template<typename item_type>
	void _sort_<item_type>::_heapSort (item_type Array[], int first, int cElems)
{
	int half;
	int parent;

	if (cElems <= 1)
		return;

	half = cElems >> 1;
	for (parent = half; parent >= 1; --parent)
	{
		item_type temp;
		int level = 0;
		int child;

		child = parent;
		// bottom-up downheap

		// leaf-search for largest child path
		while (child <= half)
		{
			++level;
			child += child;
			if ( (child < cElems) &&
				 (_compare(Array[first + child], Array[first + child - 1]) > 0)
			   )
				++child;
		}
		// bottom-up-search for rotation point
		temp = Array[first + parent - 1];
		for (;;)
		{
			if (parent == child)
				break;
			if ( _compare(temp, Array[first + child - 1]) <= 0 )
				break;
			child >>= 1;
			--level;
		}
		// rotate nodes from parent to rotation point
		for (; level > 0; --level)
		{
			Array[first + (child >> level) - 1] = Array[first + (child >> (level - 1)) - 1];
		}
		Array[first + child - 1] = temp;
	}

	--cElems;
	do
	{
		item_type temp;
		int level = 0;
		int child;

		// move max element to back of array
		temp = Array[first + cElems];
		_swap<item_type>(&Array[first + cElems], &Array[first]);

		child = parent = 1;
		half = cElems >> 1;

		// bottom-up downheap

		// leaf-search for largest child path
		while (child <= half)
		{
			++level;
			child += child;
			if ( (child < cElems) &&
				 (_compare(Array[first + child], Array[first + child - 1]) > 0)
			   )
				++child;
		}
		// bottom-up-search for rotation point
		for (;;)
		{
			if (parent == child)
				break;
			if ( _compare(temp, Array[first + child - 1]) <= 0 )
				break;
			child >>= 1;
			--level;
		}
		// rotate nodes from parent to rotation point
		for (; level > 0; --level)
		{
			Array[first + (child >> level) - 1] = Array[first + (child >> (level - 1)) - 1];
		}
		
		Array[first + child - 1] = temp;
	} while (--cElems >= 1);
}


};	// namespace soige

#endif // __sort_already_included_vasya__

