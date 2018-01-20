# Microsoft Developer Studio Project File - Name="utils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=utils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utils.mak" CFG="utils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utils - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "utils - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/utils", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "utils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_USE_NO_CRT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "utils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "utils - Win32 Release"
# Name "utils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_base64_.cpp
# End Source File
# Begin Source File

SOURCE=.\_base_stream_.cpp
# End Source File
# Begin Source File

SOURCE=.\_boyer_moore_.cpp
# End Source File
# Begin Source File

SOURCE=.\_byte_input_stream_.cpp
# End Source File
# Begin Source File

SOURCE=.\_byte_output_stream_.cpp
# End Source File
# Begin Source File

SOURCE=.\_cstring_.cpp
# End Source File
# Begin Source File

SOURCE=.\_file_finder_.cpp
# End Source File
# Begin Source File

SOURCE=.\_file_input_stream_.cpp
# End Source File
# Begin Source File

SOURCE=.\_file_output_stream_.cpp
# End Source File
# Begin Source File

SOURCE=.\_input_stream_.cpp
# End Source File
# Begin Source File

SOURCE=.\_ipc_channel_.cpp
# End Source File
# Begin Source File

SOURCE=.\_num_eval_.cpp
# End Source File
# Begin Source File

SOURCE=.\_output_stream_.cpp
# End Source File
# Begin Source File

SOURCE=.\_regex_.cpp
# End Source File
# Begin Source File

SOURCE=.\_runtime_.cpp
# End Source File
# Begin Source File

SOURCE=.\_shared_memory_.cpp
# End Source File
# Begin Source File

SOURCE=.\_soundex_.cpp
# End Source File
# Begin Source File

SOURCE=.\_strfuncs_.cpp
# End Source File
# Begin Source File

SOURCE=.\_string_.cpp
# End Source File
# Begin Source File

SOURCE=.\_thread_pool_.cpp
# End Source File
# Begin Source File

SOURCE=.\_wildcard_search_.cpp
# End Source File
# Begin Source File

SOURCE=.\_win32_file_.cpp
# End Source File
# Begin Source File

SOURCE=.\_wstring_.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_array_.h
# End Source File
# Begin Source File

SOURCE=.\_base64_.h
# End Source File
# Begin Source File

SOURCE=.\_base_stream_.h
# End Source File
# Begin Source File

SOURCE=.\_boyer_moore_.h
# End Source File
# Begin Source File

SOURCE=.\_byte_stream_.h
# End Source File
# Begin Source File

SOURCE=.\_byte_streams_defs_.h
# End Source File
# Begin Source File

SOURCE=.\_common_.h
# End Source File
# Begin Source File

SOURCE=.\_cstring_.h
# End Source File
# Begin Source File

SOURCE=.\_dictionary_.h
# End Source File
# Begin Source File

SOURCE=.\_file_finder_.h
# End Source File
# Begin Source File

SOURCE=.\_file_stream_.h
# End Source File
# Begin Source File

SOURCE=.\_file_streams_defs_.h
# End Source File
# Begin Source File

SOURCE=.\_io_streams_defs_.h
# End Source File
# Begin Source File

SOURCE=.\_ipc_channel_.h
# End Source File
# Begin Source File

SOURCE=.\_list_.h
# End Source File
# Begin Source File

SOURCE=.\_lock_.h
# End Source File
# Begin Source File

SOURCE=.\_num_eval_.h
# End Source File
# Begin Source File

SOURCE=.\_ptr_.h
# End Source File
# Begin Source File

SOURCE=.\_queue_.h
# End Source File
# Begin Source File

SOURCE=.\_regex_.h
# End Source File
# Begin Source File

SOURCE=.\_runtime_.h
# End Source File
# Begin Source File

SOURCE=.\_set_.h
# End Source File
# Begin Source File

SOURCE=.\_shared_memory_.h
# End Source File
# Begin Source File

SOURCE=.\_sort_.h
# End Source File
# Begin Source File

SOURCE=.\_soundex_.h
# End Source File
# Begin Source File

SOURCE=.\_stack_array_.h
# End Source File
# Begin Source File

SOURCE=.\_stack_list_.h
# End Source File
# Begin Source File

SOURCE=.\_strfuncs_.h
# End Source File
# Begin Source File

SOURCE=.\_string_.h
# End Source File
# Begin Source File

SOURCE=.\_string_array_.h
# End Source File
# Begin Source File

SOURCE=.\_table_.h
# End Source File
# Begin Source File

SOURCE=.\_thread_pool_.h
# End Source File
# Begin Source File

SOURCE=.\_wildcard_search_.h
# End Source File
# Begin Source File

SOURCE=.\_win32_file_.h
# End Source File
# Begin Source File

SOURCE=.\_wstring_.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
