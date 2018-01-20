//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// tbl.cpp - checks the table class
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <_table_.h>
#include <_cstring_.h>
#include <_win32_file_.h>

using namespace soige;


int main(int argc, char* argv[])
{
	int i = 0;
	char* buf;
	_cstring_ temp;
	_array_<_cstring_> sarr;
	_table_<_cstring_> tbl;
	_win32_file_ f("out.txt");
	
	if(!f.open()) return -1;

	f.readLine(buf);
	temp = buf;
	free(buf);
	temp.split(sarr, "\t");
	tbl.setHeader(sarr);
	tbl.appendRow(sarr);
	while(f.readLine(buf) > 0)
	{
		i++;
		temp = buf;
		free(buf);
		temp.split(sarr, "\t");
		tbl.appendRow(sarr);
	}
	f.close();
	
	for(i=0; i<tbl.getRowCount(); i++)
		if( tbl.getValueAt(i, 0)[0] != '1' )
			printf("Bad parse: %s\n", (LPCSTR)tbl.getValueAt(i, 0));
	
	tbl.sort(2);
	for(i=1; i<tbl.getRowCount(); i++)
		if( tbl.getValueAt(i-1, 2).compare(tbl.getValueAt(i, 2)) > 0 )
			printf("Bad sort on col2. Baaad sort!\n");
	
	tbl.sort(3);
	for(i=1; i<tbl.getRowCount(); i++)
		if( tbl.getValueAt(i-1, 3).compare(tbl.getValueAt(i, 3)) > 0 )
			printf("Bad sort on col3. Baaad sort!\n");
	return 0;
}


