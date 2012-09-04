# Microsoft Developer Studio Project File - Name="bec.WindowsMessageBox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bec.WindowsMessageBox - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bec.WindowsMessageBox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bec.WindowsMessageBox.mak" CFG="bec.WindowsMessageBox - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bec.WindowsMessageBox - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Release Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Debug Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Unicode Release Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Unicode Debug Multithreaded" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Unicode Release Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Unicode Debug Multithreaded DLL" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Release NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Debug NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Release Multithreaded NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Debug Multithreaded NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Release Multithreaded DLL NoX" (based on "Win32 (x86) Static Library")
!MESSAGE "bec.WindowsMessageBox - Win32 Debug Multithreaded DLL NoX" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Release Multithreaded"

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
# ADD BASE CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Debug Multithreaded"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Release Multithreaded DLL"

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
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Debug Multithreaded DLL"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Unicode Release"

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
# ADD BASE CPP /nologo /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Unicode Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Unicode Release Multithreaded"

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
# ADD BASE CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.mt.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.mt.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Unicode Debug Multithreaded"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.mt.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.mt.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Unicode Release Multithreaded DLL"

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
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.dll.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.dll.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Unicode Debug Multithreaded DLL"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.widestring.dll.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Release NoX"

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
# ADD BASE CPP /nologo /W3 /WX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.nox.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.nox.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Debug NoX"

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
# ADD BASE CPP /nologo /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.nox.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.nox.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Release Multithreaded NoX"

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
# ADD BASE CPP /nologo /MT /W3 /WX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.nox.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.nox.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Debug Multithreaded NoX"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.nox.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.mt.nox.debug.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Release Multithreaded DLL NoX"

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
# ADD BASE CPP /nologo /MD /W3 /WX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.nox.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.nox.lib"

!ELSEIF  "$(CFG)" == "bec.WindowsMessageBox - Win32 Debug Multithreaded DLL NoX"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.nox.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bec.WindowsMessageBox.vc6.dll.nox.debug.lib"

!ENDIF

# Begin Target

# Name "bec.WindowsMessageBox - Win32 Release"
# Name "bec.WindowsMessageBox - Win32 Debug"
# Name "bec.WindowsMessageBox - Win32 Release Multithreaded"
# Name "bec.WindowsMessageBox - Win32 Debug Multithreaded"
# Name "bec.WindowsMessageBox - Win32 Release Multithreaded DLL"
# Name "bec.WindowsMessageBox - Win32 Debug Multithreaded DLL"
# Name "bec.WindowsMessageBox - Win32 Unicode Release"
# Name "bec.WindowsMessageBox - Win32 Unicode Debug"
# Name "bec.WindowsMessageBox - Win32 Unicode Release Multithreaded"
# Name "bec.WindowsMessageBox - Win32 Unicode Debug Multithreaded"
# Name "bec.WindowsMessageBox - Win32 Unicode Release Multithreaded DLL"
# Name "bec.WindowsMessageBox - Win32 Unicode Debug Multithreaded DLL"
# Name "bec.WindowsMessageBox - Win32 Release NoX"
# Name "bec.WindowsMessageBox - Win32 Debug NoX"
# Name "bec.WindowsMessageBox - Win32 Release Multithreaded NoX"
# Name "bec.WindowsMessageBox - Win32 Debug Multithreaded NoX"
# Name "bec.WindowsMessageBox - Win32 Release Multithreaded DLL NoX"
# Name "bec.WindowsMessageBox - Win32 Debug Multithreaded DLL NoX"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\src\backends\bec.WindowsMessageBox.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "STLSoft Header Files"

# PROP Default_Filter ""
# Begin Group "STLSoft"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\intel.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\msvc.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\obsolete.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\safestr.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\stlsoft.h"
# End Source File
# End Group
# End Group
# Begin Group "Pantheios Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\backends\arguments.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backend.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backends\bec.WindowsMessageBox.h"
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
