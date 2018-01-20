//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// _table_.h - header file for the _table_<> class.
//
// Provides a two-dimensional (rows-columns) table
// of objects of one type.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __table_already_included_vasya__
#define __table_already_included_vasya__

#include "_string_.h"
#include "_array_.h"
#include "_ptr_.h"

namespace soige {

template<typename elem_type> class _table_;

template<typename elem_type> class table_listener
{
	friend class _table_<elem_type>;
	typedef _table_<elem_type> typed_table;

protected:
	virtual void onTableChanged(typed_table*) = 0;
	virtual void onTableCellUpdated(typed_table*, int row, int col) = 0;
};

//------------------------------------------------------------
// The table class
//------------------------------------------------------------
template<typename elem_type> class _table_
{
public:
	typedef elem_type elem_type;
	typedef _array_<elem_type> elem_array;

	//-------------------------------------------------
	// constructors
	_table_() : INSERTION_SORT_BOUND(16)
	{
		_rowCount = 0;
	}
	_table_(const _table_& other) : INSERTION_SORT_BOUND(16)
	{
		_rowCount = other._rowCount;
		_colNames = other._colNames;
		_data.clear();
		for(int i=0; i<other._data.length(); i++)
			_data.append(_ptr_<elem_array>(new elem_array(*(other._data[i]))));
		fireTableChanged();
	}
	virtual ~_table_()
	{
		clear();
	}

	//-------------------------------------------------
	// operators
	virtual _table_& operator=(const _table_& other)
	{
		if( this->operator==(other) )
			return *this;
		_rowCount = other._rowCount;
		_colNames = other._colNames;
		_data.clear();
		for(int i=0; i<other._data.length(); i++)
			_data.append(_ptr_<elem_array>(new elem_array(*(other._data[i]))));
		fireTableChanged();
		return *this;
	}
	bool operator==(const _table_& other) const
	{
		if(this == &other) return true;
		if(_rowCount != other._rowCount || _colNames != other._colNames) return false;
		for(int i=0; i<_colNames.length(); i++)
			if(*_data[i] != *other._data[i]) return false;
		return true;
	}
	bool operator!=(const _table_& other) const
	{
		return ( !this->operator==(other) );
	}

	//-------------------------------------------------
	// attributes
	
	// Column operations
	void insertColumn(const _string_& colName, int index)
	{
		if(index < 0)
			return;
		if(index > _colNames.length())
			index = _colNames.length();

		_colNames.insert(colName, index);
		// redimension the data array as well
		_data.insert(_ptr_<elem_array>(new elem_array()), index);
		if(_rowCount > 0)
			_data[index]->resize(_rowCount);
		fireTableChanged();
	}
			
	void removeColumn(int index)
	{
		_colNames.removeAt(index);
		// redimension the data array as well
		_data.removeAt(index);
		fireTableChanged();
	}

	bool swapColumns(int col1, int col2)
	{
		if( col1 < 0 || col2 < 0 || col1 >= _colNames.length() || col2 >= _colNames.length() )
			return false;
		_ptr_<elem_array> temp = _data[col1];
		_data[col1] = _data[col2];
		_data[col2] = temp;
		fireTableChanged();
		return true;
	}

	// Reserve the specified number of columns,
	// which can then be set by setColumnName()
	void setColumnCount(int cols)
	{
		if(cols < 0)
			return;
		int prevCols = _colNames.length();
		_colNames.resize(cols);
		// redimension the data array as well
		_data.resize(cols);
		int i;
		for(i=0; i<cols; i++)
			if(_data[i] == NULL) _data[i] = new elem_array();
		if(_rowCount > 0 && prevCols < cols)
			for(i=prevCols; i<cols; i++)
				_data[i]->resize(_rowCount);

		fireTableChanged();
	}
	
	void setColumnName(int col, const _string_& colName)
	{
		if(col < 0 || col >= _colNames.length())
			return;
		_colNames[col] = colName;
		fireTableChanged();
	}

	void setHeader(const _array_<_string_>& colNames);
	
	int getColumnCount() const
	{
		return _colNames.length();
	}
	
	_string_ getColumnName(int col) const
	{
		return _colNames[col];
	}
	
	int getColumnByName(const _string_& colName) const
	{
		return _colNames.find(colName);
	}
	
	// Row operations
	void appendRow(const _array_<elem_type>& newRow)
	{
		insertRow(newRow, _rowCount);
	}
	
	bool insertRow(const _array_<elem_type>& newRow, int index);
	bool swapRows(int row1, int row2);

	int getRowCount() const
	{
		return _rowCount;
	}
	
	void setValueAt(int row, int col, const elem_type& newVal)
	{
		_data.get(col)->get(row) = newVal;
		fireTableCellUpdated(row, col);
	}

	const elem_type& getValueAt(int row, int col) const
	{
		return _data.get(col)->get(row);
	}
	
	bool removeRow(int row)
	{
		if(row < 0 || row >= _rowCount)
			return false;
		for(int i=0; i<_data.length(); i++)
			_data[i]->removeAt(row);
		_rowCount -= 1;
		fireTableChanged();
		return true;
	}
	
	// Clear all the rows, but leave all column definitions intact
	void removeAllRows()
	{
		for(int i=0; i<_data.length(); i++)
			_data[i]->clear();
		_rowCount = 0;
		fireTableChanged();
	}
	
	// Removes everything from the table,
	// including both columns and data
	void clear()
	{
		_colNames.clear();
		_data.clear();
		_rowCount = 0;
		fireTableChanged();
	}
	
	//
	bool sort(int col)
	{
		if(col >= _colNames.length())
			return false;
		if(_rowCount < 2)
			return true;
		_quickSort(col, 0, _rowCount-1);
		return true;
	}

	//
	bool find(const elem_type& val, int* pRow, int* pCol, int fromRow = 0, int fromCol = 0) const;

	// Table change notification
	void addTableListener(table_listener* pL)
	{
		_listeners.append(pL);
	}
	void removeTableListener(table_listener* pL)
	{
		_listeners.remove(pL);
	}
	void fireTableChanged()
	{
		for(int i=0; i<_listeners.length(); i++)
			_listeners[i]->onTableChanged(this);
	}
	void fireTableCellUpdated(int row, int col)
	{
		for(int i=0; i<_listeners.length(); i++)
			_listeners[i]->onTableCellUpdated(this, row, col);
	}

protected:
	// column names
	_array_<_string_> _colNames;
	
	// The two-dimensional array for data;
	// indexed not [row][col], but [col][row],
	// because _data is array of *columns* (pointers to columns, to be exact),
	// and each of these "columns" is in turn array of rows
	_array_< _ptr_<elem_array> > _data;
	
	int _rowCount;

	// change listeners
	_array_< table_listener<elem_type>* > _listeners;

	// boundary point to use insertion sort
	int const INSERTION_SORT_BOUND;

protected:
	// @sortCol is the column to sort on
	void _quickSort(int sortCol, int first, int last);
	void _heapSort(int sortCol, int first, int cElems);
	
	// Assignments
	// to a temp array from table row
	void _assignRow(elem_array& arr, int row)
	{
		arr.resize(_colNames.length());
		for(int i=0; i<arr.length(); i++)
			arr[i] = _data.get(i)->get(row);
	}
	// to a table row from temp array
	void _assignRow(int row, elem_array& arr)
	{
		for(int i=0; i<_colNames.length(); i++)
			_data.get(i)->get(row) = arr[i];
	}
	// from one table row (row2 == src) to another (row1 == dest)
	void _assignRow(int row1, int row2)
	{
		for(int i=0; i<_colNames.length(); i++)
			_data.get(i)->get(row1) = _data.get(i)->get(row2);
	}
};


template<typename elem_type>
	void _table_<elem_type>::setHeader ( const _array_<_string_>& colNames )
{
	int prevCols = _colNames.length();
	_colNames = colNames;
	// redimension the data array as well
	_data.resize(_colNames.length());
	int i;
	for(i=0; i<_colNames.length(); i++)
		if(_data[i] == NULL) _data[i] = new elem_array();
	if(_rowCount>0 && prevCols<_colNames.length())
		for(i=prevCols; i<_colNames.length(); i++)
			_data[i]->resize(_rowCount);

	fireTableChanged();
}


template<typename elem_type>
	bool _table_<elem_type>::insertRow ( const _array_<elem_type>& newRow, int index )
{
	if(index < 0)
		return false;
	if(index > _rowCount)
		index = _rowCount;

	for(int i=0; i<_data.length(); i++)
	{
		if(i < newRow.length())
			_data[i]->insert(newRow[i], index);
		else
		// supplied number of values is fewer than table's cols; fill with empty elems
			_data[i]->insert(elem_type(), index);
	}
	_rowCount += 1;
	fireTableChanged();
	return true;
}


template<typename elem_type>
	bool _table_<elem_type>::swapRows ( int row1, int row2 )
{
	if(row1 < 0 || row2 < 0 || row1 >= _rowCount || row2 >= _rowCount)
		return false;

	int i;
	elem_array temp;
	temp.resize(_colNames.length());
	
	for(i=0; i<temp.length(); i++)
		temp[i] = _data.get(i)->get(row1);
	for(i=0; i<temp.length(); i++)
		_data.get(i)->get(row1) = _data.get(i)->get(row2);
	for(i=0; i<temp.length(); i++)
		_data.get(i)->get(row2) = temp[i];

	return true;
}


template<typename elem_type>
	bool _table_<elem_type>::find ( const elem_type& val,
									int* pRow, int* pCol,
									int fromRow, int fromCol ) const
{
	*pRow = *pCol = -1;
	
	if(fromRow >= _rowCount || fromCol >= _colNames.length())
		return false;

	int i, j;
	for(i=fromRow; i<_rowCount; i++)
	{
		for(j=fromCol; j<_colNames.length(); j++)
		{
			if(_data.get(j)->get(i) == val)
			{
				*pRow = i;
				*pCol = j;
				return true;
			}
		}
	}
	
	// not found
	return false;
}


template<typename elem_type>
	void _table_<elem_type>::_quickSort ( int sortCol, int first, int last )
{
	int stack_pointer = 0;
	int first_stack[32];
	int last_stack[32];

	for (;;)
	{
		if (last - first <= INSERTION_SORT_BOUND)
		{
			// for small sort, use insertion sort
			int indx;
			elem_array prev_val;
			elem_array cur_val;
			_assignRow(prev_val, first);

			for (indx = first + 1; indx <= last; ++indx)
			{
				_assignRow(cur_val, indx);
				if ( _compare(prev_val[sortCol], cur_val[sortCol]) > 0 )
				{
					int indx2;
					// out of order
					_assignRow(indx, prev_val);

					for (indx2 = indx - 1; indx2 > first; --indx2)
					{
						elem_array temp_val;
						_assignRow(temp_val, indx2 - 1);
						if ( _compare(temp_val[sortCol], cur_val[sortCol]) > 0 )
							_assignRow(indx2, temp_val);
						else
							break;
					}
					_assignRow(indx2, cur_val);
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
			elem_array pivot;
			int med = (first + last) >> 1;

			// Choose pivot from first, last, and median position.
			// Sort the three elements.
			if ( _compare(_data.get(sortCol)->get(first), _data.get(sortCol)->get(last)) > 0 )
				swapRows(first, last);

			if ( _compare(_data.get(sortCol)->get(first), _data.get(sortCol)->get(med)) > 0 )
				swapRows(med, first);

			if ( _compare(_data.get(sortCol)->get(med), _data.get(sortCol)->get(last)) > 0 )
				swapRows(last, med);

			_assignRow(pivot, med);

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
				} while ( _compare(pivot[sortCol], _data.get(sortCol)->get(down)) > 0 );

				do
				{
					--up;
				} while ( _compare(_data.get(sortCol)->get(up), pivot[sortCol]) > 0 );

				if (up > down)
					swapRows(down, up);  // interchange L[down] and L[up]
				else
					break;
			}

			int len1; // length of first segment
			int len2; // length of second segment
			len1 = up - first + 1;
			len2 = last - up;
			if (len1 >= len2)
			{
				if ( (len1 >> 5) > len2 )
				{
					// badly balanced partitions, heap sort first segment
					_heapSort(sortCol, first, len1);
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
					_heapSort(sortCol, up + 1, len2);
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


template<typename elem_type>
	void _table_<elem_type>::_heapSort ( int sortCol, int first, int cElems )
{
	int half;
	int parent;

	if (cElems <= 1)
		return;

	half = cElems >> 1;
	for (parent = half; parent >= 1; --parent)
	{
		elem_array temp;
		int level = 0;
		int child;

		child = parent;
		// bottom-up downheap

		// leaf-search for largest child path
		while (child <= half)
		{
			++level;
			child += child;
			if (
				(child < cElems) &&
				(_compare(_data.get(sortCol)->get(first + child), _data.get(sortCol)->get(first + child - 1)) > 0)
			   )
				++child;
		}
		// bottom-up-search for rotation point
		_assignRow(temp, first + parent - 1);
		for (;;)
		{
			if (parent == child)
				break;
			if ( _compare(temp[sortCol], _data.get(sortCol)->get(first + child - 1)) <= 0 )
				break;
			child >>= 1;
			--level;
		}
		// rotate nodes from parent to rotation point
		for (; level > 0; --level)
		{
			_assignRow( first + (child >> level) - 1,
						first + (child >> (level - 1)) - 1 );
		}
		_assignRow(first + child - 1, temp);
	}

	--cElems;
	do
	{
		elem_array temp1;
		int level = 0;
		int child;

		// move max element to back of array
		swapRows(first + cElems, first);
		_assignRow(temp1, first + cElems);

		child = parent = 1;
		half = cElems >> 1;

		// bottom-up downheap

		// leaf-search for largest child path
		while (child <= half)
		{
			++level;
			child += child;
			if (
				(child < cElems) &&
				(_compare(_data.get(sortCol)->get(first + child), _data.get(sortCol)->get(first + child - 1)) > 0)
			   )
				++child;
		}
		// bottom-up-search for rotation point
		for (;;)
		{
			if (parent == child)
				break;
			if ( _compare(temp1[sortCol], _data.get(sortCol)->get(first + child - 1)) <= 0 )
				break;
			child >>= 1;
			--level;
		}
		// rotate nodes from parent to rotation point
		for (; level > 0; --level)
		{
			_assignRow( first + (child >> level) - 1,
						first + (child >> (level - 1)) - 1 );
		}
		
		_assignRow(first + child - 1, temp1);

	} while (--cElems >= 1);
}


};	// namespace soige

#endif // __table_already_included_vasya__

