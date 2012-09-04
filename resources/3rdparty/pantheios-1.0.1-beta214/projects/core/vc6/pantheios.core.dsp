# Microsoft Developer Studio Project File - Name="pantheios.core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pantheios.core - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pantheios.core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pantheios.core.mak" CFG="pantheios.core - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pantheios.core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Unicode Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Unicode Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Unicode Release Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Unicode Debug Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release Multithreaded NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug Multithreaded NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release Multithreaded DLL NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug Multithreaded DLL NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release Multithreaded pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug Multithreaded pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Release Multithreaded DLL pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "pantheios.core - Win32 Debug Multithreaded DLL pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pantheios.core - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded"

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
# ADD BASE CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL"

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
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "URelease"
# PROP BASE Intermediate_Dir "URelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "URelease"
# PROP Intermediate_Dir "URelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebug"
# PROP BASE Intermediate_Dir "UDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebug"
# PROP Intermediate_Dir "UDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UReleaseMT"
# PROP BASE Intermediate_Dir "UReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UReleaseMT"
# PROP Intermediate_Dir "UReleaseMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.mt.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.mt.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebugMT"
# PROP BASE Intermediate_Dir "UDebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebugMT"
# PROP Intermediate_Dir "UDebugMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.mt.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.mt.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UReleaseDLL"
# PROP BASE Intermediate_Dir "UReleaseDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UReleaseDLL"
# PROP Intermediate_Dir "UReleaseDLL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.dll.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.dll.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebugDLL"
# PROP BASE Intermediate_Dir "UDebugDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebugDLL"
# PROP Intermediate_Dir "UDebugDLL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.widestring.dll.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release NoX"

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
# ADD BASE CPP /nologo /W3 /WX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.nox.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.nox.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug NoX"

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
# ADD BASE CPP /nologo /W3 /Gm /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.nox.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.nox.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded NoX"

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
# ADD BASE CPP /nologo /MT /W3 /WX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.nox.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.nox.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded NoX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugMTNoX"
# PROP BASE Intermediate_Dir "DebugMTNoX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMTNoX"
# PROP Intermediate_Dir "DebugMTNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.nox.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.mt.nox.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL NoX"

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
# ADD BASE CPP /nologo /MD /W3 /WX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /O2 /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.nox.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.nox.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL NoX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLLNoX"
# PROP BASE Intermediate_Dir "DebugDLLNoX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLLNoX"
# PROP Intermediate_Dir "DebugDLLNoX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.nox.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.dll.nox.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUNIX"
# PROP BASE Intermediate_Dir "ReleaseUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUNIX"
# PROP Intermediate_Dir "ReleaseUNIX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugUNIX"
# PROP BASE Intermediate_Dir "DebugUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUNIX"
# PROP Intermediate_Dir "DebugUNIX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMTUNIX"
# PROP BASE Intermediate_Dir "ReleaseMTUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMTUNIX"
# PROP Intermediate_Dir "ReleaseMTUNIX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.mt.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.mt.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded pseudoUNIX"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.mt.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.mt.debug.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLLUNIX"
# PROP BASE Intermediate_Dir "ReleaseDLLUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLLUNIX"
# PROP Intermediate_Dir "ReleaseDLLUNIX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.dll.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.dll.lib"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLLUNIX"
# PROP BASE Intermediate_Dir "DebugDLLUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLLUNIX"
# PROP Intermediate_Dir "DebugDLLUNIX"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../lib/pantheios.1.core.vc6.unix.dll.debug.lib"

!ENDIF 

# Begin Target

# Name "pantheios.core - Win32 Release"
# Name "pantheios.core - Win32 Debug"
# Name "pantheios.core - Win32 Release Multithreaded"
# Name "pantheios.core - Win32 Debug Multithreaded"
# Name "pantheios.core - Win32 Release Multithreaded DLL"
# Name "pantheios.core - Win32 Debug Multithreaded DLL"
# Name "pantheios.core - Win32 Unicode Release"
# Name "pantheios.core - Win32 Unicode Debug"
# Name "pantheios.core - Win32 Unicode Release Multithreaded"
# Name "pantheios.core - Win32 Unicode Debug Multithreaded"
# Name "pantheios.core - Win32 Unicode Release Multithreaded DLL"
# Name "pantheios.core - Win32 Unicode Debug Multithreaded DLL"
# Name "pantheios.core - Win32 Release NoX"
# Name "pantheios.core - Win32 Debug NoX"
# Name "pantheios.core - Win32 Release Multithreaded NoX"
# Name "pantheios.core - Win32 Debug Multithreaded NoX"
# Name "pantheios.core - Win32 Release Multithreaded DLL NoX"
# Name "pantheios.core - Win32 Debug Multithreaded DLL NoX"
# Name "pantheios.core - Win32 Release pseudoUNIX"
# Name "pantheios.core - Win32 Debug pseudoUNIX"
# Name "pantheios.core - Win32 Release Multithreaded pseudoUNIX"
# Name "pantheios.core - Win32 Debug Multithreaded pseudoUNIX"
# Name "pantheios.core - Win32 Release Multithreaded DLL pseudoUNIX"
# Name "pantheios.core - Win32 Debug Multithreaded DLL pseudoUNIX"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\core\api.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\api.deprecated.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\api.exitprocess.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\api.logputs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\auto.cpp
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\generated\log_functions.c"
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\src\core\logprintf.c
# End Source File
# End Group
# Begin Group "inserters"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\src\inserters\args.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\b64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\blob.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\boolean.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\hostid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\integer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\m2w.cpp

!IF  "$(CFG)" == "pantheios.core - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release Multithreaded"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug Multithreaded"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release Multithreaded DLL"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug Multithreaded DLL"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release NoX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug NoX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded NoX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded NoX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL NoX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL NoX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release pseudoUNIX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug pseudoUNIX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded pseudoUNIX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded pseudoUNIX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL pseudoUNIX"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL pseudoUNIX"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\pointer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\processid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\real.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\slice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\threadid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\src\inserters\w2m.cpp

!IF  "$(CFG)" == "pantheios.core - Win32 Release"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release Multithreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug Multithreaded"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Release Multithreaded DLL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Unicode Debug Multithreaded DLL"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release NoX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug NoX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded NoX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded NoX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL NoX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL NoX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release pseudoUNIX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug pseudoUNIX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded pseudoUNIX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded pseudoUNIX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Release Multithreaded DLL pseudoUNIX"

!ELSEIF  "$(CFG)" == "pantheios.core - Win32 Debug Multithreaded DLL pseudoUNIX"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\src\inserters\exception.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "STLSoft Header Files"

# PROP Default_Filter ""
# Begin Group "STLSoft"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\memory\allocator_base.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\memory\allocator_features.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\memory\allocator_selector.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\algorithms\std\alt.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\memory\auto_buffer.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\basic_string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\algorithms\bounded.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\conversion\byte_format_functions.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\c_string.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\capabilities.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\conversion\char_conversions.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\string\char_traits.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\collections\util\collections.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\synch\concepts.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\constraints.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\error\conversion_error.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\iterators\cstring_concatenator_iterator.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\exception.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\fwd.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\conversion\integer_to_string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\integral_printf_traits.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\intel.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_const_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_integral_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_pointer_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_same_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\iterator_generators.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\iterator_helper.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\library_discriminator.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\synch\lock_scope.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\iterators\member_selector_iterator.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\util\meta_.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\minmax.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\msvc.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\n_types.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\synch\null_mutex.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\obsolete.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\algorithms\pod.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\safestr.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\conversion\sap_cast.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\smartptr\scoped_handle.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\string\shim_string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\sign_traits.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\string\simple_string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\size_of.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\size_traits.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\synch\spin_policies.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std_swap.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\stlsoft.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\string\string_traits_fwd.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\string\string_view.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\conversion\union_cast.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\yesno.hpp"
# End Source File
# End Group
# Begin Group "PlatformSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\synch\atomic_functions.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\synch\atomic_types.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\synch\util\features.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\platformstl.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\platformstl.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\synch\spin_mutex.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\synch\thread_mutex.hpp"
# End Source File
# End Group
# Begin Group "UNIXSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\unixstl\synch\util\features.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\unixstl\unixstl.h"
# End Source File
# End Group
# Begin Group "WinSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\synch\atomic_functions.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\synch\atomic_types.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\synch\spin_mutex.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\synch\thread_mutex.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\winstl.h"
# End Source File
# End Group
# End Group
# Begin Group "b64 Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\b64\b64.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\b64.hpp"
# End Source File
# End Group
# Begin Group "Pantheios Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\args.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\memory\auto_buffer_selector.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backend.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\blob.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\boolean.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\quality\contract.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\exception.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\fmt.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\frontend.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\hostid.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\system\hostname.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\init_codes.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\initialiser.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\integer.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\lean.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\severity\levels.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\generated\log_dispatch_functions.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\generated\log_dispatch_functions.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\generated\log_functions.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\generated\log_functions.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\generated\log_sev_functions.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\m2w.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\nox.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\pantheios.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\pantheios.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\pointer.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\system\processid.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\processid.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\real.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\safestr.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\slice.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\string\snprintf.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\stock_levels.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\string\strdup.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\string_encoding.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\system\threadid.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\threadid.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\threading.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\inserters\w2m.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\winlean.h"
# End Source File
# End Group
# Begin Group "UNIXem Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(UNIXEM)\include\unistd.h"
# End Source File
# End Group
# Begin Group "Synesis Software Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(SYNESIS_DEV)\libraries\include\syslog.h"
# End Source File
# End Group
# End Group
# Begin Group "Script Files"

# PROP Default_Filter "pl;py;rb;vbs"
# Begin Source File

SOURCE="..\..\..\scripts\make_hdrs.rb"
# End Source File
# End Group
# End Target
# End Project
