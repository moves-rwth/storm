# Microsoft Developer Studio Project File - Name="test.scratch.be.syslog.WithCallback" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded DLL pseudoUNIX
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test.scratch.be.syslog.WithCallback.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test.scratch.be.syslog.WithCallback.mak" CFG="test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded DLL pseudoUNIX"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test.scratch.be.syslog.WithCallback - Win32 Release pseudoUNIX" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.syslog.WithCallback - Win32 Debug pseudoUNIX" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.syslog.WithCallback - Win32 Release Multithreaded pseudoUNIX" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded pseudoUNIX" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.syslog.WithCallback - Win32 Release Multithreaded DLL pseudoUNIX" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded DLL pseudoUNIX" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test.scratch.be.syslog.WithCallback - Win32 Release pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseUNIX"
# PROP BASE Intermediate_Dir "ReleaseUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseUNIX"
# PROP Intermediate_Dir "ReleaseUNIX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.syslog.WithCallback - Win32 Debug pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugUNIX"
# PROP BASE Intermediate_Dir "DebugUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugUNIX"
# PROP Intermediate_Dir "DebugUNIX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.syslog.WithCallback - Win32 Release Multithreaded pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMTUNIX"
# PROP BASE Intermediate_Dir "ReleaseMTUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMTUNIX"
# PROP Intermediate_Dir "ReleaseMTUNIX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.mt.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.mt.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.mt.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugMTUNIX"
# PROP BASE Intermediate_Dir "DebugMTUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMTUNIX"
# PROP Intermediate_Dir "DebugMTUNIX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.mt.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.mt.debug.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.mt.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.syslog.WithCallback - Win32 Release Multithreaded DLL pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDLLUNIX"
# PROP BASE Intermediate_Dir "ReleaseDLLUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDLLUNIX"
# PROP Intermediate_Dir "ReleaseDLLUNIX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.dll.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.dll.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.dll.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded DLL pseudoUNIX"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDLLUNIX"
# PROP BASE Intermediate_Dir "DebugDLLUNIX"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDLLUNIX"
# PROP Intermediate_Dir "DebugDLLUNIX"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /I "$(UNIXEM)/include" /I "$(PTHREADS_WIN32)/include" /I "$(SYNESIS_DEV)/libraries/include" /D "UNIX" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.dll.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.dll.debug.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.syslog.WithCallback.vc6.unix.dll.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(UNIXEM)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ENDIF 

# Begin Target

# Name "test.scratch.be.syslog.WithCallback - Win32 Release pseudoUNIX"
# Name "test.scratch.be.syslog.WithCallback - Win32 Debug pseudoUNIX"
# Name "test.scratch.be.syslog.WithCallback - Win32 Release Multithreaded pseudoUNIX"
# Name "test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded pseudoUNIX"
# Name "test.scratch.be.syslog.WithCallback - Win32 Release Multithreaded DLL pseudoUNIX"
# Name "test.scratch.be.syslog.WithCallback - Win32 Debug Multithreaded DLL pseudoUNIX"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\implicit_link.cpp
# End Source File
# Begin Source File

SOURCE=..\test.scratch.be.syslog.WithCallback.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "STLSoft Header Files"

# PROP Default_Filter ""
# Begin Group "STLSoft"

# PROP Default_Filter ""
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

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\c_string.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\capabilities.hpp"
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

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\exception.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\fwd.h"
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

SOURCE="$(STLSOFT)\include\stlsoft\util\std\iterator_helper.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\library_discriminator.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\util\meta_.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\cccap\msvc.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\n_types.hpp"
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

SOURCE="$(STLSOFT)\include\stlsoft\string\shim_string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\sign_traits.hpp"
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

SOURCE="$(STLSOFT)\include\stlsoft\shims\access\string\std\time.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\yesno.hpp"
# End Source File
# End Group
# Begin Group "PlatformSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\platformstl.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\platformstl\synch\sleep_functions.h"
# End Source File
# End Group
# Begin Group "UNIXSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(STLSOFT)\include\unixstl\shims\access\string\dirent.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\unixstl\synch\sleep_functions.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\unixstl\shims\access\string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\unixstl\unixstl.h"
# End Source File
# End Group
# End Group
# Begin Group "Pantheios Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backend.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\be.syslog.WithCallback.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backends\bec.syslog.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\bec.syslog.WithCallback.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\test\compiler_warnings_suppression.first_include.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\util\test\compiler_warnings_suppression.last_include.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\quality\contract.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\core.h"
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
# Begin Group "UNIXem Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(UNIXEM)\include\dirent.h"
# End Source File
# Begin Source File

SOURCE="$(UNIXEM)\include\unixem\dirent.h"
# End Source File
# Begin Source File

SOURCE="$(UNIXEM)\include\unixem\implicit_link.h"
# End Source File
# Begin Source File

SOURCE="$(UNIXEM)\include\sys\select.h"
# End Source File
# Begin Source File

SOURCE="$(UNIXEM)\include\unixem\unixem.h"
# End Source File
# End Group
# Begin Group "xTests Header Files"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE="$(SYNESIS_DEV)\libraries\include\syslog.h"
# End Source File
# End Group
# End Target
# End Project
