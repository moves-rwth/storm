# Microsoft Developer Studio Project File - Name="be.N" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=be.N - Win32 Debug Multithreaded pseudoUNIX
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "be.N.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "be.N.mak" CFG="be.N - Win32 Debug Multithreaded pseudoUNIX"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "be.N - Win32 Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Debug Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Release Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Release Multithreaded DLL Nox" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Release Multithreaded Nox" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Release Nox" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Debug Multithreaded DLL Nox" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Debug Multithreaded Nox" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Debug Nox" (based on "Win32 (x86) Static Library")
!MESSAGE "be.N - Win32 Debug Multithreaded pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "be.N - Win32 Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugMT"
# PROP BASE Intermediate_Dir "DebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMT"
# PROP Intermediate_Dir "DebugMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"pantheios/pantheios.hpp" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.mt.debug.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMT"
# PROP BASE Intermediate_Dir "ReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMT"
# PROP Intermediate_Dir "ReleaseMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"pantheios/pantheios.hpp" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.mt.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Debug Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLL"
# PROP BASE Intermediate_Dir "DebugDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLL"
# PROP Intermediate_Dir "DebugDLL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"pantheios/pantheios.hpp" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.dll.debug.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Release Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLL"
# PROP BASE Intermediate_Dir "ReleaseDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLL"
# PROP Intermediate_Dir "ReleaseDLL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"pantheios/pantheios.hpp" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.dll.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"pantheios/pantheios.hpp" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.debug.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"pantheios/pantheios.hpp" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Release Multithreaded DLL Nox"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLLNoX"
# PROP BASE Intermediate_Dir "ReleaseDLLNoX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLLNoX"
# PROP Intermediate_Dir "ReleaseDLLNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PANTHEIOS_MIN_CRT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.dll.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.dll.nox.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Release Multithreaded Nox"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMTNoX"
# PROP BASE Intermediate_Dir "ReleaseMTNoX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMTNoX"
# PROP Intermediate_Dir "ReleaseMTNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PANTHEIOS_MIN_CRT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.mt.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.mt.nox.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Release Nox"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseNoX"
# PROP BASE Intermediate_Dir "ReleaseNoX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseNoX"
# PROP Intermediate_Dir "ReleaseNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /W3 /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PANTHEIOS_MIN_CRT" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.nox.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Debug Multithreaded DLL Nox"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLLNox"
# PROP BASE Intermediate_Dir "DebugDLLNox"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLLNoX"
# PROP Intermediate_Dir "DebugDLLNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PANTHEIOS_MIN_CRT" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.dll.nox.debug.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Debug Multithreaded Nox"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugMTNox"
# PROP BASE Intermediate_Dir "DebugMTNox"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMTNoX"
# PROP Intermediate_Dir "DebugMTNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PANTHEIOS_MIN_CRT" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.mt.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.mt.nox.debug.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Debug Nox"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugNoX"
# PROP BASE Intermediate_Dir "DebugNoX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugNoX"
# PROP Intermediate_Dir "DebugNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PANTHEIOS_MIN_CRT" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.nox.debug.lib"

!ELSEIF  "$(CFG)" == "be.N - Win32 Debug Multithreaded pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugMTUNIX"
# PROP BASE Intermediate_Dir "DebugMTUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMTUNIX"
# PROP Intermediate_Dir "DebugMTUNIX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "PANTHEIOS_MIN_CRT" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "UNIXSTL_NO_WIN32_NATIVE_ATOMIC_FUNCTIONS" /D "_REENTRANT" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.be.N.vc6.unix.mt.debug.lib"

!ENDIF

# Begin Target

# Name "be.N - Win32 Debug Multithreaded"
# Name "be.N - Win32 Release Multithreaded"
# Name "be.N - Win32 Debug Multithreaded DLL"
# Name "be.N - Win32 Release Multithreaded DLL"
# Name "be.N - Win32 Debug"
# Name "be.N - Win32 Release"
# Name "be.N - Win32 Release Multithreaded DLL Nox"
# Name "be.N - Win32 Release Multithreaded Nox"
# Name "be.N - Win32 Release Nox"
# Name "be.N - Win32 Debug Multithreaded DLL Nox"
# Name "be.N - Win32 Debug Multithreaded Nox"
# Name "be.N - Win32 Debug Nox"
# Name "be.N - Win32 Debug Multithreaded pseudoUNIX"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\src\backends\be.N.c
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "STLSoft Header Files"

# PROP Default_Filter ""
# Begin Group "STLSoft"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\msvc.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\obsolete.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\stlsoft.h"
# End Source File
# End Group
# End Group
# Begin Group "Pantheios Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backend.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backends\be.N.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\quality\contract.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\frontend.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\init_codes.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\generated\log_functions.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\pantheios.h"
# End Source File
# End Group
# End Group
# End Target
# End Project
