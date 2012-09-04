# Microsoft Developer Studio Project File - Name="bel.syslog" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bel.syslog - Win32 Release pseudoUNIX
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bel.syslog.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bel.syslog.mak" CFG="bel.syslog - Win32 Release pseudoUNIX"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bel.syslog - Win32 Release pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "bel.syslog - Win32 Debug pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "bel.syslog - Win32 Release Multithreaded pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "bel.syslog - Win32 Debug Multithreaded pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "bel.syslog - Win32 Release Multithreaded DLL pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE "bel.syslog - Win32 Debug Multithreaded DLL pseudoUNIX" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bel.syslog - Win32 Release pseudoUNIX"

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
# ADD BASE CPP /nologo /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.lib"

!ELSEIF  "$(CFG)" == "bel.syslog - Win32 Debug pseudoUNIX"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.debug.lib"

!ELSEIF  "$(CFG)" == "bel.syslog - Win32 Release Multithreaded pseudoUNIX"

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
# ADD BASE CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.mt.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.mt.lib"

!ELSEIF  "$(CFG)" == "bel.syslog - Win32 Debug Multithreaded pseudoUNIX"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.mt.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.mt.debug.lib"

!ELSEIF  "$(CFG)" == "bel.syslog - Win32 Release Multithreaded DLL pseudoUNIX"

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
# ADD BASE CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.dll.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.dll.lib"

!ELSEIF  "$(CFG)" == "bel.syslog - Win32 Debug Multithreaded DLL pseudoUNIX"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../../../include" /I "$(STLSOFT)/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.dll.debug.lib"
# ADD LIB32 /nologo /out:"../../../../lib/pantheios.1.bel.syslog.vc6.unix.dll.debug.lib"

!ENDIF

# Begin Target

# Name "bel.syslog - Win32 Release pseudoUNIX"
# Name "bel.syslog - Win32 Debug pseudoUNIX"
# Name "bel.syslog - Win32 Release Multithreaded pseudoUNIX"
# Name "bel.syslog - Win32 Debug Multithreaded pseudoUNIX"
# Name "bel.syslog - Win32 Release Multithreaded DLL pseudoUNIX"
# Name "bel.syslog - Win32 Debug Multithreaded DLL pseudoUNIX"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\src\backends\bel\bel.syslog.c
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

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backends\be.lrsplit.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backends\bec.syslog.h"
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
