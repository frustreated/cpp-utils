//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// tbl.cpp - checks the table class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_table_.h>
#include <_string_.h>

using namespace soige;

void check_table();

int main(int argc, char* argv[])
{
	printf("Checking _table_\n");
	check_table();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// table tests
void check_table()
{
	_table_<int> int_tbl;
	_array_<_string_> header;
	header.append(_string_("col1"));
	header.append(_string_("col2"));
	header.append(_string_("col3"));
	int_tbl.setHeader(header);
	_array_<int> row;
	row.append(89); row.append(45); row.append(269);
	int_tbl.appendRow(row);
	row.clear();
	row.append(1); row.append(0); row.append(7456);
	int_tbl.appendRow(row);
	row.clear();
	row.append(1087); row.append(83); row.append(6);
	int_tbl.appendRow(row);
	row.clear();
	row.append(78); row.append(345); row.append(889);
	int_tbl.appendRow(row);
	int_tbl.sort(1);
	int_tbl.sort(2);
	int_tbl.getValueAt(0, 0);
	int_tbl.insertColumn(_string_("col1.5"), 1);
	row.clear();
	row.append(2346); row.append(55); row.append(9);
	int_tbl.insertRow(row, 2);
	int_tbl = int_tbl;
	bool b = int_tbl == int_tbl;
	int n = int_tbl.getValueAt(1, 0);
	n = int_tbl.getValueAt(1, 2);
	int_tbl.setColumnName(1, _string_("another col1"));
	ulong nrow=0, ncol=0;
	int_tbl.find(7456, &nrow, &ncol);
	n = int_tbl.getColumnByName(_string_("col2"));
	_string_ s = int_tbl.getColumnName(1);
	n = int_tbl.getRowCount();
	int_tbl.removeColumn(1);
	int_tbl.removeRow(2);
	int_tbl.setValueAt(2, 0, 7978655);
	n = int_tbl.getColumnCount();
	int_tbl.setColumnCount(5);
	int_tbl.swapColumns(0, 1);
	int_tbl.swapRows(0, 2);
	int_tbl.removeAllRows();
	int_tbl.clear();
}


