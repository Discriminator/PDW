# Microsoft Developer Studio Project File - Name="PDW3_0" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PDW3_0 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PDW.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PDW.mak" CFG="PDW3_0 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PDW3_0 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PDW3_0 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PDW3_0 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Release/PDW3_0.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib comdlg32.lib comctl32.lib ws2_32.lib gdi32.lib user32.lib shell32.lib wininet.lib kernel32.lib shell32.lib libcmt.lib oldnames.lib libcpmt.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release/PDW3_0.exe"
# SUBTRACT LINK32 /profile /nodefaultlib

!ELSEIF  "$(CFG)" == "PDW3_0 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug/PDW3_0.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib comctl32.lib comdlg32.lib kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/PDW3_0.exe"
# SUBTRACT LINK32 /profile /nodefaultlib

!ENDIF 

# Begin Target

# Name "PDW3_0 - Win32 Release"
# Name "PDW3_0 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\acars.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\binary.cpp

!IF  "$(CFG)" == "PDW3_0 - Win32 Release"

!ELSEIF  "$(CFG)" == "PDW3_0 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\utils\Debug.cpp

!IF  "$(CFG)" == "PDW3_0 - Win32 Release"

!ELSEIF  "$(CFG)" == "PDW3_0 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\decode.cpp
# End Source File
# Begin Source File

SOURCE=.\ermes.cpp
# End Source File
# Begin Source File

SOURCE=.\Flex.cpp
# End Source File
# Begin Source File

SOURCE=.\Gfx.cpp
# End Source File
# Begin Source File

SOURCE=.\helper_funcs.cpp
# End Source File
# Begin Source File

SOURCE=.\Initapp.cpp
# End Source File
# Begin Source File

SOURCE=.\language.cpp
# End Source File
# Begin Source File

SOURCE=.\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\Misc.cpp
# End Source File
# Begin Source File

SOURCE=.\mobitex.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\Ostype.cpp
# End Source File
# Begin Source File

SOURCE=.\PDW.cpp
# End Source File
# Begin Source File

SOURCE=.\Pocsag.cpp
# End Source File
# Begin Source File

SOURCE=.\printer.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\rs232.cpp
# End Source File
# Begin Source File

SOURCE=.\Rsrc.rc
# End Source File
# Begin Source File

SOURCE=.\sigind.cpp
# End Source File
# Begin Source File

SOURCE=.\utils\smtp.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_in.cpp
# End Source File
# Begin Source File

SOURCE=.\toolbar.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Headers\acars.h
# End Source File
# Begin Source File

SOURCE=.\utils\binary.h
# End Source File
# Begin Source File

SOURCE=.\Headers\decode.h
# End Source File
# Begin Source File

SOURCE=.\Headers\ermes.h
# End Source File
# Begin Source File

SOURCE=.\Headers\gfx.h
# End Source File
# Begin Source File

SOURCE=.\Headers\helper_funcs.h
# End Source File
# Begin Source File

SOURCE=.\Headers\initapp.h
# End Source File
# Begin Source File

SOURCE=.\Headers\language.h
# End Source File
# Begin Source File

SOURCE=.\Headers\menu.h
# End Source File
# Begin Source File

SOURCE=.\Headers\misc.h
# End Source File
# Begin Source File

SOURCE=.\Headers\mobitex.h
# End Source File
# Begin Source File

SOURCE=.\utils\OSTYPE.H
# End Source File
# Begin Source File

SOURCE=.\Headers\Pdw.h
# End Source File
# Begin Source File

SOURCE=.\Headers\printer.h
# End Source File
# Begin Source File

SOURCE=.\Headers\Resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\utils\rs232.h
# End Source File
# Begin Source File

SOURCE=.\Headers\sigind.h
# End Source File
# Begin Source File

SOURCE=.\Headers\Slicer.h
# End Source File
# Begin Source File

SOURCE=.\utils\smtp.h
# End Source File
# Begin Source File

SOURCE=.\utils\smtp_int.h
# End Source File
# Begin Source File

SOURCE=.\Headers\sound_in.h
# End Source File
# Begin Source File

SOURCE=.\Headers\toolbar.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\GFX\211.gif
# End Source File
# Begin Source File

SOURCE=.\Gfx\clr.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\copy.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\cpyLO.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\cpyUP.bmp
# End Source File
# Begin Source File

SOURCE=.\GFX\exclamation.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\filter.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\help.BMP
# End Source File
# Begin Source File

SOURCE=.\Gfx\mode.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\open.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\options.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\pause.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\pdw.ico
# End Source File
# Begin Source File

SOURCE=.\Gfx\pdwlogo2.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\prt.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\save.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\sigind.bmp
# End Source File
# Begin Source File

SOURCE=.\Gfx\stats.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\PDW3_0.exe.manifest
# End Source File
# End Target
# End Project
