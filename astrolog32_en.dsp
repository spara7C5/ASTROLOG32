# Microsoft Developer Studio Project File - Name="astrolog32_en" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=astrolog32_en - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "astrolog32_en.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "astrolog32_en.mak" CFG="astrolog32_en - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "astrolog32_en - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "astrolog32_en - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "astrolog32_en - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_en"
# PROP Intermediate_Dir "Release_en"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\sweph\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "LANGUAGE_ENGLISH" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib swelib.lib /nologo /subsystem:windows /machine:I386 /out:"Release_en/astrolog32.exe" /libpath:"C:\sweph\lib\release"

!ELSEIF  "$(CFG)" == "astrolog32_en - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_en"
# PROP Intermediate_Dir "Debug_en"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\sweph\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "LANGUAGE_ENGLISH" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib swelib.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug_en/astrolog32.exe" /pdbtype:sept /libpath:"C:\sweph\lib\debug"

!ENDIF 

# Begin Target

# Name "astrolog32_en - Win32 Release"
# Name "astrolog32_en - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\astrolog.c
# End Source File
# Begin Source File

SOURCE=.\atlas.c
# End Source File
# Begin Source File

SOURCE=.\calc.c
# End Source File
# Begin Source File

SOURCE=.\charts0.c
# End Source File
# Begin Source File

SOURCE=.\charts1.c
# End Source File
# Begin Source File

SOURCE=.\charts2.c
# End Source File
# Begin Source File

SOURCE=.\charts3.c
# End Source File
# Begin Source File

SOURCE=.\data.c
# End Source File
# Begin Source File

SOURCE=.\data_en.c
# End Source File
# Begin Source File

SOURCE=.\etc_en.c
# End Source File
# Begin Source File

SOURCE=.\general.c
# End Source File
# Begin Source File

SOURCE=.\houses.c
# End Source File
# Begin Source File

SOURCE=.\intrpalt.c
# End Source File
# Begin Source File

SOURCE=.\intrpret.c
# End Source File
# Begin Source File

SOURCE=.\io.c
# End Source File
# Begin Source File

SOURCE=.\matrix.c
# End Source File
# Begin Source File

SOURCE=.\process_switches.c
# End Source File
# Begin Source File

SOURCE=.\search_charts.c
# End Source File
# Begin Source File

SOURCE=.\swe_call.c
# End Source File
# Begin Source File

SOURCE=.\wdialog.c
# End Source File
# Begin Source File

SOURCE=.\wdialog2.c
# End Source File
# Begin Source File

SOURCE=.\wdriver.c
# End Source File
# Begin Source File

SOURCE=.\wizards.c
# End Source File
# Begin Source File

SOURCE=.\xcharts0.c
# End Source File
# Begin Source File

SOURCE=.\xcharts1.c
# End Source File
# Begin Source File

SOURCE=.\xcharts2.c
# End Source File
# Begin Source File

SOURCE=.\xdata.c
# End Source File
# Begin Source File

SOURCE=.\xdevice.c
# End Source File
# Begin Source File

SOURCE=.\xgeneral.c
# End Source File
# Begin Source File

SOURCE=.\xscreen.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\astrolog.h
# End Source File
# Begin Source File

SOURCE=.\extern.h
# End Source File
# Begin Source File

SOURCE=.\macro_functions.h
# End Source File
# Begin Source File

SOURCE=.\resources_en.h
# End Source File
# Begin Source File

SOURCE=.\strings_en.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\astrlog1.ico
# End Source File
# Begin Source File

SOURCE=.\astrlog2.ico
# End Source File
# Begin Source File

SOURCE=.\astrlog3.ico
# End Source File
# Begin Source File

SOURCE=.\astrlog4.ico
# End Source File
# Begin Source File

SOURCE=.\pictures\belier_fancoise.bmp
# End Source File
# Begin Source File

SOURCE=.\pictures\centaurebleu_francoise.bmp
# End Source File
# Begin Source File

SOURCE=.\pictures\feu_dennis2.bmp
# End Source File
# Begin Source File

SOURCE=.\pictures\lune_fancoise.bmp
# End Source File
# Begin Source File

SOURCE=.\pictures\mainsscorpion_francoise.bmp
# End Source File
# Begin Source File

SOURCE=.\resources_en.rc
# End Source File
# Begin Source File

SOURCE=.\pictures\terre_francoise2.bmp
# End Source File
# Begin Source File

SOURCE=.\pictures\terreScorpion_francoise.bmp
# End Source File
# Begin Source File

SOURCE=.\pictures\terreScorpion_francoise2.bmp
# End Source File
# End Group
# End Target
# End Project
