//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// stm.cpp - checks the stream classes
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <crtdbg.h>

#include <_byte_stream_.h>
#include <_file_stream_.h>

using namespace soige;

void check_byte_stream();
void check_file_stream();

int main(int argc, char* argv[])
{
	printf("Checking streams\n");
	check_byte_stream();
	check_file_stream();
	_CrtDumpMemoryLeaks();
	return 0;
}


//------------------------------------
// stream tests
class bubba : public serializable
{
public:
	int i;
	float f;
	bubba()
	{
		f = i = 0;
	}
	bool writeObject(output_stream* pOut)
	{
		i = 12316;
		f = 56.757F;
		pOut->writeInt(i);
		pOut->writeFloat(f);
		return true;
	}
	bool readObject(input_stream* pIn)
	{
		i = 0;
		f = 0;
		return (pIn->readInt(i) && pIn->readFloat(f));
	}
};

void check_byte_stream()
{
	_byte_output_stream_ bos;
	bos.writeChar('t');
	bos.writeBool(false);
	char* s = "We went to school before Dawn.";
	bos.writeCString(s);
	bubba bub;
	bos.writeObject(&bub);
	wchar_t* ws = L"Dawn was still thinking.";
	bos.writeWString(ws);
	bos.writeBSTR(NULL);
	bos.writeDouble(3.43541298);
	bos.writeWString(NULL);
	bos.close();

	ulong count = 0;
	_byte_input_stream_ bis(bos.getStreamPtr());
	char c; bis.readChar(c);
	bool b; bis.readBool(b);
	char* s1 = NULL;
	bis.readCString(NULL, &count);
	s1 = new char[count];
	bis.readCString(s1, &count);
	delete[] s1;
	bis.readObject(bub);
	count = bis.nextSize()/sizeof(wchar_t);
	wchar_t* ws1 = new wchar_t[128];
	bis.readWString(ws1, &count);
	delete [] ws1;
	BSTR bstr;
	bis.readBSTR(&bstr);
	if(bstr) SysFreeString(bstr);
	double d; bis.readDouble(d);
	try { // this throws an excep
		long l; bis.readLong(l);
	} catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
	count = 0;
	bis.readWString(ws1, &count);
}


void check_file_stream()
{
	_file_output_stream_ fos(".\\stream.dat");
	fos.writeChar('t');
	fos.writeBool(false);
	char* s = "We went to school before Dawn.";
	fos.writeCString(s);
	bubba bub;
	fos.writeObject(&bub);
	wchar_t* ws = L"Dawn was still thinking.";
	fos.writeWString(ws);
	fos.writeBSTR(NULL);
	fos.writeDouble(3.43541298);
	fos.writeWString(NULL);
	fos.close();
	fos.sizeOfStream();
	fos.sizeOfData();

	ulong count = 0;

	_file_input_stream_ fis(".\\stream.dat");
	char c; fis.readChar(c);
	bool b; fis.readBool(b);
	char* s1 = NULL;
	fis.readCString(NULL, &count);
	s1 = new char[count];
	fis.readCString(s1, &count);
	delete[] s1;
	fis.readObject(bub);
	count = fis.nextSize()/sizeof(wchar_t);
	wchar_t* ws1 = new wchar_t[128];
	fis.readWString(ws1, &count);
	delete [] ws1;
	BSTR bstr;
	fis.readBSTR(&bstr);
	if(bstr) SysFreeString(bstr);
	double d; fis.readDouble(d);
	try { // this throws an excep
		long l; fis.readLong(l);
	} catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
	count = 0;
	fis.readWString(ws1, &count);

	fis.reset();
	fis.readChar(c);

/*
	BYTE* p = new BYTE[fos.sizeOfStream()];
	_win32_file_ f(fos.getFileName());
	f.read(p, f.getFileSize());
	f.close();
	_byte_input_stream_ bis(p);
	char c; bis.readChar(c);
	bool b; bis.readBool(b);
	char* s1 = NULL;
	bis.readCString(NULL, &count);
	s1 = new char[count];
	bis.readCString(s1, &count);
	delete[] s1;
	bis.readObject(bub);
	count = bis.nextSize()/sizeof(wchar_t);
	wchar_t* ws1 = new wchar_t[128];
	bis.readWString(ws1, &count);
	delete [] ws1;
	BSTR bstr;
	bis.readBSTR(&bstr);
	if(bstr) SysFreeString(bstr);
	double d; bis.readDouble(d);
	try { // this throws an excep
		long l; bis.readLong(l);
	} catch(soige_error& e) {
		printf("Caught exception: %s\n", e.what());
	}
	count = 0;
	bis.readWString(ws1, &count);
	delete[] p;
*/
}


