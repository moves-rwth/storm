# Microsoft Developer Studio Project File - Name="test.scratch.be.WindowsSyslog" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "test.scratch.be.WindowsSyslog.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "test.scratch.be.WindowsSyslog.mak" CFG="test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded DLL"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "test.scratch.be.WindowsSyslog - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.WindowsSyslog - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.WindowsSyslog - Win32 Release Multithreaded" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.WindowsSyslog - Win32 Release Multithreaded DLL" (based on "Win32 (x86) Application")
!MESSAGE "test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded DLL" (based on "Win32 (x86) Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test.scratch.be.WindowsSyslog - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.WindowsSyslog - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.WindowsSyslog - Win32 Release Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseMt"
# PROP BASE Intermediate_Dir "ReleaseMt"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMt"
# PROP Intermediate_Dir "ReleaseMt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.mt.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.mt.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.mt.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugMt"
# PROP BASE Intermediate_Dir "DebugMt"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMt"
# PROP Intermediate_Dir "DebugMt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.mt.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.mt.debug.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.mt.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.WindowsSyslog - Win32 Release Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ReleaseDll"
# PROP BASE Intermediate_Dir "ReleaseDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDll"
# PROP Intermediate_Dir "ReleaseDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.dll.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.dll.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.dll.exe" /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ELSEIF  "$(CFG)" == "test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DebugDll"
# PROP BASE Intermediate_Dir "DebugDll"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugDll"
# PROP Intermediate_Dir "DebugDll"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /I "$(XTESTS_C_ROOT)/include" /I "../../../../include" /I "$(STLSOFT)/include" /D "WIN32" /D "_STLSOFT_FORCE_ANY_COMPILER" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib advapi32.lib ole32.lib oleaut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.dll.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib"
# ADD LINK32 libWinSysLog.vc6.dll.debug.lib pthreadVC.lib kernel32.lib advapi32.lib ole32.lib oleaut32.lib ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../../../../bin/test.scratch.be.WindowsSyslog.vc6.dll.debug.exe" /pdbtype:sept /libpath:"$(XTESTS_C_ROOT)/lib" /libpath:"$(PANTHEIOS_ROOT)/lib" /libpath:"$(SHWILD_ROOT)/lib" /libpath:"$(SYNESIS_DEV)/lib" /libpath:"$(PTHREADS_WIN32)/lib"

!ENDIF

# Begin Target

# Name "test.scratch.be.WindowsSyslog - Win32 Release"
# Name "test.scratch.be.WindowsSyslog - Win32 Debug"
# Name "test.scratch.be.WindowsSyslog - Win32 Release Multithreaded"
# Name "test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded"
# Name "test.scratch.be.WindowsSyslog - Win32 Release Multithreaded DLL"
# Name "test.scratch.be.WindowsSyslog - Win32 Debug Multithreaded DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\implicit_link.cpp
# End Source File
# Begin Source File

SOURCE=..\test.scratch.be.WindowsSyslog.cpp
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

SOURCE="$(STLSOFT)\include\stlsoft\internal\head_version.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\conversion\integer_to_string.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_bool_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_character_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_floating_point_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_fundamental_type.hpp"
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

SOURCE="$(STLSOFT)\include\stlsoft\meta\is_void_type.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\iterator_generators.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\iterator_helper.hpp"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\internal\lead_version.h"
# End Source File
# Begin Source File

SOURCE="$(STLSOFT)\include\stlsoft\util\std\library_discriminator.hpp"
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

SOURCE="$(STLSOFT)\include\platformstl\synch\sleep_functions.h"
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

SOURCE="$(STLSOFT)\include\winstl\synch\sleep_functions.h"
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

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backend.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\be.WindowsSyslog.h"
# End Source File
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\implicit_link\bec.WindowsSyslog.h"
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
# End Group
# Begin Source File

SOURCE="$(PANTHEIOS_ROOT)\include\pantheios\backends\bec.WindowsSyslog.h"
# End Source File
# End Target
# End Project
