# Microsoft Developer Studio Project File - Name="test.scratch.extras.com.invoke_nothrow_method" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=test.scratch.extras.com.invoke_nothrow_method - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test.scratch.extras.com.invoke_nothrow_method.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test.scratch.extras.com.invoke_nothrow_method.mak" CFG="test.scratch.extras.com.invoke_nothrow_method - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Release Multithreaded" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug Multithreaded" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Release Multithreaded DLL" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug Multithreaded DLL" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release Multithreaded" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug Multithreaded" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release Multithreaded DLL" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug Multithreaded DLL" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMT"
# PROP BASE Intermediate_Dir "ReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMT"
# PROP Intermediate_Dir "ReleaseMT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.mt.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.mt.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugMT"
# PROP BASE Intermediate_Dir "DebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMT"
# PROP Intermediate_Dir "DebugMT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.mt.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.mt.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Release Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLL"
# PROP BASE Intermediate_Dir "ReleaseDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLL"
# PROP Intermediate_Dir "ReleaseDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.dll.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.dll.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLL"
# PROP BASE Intermediate_Dir "DebugDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLL"
# PROP Intermediate_Dir "DebugDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.dll.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.dll.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "URelease"
# PROP BASE Intermediate_Dir "URelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "URelease"
# PROP Intermediate_Dir "URelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebug"
# PROP BASE Intermediate_Dir "UDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebug"
# PROP Intermediate_Dir "UDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UReleaseMT"
# PROP BASE Intermediate_Dir "UReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UReleaseMT"
# PROP Intermediate_Dir "UReleaseMT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.mt.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.mt.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebugMT"
# PROP BASE Intermediate_Dir "UDebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebugMT"
# PROP Intermediate_Dir "UDebugMT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.mt.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.mt.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "UReleaseDLL"
# PROP BASE Intermediate_Dir "UReleaseDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "UReleaseDLL"
# PROP Intermediate_Dir "UReleaseDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.dll.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.dll.exe" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ELSEIF  "$(CFG)" == "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "UDebugDLL"
# PROP BASE Intermediate_Dir "UDebugDLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "UDebugDLL"
# PROP Intermediate_Dir "UDebugDLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(SHWILD_ROOT)/include" /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_DEBUG" /D "_UNICODE" /D "UNICODE" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.dll.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 
# ADD LINK32 advapi32.lib kernel32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.extras.com.invoke_nothrow_method.vc6.widestring.dll.debug.exe" /pdbtype:sept /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" 

!ENDIF

# Begin Target

# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Release"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Release Multithreaded"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug Multithreaded"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Release Multithreaded DLL"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Debug Multithreaded DLL"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release Multithreaded"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug Multithreaded"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Release Multithreaded DLL"
# Name "test.scratch.extras.com.invoke_nothrow_method - Win32 Unicode Debug Multithreaded DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\implicit_link.cpp
# End Source File
# Begin Source File

SOURCE=..\test.scratch.extras.com.invoke_nothrow_method.cpp
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

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\c_string.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\string\char_traits.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\collections\util\collections.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\constraints.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\error\conversion_error.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\string\cstring_maker.hpp"
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

SOURCE="$(STLSOFT)\include\stlsoft\util\std\iterator_generators.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\iterator_helper.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\library_discriminator.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\minmax.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\msvc.h"
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

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\time.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\conversion\union_cast.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\yesno.hpp"
# End Source File
# End Group
# Begin Group "COMSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\comstl\string\BSTR_functions.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\comstl\comstl.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\comstl\shims\access\string\guid.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\comstl\shims\access\string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\comstl\shims\access\string\variant.hpp"
# End Source File
# End Group
# Begin Group "PlatformSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\platformstl.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\platformstl.hpp"
# End Source File
# End Group
# Begin Group "WinSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\error\conversion_error.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\registry\util\defs.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\shims\conversion\to_SYSTEMTIME\FILETIME.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\time\format_functions.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\window\functions.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\registry\functions.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\shims\access\string\HWND.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\window\util\ident_.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\memory\processheap_allocator.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\registry\reg_traits.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\shims\access\string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\system\system_traits.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\shims\access\string\time.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\conversion\windows_type_conversions.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\winstl\winstl.h"
# End Source File
# End Group
# End Group
# Begin Group "Pantheios Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\be.fprintf.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\bec.fprintf.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\quality\contract.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\core.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\extras\com\exception_helpers.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\fe.simple.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\implicit_link_base_.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\initialiser.hpp"
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

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\pantheios.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\pantheios.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\stock_levels.hpp"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\internal\string_encoding.h"
# End Source File
# End Group
# End Group
# End Target
# End Project
