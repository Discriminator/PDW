# Microsoft Developer Studio Generated NMAKE File, Based on PDW.dsp
!IF "$(CFG)" == ""
CFG=PDW2_5 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to PDW2_5 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PDW2_5 - Win32 Release" && "$(CFG)" != "PDW2_5 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PDW.mak" CFG="PDW2_5 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PDW2_5 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PDW2_5 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\PDW2_5.exe"


CLEAN :
	-@erase "$(INTDIR)\acars.obj"
	-@erase "$(INTDIR)\binary.obj"
	-@erase "$(INTDIR)\Debug.obj"
	-@erase "$(INTDIR)\decode.obj"
	-@erase "$(INTDIR)\ermes.obj"
	-@erase "$(INTDIR)\Flex.obj"
	-@erase "$(INTDIR)\Gfx.obj"
	-@erase "$(INTDIR)\helper_funcs.obj"
	-@erase "$(INTDIR)\Initapp.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\Misc.obj"
	-@erase "$(INTDIR)\mobitex.obj"
	-@erase "$(INTDIR)\Ostype.obj"
	-@erase "$(INTDIR)\PDW.obj"
	-@erase "$(INTDIR)\Pocsag.obj"
	-@erase "$(INTDIR)\printer.obj"
	-@erase "$(INTDIR)\rs232.obj"
	-@erase "$(INTDIR)\Rsrc.res"
	-@erase "$(INTDIR)\sigind.obj"
	-@erase "$(INTDIR)\smtp.obj"
	-@erase "$(INTDIR)\sound_in.obj"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PDW2_5.exe"
	-@erase "$(OUTDIR)\PDW2_5.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\PDW.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Rsrc.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PDW2_5.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib comdlg32.lib comctl32.lib ws2_32.lib gdi32.lib user32.lib shell32.lib wininet.lib kernel32.lib shell32.lib libcmt.lib oldnames.lib libcpmt.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\PDW2_5.pdb" /map:"$(INTDIR)\PDW2_5.map" /machine:I386 /out:"$(OUTDIR)\PDW2_5.exe" 
LINK32_OBJS= \
	"$(INTDIR)\acars.obj" \
	"$(INTDIR)\binary.obj" \
	"$(INTDIR)\Debug.obj" \
	"$(INTDIR)\decode.obj" \
	"$(INTDIR)\ermes.obj" \
	"$(INTDIR)\Flex.obj" \
	"$(INTDIR)\Gfx.obj" \
	"$(INTDIR)\helper_funcs.obj" \
	"$(INTDIR)\Initapp.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\Misc.obj" \
	"$(INTDIR)\mobitex.obj" \
	"$(INTDIR)\Ostype.obj" \
	"$(INTDIR)\PDW.obj" \
	"$(INTDIR)\Pocsag.obj" \
	"$(INTDIR)\printer.obj" \
	"$(INTDIR)\rs232.obj" \
	"$(INTDIR)\sigind.obj" \
	"$(INTDIR)\smtp.obj" \
	"$(INTDIR)\sound_in.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\Rsrc.res"

"$(OUTDIR)\PDW2_5.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\PDW2_5.exe" "$(OUTDIR)\PDW2_5.bsc"


CLEAN :
	-@erase "$(INTDIR)\acars.obj"
	-@erase "$(INTDIR)\acars.sbr"
	-@erase "$(INTDIR)\binary.obj"
	-@erase "$(INTDIR)\binary.sbr"
	-@erase "$(INTDIR)\Debug.obj"
	-@erase "$(INTDIR)\Debug.sbr"
	-@erase "$(INTDIR)\decode.obj"
	-@erase "$(INTDIR)\decode.sbr"
	-@erase "$(INTDIR)\ermes.obj"
	-@erase "$(INTDIR)\ermes.sbr"
	-@erase "$(INTDIR)\Flex.obj"
	-@erase "$(INTDIR)\Flex.sbr"
	-@erase "$(INTDIR)\Gfx.obj"
	-@erase "$(INTDIR)\Gfx.sbr"
	-@erase "$(INTDIR)\helper_funcs.obj"
	-@erase "$(INTDIR)\helper_funcs.sbr"
	-@erase "$(INTDIR)\Initapp.obj"
	-@erase "$(INTDIR)\Initapp.sbr"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\language.sbr"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menu.sbr"
	-@erase "$(INTDIR)\Misc.obj"
	-@erase "$(INTDIR)\Misc.sbr"
	-@erase "$(INTDIR)\mobitex.obj"
	-@erase "$(INTDIR)\mobitex.sbr"
	-@erase "$(INTDIR)\Ostype.obj"
	-@erase "$(INTDIR)\Ostype.sbr"
	-@erase "$(INTDIR)\PDW.obj"
	-@erase "$(INTDIR)\PDW.sbr"
	-@erase "$(INTDIR)\Pocsag.obj"
	-@erase "$(INTDIR)\Pocsag.sbr"
	-@erase "$(INTDIR)\printer.obj"
	-@erase "$(INTDIR)\printer.sbr"
	-@erase "$(INTDIR)\rs232.obj"
	-@erase "$(INTDIR)\rs232.sbr"
	-@erase "$(INTDIR)\Rsrc.res"
	-@erase "$(INTDIR)\sigind.obj"
	-@erase "$(INTDIR)\sigind.sbr"
	-@erase "$(INTDIR)\smtp.obj"
	-@erase "$(INTDIR)\smtp.sbr"
	-@erase "$(INTDIR)\sound_in.obj"
	-@erase "$(INTDIR)\sound_in.sbr"
	-@erase "$(INTDIR)\toolbar.obj"
	-@erase "$(INTDIR)\toolbar.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PDW2_5.bsc"
	-@erase "$(OUTDIR)\PDW2_5.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\PDW.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x809 /fo"$(INTDIR)\Rsrc.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PDW2_5.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\acars.sbr" \
	"$(INTDIR)\binary.sbr" \
	"$(INTDIR)\Debug.sbr" \
	"$(INTDIR)\decode.sbr" \
	"$(INTDIR)\ermes.sbr" \
	"$(INTDIR)\Flex.sbr" \
	"$(INTDIR)\Gfx.sbr" \
	"$(INTDIR)\helper_funcs.sbr" \
	"$(INTDIR)\Initapp.sbr" \
	"$(INTDIR)\language.sbr" \
	"$(INTDIR)\menu.sbr" \
	"$(INTDIR)\Misc.sbr" \
	"$(INTDIR)\mobitex.sbr" \
	"$(INTDIR)\Ostype.sbr" \
	"$(INTDIR)\PDW.sbr" \
	"$(INTDIR)\Pocsag.sbr" \
	"$(INTDIR)\printer.sbr" \
	"$(INTDIR)\rs232.sbr" \
	"$(INTDIR)\sigind.sbr" \
	"$(INTDIR)\smtp.sbr" \
	"$(INTDIR)\sound_in.sbr" \
	"$(INTDIR)\toolbar.sbr"

"$(OUTDIR)\PDW2_5.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=winmm.lib comctl32.lib comdlg32.lib kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"$(OUTDIR)\PDW2_5.exe" 
LINK32_OBJS= \
	"$(INTDIR)\acars.obj" \
	"$(INTDIR)\binary.obj" \
	"$(INTDIR)\Debug.obj" \
	"$(INTDIR)\decode.obj" \
	"$(INTDIR)\ermes.obj" \
	"$(INTDIR)\Flex.obj" \
	"$(INTDIR)\Gfx.obj" \
	"$(INTDIR)\helper_funcs.obj" \
	"$(INTDIR)\Initapp.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\Misc.obj" \
	"$(INTDIR)\mobitex.obj" \
	"$(INTDIR)\Ostype.obj" \
	"$(INTDIR)\PDW.obj" \
	"$(INTDIR)\Pocsag.obj" \
	"$(INTDIR)\printer.obj" \
	"$(INTDIR)\rs232.obj" \
	"$(INTDIR)\sigind.obj" \
	"$(INTDIR)\smtp.obj" \
	"$(INTDIR)\sound_in.obj" \
	"$(INTDIR)\toolbar.obj" \
	"$(INTDIR)\Rsrc.res"

"$(OUTDIR)\PDW2_5.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("PDW.dep")
!INCLUDE "PDW.dep"
!ELSE 
!MESSAGE Warning: cannot find "PDW.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "PDW2_5 - Win32 Release" || "$(CFG)" == "PDW2_5 - Win32 Debug"
SOURCE=.\acars.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\acars.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\acars.obj"	"$(INTDIR)\acars.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\utils\binary.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\binary.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\binary.obj"	"$(INTDIR)\binary.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\utils\Debug.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\Debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\Debug.obj"	"$(INTDIR)\Debug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\decode.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\decode.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\decode.obj"	"$(INTDIR)\decode.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ermes.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\ermes.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\ermes.obj"	"$(INTDIR)\ermes.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Flex.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\Flex.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\Flex.obj"	"$(INTDIR)\Flex.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Gfx.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\Gfx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\Gfx.obj"	"$(INTDIR)\Gfx.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\helper_funcs.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\helper_funcs.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\helper_funcs.obj"	"$(INTDIR)\helper_funcs.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\html.cpp
SOURCE=.\Initapp.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\Initapp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\Initapp.obj"	"$(INTDIR)\Initapp.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\language.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\language.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\language.obj"	"$(INTDIR)\language.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\menu.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\menu.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\menu.obj"	"$(INTDIR)\menu.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Misc.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\Misc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\Misc.obj"	"$(INTDIR)\Misc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mobitex.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\mobitex.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\mobitex.obj"	"$(INTDIR)\mobitex.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\utils\Ostype.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\Ostype.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\Ostype.obj"	"$(INTDIR)\Ostype.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\PDW.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\PDW.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\PDW.obj"	"$(INTDIR)\PDW.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Pocsag.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\Pocsag.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\Pocsag.obj"	"$(INTDIR)\Pocsag.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\printer.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\printer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\printer.obj"	"$(INTDIR)\printer.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\utils\rs232.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\rs232.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\rs232.obj"	"$(INTDIR)\rs232.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Rsrc.rc

"$(INTDIR)\Rsrc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\sigind.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\sigind.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\sigind.obj"	"$(INTDIR)\sigind.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\utils\smtp.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\smtp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\smtp.obj"	"$(INTDIR)\smtp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\sound_in.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\sound_in.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\sound_in.obj"	"$(INTDIR)\sound_in.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\toolbar.cpp

!IF  "$(CFG)" == "PDW2_5 - Win32 Release"


"$(INTDIR)\toolbar.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PDW2_5 - Win32 Debug"


"$(INTDIR)\toolbar.obj"	"$(INTDIR)\toolbar.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

