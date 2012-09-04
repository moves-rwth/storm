############################################################################
# File: 	makefile
#
# Purpose:	Makefile for the Pantheios project, for the Arturius C/C++ Compiler Multiplexer
#			compiler, on Win32
#
# Created:	15th Feburary 2005
# Updated:	31st July 2012
#
############################################################################


############################################################################
# Symbols

########################################
# Project & compiler version

COMP_TAG			=	ar
ARCH_TAG            =   
OS_TAG				=   
PROJ_TAG			=	pantheios
PROJ_VER_MAJOR		=	1
PROJ_TAG_VER		=	$(PROJ_TAG).$(PROJ_VER_MAJOR)
COMP_ARCH_OS_TAG	=	$(COMP_TAG)$(OS_TAG)$(ARCH_TAG)

B64_VER_MAJOR		=   1
B64_TAG_VER			=   b64.$(B64_VER_MAJOR)

SHWILD_VER_MAJOR    =   0
SHWILD_TAG_VER      =   shwild.$(SHWILD_VER_MAJOR)

XTESTS_VER_MAJOR    =   0
XTESTS_TAG_VER      =   xtests.$(XTESTS_VER_MAJOR)

PROJ_MAKEFILE_NAME  =   makefile

NOTHING				=

########################################
# Testing verbosity

TEST_VERBOSITY		=	2

########################################
# Directories

PROJ_BASE_DIR			=	..\..
PROJ_BIN_DIR			=	$(PROJ_BASE_DIR)\bin
PROJ_INCLUDE_DIR		=	$(PROJ_BASE_DIR)\include
PROJ_LIB_DIR			=	$(PROJ_BASE_DIR)\lib
PROJ_OBJ_DIR			=	.
PROJ_SRC_DIR			=	$(PROJ_BASE_DIR)\src
PROJ_BACKENDS_DIR		=	$(PROJ_BASE_DIR)\backends
PROJ_FRONTENDS_DIR		=	$(PROJ_BASE_DIR)\frontends
PROJ_EXAMPLES_DIR		=	$(PROJ_BASE_DIR)\examples
PROJ_SAMPLES_DIR        =   $(PROJ_BASE_DIR)\samples
PROJ_TEST_ROOT_DIR		=	$(PROJ_BASE_DIR)\test
PROJ_TEST_COMPONENT_DIR	=	$(PROJ_TEST_ROOT_DIR)\component
PROJ_TEST_PERFORMANCE_DIR	=	$(PROJ_TEST_ROOT_DIR)\performance
PROJ_TEST_SCRATCH_DIR	=	$(PROJ_TEST_ROOT_DIR)\scratch
PROJ_TEST_UNIT_DIR		=	$(PROJ_TEST_ROOT_DIR)\unit

B64_INCLUDE_DIR			=   $(PROJ_BASE_DIR)\include
B64_SRC_DIR				=   $(PROJ_SRC_DIR)\b64

SHWILD_INCLUDE_DIR      =   $(PROJ_BASE_DIR)\include
SHWILD_SRC_DIR          =   $(PROJ_SRC_DIR)\shwild

XTESTS_INCLUDE_DIR      =   $(PROJ_BASE_DIR)\include
XTESTS_SRC_DIR          =   $(PROJ_SRC_DIR)\xtests

########################################
# tools

CC                      =   arcc.debug
CPPC                    =   arcc.debug
AR                      =   lib
LD                      =   arld
RM                      =   del /f
CP                      =   copy
RC                      =   rc

# Includes

INCLUDES_LEADER 	=	

PROJ_INCLUDE		=	-I$(PROJ_INCLUDE_DIR)

ENV_INCLUDES 	=

COMMON_INCLUDES		=

CC_INCLUDES_BASE=\
$(INCLUDES_LEADER)\
$(PROJ_INCLUDE)\
-I"$(STLSOFT)\include"\
$(COMMON_INCLUDES)\
$(ENV_INCLUDES)\
 \

CC_INCLUDES=\
$(CC_INCLUDES_BASE)\
 \



CPPC_INCLUDES_BASE=\
$(INCLUDES_LEADER)\
$(PROJ_INCLUDE)\
-I"$(STLSOFT)\include"\
$(COMMON_INCLUDES)\
$(ENV_INCLUDES)\
 \

CPPC_INCLUDES=\
$(CPPC_INCLUDES_BASE)\
 \



# b64

CC_B64_INCLUDES_BASE =\
$(CC_INCLUDES_BASE)\
 \

CC_B64_INCLUDES_DEBUG =\
$(CC_B64_INCLUDES_BASE)\
 \

CC_B64_INCLUDES_RELEASE =\
$(CC_B64_INCLUDES_BASE)\
 \


CPPC_B64_INCLUDES_BASE =\
$(CPPC_INCLUDES_BASE)\
 \

CPPC_B64_INCLUDES_DEBUG =\
$(CPPC_B64_INCLUDES_BASE)\
 \

CPPC_B64_INCLUDES_RELEASE =\
$(CPPC_B64_INCLUDES_BASE)\
 \


# shwild

CPPC_SHWILD_INCLUDES_BASE =\
$(CPPC_INCLUDES_BASE)\
 \

CPPC_SHWILD_INCLUDES_DEBUG =\
$(CPPC_SHWILD_INCLUDES_BASE)\
 \

CPPC_SHWILD_INCLUDES_RELEASE =\
$(CPPC_SHWILD_INCLUDES_BASE)\
 \


# xTests

CPPC_XTESTS_INCLUDES_BASE =\
$(CPPC_INCLUDES_BASE)\
 \

CPPC_XTESTS_INCLUDES_DEBUG =\
$(CPPC_XTESTS_INCLUDES_BASE)\
 \

CPPC_XTESTS_INCLUDES_RELEASE =\
$(CPPC_XTESTS_INCLUDES_BASE)\
 \




# Lib

LIBS_LEADER 		=	

# lib paths

ENV_LIBPATHS		=
COMMON_LIBPATHS			=



LIBPATHS=\
$(LIBS_LEADER)\
$(COMMON_LIBPATHS)\
$(ENV_LIBPATHS)\
\


# lib names

TARGET_LIB_DEBUG	= 
TARGET_LIB_RELEASE	= 

# #defines
#
# - OS
# - 

OS_DEFS				=	-DWIN32

COMMON_DEFS 		=

CUSTOM_DEFS			=

CC_CUSTOM_DEFS		=	$(CUSTOM_DEFS)

CPPC_CUSTOM_DEFS	=	$(CUSTOM_DEFS)

CC_DEFS 			=	$(COMMON_DEFS) $(OS_DEFS) $(CC_CUSTOM_DEFS)

CPPC_DEFS			=	$(COMMON_DEFS) $(OS_DEFS) $(CPPC_CUSTOM_DEFS)

# CC flags

CC_F_NOLOGO 			=	
CC_F_COMPILE_ONLY		=	-c
CC_F_WARNING_MAX		=	--warning-level=maximum
CC_F_WARNING_ERR		=	--warnings-as-errors=on
CC_F_WARNINGS_LIMIT		=	
CC_F_MINIMUM_SIZE		=	--minimum-size
#CC_F_MAXIMUM_SPEED		=	--maximum-speed
CC_APP_F_WARNING_MAX	=	$(CC_F_WARNING_MAX)
CC_APP_F_WARNING_ERR	=	$(CC_F_WARNING_ERR)
CC_F_ADDITIONAL			=	

# CPPC flags

CPPC_F_NOLOGO			=	
CPPC_F_COMPILE_ONLY 	=	-c
CPPC_F_WARNING_MAX		=	--warning-level=maximum
CPPC_F_WARNING_ERR		=	--warnings-as-errors=on
CPPC_F_EXCEPTIONS_ON	=	--exception-handling=on
CPPC_F_EXCEPTIONS_OFF	=	--exception-handling=off
CPPC_F_EXCEPTIONS		=	$(CPPC_F_EXCEPTIONS_ON)
CPPC_F_RTTI_ON			=	
CPPC_F_RTTI_OFF 		=	
CPPC_F_RTTI				=	$(CPPC_F_RTTI_ON)
CPPC_F_WARNINGS_LIMIT	=	
CPPC_F_MINIMUM_SIZE 	=	--minimum-size
CPPC_F_MAXIMUM_SPEED	=	--maximum-speed
CPPC_F_OPTIMISED		=	$(CPPC_F_MAXIMUM_SPEED)
CPPC_APP_F_WARNING_MAX	=	$(CPPC_F_WARNING_MAX)
CPPC_APP_F_WARNING_ERR	=	$(CPPC_F_WARNING_ERR)
CPPC_F_ADDITIONAL		=	



CC_TOOL_FLAGS		=	--compilers=bc56,dmb-stlport,gcc34,icl8,vc6,vc71,ow12 --active-compiler=vc/6 --announce-tool-commands

CPPC_TOOL_FLAGS 	=	--compilers=bc56,dmb-stlport,gcc34,icl8,vc6,vc71 --active-compiler=vc/6 --announce-tool-commands



CC_FLAGS=\
$(CC_TOOL_FLAGS)\
$(CC_F_ADDITIONAL)\
$(CC_F_NOLOGO)\
$(CC_F_COMPILE_ONLY)\
$(CC_F_WARNING_MAX)\
$(CC_F_WARNING_ERR)\
$(CC_F_WARNINGS_LIMIT)\
$(CC_F_MINIMUM_SIZE)\
 \

CPPC_FLAGS=\
$(CPPC_TOOL_FLAGS)\
$(CPPC_F_ADDITIONAL)\
$(CPPC_F_NOLOGO)\
$(CPPC_F_COMPILE_ONLY)\
$(CPPC_F_WARNING_MAX)\
$(CPPC_F_WARNING_ERR)\
$(CPPC_F_WARNINGS_LIMIT)\
$(CPPC_F_EXCEPTIONS)\
$(CPPC_F_RTTI)\
 \


# - App (base)
CC_APP_FLAGS=\
$(CC_TOOL_FLAGS)\
$(CC_F_ADDITIONAL)\
$(CC_F_NOLOGO)\
$(CC_F_COMPILE_ONLY)\
$(CC_APP_F_WARNING_MAX)\
$(CC_APP_F_WARNING_ERR)\
$(CC_F_WARNINGS_LIMIT)\
$(CC_F_MINIMUM_SIZE)\
 \

CPPC_APP_FLAGS=\
$(CPPC_TOOL_FLAGS)\
$(CPPC_F_ADDITIONAL)\
$(CPPC_F_NOLOGO)\
$(CPPC_F_COMPILE_ONLY)\
$(CPPC_APP_F_WARNING_MAX)\
$(CPPC_APP_F_WARNING_ERR)\
$(CPPC_F_WARNINGS_LIMIT)\
$(CPPC_F_EXCEPTIONS)\
$(CPPC_F_RTTI)\
 \


# - b64
CC_B64_FLAGS = $(CC_APP_FLAGS)

CPPC_B64_FLAGS = $(CPPC_APP_FLAGS)


# - shwild
CC_SHWILD_FLAGS = $(CC_APP_FLAGS)

CPPC_SHWILD_FLAGS = $(CPPC_APP_FLAGS)


# - xTests
CC_XTESTS_FLAGS = $(CC_APP_FLAGS)

CPPC_XTESTS_FLAGS = $(CPPC_APP_FLAGS)


# - Example
CC_EXAMPLE_FLAGS = $(CC_APP_FLAGS)

CPPC_EXAMPLE_FLAGS = $(CPPC_APP_FLAGS)


# - Test (Component)
CC_TEST_COMPONENT_FLAGS = $(CC_APP_FLAGS)

CPPC_TEST_COMPONENT_FLAGS = $(CPPC_APP_FLAGS)


# - Test (Scratch)
CC_TEST_SCRATCH_FLAGS = $(CC_APP_FLAGS)

CPPC_TEST_SCRATCH_FLAGS = $(CPPC_APP_FLAGS)


# - Test (Unit)
CC_TEST_UNIT_FLAGS = $(CC_APP_FLAGS)

CPPC_TEST_UNIT_FLAGS = $(CPPC_APP_FLAGS)





# Arguments (= flags + defines + includes)

# - Libs
CC_LIB_ARGS_DEBUG			=	$(CC_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CC_LIB_ARGS_RELEASE			=	$(CC_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS) -DPANTHEIOS_NO_AUTO_INIT

CPPC_LIB_ARGS_DEBUG			=	$(CPPC_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CPPC_LIB_ARGS_RELEASE			=	$(CPPC_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS) -DPANTHEIOS_NO_AUTO_INIT


# - b64
CC_B64_ARGS_DEBUG			=	$(CC_B64_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_B64_ARGS_RELEASE			=	$(CC_B64_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS)

CPPC_B64_ARGS_DEBUG			=	$(CPPC_B64_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_B64_ARGS_RELEASE			=	$(CPPC_B64_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS)


# - shwild
CC_SHWILD_ARGS_DEBUG			=	$(CC_SHWILD_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_SHWILD_ARGS_RELEASE			=	$(CC_SHWILD_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS)

CPPC_SHWILD_ARGS_DEBUG		=	$(CPPC_SHWILD_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_SHWILD_ARGS_RELEASE		=	$(CPPC_SHWILD_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS)


# - xTests
CC_XTESTS_ARGS_DEBUG			=	$(CC_XTESTS_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_XTESTS_ARGS_RELEASE			=	$(CC_XTESTS_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS)

CPPC_XTESTS_ARGS_DEBUG			=	$(CPPC_XTESTS_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_XTESTS_ARGS_RELEASE			=	$(CPPC_XTESTS_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS)


# - Examples
CC_EXAMPLE_ARGS_DEBUG		=	$(CC_EXAMPLE_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_EXAMPLE_ARGS_RELEASE		=	$(CC_EXAMPLE_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS)

CPPC_EXAMPLE_ARGS_DEBUG		=	$(CPPC_EXAMPLE_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_EXAMPLE_ARGS_RELEASE		=	$(CPPC_EXAMPLE_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS)


# - Test (Component)
CC_TEST_COMPONENT_ARGS_DEBUG			=	$(CC_TEST_COMPONENT_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_COMPONENT_ARGS_RELEASE			=	$(CC_TEST_COMPONENT_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS)

CPPC_TEST_COMPONENT_ARGS_DEBUG			=	$(CPPC_TEST_COMPONENT_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_COMPONENT_ARGS_RELEASE			=	$(CPPC_TEST_COMPONENT_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS)


# - Test (Scratch)
CC_TEST_SCRATCH_ARGS_DEBUG			=	$(CC_TEST_SCRATCH_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_SCRATCH_ARGS_RELEASE			=	$(CC_TEST_SCRATCH_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS)

CPPC_TEST_SCRATCH_ARGS_DEBUG			=	$(CPPC_TEST_SCRATCH_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_SCRATCH_ARGS_RELEASE			=	$(CPPC_TEST_SCRATCH_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS)


# - Test (Unit)
CC_TEST_UNIT_ARGS_DEBUG			=	$(CC_TEST_UNIT_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_UNIT_ARGS_RELEASE			=	$(CC_TEST_UNIT_FLAGS) -DNDEBUG -U_DEBUG $(CC_F_MAXIMUM_SPEED)  $(CC_DEFS)

CPPC_TEST_UNIT_ARGS_DEBUG			=	$(CPPC_TEST_UNIT_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_UNIT_ARGS_RELEASE			=	$(CPPC_TEST_UNIT_FLAGS) -DNDEBUG -U_DEBUG $(CPPC_F_MAXIMUM_SPEED)  $(CPPC_DEFS)


# AR args

AR_ARGS 			=	


# LD flags

LD_ARGS 		=	

LD_F_NOLOGO = 

# LD args

LD_ARGS_DEBUG =  $(LD_ARGS) $(LD_F_NOLOGO) $(LIBPATHS) $(COMMON_LIB)
LD_ARGS_RELEASE =  $(LD_ARGS) $(LD_F_NOLOGO) $(LIBPATHS) $(COMMON_LIB)

############################################################################
# Headers

CORE_HEADERS=\
 $(PROJ_INCLUDE_DIR)\pantheios\pantheios.h\
 $(PROJ_INCLUDE_DIR)\pantheios\backend.h\
 $(PROJ_INCLUDE_DIR)\pantheios\init_codes.h\
 $(PROJ_INCLUDE_DIR)\pantheios\frontend.h\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\pantheios.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\trace.h\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\args.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\b.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\b64.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\blob.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\boolean.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\ch.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\character.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\exception.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\fmt.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\hex_ptr.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\hostid.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\integer.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\m2w.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\p.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\pad.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\pointer.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\processid.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\real.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\slice.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\threadid.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\w2m.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\variant_bool.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\vb.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\xi.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\inserters\xp.hpp\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\internal\initialiser.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\internal\stock_levels.hpp\
 $(PROJ_INCLUDE_DIR)\pantheios\internal\nox.h\
 $(PROJ_INCLUDE_DIR)\pantheios\internal\safestr.h\
 $(PROJ_INCLUDE_DIR)\pantheios\internal\threading.h\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\quality\contract.h\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\internal\generated\log_dispatch_functions.h\
 $(PROJ_INCLUDE_DIR)\pantheios\internal\generated\log_functions.c\
 $(PROJ_INCLUDE_DIR)\pantheios\internal\generated\log_functions.h\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\severity\levels.hpp\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\util\backends\arguments.h\
 $(PROJ_INCLUDE_DIR)\pantheios\util\backends\context.hpp\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\util\core\apidefs.hpp\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\util\memory\auto_buffer_selector.hpp\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\util\severity\ACE.h\
 $(PROJ_INCLUDE_DIR)\pantheios\util\severity\syslog.h\
 $(PROJ_INCLUDE_DIR)\pantheios\util\severity\WindowsEventLog.h\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\util\string\snprintf.h\
 $(PROJ_INCLUDE_DIR)\pantheios\util\string\strdup.h\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\util\system\hostname.h\
 $(PROJ_INCLUDE_DIR)\pantheios\util\system\processid.h\
 $(PROJ_INCLUDE_DIR)\pantheios\util\system\threadid.h\
 \
 $(PROJ_INCLUDE_DIR)\pantheios\util\time\currenttime.h\
 \
 \

BE_HEADERS=\
 $(PROJ_INCLUDE_DIR)\pantheios\pantheios.h \
 $(PROJ_INCLUDE_DIR)\pantheios\backend.h \
 \

FE_HEADERS=\
 $(PROJ_INCLUDE_DIR)\pantheios\pantheios.h \
 $(PROJ_INCLUDE_DIR)\pantheios\frontend.h \
 \

TEST_C_HEADERS=\
 $(PROJ_INCLUDE_DIR)\pantheios\backend.h\
 $(PROJ_INCLUDE_DIR)\pantheios\frontend.h\
 $(PROJ_INCLUDE_DIR)\pantheios\pantheios.h\
 \

TEST_CPP_HEADERS=\
 $(CORE_HEADERS)\
 $(PROJ_INCLUDE_DIR)\pantheios\backends\bec.test.h\
 \

B64_HEADERS=\
 $(PROJ_INCLUDE_DIR)\b64\b64.h\
 $(PROJ_INCLUDE_DIR)\b64\b64.hpp\
 \

SHWILD_HEADERS=\
 $(PROJ_INCLUDE_DIR)\shwild\shwild.h\
 $(PROJ_INCLUDE_DIR)\shwild\shwild.hpp\
 \

XTESTS_HEADERS=\
 $(PROJ_INCLUDE_DIR)\xtests\xtests.h\
 \

############################################################################
# Names

############################################################
# Targets

########################################
# Libraries

#  util
LIB_UTIL_DEBUG				=   $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).util.$(COMP_ARCH_OS_TAG).debug.lib

#  core
LIB_CORE_DEBUG				=   $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).core.$(COMP_ARCH_OS_TAG).debug.lib

#  appl
LIB_APPL_DEBUG				=   $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).appl.$(COMP_ARCH_OS_TAG).debug.lib

#  front-end(s)
LIB_FE_ALL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.all.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_ALL_WC_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.all.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_FAIL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.fail.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_N_DEBUG				=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.N.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_N_WC_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.N.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_NULL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.null.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_NULL_WC_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.null.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_SIMPLE_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.simple.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_SIMPLE_WC_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.simple.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_WINREG_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.WindowsRegistry.$(COMP_ARCH_OS_TAG).debug.lib
LIB_FE_WINREG_WC_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.WindowsRegistry.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib

#  back-end(s)
LIB_BEC_STOCK_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.stock.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_LRSPLIT_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.lrsplit.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_N_DEBUG 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.N.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_TEST_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.test.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_TEST_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.test.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_TEST_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.test.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_TEST_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.test.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_FAIL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.fail.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_FAIL_DEBUG 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.fail.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_FAIL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.fail.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_FAIL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.fail.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_FILE_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.file.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_FILE_WC_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.file.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_FILE_DEBUG 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.file.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_FILE_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.file.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_FILE_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.file.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_FPRINTF_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.fprintf.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_FPRINTF_WC_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.fprintf.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_FPRINTF_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.fprintf.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_FPRINTF_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.fprintf.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_FPRINTF_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.fprintf.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_NULL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.null.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_NULL_DEBUG 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.null.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_NULL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.null.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_NULL_DEBUG			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.null.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_COMERROBJ_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.COMErrorObject.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_COMERROBJWC_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.COMErrorObject.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_COMERROBJ_DEBUG 	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.COMErrorObject.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_COMERROBJ_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.COMErrorObject.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_COMERROBJ_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.COMErrorObject.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_SPEECH_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.speech.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_SPEECHWC_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.speech.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_SPEECH_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.speech.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_SPEECH_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.speech.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_SPEECH_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.speech.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_WINEVLOG_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsEventLog.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_WINEVLOG_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsEventLog.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_WINEVLOG_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsEventLog.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_WINEVLOG_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsEventLog.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_WINMSGBX_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsMessageBox.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_WINMSGBX_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsMessageBox.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_WINMSGBX_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsMessageBox.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_WINMSGBX_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsMessageBox.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_W32CONS_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsConsole.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_W32CONSWC_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsConsole.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_W32CONS_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsConsole.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_W32CONS_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsConsole.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_W32CONS_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsConsole.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_WINDBGR_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsDebugger.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_WINDBGR_WC_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsDebugger.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_WINDBGR_DEBUG 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsDebugger.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_WINDBGR_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsDebugger.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_WINDBGR_DEBUG		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsDebugger.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_WINSYSLOG_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsSyslog.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEC_WINSYSLOG_WC_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsSyslog.WithCallback.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BE_WINSYSLOG_DEBUG 	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsSyslog.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BEL_WINSYSLOG_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsSyslog.$(COMP_ARCH_OS_TAG).debug.lib
LIB_BER_WINSYSLOG_DEBUG	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsSyslog.$(COMP_ARCH_OS_TAG).debug.lib

#  util
LIB_UTIL_RELEASE				=   $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).util.$(COMP_ARCH_OS_TAG).lib

#  core
LIB_CORE_RELEASE				=   $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).core.$(COMP_ARCH_OS_TAG).lib

#  appl
LIB_APPL_RELEASE				=   $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).appl.$(COMP_ARCH_OS_TAG).lib

#  front-end(s)
LIB_FE_ALL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.all.$(COMP_ARCH_OS_TAG).lib
LIB_FE_ALL_WC_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.all.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_FE_FAIL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.fail.$(COMP_ARCH_OS_TAG).lib
LIB_FE_N_RELEASE				=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.N.$(COMP_ARCH_OS_TAG).lib
LIB_FE_N_WC_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.N.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_FE_NULL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.null.$(COMP_ARCH_OS_TAG).lib
LIB_FE_NULL_WC_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.null.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_FE_SIMPLE_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.simple.$(COMP_ARCH_OS_TAG).lib
LIB_FE_SIMPLE_WC_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.simple.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_FE_WINREG_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.WindowsRegistry.$(COMP_ARCH_OS_TAG).lib
LIB_FE_WINREG_WC_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.WindowsRegistry.WithCallback.$(COMP_ARCH_OS_TAG).lib

#  back-end(s)
LIB_BEC_STOCK_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.stock.$(COMP_ARCH_OS_TAG).lib
LIB_BE_LRSPLIT_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.lrsplit.$(COMP_ARCH_OS_TAG).lib
LIB_BE_N_RELEASE 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.N.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_TEST_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.test.$(COMP_ARCH_OS_TAG).lib
LIB_BE_TEST_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.test.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_TEST_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.test.$(COMP_ARCH_OS_TAG).lib
LIB_BER_TEST_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.test.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_FAIL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.fail.$(COMP_ARCH_OS_TAG).lib
LIB_BE_FAIL_RELEASE 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.fail.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_FAIL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.fail.$(COMP_ARCH_OS_TAG).lib
LIB_BER_FAIL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.fail.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_FILE_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.file.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_FILE_WC_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.file.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_BE_FILE_RELEASE 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.file.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_FILE_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.file.$(COMP_ARCH_OS_TAG).lib
LIB_BER_FILE_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.file.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_FPRINTF_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.fprintf.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_FPRINTF_WC_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.fprintf.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_BE_FPRINTF_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.fprintf.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_FPRINTF_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.fprintf.$(COMP_ARCH_OS_TAG).lib
LIB_BER_FPRINTF_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.fprintf.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_NULL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.null.$(COMP_ARCH_OS_TAG).lib
LIB_BE_NULL_RELEASE 			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.null.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_NULL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.null.$(COMP_ARCH_OS_TAG).lib
LIB_BER_NULL_RELEASE			=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.null.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_COMERROBJ_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.COMErrorObject.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_COMERROBJWC_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.COMErrorObject.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_BE_COMERROBJ_RELEASE 	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.COMErrorObject.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_COMERROBJ_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.COMErrorObject.$(COMP_ARCH_OS_TAG).lib
LIB_BER_COMERROBJ_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.COMErrorObject.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_SPEECH_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.speech.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_SPEECHWC_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.speech.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_BE_SPEECH_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.speech.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_SPEECH_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.speech.$(COMP_ARCH_OS_TAG).lib
LIB_BER_SPEECH_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.speech.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_WINEVLOG_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsEventLog.$(COMP_ARCH_OS_TAG).lib
LIB_BE_WINEVLOG_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsEventLog.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_WINEVLOG_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsEventLog.$(COMP_ARCH_OS_TAG).lib
LIB_BER_WINEVLOG_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsEventLog.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_WINMSGBX_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsMessageBox.$(COMP_ARCH_OS_TAG).lib
LIB_BE_WINMSGBX_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsMessageBox.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_WINMSGBX_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsMessageBox.$(COMP_ARCH_OS_TAG).lib
LIB_BER_WINMSGBX_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsMessageBox.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_W32CONS_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsConsole.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_W32CONSWC_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsConsole.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_BE_W32CONS_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsConsole.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_W32CONS_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsConsole.$(COMP_ARCH_OS_TAG).lib
LIB_BER_W32CONS_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsConsole.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_WINDBGR_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsDebugger.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_WINDBGR_WC_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsDebugger.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_BE_WINDBGR_RELEASE 		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsDebugger.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_WINDBGR_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsDebugger.$(COMP_ARCH_OS_TAG).lib
LIB_BER_WINDBGR_RELEASE		=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsDebugger.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_WINSYSLOG_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsSyslog.$(COMP_ARCH_OS_TAG).lib
LIB_BEC_WINSYSLOG_WC_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.WindowsSyslog.WithCallback.$(COMP_ARCH_OS_TAG).lib
LIB_BE_WINSYSLOG_RELEASE 	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.WindowsSyslog.$(COMP_ARCH_OS_TAG).lib
LIB_BEL_WINSYSLOG_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.WindowsSyslog.$(COMP_ARCH_OS_TAG).lib
LIB_BER_WINSYSLOG_RELEASE	=	$(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.WindowsSyslog.$(COMP_ARCH_OS_TAG).lib


# b64
LIB_B64_DEBUG			=   $(PROJ_LIB_DIR)\$(B64_TAG_VER).pan.$(COMP_ARCH_OS_TAG).debug.lib
LIB_B64_RELEASE			=   $(PROJ_LIB_DIR)\$(B64_TAG_VER).pan.$(COMP_ARCH_OS_TAG).lib

# shwild
LIB_SHWILD_DEBUG         =   $(PROJ_LIB_DIR)\$(SHWILD_TAG_VER).pan.$(COMP_ARCH_OS_TAG).debug.lib
LIB_SHWILD_RELEASE         =   $(PROJ_LIB_DIR)\$(SHWILD_TAG_VER).pan.$(COMP_ARCH_OS_TAG).lib

# xtests
LIB_XTESTS_DEBUG         =   $(PROJ_LIB_DIR)\$(XTESTS_TAG_VER).pan.$(COMP_ARCH_OS_TAG).debug.lib
LIB_XTESTS_RELEASE         =   $(PROJ_LIB_DIR)\$(XTESTS_TAG_VER).pan.$(COMP_ARCH_OS_TAG).lib

########################################
# Examples

TARG_EX_P_be_01_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.01.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_be_02_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.02.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_be_03_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.03.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_be_04_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.04.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_be_05_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.05.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_be_06_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.06.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_cp_01_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.cp.01.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_cp_02_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.cp.02.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_fm_01_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fm.01.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_fm_02_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fm.02.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_fm_03_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fm.03.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_fe_01_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fe.01.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_01_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.01.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_02_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.02.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_03_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.03.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_04_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.04.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_05_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.05.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_06_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.06.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_07_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.07.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_08_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.08.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_09_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.09.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_10_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.10.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_11_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.11.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_12_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.12.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_13_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.13.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_14_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.14.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_15_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.15.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_16_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.16.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_in_17_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.17.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_ms_01_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.01.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_ms_02_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.02.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_ms_03_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.03.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_ms_04_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.04.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_ms_05_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.05.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_ms_06_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.06.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_ut_01_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ut.01.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_0_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.0.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_1_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.1.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_2_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.2.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_3_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.3.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_4_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.4.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_5_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.5.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_6_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.6.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_C_7_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.7.$(COMP_ARCH_OS_TAG).debug.exe
TARG_EX_P_be_01_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.01.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_be_02_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.02.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_be_03_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.03.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_be_04_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.04.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_be_05_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.05.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_be_06_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.be.06.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_cp_01_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.cp.01.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_cp_02_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.cp.02.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_fm_01_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fm.01.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_fm_02_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fm.02.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_fm_03_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fm.03.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_fe_01_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.fe.01.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_01_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.01.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_02_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.02.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_03_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.03.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_04_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.04.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_05_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.05.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_06_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.06.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_07_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.07.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_08_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.08.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_09_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.09.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_10_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.10.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_11_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.11.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_12_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.12.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_13_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.13.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_14_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.14.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_15_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.15.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_16_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.16.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_in_17_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.in.17.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_ms_01_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.01.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_ms_02_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.02.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_ms_03_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.03.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_ms_04_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.04.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_ms_05_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.05.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_ms_06_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ms.06.$(COMP_ARCH_OS_TAG).exe
TARG_EX_P_ut_01_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.cpp.ut.01.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_0_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.0.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_1_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.1.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_2_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.2.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_3_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.3.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_4_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.4.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_5_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.5.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_6_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.6.$(COMP_ARCH_OS_TAG).exe
TARG_EX_C_7_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.c.7.$(COMP_ARCH_OS_TAG).exe

########################################
# Tests

##############################
# Component Tests

TARG_TEST_COMP_core_initseq_DEBUG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.core.init_sequence.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_core_initseq_RELEASE	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.core.init_sequence.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_custsev_DEBUG			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.custom_severity.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_custsev_RELEASE			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.custom_severity.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_args_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.args.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_args_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.args.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_b64_DEBUG			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.b64.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_b64_RELEASE			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.b64.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_bool_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.boolean.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_bool_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.boolean.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_hostid_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.hostId.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_hostid_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.hostId.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_int_DEBUG			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.integer.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_int_RELEASE			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.integer.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_pad_DEBUG			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.pad.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_pad_RELEASE			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.pad.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_ptr_DEBUG			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.pointer.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_ptr_RELEASE			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.pointer.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_prid_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.processId.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_prid_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.processId.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_real_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.real.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_real_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.real.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_slice_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.slice.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_slice_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.slice.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_thrid_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.threadId.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_thrid_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.threadId.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_vbool_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.variant_bool.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_vbool_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.variant_bool.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_ins_w2m_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.w2m.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_ins_w2m_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserters.w2m.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_log_1_DEBUG			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.log.1.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_log_1_RELEASE			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.log.1.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_pan_puts_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.core.pan_logputs.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_pan_puts_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.core.pan_logputs.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_COMP_pan_prtf_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.core.pan_logprintf.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_COMP_pan_prtf_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.core.pan_logprintf.$(COMP_ARCH_OS_TAG).exe

##############################
# Performance Tests

TARG_TEST_PERF_ins_w2m_DEBUG		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserter.w2m.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_PERF_ins_w2m_RELEASE		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.inserter.w2m.$(COMP_ARCH_OS_TAG).exe

##############################
# Scratch Tests

# test programs
#
# - be.fprintf
# - be.syslog / be.WindowsDebugger
#
# - be.lrsplit & fprintf + syslog / WindowsSyslog
#
# The strange naming conventions of these targets are to placate some
# make utilities, which have a limit to the identifier length.
TARG_TEST_DEBUG_APIwBE_FPRINTF					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.fprintf.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBE_COMERROBJ				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.COMErrorObject.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBE_WINEVLOG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsEventLog.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBE_WINMSGBX				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsMessageBox.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBE_W32CONS					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsConsole.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBE_WINDBGR					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsDebugger.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBE_WINSYSLOG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsSyslog.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBELR_2_WINDBGRFPRINTF		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.WinDbg_l-r.fprintf.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_DEBUG_APIwBELR_3_WINDBGRWINSYSLOG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.WinDbg_l-WSyslog_r.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_FE_DEBUG 						=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.fe.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_BE_N_FAIL_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.N.fail.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_FE_SIMPLE_WC_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.fe.simple.WC.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_RELEASE_APIwBE_FPRINTF					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.fprintf.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBE_COMERROBJ				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.COMErrorObject.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBE_WINEVLOG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsEventLog.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBE_WINMSGBX				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsMessageBox.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBE_W32CONS					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsConsole.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBE_WINDBGR					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsDebugger.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBE_WINSYSLOG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.WindowsSyslog.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBELR_2_WINDBGRFPRINTF		=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.WinDbg_l-r.fprintf.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_RELEASE_APIwBELR_3_WINDBGRWINSYSLOG	=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.WinDbg_l-WSyslog_r.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_FE_RELEASE 						=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.fe.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_BE_N_FAIL_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.be.N.fail.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_FE_SIMPLE_WC_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.scratch.fe.simple.WC.$(COMP_ARCH_OS_TAG).exe

##############################
# Unit Tests
#
# NOTE: dynamic_initialisation is contracted, as scares CodeWarrior's linker (for being longer than 63 chars)

TARG_TEST_UNIT_lvls_dyninit_DEBUG			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.levels.dynamic_init.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_lvls_dyninit_RELEASE			=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.levels.dynamic_init.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_lvls_vals_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.levels.values.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_lvls_vals_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.levels.values.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_getcurrtm_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.getcurrenttime.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_getcurrtm_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.getcurrenttime.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_getversion_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.getversion.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_getversion_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.getversion.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_gethostname_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.gethostname.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_gethostname_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.gethostname.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_onbailout_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.onbailout.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_strnlen_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.strnlen.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_onbailout_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.onbailout.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_strnlen_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.util.strnlen.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_bec_CEO_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.COMErrorObject.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_bec_CEO_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.COMErrorObject.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_bec_file_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.file.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_bec_file_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.file.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_bec_fprintf_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.fprintf.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_bec_fprintf_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.fprintf.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_bec_fail_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.fail.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_bec_fail_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.bec.fail.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_be_fail_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_be_fail_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_be_fail_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_be_fail_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_be_lrsplit_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.lrsplit.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_be_lrsplit_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.lrsplit.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_be_N_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.N.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_be_N_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.be.N.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_fe_all_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.all.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_fe_all_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.all.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_fe_fail_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.fail.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_fe_fail_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.fail.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_fe_N_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.N.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_fe_N_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.N.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_fe_null_DEBUG					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.null.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_fe_null_RELEASE					=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.null.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_fe_simple_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.simple.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_fe_simple_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.simple.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_fe_WinReg_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.WindowsRegistry.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_fe_WinReg_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.fe.WindowsRegistry.$(COMP_ARCH_OS_TAG).exe

TARG_TEST_UNIT_ins_int_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.inserter.integer.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_ins_w2m_DEBUG				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.inserter.w2m.$(COMP_ARCH_OS_TAG).debug.exe
TARG_TEST_UNIT_ins_int_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.inserter.integer.$(COMP_ARCH_OS_TAG).exe
TARG_TEST_UNIT_ins_w2m_RELEASE				=	$(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.inserter.w2m.$(COMP_ARCH_OS_TAG).exe

############################################################
# Objects

########################################
# Libraries

# core

OBJ_utl_bailout_DEBUG		= $(PROJ_OBJ_DIR)\util.bailout.debug.obj
OBJ_utl_init_code_strs_DEBUG	= $(PROJ_OBJ_DIR)\util.init_err_strings.debug.obj
OBJ_utl_sev_strs_DEBUG		= $(PROJ_OBJ_DIR)\util.sev_strings.debug.obj
OBJ_utl_snprintf_DEBUG 	= $(PROJ_OBJ_DIR)\util.snprintf.debug.obj
OBJ_utl_strdup_DEBUG 		= $(PROJ_OBJ_DIR)\util.strdup.debug.obj
OBJ_utl_procid_DEBUG 		= $(PROJ_OBJ_DIR)\util.procid.debug.obj
OBJ_utl_thread_DEBUG 		= $(PROJ_OBJ_DIR)\util.thread.debug.obj
OBJ_utl_be_context_DEBUG	= $(PROJ_OBJ_DIR)\util.be_context.debug.obj
OBJ_utl_core_apidefs_DEBUG	= $(PROJ_OBJ_DIR)\util.core_apidefs.debug.obj
OBJ_utl_be_parse_DEBUG		= $(PROJ_OBJ_DIR)\util.be_parse.debug.obj
OBJ_utl_strnlen_DEBUG		= $(PROJ_OBJ_DIR)\util.strnlen.debug.obj
OBJ_utl_strnlenp_DEBUG		= $(PROJ_OBJ_DIR)\util.strnlen.c++.debug.obj
OBJ_utl_time_DEBUG			= $(PROJ_OBJ_DIR)\util.time.debug.obj
OBJ_utl_host_name_DEBUG	= $(PROJ_OBJ_DIR)\util.host_name.debug.obj
OBJ_cr_api_DEBUG 			= $(PROJ_OBJ_DIR)\core.api.debug.obj
OBJ_cr_api_dep_DEBUG 		= $(PROJ_OBJ_DIR)\core.api.deprecated.debug.obj
OBJ_cr_api_exit_DEBUG 		= $(PROJ_OBJ_DIR)\core.api.exitprocess.debug.obj
OBJ_cr_api_logprintf_DEBUG	= $(PROJ_OBJ_DIR)\core.api.logprintf.debug.obj
OBJ_cr_api_logputs_DEBUG	= $(PROJ_OBJ_DIR)\core.api.logputs.debug.obj
OBJ_cr_auto_DEBUG 			= $(PROJ_OBJ_DIR)\core.auto.debug.obj
OBJ_ins_args_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.args.debug.obj
OBJ_ins_b64_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.b64.debug.obj
OBJ_ins_blob_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.blob.debug.obj
OBJ_ins_bool_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.boolean.debug.obj
OBJ_ins_excp_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.exception.debug.obj
OBJ_ins_host_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.hostid.debug.obj
OBJ_ins_int_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.integer.debug.obj
OBJ_ins_ptr_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.pointer.debug.obj
OBJ_ins_prid_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.processid.debug.obj
OBJ_ins_real_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.real.debug.obj
OBJ_ins_slice_DEBUG	 	= $(PROJ_OBJ_DIR)\inserters.slice.debug.obj
OBJ_ins_thrid_DEBUG	 	= $(PROJ_OBJ_DIR)\inserters.threadid.debug.obj
OBJ_ins_w2m_DEBUG	 		= $(PROJ_OBJ_DIR)\inserters.w2m.debug.obj
#PDB_bec_test_DEBUG 		= $(PROJ_OBJ_DIR)\bec.test.debug.pdb
OBJ_bec_test_DEBUG 		= $(PROJ_OBJ_DIR)\bec.test.debug.obj
OBJ_be_test_DEBUG 			= $(PROJ_OBJ_DIR)\be.test.debug.obj
OBJ_bel_test_DEBUG 		= $(PROJ_OBJ_DIR)\bel.test.debug.obj
OBJ_ber_test_DEBUG 		= $(PROJ_OBJ_DIR)\ber.test.debug.obj
OBJ_be_lrsplit_DEBUG		= $(PROJ_OBJ_DIR)\be.lrsplit.debug.obj
OBJ_be_N_DEBUG				= $(PROJ_OBJ_DIR)\be.N.debug.obj
OBJ_bec_fail_DEBUG			= $(PROJ_OBJ_DIR)\bec.fail.debug.obj
OBJ_be_fail_DEBUG			= $(PROJ_OBJ_DIR)\be.fail.debug.obj
OBJ_bel_fail_DEBUG			= $(PROJ_OBJ_DIR)\bel.fail.debug.obj
OBJ_ber_fail_DEBUG			= $(PROJ_OBJ_DIR)\ber.fail.debug.obj
OBJ_bec_file_DEBUG			= $(PROJ_OBJ_DIR)\bec.file.debug.obj
OBJ_bec_file_WC_DEBUG		= $(PROJ_OBJ_DIR)\bec.file.WithCallback.debug.obj
OBJ_be_file_DEBUG			= $(PROJ_OBJ_DIR)\be.file.debug.obj
OBJ_bel_file_DEBUG			= $(PROJ_OBJ_DIR)\bel.file.debug.obj
OBJ_ber_file_DEBUG			= $(PROJ_OBJ_DIR)\ber.file.debug.obj
OBJ_bec_fprintf_DEBUG		= $(PROJ_OBJ_DIR)\bec.fprintf.debug.obj
OBJ_bec_fprintf_WC_DEBUG	= $(PROJ_OBJ_DIR)\bec.fprintf.WithCallback.debug.obj
OBJ_be_fprintf_DEBUG		= $(PROJ_OBJ_DIR)\be.fprintf.debug.obj
OBJ_bel_fprintf_DEBUG		= $(PROJ_OBJ_DIR)\bel.fprintf.debug.obj
OBJ_ber_fprintf_DEBUG		= $(PROJ_OBJ_DIR)\ber.fprintf.debug.obj
OBJ_bec_null_DEBUG			= $(PROJ_OBJ_DIR)\bec.null.debug.obj
OBJ_be_null_DEBUG			= $(PROJ_OBJ_DIR)\be.null.debug.obj
OBJ_bel_null_DEBUG			= $(PROJ_OBJ_DIR)\bel.null.debug.obj
OBJ_ber_null_DEBUG			= $(PROJ_OBJ_DIR)\ber.null.debug.obj
OBJ_bec_COMERROBJ_DEBUG	= $(PROJ_OBJ_DIR)\bec.COMErrorObject.debug.obj
OBJ_bec_COMERROBJ_WC_DEBUG	= $(PROJ_OBJ_DIR)\bec.COMErrorObject.WithCallback.debug.obj
OBJ_be_COMERROBJ_DEBUG		= $(PROJ_OBJ_DIR)\be.COMErrorObject.debug.obj
OBJ_bel_COMERROBJ_DEBUG	= $(PROJ_OBJ_DIR)\bel.COMErrorObject.debug.obj
OBJ_ber_COMERROBJ_DEBUG	= $(PROJ_OBJ_DIR)\ber.COMErrorObject.debug.obj
OBJ_bec_SPEECH_DEBUG		= $(PROJ_OBJ_DIR)\bec.speech.debug.obj
OBJ_bec_SPEECH_WC_DEBUG	= $(PROJ_OBJ_DIR)\bec.speech.WithCallback.debug.obj
OBJ_be_SPEECH_DEBUG		= $(PROJ_OBJ_DIR)\be.speech.debug.obj
OBJ_bel_SPEECH_DEBUG		= $(PROJ_OBJ_DIR)\bel.speech.debug.obj
OBJ_ber_SPEECH_DEBUG		= $(PROJ_OBJ_DIR)\ber.speech.debug.obj
OBJ_bec_WINEVLOG_DEBUG		= $(PROJ_OBJ_DIR)\bec.WindowsEventLog.debug.obj
OBJ_be_WINEVLOG_DEBUG		= $(PROJ_OBJ_DIR)\be.WindowsEventLog.debug.obj
OBJ_bel_WINEVLOG_DEBUG		= $(PROJ_OBJ_DIR)\bel.WindowsEventLog.debug.obj
OBJ_ber_WINEVLOG_DEBUG		= $(PROJ_OBJ_DIR)\ber.WindowsEventLog.debug.obj
OBJ_bec_WINMSGBX_DEBUG		= $(PROJ_OBJ_DIR)\bec.WindowsMessageBox.debug.obj
OBJ_be_WINMSGBX_DEBUG		= $(PROJ_OBJ_DIR)\be.WindowsMessageBox.debug.obj
OBJ_bel_WINMSGBX_DEBUG		= $(PROJ_OBJ_DIR)\bel.WindowsMessageBox.debug.obj
OBJ_ber_WINMSGBX_DEBUG		= $(PROJ_OBJ_DIR)\ber.WindowsMessageBox.debug.obj
OBJ_bec_W32CONS_DEBUG		= $(PROJ_OBJ_DIR)\bec.WindowsConsole.debug.obj
OBJ_bec_W32CONS_WC_DEBUG	= $(PROJ_OBJ_DIR)\bec.WindowsConsole.WithCallback.debug.obj
OBJ_be_W32CONS_DEBUG		= $(PROJ_OBJ_DIR)\be.WindowsConsole.debug.obj
OBJ_bel_W32CONS_DEBUG		= $(PROJ_OBJ_DIR)\bel.WindowsConsole.debug.obj
OBJ_ber_W32CONS_DEBUG		= $(PROJ_OBJ_DIR)\ber.WindowsConsole.debug.obj
OBJ_bec_WINDBGR_DEBUG		= $(PROJ_OBJ_DIR)\bec.WindowsDebugger.debug.obj
OBJ_bec_WINDBGR_WC_DEBUG	= $(PROJ_OBJ_DIR)\bec.WindowsDebugger.WithCallback.debug.obj
OBJ_be_WINDBGR_DEBUG		= $(PROJ_OBJ_DIR)\be.WindowsDebugger.debug.obj
OBJ_bel_WINDBGR_DEBUG		= $(PROJ_OBJ_DIR)\bel.WindowsDebugger.debug.obj
OBJ_ber_WINDBGR_DEBUG		= $(PROJ_OBJ_DIR)\ber.WindowsDebugger.debug.obj
OBJ_bec_WINSYSLOG_DEBUG	= $(PROJ_OBJ_DIR)\bec.WindowsSyslog.debug.obj
OBJ_bec_WINSYSLOG_WC_DEBUG	= $(PROJ_OBJ_DIR)\bec.WindowsSyslog.WithCallback.debug.obj
OBJ_be_WINSYSLOG_DEBUG		= $(PROJ_OBJ_DIR)\be.WindowsSyslog.debug.obj
OBJ_bel_WINSYSLOG_DEBUG	= $(PROJ_OBJ_DIR)\bel.WindowsSyslog.debug.obj
OBJ_ber_WINSYSLOG_DEBUG	= $(PROJ_OBJ_DIR)\ber.WindowsSyslog.debug.obj
OBJ_fe_all_DEBUG 			= $(PROJ_OBJ_DIR)\fe.all.debug.obj
OBJ_fe_all_WC_DEBUG 		= $(PROJ_OBJ_DIR)\fe.all.WithCallback.debug.obj
OBJ_fe_fail_DEBUG 			= $(PROJ_OBJ_DIR)\fe.fail.debug.obj
OBJ_fe_N_DEBUG 			= $(PROJ_OBJ_DIR)\fe.N.debug.obj
OBJ_fe_N_WC_DEBUG 			= $(PROJ_OBJ_DIR)\fe.N.WithCallback.debug.obj
OBJ_fe_null_DEBUG 			= $(PROJ_OBJ_DIR)\fe.null.debug.obj
OBJ_fe_null_WC_DEBUG 		= $(PROJ_OBJ_DIR)\fe.null.WithCallback.debug.obj
OBJ_fe_simple_DEBUG 		= $(PROJ_OBJ_DIR)\fe.simple.debug.obj
OBJ_fe_simple_WC_DEBUG 	= $(PROJ_OBJ_DIR)\fe.simple.WithCallback.debug.obj
OBJ_fe_winreg_DEBUG 		= $(PROJ_OBJ_DIR)\fe.WindowsRegistry.debug.obj
OBJ_fe_winreg_WC_DEBUG 	= $(PROJ_OBJ_DIR)\fe.WindowsRegistry.WithCallback.debug.obj

OBJ_UTIL_DEBUG=\
 $(OBJ_utl_bailout_DEBUG)\
 $(OBJ_utl_init_code_strs_DEBUG)\
 $(OBJ_utl_sev_strs_DEBUG)\
 $(OBJ_utl_snprintf_DEBUG)\
 $(OBJ_utl_strdup_DEBUG)\
 $(OBJ_utl_procid_DEBUG)\
 $(OBJ_utl_thread_DEBUG)\
 $(OBJ_utl_be_context_DEBUG)\
 $(OBJ_utl_core_apidefs_DEBUG)\
 $(OBJ_utl_be_parse_DEBUG)\
 $(OBJ_utl_strnlen_DEBUG)\
 $(OBJ_utl_strnlenp_DEBUG)\
 $(OBJ_utl_time_DEBUG)\
 $(OBJ_utl_host_name_DEBUG)\
 \

OBJ_CORE_DEBUG=\
 $(OBJ_cr_api_DEBUG)\
 $(OBJ_cr_api_dep_DEBUG)\
 $(OBJ_cr_api_exit_DEBUG)\
 $(OBJ_cr_api_logprintf_DEBUG)\
 $(OBJ_cr_api_logputs_DEBUG)\
 $(OBJ_cr_auto_DEBUG)\
 $(OBJ_ins_args_DEBUG)\
 $(OBJ_ins_b64_DEBUG)\
 $(OBJ_ins_blob_DEBUG)\
 $(OBJ_ins_bool_DEBUG)\
 $(OBJ_ins_excp_DEBUG)\
 $(OBJ_ins_host_DEBUG)\
 $(OBJ_ins_int_DEBUG)\
 $(OBJ_ins_ptr_DEBUG)\
 $(OBJ_ins_prid_DEBUG)\
 $(OBJ_ins_real_DEBUG)\
 $(OBJ_ins_slice_DEBUG)\
 $(OBJ_ins_thrid_DEBUG)\
 $(OBJ_ins_w2m_DEBUG)\
 \

OBJ_APPL_DEBUG=\
 $(OBJ_ins_args_DEBUG)\
 $(OBJ_ins_b64_DEBUG)\
 $(OBJ_ins_blob_DEBUG)\
 $(OBJ_ins_bool_DEBUG)\
 $(OBJ_ins_excp_DEBUG)\
 $(OBJ_ins_host_DEBUG)\
 $(OBJ_ins_int_DEBUG)\
 $(OBJ_ins_ptr_DEBUG)\
 $(OBJ_ins_prid_DEBUG)\
 $(OBJ_ins_real_DEBUG)\
 $(OBJ_ins_slice_DEBUG)\
 $(OBJ_ins_thrid_DEBUG)\
 $(OBJ_ins_w2m_DEBUG)\
 \


OBJ_BE_LRSPLIT_DEBUG=\
 $(OBJ_be_lrsplit_DEBUG)\
 \

OBJ_BE_N_DEBUG=\
 $(OBJ_be_N_DEBUG)\
 \

OBJ_BEC_TEST_DEBUG=\
 $(OBJ_bec_test_DEBUG)\
 \


OBJ_BE_TEST_DEBUG=\
 $(OBJ_be_test_DEBUG)\
 \


OBJ_BEL_TEST_DEBUG=\
 $(OBJ_bel_test_DEBUG)\
 \


OBJ_BER_TEST_DEBUG=\
 $(OBJ_ber_test_DEBUG)\
 \


OBJ_BEC_FAIL_DEBUG=\
 $(OBJ_bec_fail_DEBUG)\
 \

OBJ_BE_FAIL_DEBUG=\
 $(OBJ_be_fail_DEBUG)\
 \

OBJ_BEL_FAIL_DEBUG=\
 $(OBJ_bel_fail_DEBUG)\
 \

OBJ_BER_FAIL_DEBUG=\
 $(OBJ_ber_fail_DEBUG)\
 \

OBJ_BEC_FILE_DEBUG=\
 $(OBJ_bec_file_DEBUG)\
 \

OBJ_BEC_FILE_WC_DEBUG=\
 $(OBJ_bec_file_WC_DEBUG)\
 \

OBJ_BE_FILE_DEBUG=\
 $(OBJ_be_file_DEBUG)\
 \

OBJ_BEL_FILE_DEBUG=\
 $(OBJ_bel_file_DEBUG)\
 \

OBJ_BER_FILE_DEBUG=\
 $(OBJ_ber_file_DEBUG)\
 \

OBJ_BEC_FPRINTF_DEBUG=\
 $(OBJ_bec_fprintf_DEBUG)\
 \

OBJ_BEC_FPRINTF_WC_DEBUG=\
 $(OBJ_bec_fprintf_WC_DEBUG)\
 \

OBJ_BE_FPRINTF_DEBUG=\
 $(OBJ_be_fprintf_DEBUG)\
 \

OBJ_BEL_FPRINTF_DEBUG=\
 $(OBJ_bel_fprintf_DEBUG)\
 \

OBJ_BER_FPRINTF_DEBUG=\
 $(OBJ_ber_fprintf_DEBUG)\
 \

OBJ_BEC_NULL_DEBUG=\
 $(OBJ_bec_null_DEBUG)\
 \

OBJ_BE_NULL_DEBUG=\
 $(OBJ_be_null_DEBUG)\
 \

OBJ_BEL_NULL_DEBUG=\
 $(OBJ_bel_null_DEBUG)\
 \

OBJ_BER_NULL_DEBUG=\
 $(OBJ_ber_null_DEBUG)\
 \

OBJ_BEC_COMERROBJ_DEBUG=\
 $(OBJ_bec_COMERROBJ_DEBUG)\
 \

OBJ_BEC_COMERROBJWC_DEBUG=\
 $(OBJ_bec_COMERROBJ_WC_DEBUG)\
 \

OBJ_BE_COMERROBJ_DEBUG=\
 $(OBJ_be_COMERROBJ_DEBUG)\
 \

OBJ_BEL_COMERROBJ_DEBUG=\
 $(OBJ_bel_COMERROBJ_DEBUG)\
 \

OBJ_BER_COMERROBJ_DEBUG=\
 $(OBJ_ber_COMERROBJ_DEBUG)\
 \

OBJ_BEC_SPEECH_DEBUG=\
 $(OBJ_bec_SPEECH_DEBUG)\
 \

OBJ_BEC_SPEECHWC_DEBUG=\
 $(OBJ_bec_SPEECH_WC_DEBUG)\
 \

OBJ_BE_SPEECH_DEBUG=\
 $(OBJ_be_SPEECH_DEBUG)\
 \

OBJ_BEL_SPEECH_DEBUG=\
 $(OBJ_bel_SPEECH_DEBUG)\
 \

OBJ_BER_SPEECH_DEBUG=\
 $(OBJ_ber_SPEECH_DEBUG)\
 \

OBJ_BEC_WINEVLOG_DEBUG=\
 $(OBJ_bec_WINEVLOG_DEBUG)\
 \

OBJ_BE_WINEVLOG_DEBUG=\
 $(OBJ_be_WINEVLOG_DEBUG)\
 \

OBJ_BEL_WINEVLOG_DEBUG=\
 $(OBJ_bel_WINEVLOG_DEBUG)\
 \

OBJ_BER_WINEVLOG_DEBUG=\
 $(OBJ_ber_WINEVLOG_DEBUG)\
 \

OBJ_BEC_WINMSGBX_DEBUG=\
 $(OBJ_bec_WINMSGBX_DEBUG)\
 \

OBJ_BE_WINMSGBX_DEBUG=\
 $(OBJ_be_WINMSGBX_DEBUG)\
 \

OBJ_BEL_WINMSGBX_DEBUG=\
 $(OBJ_bel_WINMSGBX_DEBUG)\
 \

OBJ_BER_WINMSGBX_DEBUG=\
 $(OBJ_ber_WINMSGBX_DEBUG)\
 \

OBJ_BEC_W32CONS_DEBUG=\
 $(OBJ_bec_W32CONS_DEBUG)\
 \

OBJ_BEC_W32CONSWC_DEBUG=\
 $(OBJ_bec_W32CONS_WC_DEBUG)\
 \

OBJ_BE_W32CONS_DEBUG=\
 $(OBJ_be_W32CONS_DEBUG)\
 \

OBJ_BEL_W32CONS_DEBUG=\
 $(OBJ_bel_W32CONS_DEBUG)\
 \

OBJ_BER_W32CONS_DEBUG=\
 $(OBJ_ber_W32CONS_DEBUG)\
 \

OBJ_BEC_WINDBGR_DEBUG=\
 $(OBJ_bec_WINDBGR_DEBUG)\
 \

OBJ_BEC_WINDBGR_WC_DEBUG=\
 $(OBJ_bec_WINDBGR_WC_DEBUG)\
 \

OBJ_BE_WINDBGR_DEBUG=\
 $(OBJ_be_WINDBGR_DEBUG)\
 \

OBJ_BEL_WINDBGR_DEBUG=\
 $(OBJ_bel_WINDBGR_DEBUG)\
 \

OBJ_BER_WINDBGR_DEBUG=\
 $(OBJ_ber_WINDBGR_DEBUG)\
 \

OBJ_BEC_WINSYSLOG_DEBUG=\
 $(OBJ_bec_WINSYSLOG_DEBUG)\
 \

OBJ_BEC_WINSYSLOG_WC_DEBUG=\
 $(OBJ_bec_WINSYSLOG_WC_DEBUG)\
 \

OBJ_BE_WINSYSLOG_DEBUG=\
 $(OBJ_be_WINSYSLOG_DEBUG)\
 \

OBJ_BEL_WINSYSLOG_DEBUG=\
 $(OBJ_bel_WINSYSLOG_DEBUG)\
 \

OBJ_BER_WINSYSLOG_DEBUG=\
 $(OBJ_ber_WINSYSLOG_DEBUG)\
 \

OBJ_FE_ALL_DEBUG=\
 $(OBJ_fe_all_DEBUG)\
 \

OBJ_FE_ALL_WC_DEBUG=\
 $(OBJ_fe_all_WC_DEBUG)\
 \

OBJ_FE_FAIL_DEBUG=\
 $(OBJ_fe_fail_DEBUG)\
 \

OBJ_FE_N_DEBUG=\
 $(OBJ_fe_N_DEBUG)\
 \

OBJ_FE_N_WC_DEBUG=\
 $(OBJ_fe_N_WC_DEBUG)\
 \

OBJ_FE_NULL_DEBUG=\
 $(OBJ_fe_null_DEBUG)\
 \

OBJ_FE_NULL_WC_DEBUG=\
 $(OBJ_fe_null_WC_DEBUG)\
 \

OBJ_FE_SIMPLE_DEBUG=\
 $(OBJ_fe_simple_DEBUG)\
 \

OBJ_FE_SIMPLE_WC_DEBUG=\
 $(OBJ_fe_simple_WC_DEBUG)\
 \

OBJ_FE_WINREG_DEBUG=\
 $(OBJ_fe_winreg_DEBUG)\
 \

OBJ_FE_WINREG_WC_DEBUG=\
 $(OBJ_fe_winreg_WC_DEBUG)\
 \

OBJ_utl_bailout_RELEASE		= $(PROJ_OBJ_DIR)\util.bailout.obj
OBJ_utl_init_code_strs_RELEASE	= $(PROJ_OBJ_DIR)\util.init_err_strings.obj
OBJ_utl_sev_strs_RELEASE		= $(PROJ_OBJ_DIR)\util.sev_strings.obj
OBJ_utl_snprintf_RELEASE 	= $(PROJ_OBJ_DIR)\util.snprintf.obj
OBJ_utl_strdup_RELEASE 		= $(PROJ_OBJ_DIR)\util.strdup.obj
OBJ_utl_procid_RELEASE 		= $(PROJ_OBJ_DIR)\util.procid.obj
OBJ_utl_thread_RELEASE 		= $(PROJ_OBJ_DIR)\util.thread.obj
OBJ_utl_be_context_RELEASE	= $(PROJ_OBJ_DIR)\util.be_context.obj
OBJ_utl_core_apidefs_RELEASE	= $(PROJ_OBJ_DIR)\util.core_apidefs.obj
OBJ_utl_be_parse_RELEASE		= $(PROJ_OBJ_DIR)\util.be_parse.obj
OBJ_utl_strnlen_RELEASE		= $(PROJ_OBJ_DIR)\util.strnlen.obj
OBJ_utl_strnlenp_RELEASE		= $(PROJ_OBJ_DIR)\util.strnlen.c++.obj
OBJ_utl_time_RELEASE			= $(PROJ_OBJ_DIR)\util.time.obj
OBJ_utl_host_name_RELEASE	= $(PROJ_OBJ_DIR)\util.host_name.obj
OBJ_cr_api_RELEASE 			= $(PROJ_OBJ_DIR)\core.api.obj
OBJ_cr_api_dep_RELEASE 		= $(PROJ_OBJ_DIR)\core.api.deprecated.obj
OBJ_cr_api_exit_RELEASE 		= $(PROJ_OBJ_DIR)\core.api.exitprocess.obj
OBJ_cr_api_logprintf_RELEASE	= $(PROJ_OBJ_DIR)\core.api.logprintf.obj
OBJ_cr_api_logputs_RELEASE	= $(PROJ_OBJ_DIR)\core.api.logputs.obj
OBJ_cr_auto_RELEASE 			= $(PROJ_OBJ_DIR)\core.auto.obj
OBJ_ins_args_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.args.obj
OBJ_ins_b64_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.b64.obj
OBJ_ins_blob_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.blob.obj
OBJ_ins_bool_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.boolean.obj
OBJ_ins_excp_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.exception.obj
OBJ_ins_host_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.hostid.obj
OBJ_ins_int_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.integer.obj
OBJ_ins_ptr_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.pointer.obj
OBJ_ins_prid_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.processid.obj
OBJ_ins_real_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.real.obj
OBJ_ins_slice_RELEASE	 	= $(PROJ_OBJ_DIR)\inserters.slice.obj
OBJ_ins_thrid_RELEASE	 	= $(PROJ_OBJ_DIR)\inserters.threadid.obj
OBJ_ins_w2m_RELEASE	 		= $(PROJ_OBJ_DIR)\inserters.w2m.obj
#PDB_bec_test_RELEASE 		= $(PROJ_OBJ_DIR)\bec.test.pdb
OBJ_bec_test_RELEASE 		= $(PROJ_OBJ_DIR)\bec.test.obj
OBJ_be_test_RELEASE 			= $(PROJ_OBJ_DIR)\be.test.obj
OBJ_bel_test_RELEASE 		= $(PROJ_OBJ_DIR)\bel.test.obj
OBJ_ber_test_RELEASE 		= $(PROJ_OBJ_DIR)\ber.test.obj
OBJ_be_lrsplit_RELEASE		= $(PROJ_OBJ_DIR)\be.lrsplit.obj
OBJ_be_N_RELEASE				= $(PROJ_OBJ_DIR)\be.N.obj
OBJ_bec_fail_RELEASE			= $(PROJ_OBJ_DIR)\bec.fail.obj
OBJ_be_fail_RELEASE			= $(PROJ_OBJ_DIR)\be.fail.obj
OBJ_bel_fail_RELEASE			= $(PROJ_OBJ_DIR)\bel.fail.obj
OBJ_ber_fail_RELEASE			= $(PROJ_OBJ_DIR)\ber.fail.obj
OBJ_bec_file_RELEASE			= $(PROJ_OBJ_DIR)\bec.file.obj
OBJ_bec_file_WC_RELEASE		= $(PROJ_OBJ_DIR)\bec.file.WithCallback.obj
OBJ_be_file_RELEASE			= $(PROJ_OBJ_DIR)\be.file.obj
OBJ_bel_file_RELEASE			= $(PROJ_OBJ_DIR)\bel.file.obj
OBJ_ber_file_RELEASE			= $(PROJ_OBJ_DIR)\ber.file.obj
OBJ_bec_fprintf_RELEASE		= $(PROJ_OBJ_DIR)\bec.fprintf.obj
OBJ_bec_fprintf_WC_RELEASE	= $(PROJ_OBJ_DIR)\bec.fprintf.WithCallback.obj
OBJ_be_fprintf_RELEASE		= $(PROJ_OBJ_DIR)\be.fprintf.obj
OBJ_bel_fprintf_RELEASE		= $(PROJ_OBJ_DIR)\bel.fprintf.obj
OBJ_ber_fprintf_RELEASE		= $(PROJ_OBJ_DIR)\ber.fprintf.obj
OBJ_bec_null_RELEASE			= $(PROJ_OBJ_DIR)\bec.null.obj
OBJ_be_null_RELEASE			= $(PROJ_OBJ_DIR)\be.null.obj
OBJ_bel_null_RELEASE			= $(PROJ_OBJ_DIR)\bel.null.obj
OBJ_ber_null_RELEASE			= $(PROJ_OBJ_DIR)\ber.null.obj
OBJ_bec_COMERROBJ_RELEASE	= $(PROJ_OBJ_DIR)\bec.COMErrorObject.obj
OBJ_bec_COMERROBJ_WC_RELEASE	= $(PROJ_OBJ_DIR)\bec.COMErrorObject.WithCallback.obj
OBJ_be_COMERROBJ_RELEASE		= $(PROJ_OBJ_DIR)\be.COMErrorObject.obj
OBJ_bel_COMERROBJ_RELEASE	= $(PROJ_OBJ_DIR)\bel.COMErrorObject.obj
OBJ_ber_COMERROBJ_RELEASE	= $(PROJ_OBJ_DIR)\ber.COMErrorObject.obj
OBJ_bec_SPEECH_RELEASE		= $(PROJ_OBJ_DIR)\bec.speech.obj
OBJ_bec_SPEECH_WC_RELEASE	= $(PROJ_OBJ_DIR)\bec.speech.WithCallback.obj
OBJ_be_SPEECH_RELEASE		= $(PROJ_OBJ_DIR)\be.speech.obj
OBJ_bel_SPEECH_RELEASE		= $(PROJ_OBJ_DIR)\bel.speech.obj
OBJ_ber_SPEECH_RELEASE		= $(PROJ_OBJ_DIR)\ber.speech.obj
OBJ_bec_WINEVLOG_RELEASE		= $(PROJ_OBJ_DIR)\bec.WindowsEventLog.obj
OBJ_be_WINEVLOG_RELEASE		= $(PROJ_OBJ_DIR)\be.WindowsEventLog.obj
OBJ_bel_WINEVLOG_RELEASE		= $(PROJ_OBJ_DIR)\bel.WindowsEventLog.obj
OBJ_ber_WINEVLOG_RELEASE		= $(PROJ_OBJ_DIR)\ber.WindowsEventLog.obj
OBJ_bec_WINMSGBX_RELEASE		= $(PROJ_OBJ_DIR)\bec.WindowsMessageBox.obj
OBJ_be_WINMSGBX_RELEASE		= $(PROJ_OBJ_DIR)\be.WindowsMessageBox.obj
OBJ_bel_WINMSGBX_RELEASE		= $(PROJ_OBJ_DIR)\bel.WindowsMessageBox.obj
OBJ_ber_WINMSGBX_RELEASE		= $(PROJ_OBJ_DIR)\ber.WindowsMessageBox.obj
OBJ_bec_W32CONS_RELEASE		= $(PROJ_OBJ_DIR)\bec.WindowsConsole.obj
OBJ_bec_W32CONS_WC_RELEASE	= $(PROJ_OBJ_DIR)\bec.WindowsConsole.WithCallback.obj
OBJ_be_W32CONS_RELEASE		= $(PROJ_OBJ_DIR)\be.WindowsConsole.obj
OBJ_bel_W32CONS_RELEASE		= $(PROJ_OBJ_DIR)\bel.WindowsConsole.obj
OBJ_ber_W32CONS_RELEASE		= $(PROJ_OBJ_DIR)\ber.WindowsConsole.obj
OBJ_bec_WINDBGR_RELEASE		= $(PROJ_OBJ_DIR)\bec.WindowsDebugger.obj
OBJ_bec_WINDBGR_WC_RELEASE	= $(PROJ_OBJ_DIR)\bec.WindowsDebugger.WithCallback.obj
OBJ_be_WINDBGR_RELEASE		= $(PROJ_OBJ_DIR)\be.WindowsDebugger.obj
OBJ_bel_WINDBGR_RELEASE		= $(PROJ_OBJ_DIR)\bel.WindowsDebugger.obj
OBJ_ber_WINDBGR_RELEASE		= $(PROJ_OBJ_DIR)\ber.WindowsDebugger.obj
OBJ_bec_WINSYSLOG_RELEASE	= $(PROJ_OBJ_DIR)\bec.WindowsSyslog.obj
OBJ_bec_WINSYSLOG_WC_RELEASE	= $(PROJ_OBJ_DIR)\bec.WindowsSyslog.WithCallback.obj
OBJ_be_WINSYSLOG_RELEASE		= $(PROJ_OBJ_DIR)\be.WindowsSyslog.obj
OBJ_bel_WINSYSLOG_RELEASE	= $(PROJ_OBJ_DIR)\bel.WindowsSyslog.obj
OBJ_ber_WINSYSLOG_RELEASE	= $(PROJ_OBJ_DIR)\ber.WindowsSyslog.obj
OBJ_fe_all_RELEASE 			= $(PROJ_OBJ_DIR)\fe.all.obj
OBJ_fe_all_WC_RELEASE 		= $(PROJ_OBJ_DIR)\fe.all.WithCallback.obj
OBJ_fe_fail_RELEASE 			= $(PROJ_OBJ_DIR)\fe.fail.obj
OBJ_fe_N_RELEASE 			= $(PROJ_OBJ_DIR)\fe.N.obj
OBJ_fe_N_WC_RELEASE 			= $(PROJ_OBJ_DIR)\fe.N.WithCallback.obj
OBJ_fe_null_RELEASE 			= $(PROJ_OBJ_DIR)\fe.null.obj
OBJ_fe_null_WC_RELEASE 		= $(PROJ_OBJ_DIR)\fe.null.WithCallback.obj
OBJ_fe_simple_RELEASE 		= $(PROJ_OBJ_DIR)\fe.simple.obj
OBJ_fe_simple_WC_RELEASE 	= $(PROJ_OBJ_DIR)\fe.simple.WithCallback.obj
OBJ_fe_winreg_RELEASE 		= $(PROJ_OBJ_DIR)\fe.WindowsRegistry.obj
OBJ_fe_winreg_WC_RELEASE 	= $(PROJ_OBJ_DIR)\fe.WindowsRegistry.WithCallback.obj

OBJ_UTIL_RELEASE=\
 $(OBJ_utl_bailout_RELEASE)\
 $(OBJ_utl_init_code_strs_RELEASE)\
 $(OBJ_utl_sev_strs_RELEASE)\
 $(OBJ_utl_snprintf_RELEASE)\
 $(OBJ_utl_strdup_RELEASE)\
 $(OBJ_utl_procid_RELEASE)\
 $(OBJ_utl_thread_RELEASE)\
 $(OBJ_utl_be_context_RELEASE)\
 $(OBJ_utl_core_apidefs_RELEASE)\
 $(OBJ_utl_be_parse_RELEASE)\
 $(OBJ_utl_strnlen_RELEASE)\
 $(OBJ_utl_strnlenp_RELEASE)\
 $(OBJ_utl_time_RELEASE)\
 $(OBJ_utl_host_name_RELEASE)\
 \

OBJ_CORE_RELEASE=\
 $(OBJ_cr_api_RELEASE)\
 $(OBJ_cr_api_dep_RELEASE)\
 $(OBJ_cr_api_exit_RELEASE)\
 $(OBJ_cr_api_logprintf_RELEASE)\
 $(OBJ_cr_api_logputs_RELEASE)\
 $(OBJ_cr_auto_RELEASE)\
 $(OBJ_ins_args_RELEASE)\
 $(OBJ_ins_b64_RELEASE)\
 $(OBJ_ins_blob_RELEASE)\
 $(OBJ_ins_bool_RELEASE)\
 $(OBJ_ins_excp_RELEASE)\
 $(OBJ_ins_host_RELEASE)\
 $(OBJ_ins_int_RELEASE)\
 $(OBJ_ins_ptr_RELEASE)\
 $(OBJ_ins_prid_RELEASE)\
 $(OBJ_ins_real_RELEASE)\
 $(OBJ_ins_slice_RELEASE)\
 $(OBJ_ins_thrid_RELEASE)\
 $(OBJ_ins_w2m_RELEASE)\
 \

OBJ_APPL_RELEASE=\
 $(OBJ_ins_args_RELEASE)\
 $(OBJ_ins_b64_RELEASE)\
 $(OBJ_ins_blob_RELEASE)\
 $(OBJ_ins_bool_RELEASE)\
 $(OBJ_ins_excp_RELEASE)\
 $(OBJ_ins_host_RELEASE)\
 $(OBJ_ins_int_RELEASE)\
 $(OBJ_ins_ptr_RELEASE)\
 $(OBJ_ins_prid_RELEASE)\
 $(OBJ_ins_real_RELEASE)\
 $(OBJ_ins_slice_RELEASE)\
 $(OBJ_ins_thrid_RELEASE)\
 $(OBJ_ins_w2m_RELEASE)\
 \


OBJ_BE_LRSPLIT_RELEASE=\
 $(OBJ_be_lrsplit_RELEASE)\
 \

OBJ_BE_N_RELEASE=\
 $(OBJ_be_N_RELEASE)\
 \

OBJ_BEC_TEST_RELEASE=\
 $(OBJ_bec_test_RELEASE)\
 \


OBJ_BE_TEST_RELEASE=\
 $(OBJ_be_test_RELEASE)\
 \


OBJ_BEL_TEST_RELEASE=\
 $(OBJ_bel_test_RELEASE)\
 \


OBJ_BER_TEST_RELEASE=\
 $(OBJ_ber_test_RELEASE)\
 \


OBJ_BEC_FAIL_RELEASE=\
 $(OBJ_bec_fail_RELEASE)\
 \

OBJ_BE_FAIL_RELEASE=\
 $(OBJ_be_fail_RELEASE)\
 \

OBJ_BEL_FAIL_RELEASE=\
 $(OBJ_bel_fail_RELEASE)\
 \

OBJ_BER_FAIL_RELEASE=\
 $(OBJ_ber_fail_RELEASE)\
 \

OBJ_BEC_FILE_RELEASE=\
 $(OBJ_bec_file_RELEASE)\
 \

OBJ_BEC_FILE_WC_RELEASE=\
 $(OBJ_bec_file_WC_RELEASE)\
 \

OBJ_BE_FILE_RELEASE=\
 $(OBJ_be_file_RELEASE)\
 \

OBJ_BEL_FILE_RELEASE=\
 $(OBJ_bel_file_RELEASE)\
 \

OBJ_BER_FILE_RELEASE=\
 $(OBJ_ber_file_RELEASE)\
 \

OBJ_BEC_FPRINTF_RELEASE=\
 $(OBJ_bec_fprintf_RELEASE)\
 \

OBJ_BEC_FPRINTF_WC_RELEASE=\
 $(OBJ_bec_fprintf_WC_RELEASE)\
 \

OBJ_BE_FPRINTF_RELEASE=\
 $(OBJ_be_fprintf_RELEASE)\
 \

OBJ_BEL_FPRINTF_RELEASE=\
 $(OBJ_bel_fprintf_RELEASE)\
 \

OBJ_BER_FPRINTF_RELEASE=\
 $(OBJ_ber_fprintf_RELEASE)\
 \

OBJ_BEC_NULL_RELEASE=\
 $(OBJ_bec_null_RELEASE)\
 \

OBJ_BE_NULL_RELEASE=\
 $(OBJ_be_null_RELEASE)\
 \

OBJ_BEL_NULL_RELEASE=\
 $(OBJ_bel_null_RELEASE)\
 \

OBJ_BER_NULL_RELEASE=\
 $(OBJ_ber_null_RELEASE)\
 \

OBJ_BEC_COMERROBJ_RELEASE=\
 $(OBJ_bec_COMERROBJ_RELEASE)\
 \

OBJ_BEC_COMERROBJWC_RELEASE=\
 $(OBJ_bec_COMERROBJ_WC_RELEASE)\
 \

OBJ_BE_COMERROBJ_RELEASE=\
 $(OBJ_be_COMERROBJ_RELEASE)\
 \

OBJ_BEL_COMERROBJ_RELEASE=\
 $(OBJ_bel_COMERROBJ_RELEASE)\
 \

OBJ_BER_COMERROBJ_RELEASE=\
 $(OBJ_ber_COMERROBJ_RELEASE)\
 \

OBJ_BEC_SPEECH_RELEASE=\
 $(OBJ_bec_SPEECH_RELEASE)\
 \

OBJ_BEC_SPEECHWC_RELEASE=\
 $(OBJ_bec_SPEECH_WC_RELEASE)\
 \

OBJ_BE_SPEECH_RELEASE=\
 $(OBJ_be_SPEECH_RELEASE)\
 \

OBJ_BEL_SPEECH_RELEASE=\
 $(OBJ_bel_SPEECH_RELEASE)\
 \

OBJ_BER_SPEECH_RELEASE=\
 $(OBJ_ber_SPEECH_RELEASE)\
 \

OBJ_BEC_WINEVLOG_RELEASE=\
 $(OBJ_bec_WINEVLOG_RELEASE)\
 \

OBJ_BE_WINEVLOG_RELEASE=\
 $(OBJ_be_WINEVLOG_RELEASE)\
 \

OBJ_BEL_WINEVLOG_RELEASE=\
 $(OBJ_bel_WINEVLOG_RELEASE)\
 \

OBJ_BER_WINEVLOG_RELEASE=\
 $(OBJ_ber_WINEVLOG_RELEASE)\
 \

OBJ_BEC_WINMSGBX_RELEASE=\
 $(OBJ_bec_WINMSGBX_RELEASE)\
 \

OBJ_BE_WINMSGBX_RELEASE=\
 $(OBJ_be_WINMSGBX_RELEASE)\
 \

OBJ_BEL_WINMSGBX_RELEASE=\
 $(OBJ_bel_WINMSGBX_RELEASE)\
 \

OBJ_BER_WINMSGBX_RELEASE=\
 $(OBJ_ber_WINMSGBX_RELEASE)\
 \

OBJ_BEC_W32CONS_RELEASE=\
 $(OBJ_bec_W32CONS_RELEASE)\
 \

OBJ_BEC_W32CONSWC_RELEASE=\
 $(OBJ_bec_W32CONS_WC_RELEASE)\
 \

OBJ_BE_W32CONS_RELEASE=\
 $(OBJ_be_W32CONS_RELEASE)\
 \

OBJ_BEL_W32CONS_RELEASE=\
 $(OBJ_bel_W32CONS_RELEASE)\
 \

OBJ_BER_W32CONS_RELEASE=\
 $(OBJ_ber_W32CONS_RELEASE)\
 \

OBJ_BEC_WINDBGR_RELEASE=\
 $(OBJ_bec_WINDBGR_RELEASE)\
 \

OBJ_BEC_WINDBGR_WC_RELEASE=\
 $(OBJ_bec_WINDBGR_WC_RELEASE)\
 \

OBJ_BE_WINDBGR_RELEASE=\
 $(OBJ_be_WINDBGR_RELEASE)\
 \

OBJ_BEL_WINDBGR_RELEASE=\
 $(OBJ_bel_WINDBGR_RELEASE)\
 \

OBJ_BER_WINDBGR_RELEASE=\
 $(OBJ_ber_WINDBGR_RELEASE)\
 \

OBJ_BEC_WINSYSLOG_RELEASE=\
 $(OBJ_bec_WINSYSLOG_RELEASE)\
 \

OBJ_BEC_WINSYSLOG_WC_RELEASE=\
 $(OBJ_bec_WINSYSLOG_WC_RELEASE)\
 \

OBJ_BE_WINSYSLOG_RELEASE=\
 $(OBJ_be_WINSYSLOG_RELEASE)\
 \

OBJ_BEL_WINSYSLOG_RELEASE=\
 $(OBJ_bel_WINSYSLOG_RELEASE)\
 \

OBJ_BER_WINSYSLOG_RELEASE=\
 $(OBJ_ber_WINSYSLOG_RELEASE)\
 \

OBJ_FE_ALL_RELEASE=\
 $(OBJ_fe_all_RELEASE)\
 \

OBJ_FE_ALL_WC_RELEASE=\
 $(OBJ_fe_all_WC_RELEASE)\
 \

OBJ_FE_FAIL_RELEASE=\
 $(OBJ_fe_fail_RELEASE)\
 \

OBJ_FE_N_RELEASE=\
 $(OBJ_fe_N_RELEASE)\
 \

OBJ_FE_N_WC_RELEASE=\
 $(OBJ_fe_N_WC_RELEASE)\
 \

OBJ_FE_NULL_RELEASE=\
 $(OBJ_fe_null_RELEASE)\
 \

OBJ_FE_NULL_WC_RELEASE=\
 $(OBJ_fe_null_WC_RELEASE)\
 \

OBJ_FE_SIMPLE_RELEASE=\
 $(OBJ_fe_simple_RELEASE)\
 \

OBJ_FE_SIMPLE_WC_RELEASE=\
 $(OBJ_fe_simple_WC_RELEASE)\
 \

OBJ_FE_WINREG_RELEASE=\
 $(OBJ_fe_winreg_RELEASE)\
 \

OBJ_FE_WINREG_WC_RELEASE=\
 $(OBJ_fe_winreg_WC_RELEASE)\
 \


# b64

OBJ_b64_b64_DEBUG            = $(PROJ_OBJ_DIR)\b64.pan.b64.debug.obj

OBJ_B64_DEBUG=\
 $(OBJ_b64_b64_DEBUG)\
 \

OBJ_b64_b64_RELEASE            = $(PROJ_OBJ_DIR)\b64.pan.b64.obj

OBJ_B64_RELEASE=\
 $(OBJ_b64_b64_RELEASE)\
 \


# shwild

OBJ_shw_api_DEBUG            = $(PROJ_OBJ_DIR)\shwild.pan.api.debug.obj
OBJ_shw_matches_DEBUG        = $(PROJ_OBJ_DIR)\shwild.pan.matches.debug.obj
OBJ_shw_pattern_DEBUG        = $(PROJ_OBJ_DIR)\shwild.pan.pattern.debug.obj

OBJ_SHWILD_DEBUG=\
 $(OBJ_shw_api_DEBUG)\
 $(OBJ_shw_matches_DEBUG)\
 $(OBJ_shw_pattern_DEBUG)\
 \

OBJ_shw_api_RELEASE            = $(PROJ_OBJ_DIR)\shwild.pan.api.obj
OBJ_shw_matches_RELEASE        = $(PROJ_OBJ_DIR)\shwild.pan.matches.obj
OBJ_shw_pattern_RELEASE        = $(PROJ_OBJ_DIR)\shwild.pan.pattern.obj

OBJ_SHWILD_RELEASE=\
 $(OBJ_shw_api_RELEASE)\
 $(OBJ_shw_matches_RELEASE)\
 $(OBJ_shw_pattern_RELEASE)\
 \


# xtests

OBJ_xts_core_DEBUG           = $(PROJ_OBJ_DIR)\xtests.pan.core.api.debug.obj

OBJ_XTESTS_DEBUG=\
 $(OBJ_xts_core_DEBUG)\
 \

OBJ_xts_core_RELEASE           = $(PROJ_OBJ_DIR)\xtests.pan.core.api.obj

OBJ_XTESTS_RELEASE=\
 $(OBJ_xts_core_RELEASE)\
 \



########################################
# Examples

OBJ_EX_P_be_01_DEBUG	=	.\ex.cpp.be.01.debug.obj
OBJ_EX_P_be_02_DEBUG	=	.\ex.cpp.be.02.debug.obj
OBJ_EX_P_be_03_DEBUG	=	.\ex.cpp.be.03.debug.obj
OBJ_EX_P_be_04_DEBUG	=	.\ex.cpp.be.04.debug.obj
OBJ_EX_P_be_05_DEBUG	=	.\ex.cpp.be.05.debug.obj
OBJ_EX_P_be_06_DEBUG	=	.\ex.cpp.be.06.debug.obj
OBJ_EX_P_cp_01_DEBUG	=	.\ex.cpp.cp.01.debug.obj
OBJ_EX_P_cp_02_DEBUG	=	.\ex.cpp.cp.02.debug.obj
OBJ_EX_P_fm_01_DEBUG	=	.\ex.cpp.fm.01.debug.obj
OBJ_EX_P_fm_02_DEBUG	=	.\ex.cpp.fm.02.debug.obj
OBJ_EX_P_fm_03_DEBUG	=	.\ex.cpp.fm.03.debug.obj
OBJ_EX_P_fe_01_DEBUG	=	.\ex.cpp.fe.01.debug.obj
OBJ_EX_P_in_01_DEBUG	=	.\ex.cpp.in.01.debug.obj
OBJ_EX_P_in_02_DEBUG	=	.\ex.cpp.in.02.debug.obj
OBJ_EX_P_in_03_DEBUG	=	.\ex.cpp.in.03.debug.obj
OBJ_EX_P_in_04_DEBUG	=	.\ex.cpp.in.04.debug.obj
OBJ_EX_P_in_05_DEBUG	=	.\ex.cpp.in.05.debug.obj
OBJ_EX_P_in_06_DEBUG	=	.\ex.cpp.in.06.debug.obj
OBJ_EX_P_in_07_DEBUG	=	.\ex.cpp.in.07.debug.obj
OBJ_EX_P_in_08_DEBUG	=	.\ex.cpp.in.08.debug.obj
OBJ_EX_P_in_09_DEBUG	=	.\ex.cpp.in.09.debug.obj
OBJ_EX_P_in_10_DEBUG	=	.\ex.cpp.in.10.debug.obj
OBJ_EX_P_in_11_DEBUG	=	.\ex.cpp.in.11.debug.obj
OBJ_EX_P_in_12_DEBUG	=	.\ex.cpp.in.12.debug.obj
OBJ_EX_P_in_13_DEBUG	=	.\ex.cpp.in.13.debug.obj
OBJ_EX_P_in_14_DEBUG	=	.\ex.cpp.in.14.debug.obj
OBJ_EX_P_in_15_DEBUG	=	.\ex.cpp.in.15.debug.obj
OBJ_EX_P_in_16_DEBUG	=	.\ex.cpp.in.16.debug.obj
OBJ_EX_P_in_17_DEBUG	=	.\ex.cpp.in.17.debug.obj
OBJ_EX_P_ms_01_DEBUG	=	.\ex.cpp.m2.01.debug.obj
OBJ_EX_P_ms_02_DEBUG	=	.\ex.cpp.m2.02.debug.obj
OBJ_EX_P_ms_03_DEBUG	=	.\ex.cpp.m2.03.debug.obj
OBJ_EX_P_ms_04_DEBUG	=	.\ex.cpp.m2.04.debug.obj
OBJ_EX_P_ms_05_DEBUG	=	.\ex.cpp.m2.05.debug.obj
OBJ_EX_P_ms_06_DEBUG	=	.\ex.cpp.ms.06.debug.obj
OBJ_EX_P_ut_01_DEBUG	=	.\ex.cpp.ut.01.debug.obj
OBJ_EX_C_0_DEBUG		=	.\ex.c.0.debug.obj
OBJ_EX_C_1_DEBUG		=	.\ex.c.1.debug.obj
OBJ_EX_C_2_DEBUG		=	.\ex.c.2.debug.obj
OBJ_EX_C_3_DEBUG		=	.\ex.c.3.debug.obj
OBJ_EX_C_4_DEBUG		=	.\ex.c.4.debug.obj
OBJ_EX_C_5_DEBUG		=	.\ex.c.5.debug.obj
OBJ_EX_C_6_DEBUG		=	.\ex.c.6.debug.obj
OBJ_EX_C_7_DEBUG		=	.\ex.c.7.debug.obj
OBJ_EX_P_be_01_RELEASE	=	.\ex.cpp.be.01.obj
OBJ_EX_P_be_02_RELEASE	=	.\ex.cpp.be.02.obj
OBJ_EX_P_be_03_RELEASE	=	.\ex.cpp.be.03.obj
OBJ_EX_P_be_04_RELEASE	=	.\ex.cpp.be.04.obj
OBJ_EX_P_be_05_RELEASE	=	.\ex.cpp.be.05.obj
OBJ_EX_P_be_06_RELEASE	=	.\ex.cpp.be.06.obj
OBJ_EX_P_cp_01_RELEASE	=	.\ex.cpp.cp.01.obj
OBJ_EX_P_cp_02_RELEASE	=	.\ex.cpp.cp.02.obj
OBJ_EX_P_fm_01_RELEASE	=	.\ex.cpp.fm.01.obj
OBJ_EX_P_fm_02_RELEASE	=	.\ex.cpp.fm.02.obj
OBJ_EX_P_fm_03_RELEASE	=	.\ex.cpp.fm.03.obj
OBJ_EX_P_fe_01_RELEASE	=	.\ex.cpp.fe.01.obj
OBJ_EX_P_in_01_RELEASE	=	.\ex.cpp.in.01.obj
OBJ_EX_P_in_02_RELEASE	=	.\ex.cpp.in.02.obj
OBJ_EX_P_in_03_RELEASE	=	.\ex.cpp.in.03.obj
OBJ_EX_P_in_04_RELEASE	=	.\ex.cpp.in.04.obj
OBJ_EX_P_in_05_RELEASE	=	.\ex.cpp.in.05.obj
OBJ_EX_P_in_06_RELEASE	=	.\ex.cpp.in.06.obj
OBJ_EX_P_in_07_RELEASE	=	.\ex.cpp.in.07.obj
OBJ_EX_P_in_08_RELEASE	=	.\ex.cpp.in.08.obj
OBJ_EX_P_in_09_RELEASE	=	.\ex.cpp.in.09.obj
OBJ_EX_P_in_10_RELEASE	=	.\ex.cpp.in.10.obj
OBJ_EX_P_in_11_RELEASE	=	.\ex.cpp.in.11.obj
OBJ_EX_P_in_12_RELEASE	=	.\ex.cpp.in.12.obj
OBJ_EX_P_in_13_RELEASE	=	.\ex.cpp.in.13.obj
OBJ_EX_P_in_14_RELEASE	=	.\ex.cpp.in.14.obj
OBJ_EX_P_in_15_RELEASE	=	.\ex.cpp.in.15.obj
OBJ_EX_P_in_16_RELEASE	=	.\ex.cpp.in.16.obj
OBJ_EX_P_in_17_RELEASE	=	.\ex.cpp.in.17.obj
OBJ_EX_P_ms_01_RELEASE	=	.\ex.cpp.m2.01.obj
OBJ_EX_P_ms_02_RELEASE	=	.\ex.cpp.m2.02.obj
OBJ_EX_P_ms_03_RELEASE	=	.\ex.cpp.m2.03.obj
OBJ_EX_P_ms_04_RELEASE	=	.\ex.cpp.m2.04.obj
OBJ_EX_P_ms_05_RELEASE	=	.\ex.cpp.m2.05.obj
OBJ_EX_P_ms_06_RELEASE	=	.\ex.cpp.ms.06.obj
OBJ_EX_P_ut_01_RELEASE	=	.\ex.cpp.ut.01.obj
OBJ_EX_C_0_RELEASE		=	.\ex.c.0.obj
OBJ_EX_C_1_RELEASE		=	.\ex.c.1.obj
OBJ_EX_C_2_RELEASE		=	.\ex.c.2.obj
OBJ_EX_C_3_RELEASE		=	.\ex.c.3.obj
OBJ_EX_C_4_RELEASE		=	.\ex.c.4.obj
OBJ_EX_C_5_RELEASE		=	.\ex.c.5.obj
OBJ_EX_C_6_RELEASE		=	.\ex.c.6.obj
OBJ_EX_C_7_RELEASE		=	.\ex.c.7.obj

########################################
# Tests

##############################
# Component Tests

OBJ_TEST_COMP_core_initseq_DEBUG	=	.\test.component.core.init_sequence.debug.obj
OBJ_TEST_COMP_core_initseq_RELEASE	=	.\test.component.core.init_sequence.obj
OBJ_TEST_COMP_custsev_DEBUG	=	.\test.component.custom_severity.debug.obj
OBJ_TEST_COMP_custsev_RELEASE	=	.\test.component.custom_severity.obj
OBJ_TEST_COMP_ins_args_DEBUG	=	.\test.component.inserters.args.debug.obj
OBJ_TEST_COMP_ins_args_RELEASE	=	.\test.component.inserters.args.obj
OBJ_TEST_COMP_ins_b64_DEBUG	=	.\test.component.inserters.b64.debug.obj
OBJ_TEST_COMP_ins_b64_RELEASE	=	.\test.component.inserters.b64.obj
OBJ_TEST_COMP_ins_bool_DEBUG	=	.\test.component.inserters.boolean.debug.obj
OBJ_TEST_COMP_ins_bool_RELEASE	=	.\test.component.inserters.boolean.obj
OBJ_TEST_COMP_ins_hostid_DEBUG	=	.\test.component.inserters.hostId.debug.obj
OBJ_TEST_COMP_ins_hostid_RELEASE	=	.\test.component.inserters.hostId.obj
OBJ_TEST_COMP_ins_int_DEBUG	=	.\test.component.inserters.integer.debug.obj
OBJ_TEST_COMP_ins_int_RELEASE	=	.\test.component.inserters.integer.obj
OBJ_TEST_COMP_ins_pad_DEBUG	=	.\test.component.inserters.pad.debug.obj
OBJ_TEST_COMP_ins_pad_RELEASE	=	.\test.component.inserters.pad.obj
OBJ_TEST_COMP_ins_ptr_DEBUG	=	.\test.component.inserters.pointer.debug.obj
OBJ_TEST_COMP_ins_ptr_RELEASE	=	.\test.component.inserters.pointer.obj
OBJ_TEST_COMP_ins_prid_DEBUG	=	.\test.component.inserters.processId.debug.obj
OBJ_TEST_COMP_ins_prid_RELEASE	=	.\test.component.inserters.processId.obj
OBJ_TEST_COMP_ins_real_DEBUG	=	.\test.component.inserters.real.debug.obj
OBJ_TEST_COMP_ins_real_RELEASE	=	.\test.component.inserters.real.obj
OBJ_TEST_COMP_ins_slice_DEBUG	=	.\test.component.inserters.slice.debug.obj
OBJ_TEST_COMP_ins_slice_RELEASE	=	.\test.component.inserters.slice.obj
OBJ_TEST_COMP_ins_thrid_DEBUG	=	.\test.component.inserters.threadId.debug.obj
OBJ_TEST_COMP_ins_thrid_RELEASE	=	.\test.component.inserters.threadId.obj
OBJ_TEST_COMP_ins_vbool_DEBUG	=	.\test.component.inserters.variant_bool.debug.obj
OBJ_TEST_COMP_ins_vbool_RELEASE	=	.\test.component.inserters.variant_bool.obj
OBJ_TEST_COMP_ins_w2m_DEBUG	=	.\test.component.inserters.w2m.debug.obj
OBJ_TEST_COMP_ins_w2m_RELEASE	=	.\test.component.inserters.w2m.obj
OBJ_TEST_COMP_log_1_DEBUG	=	.\test.component.log.1.debug.obj
OBJ_TEST_COMP_log_1_RELEASE	=	.\test.component.log.1.obj
OBJ_TEST_COMP_pan_puts_DEBUG	=	.\test.component.core.pan_logputs.debug.obj
OBJ_TEST_COMP_pan_puts_RELEASE	=	.\test.component.core.pan_logputs.obj
OBJ_TEST_COMP_pan_prtf_DEBUG	=	.\test.component.core.pan_logprintf.debug.obj
OBJ_TEST_COMP_pan_prtf_RELEASE	=	.\test.component.core.pan_logprintf.obj

##############################
# Performance Tests

OBJ_TEST_PERF_ins_w2m_DEBUG		=	.\test.performance.inserters.w2m.debug.obj
OBJ_TEST_PERF_ins_w2m_RELEASE		=	.\test.performance.inserters.w2m.obj

##############################
# Scratch Tests

OBJ_TEST_API_DEBUG					=	.\test.api.debug.obj
OBJ_TEST_FE_DEBUG					=	.\test.fe.debug.obj
OBJ_TEST_BE_N_DEBUG				=	.\test.be.N.fail.debug.obj
OBJ_TEST_FE_SIMPLE_WC_DEBUG		=	.\test.fe.simple.WithCallback.debug.obj
OBJ_TEST_API_RELEASE					=	.\test.api.obj
OBJ_TEST_FE_RELEASE					=	.\test.fe.obj
OBJ_TEST_BE_N_RELEASE				=	.\test.be.N.fail.obj
OBJ_TEST_FE_SIMPLE_WC_RELEASE		=	.\test.fe.simple.WithCallback.obj

##############################
# Unit Tests

OBJ_TEST_UNIT_lvls_dyninit_DEBUG	=	.\test.unit.levels.dynamic_init.debug.obj
OBJ_TEST_UNIT_lvls_dyninit_RELEASE	=	.\test.unit.levels.dynamic_init.obj
OBJ_TEST_UNIT_lvls_vals_DEBUG	=	.\test.unit.levels.values.debug.obj
OBJ_TEST_UNIT_lvls_vals_RELEASE	=	.\test.unit.levels.values.obj
OBJ_TEST_UNIT_getcurrtm_DEBUG	=	.\test.unit.util.getcurrenttime.debug.obj
OBJ_TEST_UNIT_getcurrtm_RELEASE	=	.\test.unit.util.getcurrenttime.obj
OBJ_TEST_UNIT_getversion_DEBUG	=	.\test.unit.getversion.debug.obj
OBJ_TEST_UNIT_getversion_RELEASE	=	.\test.unit.getversion.obj
OBJ_TEST_UNIT_gethostname_DEBUG	=	.\test.unit.util.gethostname.debug.obj
OBJ_TEST_UNIT_gethostname_RELEASE	=	.\test.unit.util.gethostname.obj
OBJ_TEST_UNIT_onbailout_DEBUG	=	.\test.unit.util.onbailout.debug.obj
OBJ_TEST_UNIT_strnlen_DEBUG	=	.\test.unit.util.strnlen.debug.obj
OBJ_TEST_UNIT_onbailout_RELEASE	=	.\test.unit.util.onbailout.obj
OBJ_TEST_UNIT_strnlen_RELEASE	=	.\test.unit.util.strnlen.obj
OBJ_TEST_UNIT_bec_CEO_DEBUG		=	.\test.unit.bec.COMErrorObject.debug.obj
OBJ_TEST_UNIT_bec_CEO_RELEASE		=	.\test.unit.bec.COMErrorObject.obj
OBJ_TEST_UNIT_bec_file_DEBUG		=	.\test.unit.bec.file.debug.obj
OBJ_TEST_UNIT_bec_file_RELEASE		=	.\test.unit.bec.file.obj
OBJ_TEST_UNIT_bec_fprintf_DEBUG	=	.\test.unit.bec.fprintf.debug.obj
OBJ_TEST_UNIT_bec_fprintf_RELEASE	=	.\test.unit.bec.fprintf.obj
OBJ_TEST_UNIT_bec_fail_DEBUG		=	.\test.unit.bec.fail.debug.obj
OBJ_TEST_UNIT_bec_fail_RELEASE		=	.\test.unit.bec.fail.obj
OBJ_TEST_UNIT_be_fail_DEBUG		=	.\test.unit.be.fail.debug.obj
OBJ_TEST_UNIT_be_fail_RELEASE		=	.\test.unit.be.fail.obj
OBJ_TEST_UNIT_be_lrsplit_DEBUG	=	.\test.unit.be.lrsplit.debug.obj
OBJ_TEST_UNIT_be_lrsplit_RELEASE	=	.\test.unit.be.lrsplit.obj
OBJ_TEST_UNIT_be_N_DEBUG			=	.\test.unit.be.N.debug.obj
OBJ_TEST_UNIT_be_N_RELEASE			=	.\test.unit.be.N.obj
OBJ_TEST_UNIT_fe_all_DEBUG		=	.\test.unit.fe.all.debug.obj
OBJ_TEST_UNIT_fe_all_RELEASE		=	.\test.unit.fe.all.obj
OBJ_TEST_UNIT_fe_fail_DEBUG		=	.\test.unit.fe.fail.debug.obj
OBJ_TEST_UNIT_fe_fail_RELEASE		=	.\test.unit.fe.fail.obj
OBJ_TEST_UNIT_fe_N_DEBUG			=	.\test.unit.fe.N.debug.obj
OBJ_TEST_UNIT_fe_N_RELEASE			=	.\test.unit.fe.N.obj
OBJ_TEST_UNIT_fe_null_DEBUG		=	.\test.unit.fe.null.debug.obj
OBJ_TEST_UNIT_fe_null_RELEASE		=	.\test.unit.fe.null.obj
OBJ_TEST_UNIT_fe_simple_DEBUG	=	.\test.unit.fe.simple.debug.obj
OBJ_TEST_UNIT_fe_simple_RELEASE	=	.\test.unit.fe.simple.obj
OBJ_TEST_UNIT_fe_WinReg_DEBUG	=	.\test.unit.fe.WindowsRegistry.debug.obj
OBJ_TEST_UNIT_fe_WinReg_RELEASE	=	.\test.unit.fe.WindowsRegistry.obj

OBJ_TEST_UNIT_ins_int_DEBUG	=	.\test.unit.inserter.integer.debug.obj
OBJ_TEST_UNIT_ins_w2m_DEBUG	=	.\test.unit.inserter.w2m.debug.obj
OBJ_TEST_UNIT_ins_int_RELEASE	=	.\test.unit.inserter.integer.obj
OBJ_TEST_UNIT_ins_w2m_RELEASE	=	.\test.unit.inserter.w2m.obj

##################################################
# Target groups

########################################
# Objects

# util
OBJS_UTIL=\
 $(OBJ_UTIL_DEBUG)\
 $(OBJ_UTIL_RELEASE)\
 \

# core
OBJS_CORE=\
 $(OBJ_CORE_DEBUG)\
 $(OBJ_CORE_RELEASE)\
 \

# appl
OBJS_APPL=\
 $(OBJ_APPL_DEBUG)\
 $(OBJ_APPL_RELEASE)\
 \

# front-ends
OBJS_FE=\
 $(OBJ_FE_ALL_DEBUG)\
 $(OBJ_FE_ALL_WC_DEBUG)\
 $(OBJ_FE_FAIL_DEBUG)\
 $(OBJ_FE_N_DEBUG)\
 $(OBJ_FE_N_WC_DEBUG)\
 $(OBJ_FE_NULL_DEBUG)\
 $(OBJ_FE_NULL_WC_DEBUG)\
 $(OBJ_FE_SIMPLE_DEBUG)\
 $(OBJ_FE_SIMPLE_WC_DEBUG)\
 $(OBJ_FE_WINREG_DEBUG)\
 $(OBJ_FE_WINREG_WC_DEBUG)\
 $(OBJ_FE_ALL_RELEASE)\
 $(OBJ_FE_ALL_WC_RELEASE)\
 $(OBJ_FE_FAIL_RELEASE)\
 $(OBJ_FE_N_RELEASE)\
 $(OBJ_FE_N_WC_RELEASE)\
 $(OBJ_FE_NULL_RELEASE)\
 $(OBJ_FE_NULL_WC_RELEASE)\
 $(OBJ_FE_SIMPLE_RELEASE)\
 $(OBJ_FE_SIMPLE_WC_RELEASE)\
 $(OBJ_FE_WINREG_RELEASE)\
 $(OBJ_FE_WINREG_WC_RELEASE)\
 \

# back-ends
OBJS_BE=\
 $(OBJ_BE_LRSPLIT_DEBUG)\
 $(OBJ_BE_N_DEBUG)\
 $(OBJ_BEC_TEST_DEBUG)\
 $(OBJ_BE_TEST_DEBUG)\
 $(OBJ_BEL_TEST_DEBUG)\
 $(OBJ_BER_TEST_DEBUG)\
 $(OBJ_BEC_FAIL_DEBUG)\
 $(OBJ_BE_FAIL_DEBUG)\
 $(OBJ_BEL_FAIL_DEBUG)\
 $(OBJ_BER_FAIL_DEBUG)\
 $(OBJ_BEC_FILE_DEBUG)\
 $(OBJ_BEC_FILE_WC_DEBUG)\
 $(OBJ_BE_FILE_DEBUG)\
 $(OBJ_BEL_FILE_DEBUG)\
 $(OBJ_BER_FILE_DEBUG)\
 $(OBJ_BEC_FPRINTF_DEBUG)\
 $(OBJ_BEC_FPRINTF_WC_DEBUG)\
 $(OBJ_BE_FPRINTF_DEBUG)\
 $(OBJ_BEL_FPRINTF_DEBUG)\
 $(OBJ_BER_FPRINTF_DEBUG)\
 $(OBJ_BEC_NULL_DEBUG)\
 $(OBJ_BE_NULL_DEBUG)\
 $(OBJ_BEL_NULL_DEBUG)\
 $(OBJ_BER_NULL_DEBUG)\
 $(OBJ_BEC_COMERROBJ_DEBUG)\
 $(OBJ_BEC_COMERROBJWC_DEBUG)\
 $(OBJ_BE_COMERROBJ_DEBUG)\
 $(OBJ_BEL_COMERROBJ_DEBUG)\
 $(OBJ_BER_COMERROBJ_DEBUG)\
 $(OBJ_BEC_SPEECH_DEBUG)\
 $(OBJ_BEC_SPEECHWC_DEBUG)\
 $(OBJ_BE_SPEECH_DEBUG)\
 $(OBJ_BEL_SPEECH_DEBUG)\
 $(OBJ_BER_SPEECH_DEBUG)\
 $(OBJ_BEC_WINEVLOG_DEBUG)\
 $(OBJ_BE_WINEVLOG_DEBUG)\
 $(OBJ_BEL_WINEVLOG_DEBUG)\
 $(OBJ_BER_WINEVLOG_DEBUG)\
 $(OBJ_BEC_WINMSGBX_DEBUG)\
 $(OBJ_BE_WINMSGBX_DEBUG)\
 $(OBJ_BEL_WINMSGBX_DEBUG)\
 $(OBJ_BER_WINMSGBX_DEBUG)\
 $(OBJ_BEC_W32CONS_DEBUG)\
 $(OBJ_BEC_W32CONSWC_DEBUG)\
 $(OBJ_BE_W32CONS_DEBUG)\
 $(OBJ_BEL_W32CONS_DEBUG)\
 $(OBJ_BER_W32CONS_DEBUG)\
 $(OBJ_BEC_WINDBGR_DEBUG)\
 $(OBJ_BEC_WINDBGR_WC_DEBUG)\
 $(OBJ_BE_WINDBGR_DEBUG)\
 $(OBJ_BEL_WINDBGR_DEBUG)\
 $(OBJ_BER_WINDBGR_DEBUG)\
 $(OBJ_BEC_WINSYSLOG_DEBUG)\
 $(OBJ_BEC_WINSYSLOG_WC_DEBUG)\
 $(OBJ_BE_WINSYSLOG_DEBUG)\
 $(OBJ_BEL_WINSYSLOG_DEBUG)\
 $(OBJ_BER_WINSYSLOG_DEBUG)\
 $(OBJ_BE_LRSPLIT_RELEASE)\
 $(OBJ_BE_N_RELEASE)\
 $(OBJ_BEC_TEST_RELEASE)\
 $(OBJ_BE_TEST_RELEASE)\
 $(OBJ_BEL_TEST_RELEASE)\
 $(OBJ_BER_TEST_RELEASE)\
 $(OBJ_BEC_FAIL_RELEASE)\
 $(OBJ_BE_FAIL_RELEASE)\
 $(OBJ_BEL_FAIL_RELEASE)\
 $(OBJ_BER_FAIL_RELEASE)\
 $(OBJ_BEC_FILE_RELEASE)\
 $(OBJ_BEC_FILE_WC_RELEASE)\
 $(OBJ_BE_FILE_RELEASE)\
 $(OBJ_BEL_FILE_RELEASE)\
 $(OBJ_BER_FILE_RELEASE)\
 $(OBJ_BEC_FPRINTF_RELEASE)\
 $(OBJ_BEC_FPRINTF_WC_RELEASE)\
 $(OBJ_BE_FPRINTF_RELEASE)\
 $(OBJ_BEL_FPRINTF_RELEASE)\
 $(OBJ_BER_FPRINTF_RELEASE)\
 $(OBJ_BEC_NULL_RELEASE)\
 $(OBJ_BE_NULL_RELEASE)\
 $(OBJ_BEL_NULL_RELEASE)\
 $(OBJ_BER_NULL_RELEASE)\
 $(OBJ_BEC_COMERROBJ_RELEASE)\
 $(OBJ_BEC_COMERROBJWC_RELEASE)\
 $(OBJ_BE_COMERROBJ_RELEASE)\
 $(OBJ_BEL_COMERROBJ_RELEASE)\
 $(OBJ_BER_COMERROBJ_RELEASE)\
 $(OBJ_BEC_SPEECH_RELEASE)\
 $(OBJ_BEC_SPEECHWC_RELEASE)\
 $(OBJ_BE_SPEECH_RELEASE)\
 $(OBJ_BEL_SPEECH_RELEASE)\
 $(OBJ_BER_SPEECH_RELEASE)\
 $(OBJ_BEC_WINEVLOG_RELEASE)\
 $(OBJ_BE_WINEVLOG_RELEASE)\
 $(OBJ_BEL_WINEVLOG_RELEASE)\
 $(OBJ_BER_WINEVLOG_RELEASE)\
 $(OBJ_BEC_WINMSGBX_RELEASE)\
 $(OBJ_BE_WINMSGBX_RELEASE)\
 $(OBJ_BEL_WINMSGBX_RELEASE)\
 $(OBJ_BER_WINMSGBX_RELEASE)\
 $(OBJ_BEC_W32CONS_RELEASE)\
 $(OBJ_BEC_W32CONSWC_RELEASE)\
 $(OBJ_BE_W32CONS_RELEASE)\
 $(OBJ_BEL_W32CONS_RELEASE)\
 $(OBJ_BER_W32CONS_RELEASE)\
 $(OBJ_BEC_WINDBGR_RELEASE)\
 $(OBJ_BEC_WINDBGR_WC_RELEASE)\
 $(OBJ_BE_WINDBGR_RELEASE)\
 $(OBJ_BEL_WINDBGR_RELEASE)\
 $(OBJ_BER_WINDBGR_RELEASE)\
 $(OBJ_BEC_WINSYSLOG_RELEASE)\
 $(OBJ_BEC_WINSYSLOG_WC_RELEASE)\
 $(OBJ_BE_WINSYSLOG_RELEASE)\
 $(OBJ_BEL_WINSYSLOG_RELEASE)\
 $(OBJ_BER_WINSYSLOG_RELEASE)\
 \


# b64
OBJS_B64=\
 $(OBJ_B64_DEBUG)\
 $(OBJ_B64_RELEASE)\
 \

# shwild
OBJS_SHWILD=\
 $(OBJ_SHWILD_DEBUG)\
 $(OBJ_SHWILD_RELEASE)\
 \

# xtests
OBJS_XTESTS=\
 $(OBJ_XTESTS_DEBUG)\
 $(OBJ_XTESTS_RELEASE)\
 \

# examples
OBJS_EXAMPLE_C=\
 $(OBJ_EX_C_0_DEBUG)\
 $(OBJ_EX_C_1_DEBUG)\
 $(OBJ_EX_C_2_DEBUG)\
 $(OBJ_EX_C_3_DEBUG)\
 $(OBJ_EX_C_4_DEBUG)\
 $(OBJ_EX_C_5_DEBUG)\
 $(OBJ_EX_C_6_DEBUG)\
 $(OBJ_EX_C_7_DEBUG)\
 $(OBJ_EX_C_0_RELEASE)\
 $(OBJ_EX_C_1_RELEASE)\
 $(OBJ_EX_C_2_RELEASE)\
 $(OBJ_EX_C_3_RELEASE)\
 $(OBJ_EX_C_4_RELEASE)\
 $(OBJ_EX_C_5_RELEASE)\
 $(OBJ_EX_C_6_RELEASE)\
 $(OBJ_EX_C_7_RELEASE)\
 \

OBJS_EXAMPLE_CPP=\
 $(OBJ_EX_P_be_01_DEBUG)\
 $(OBJ_EX_P_be_02_DEBUG)\
 $(OBJ_EX_P_be_03_DEBUG)\
 $(OBJ_EX_P_be_04_DEBUG)\
 $(OBJ_EX_P_be_05_DEBUG)\
 $(OBJ_EX_P_be_06_DEBUG)\
 $(OBJ_EX_P_cp_01_DEBUG)\
 $(OBJ_EX_P_cp_02_DEBUG)\
 $(OBJ_EX_P_fm_01_DEBUG)\
 $(OBJ_EX_P_fm_02_DEBUG)\
 $(OBJ_EX_P_fm_03_DEBUG)\
 $(OBJ_EX_P_fe_01_DEBUG)\
 $(OBJ_EX_P_in_01_DEBUG)\
 $(OBJ_EX_P_in_02_DEBUG)\
 $(OBJ_EX_P_in_03_DEBUG)\
 $(OBJ_EX_P_in_04_DEBUG)\
 $(OBJ_EX_P_in_05_DEBUG)\
 $(OBJ_EX_P_in_06_DEBUG)\
 $(OBJ_EX_P_in_07_DEBUG)\
 $(OBJ_EX_P_in_08_DEBUG)\
 $(OBJ_EX_P_in_09_DEBUG)\
 $(OBJ_EX_P_in_10_DEBUG)\
 $(OBJ_EX_P_in_11_DEBUG)\
 $(OBJ_EX_P_in_12_DEBUG)\
 $(OBJ_EX_P_in_13_DEBUG)\
 $(OBJ_EX_P_in_14_DEBUG)\
 $(OBJ_EX_P_in_15_DEBUG)\
 $(OBJ_EX_P_in_16_DEBUG)\
 $(OBJ_EX_P_in_17_DEBUG)\
 $(OBJ_EX_P_ms_01_DEBUG)\
 $(OBJ_EX_P_ms_02_DEBUG)\
 $(OBJ_EX_P_ms_03_DEBUG)\
 $(OBJ_EX_P_ms_04_DEBUG)\
 $(OBJ_EX_P_ms_05_DEBUG)\
 $(OBJ_EX_P_ms_06_DEBUG)\
 $(OBJ_EX_P_ut_01_DEBUG)\
 $(OBJ_EX_P_be_01_RELEASE)\
 $(OBJ_EX_P_be_02_RELEASE)\
 $(OBJ_EX_P_be_03_RELEASE)\
 $(OBJ_EX_P_be_04_RELEASE)\
 $(OBJ_EX_P_be_05_RELEASE)\
 $(OBJ_EX_P_be_06_RELEASE)\
 $(OBJ_EX_P_cp_01_RELEASE)\
 $(OBJ_EX_P_cp_02_RELEASE)\
 $(OBJ_EX_P_fm_01_RELEASE)\
 $(OBJ_EX_P_fm_02_RELEASE)\
 $(OBJ_EX_P_fm_03_RELEASE)\
 $(OBJ_EX_P_fe_01_RELEASE)\
 $(OBJ_EX_P_in_01_RELEASE)\
 $(OBJ_EX_P_in_02_RELEASE)\
 $(OBJ_EX_P_in_03_RELEASE)\
 $(OBJ_EX_P_in_04_RELEASE)\
 $(OBJ_EX_P_in_05_RELEASE)\
 $(OBJ_EX_P_in_06_RELEASE)\
 $(OBJ_EX_P_in_07_RELEASE)\
 $(OBJ_EX_P_in_08_RELEASE)\
 $(OBJ_EX_P_in_09_RELEASE)\
 $(OBJ_EX_P_in_10_RELEASE)\
 $(OBJ_EX_P_in_11_RELEASE)\
 $(OBJ_EX_P_in_12_RELEASE)\
 $(OBJ_EX_P_in_13_RELEASE)\
 $(OBJ_EX_P_in_14_RELEASE)\
 $(OBJ_EX_P_in_15_RELEASE)\
 $(OBJ_EX_P_in_16_RELEASE)\
 $(OBJ_EX_P_in_17_RELEASE)\
 $(OBJ_EX_P_ms_01_RELEASE)\
 $(OBJ_EX_P_ms_02_RELEASE)\
 $(OBJ_EX_P_ms_03_RELEASE)\
 $(OBJ_EX_P_ms_04_RELEASE)\
 $(OBJ_EX_P_ms_05_RELEASE)\
 $(OBJ_EX_P_ms_06_RELEASE)\
 $(OBJ_EX_P_ut_01_RELEASE)\
 \

OBJS_EXAMPLE=\
 $(OBJS_EXAMPLE_C)\
 $(OBJS_EXAMPLE_CPP)\
 \


# Component Tests
OBJS_TEST_COMPONENT=\
 $(OBJ_TEST_COMP_core_initseq_DEBUG)\
 $(OBJ_TEST_COMP_custsev_DEBUG)\
 $(OBJ_TEST_COMP_ins_args_DEBUG)\
 $(OBJ_TEST_COMP_ins_b64_DEBUG)\
 $(OBJ_TEST_COMP_ins_bool_DEBUG)\
 $(OBJ_TEST_COMP_ins_hostid_DEBUG)\
 $(OBJ_TEST_COMP_ins_int_DEBUG)\
 $(OBJ_TEST_COMP_ins_pad_DEBUG)\
 $(OBJ_TEST_COMP_ins_ptr_DEBUG)\
 $(OBJ_TEST_COMP_ins_prid_DEBUG)\
 $(OBJ_TEST_COMP_ins_real_DEBUG)\
 $(OBJ_TEST_COMP_ins_slice_DEBUG)\
 $(OBJ_TEST_COMP_ins_thrid_DEBUG)\
 $(OBJ_TEST_COMP_ins_vbool_DEBUG)\
 $(OBJ_TEST_COMP_ins_w2m_DEBUG)\
 $(OBJ_TEST_COMP_log_1_DEBUG)\
 $(OBJ_TEST_COMP_pan_puts_DEBUG)\
 $(OBJ_TEST_COMP_pan_prtf_DEBUG)\
 $(OBJ_TEST_COMP_core_initseq_RELEASE)\
 $(OBJ_TEST_COMP_custsev_RELEASE)\
 $(OBJ_TEST_COMP_ins_args_RELEASE)\
 $(OBJ_TEST_COMP_ins_b64_RELEASE)\
 $(OBJ_TEST_COMP_ins_bool_RELEASE)\
 $(OBJ_TEST_COMP_ins_hostid_RELEASE)\
 $(OBJ_TEST_COMP_ins_int_RELEASE)\
 $(OBJ_TEST_COMP_ins_pad_RELEASE)\
 $(OBJ_TEST_COMP_ins_ptr_RELEASE)\
 $(OBJ_TEST_COMP_ins_prid_RELEASE)\
 $(OBJ_TEST_COMP_ins_real_RELEASE)\
 $(OBJ_TEST_COMP_ins_slice_RELEASE)\
 $(OBJ_TEST_COMP_ins_thrid_RELEASE)\
 $(OBJ_TEST_COMP_ins_vbool_RELEASE)\
 $(OBJ_TEST_COMP_ins_w2m_RELEASE)\
 $(OBJ_TEST_COMP_log_1_RELEASE)\
 $(OBJ_TEST_COMP_pan_puts_RELEASE)\
 $(OBJ_TEST_COMP_pan_prtf_RELEASE)\
 \

# Performance Tests
OBJS_TEST_PERFORMANCE=\
 $(OBJ_TEST_PERF_ins_w2m_DEBUG)\
 $(OBJ_TEST_PERF_ins_w2m_RELEASE)\
 \

# Scratch Tests
OBJS_TEST_SCRATCH=\
 $(OBJ_TEST_API_DEBUG)\
 $(OBJ_TEST_FE_DEBUG)\
 $(OBJ_TEST_BE_N_DEBUG)\
 $(OBJ_TEST_FE_SIMPLE_WC_DEBUG)\
 $(OBJ_TEST_API_RELEASE)\
 $(OBJ_TEST_FE_RELEASE)\
 $(OBJ_TEST_BE_N_RELEASE)\
 $(OBJ_TEST_FE_SIMPLE_WC_RELEASE)\
 \

# Unit Tests
OBJS_TEST_UNIT=\
 $(OBJ_TEST_UNIT_lvls_dyninit_DEBUG)\
 $(OBJ_TEST_UNIT_lvls_vals_DEBUG)\
 $(OBJ_TEST_UNIT_getcurrtm_DEBUG)\
 $(OBJ_TEST_UNIT_getversion_DEBUG)\
 $(OBJ_TEST_UNIT_gethostname_DEBUG)\
 $(OBJ_TEST_UNIT_onbailout_DEBUG)\
 $(OBJ_TEST_UNIT_strnlen_DEBUG)\
 \
 $(OBJ_TEST_UNIT_bec_CEO_DEBUG)\
 $(OBJ_TEST_UNIT_bec_file_DEBUG)\
 $(OBJ_TEST_UNIT_bec_fprintf_DEBUG)\
 $(OBJ_TEST_UNIT_bec_fail_DEBUG)\
 $(OBJ_TEST_UNIT_be_fail_DEBUG)\
 $(OBJ_TEST_UNIT_be_lrsplit_DEBUG)\
 $(OBJ_TEST_UNIT_be_N_DEBUG)\
 \
 $(OBJ_TEST_UNIT_fe_all_DEBUG)\
 $(OBJ_TEST_UNIT_fe_fail_DEBUG)\
 $(OBJ_TEST_UNIT_fe_N_DEBUG)\
 $(OBJ_TEST_UNIT_fe_null_DEBUG)\
 $(OBJ_TEST_UNIT_fe_simple_DEBUG)\
 $(OBJ_TEST_UNIT_fe_WinReg_DEBUG)\
 $(OBJ_TEST_UNIT_ins_int_DEBUG)\
 $(OBJ_TEST_UNIT_ins_w2m_DEBUG)\
 $(OBJ_TEST_UNIT_lvls_dyninit_RELEASE)\
 $(OBJ_TEST_UNIT_lvls_vals_RELEASE)\
 $(OBJ_TEST_UNIT_getcurrtm_RELEASE)\
 $(OBJ_TEST_UNIT_getversion_RELEASE)\
 $(OBJ_TEST_UNIT_gethostname_RELEASE)\
 $(OBJ_TEST_UNIT_onbailout_RELEASE)\
 $(OBJ_TEST_UNIT_strnlen_RELEASE)\
 \
 $(OBJ_TEST_UNIT_bec_CEO_RELEASE)\
 $(OBJ_TEST_UNIT_bec_file_RELEASE)\
 $(OBJ_TEST_UNIT_bec_fprintf_RELEASE)\
 $(OBJ_TEST_UNIT_bec_fail_RELEASE)\
 $(OBJ_TEST_UNIT_be_fail_RELEASE)\
 $(OBJ_TEST_UNIT_be_lrsplit_RELEASE)\
 $(OBJ_TEST_UNIT_be_N_RELEASE)\
 \
 $(OBJ_TEST_UNIT_fe_all_RELEASE)\
 $(OBJ_TEST_UNIT_fe_fail_RELEASE)\
 $(OBJ_TEST_UNIT_fe_N_RELEASE)\
 $(OBJ_TEST_UNIT_fe_null_RELEASE)\
 $(OBJ_TEST_UNIT_fe_simple_RELEASE)\
 $(OBJ_TEST_UNIT_fe_WinReg_RELEASE)\
 $(OBJ_TEST_UNIT_ins_int_RELEASE)\
 $(OBJ_TEST_UNIT_ins_w2m_RELEASE)\
 \

########################################
# Libraries

# $(LIB_UTIL [ ` [ N a m e T a g ] ` ] )\

TARGETS_LIB_UTIL=\
 $(LIB_UTIL_DEBUG)\
 $(LIB_UTIL_RELEASE)\
 \

TARGETS_LIB_CORE=\
 $(LIB_CORE_DEBUG)\
 $(LIB_CORE_RELEASE)\
 \

TARGETS_LIB_APPL=\
 $(LIB_APPL_DEBUG)\
 $(LIB_APPL_RELEASE)\
 \

TARGETS_LIB_FE_CMN=\
 $(LIB_FE_ALL_DEBUG)\
 $(LIB_FE_ALL_WC_DEBUG)\
 $(LIB_FE_FAIL_DEBUG)\
 $(LIB_FE_N_DEBUG)\
 $(LIB_FE_N_WC_DEBUG)\
 $(LIB_FE_NULL_DEBUG)\
 $(LIB_FE_NULL_WC_DEBUG)\
 $(LIB_FE_SIMPLE_DEBUG)\
 $(LIB_FE_SIMPLE_WC_DEBUG)\
 $(LIB_FE_ALL_RELEASE)\
 $(LIB_FE_ALL_WC_RELEASE)\
 $(LIB_FE_FAIL_RELEASE)\
 $(LIB_FE_N_RELEASE)\
 $(LIB_FE_N_WC_RELEASE)\
 $(LIB_FE_NULL_RELEASE)\
 $(LIB_FE_NULL_WC_RELEASE)\
 $(LIB_FE_SIMPLE_RELEASE)\
 $(LIB_FE_SIMPLE_WC_RELEASE)\
 \

TARGETS_LIB_FE=\
 $(TARGETS_LIB_FE_CMN)\
 $(LIB_FE_WINREG_DEBUG)\
 $(LIB_FE_WINREG_WC_DEBUG)\
 $(LIB_FE_WINREG_RELEASE)\
 $(LIB_FE_WINREG_WC_RELEASE)\
 \

TARGETS_LIB_BE_CMN=\
 $(LIB_BE_LRSPLIT_DEBUG)\
 $(LIB_BE_N_DEBUG)\
 $(LIB_BEC_FAIL_DEBUG)\
 $(LIB_BE_FAIL_DEBUG)\
 $(LIB_BEL_FAIL_DEBUG)\
 $(LIB_BER_FAIL_DEBUG)\
 $(LIB_BEC_FILE_DEBUG)\
 $(LIB_BEC_FILE_WC_DEBUG)\
 $(LIB_BE_FILE_DEBUG)\
 $(LIB_BEL_FILE_DEBUG)\
 $(LIB_BER_FILE_DEBUG)\
 $(LIB_BEC_FPRINTF_DEBUG)\
 $(LIB_BEC_FPRINTF_WC_DEBUG)\
 $(LIB_BE_FPRINTF_DEBUG)\
 $(LIB_BEL_FPRINTF_DEBUG)\
 $(LIB_BER_FPRINTF_DEBUG)\
 $(LIB_BEC_NULL_DEBUG)\
 $(LIB_BE_NULL_DEBUG)\
 $(LIB_BEL_NULL_DEBUG)\
 $(LIB_BER_NULL_DEBUG)\
 $(LIB_BE_LRSPLIT_RELEASE)\
 $(LIB_BE_N_RELEASE)\
 $(LIB_BEC_FAIL_RELEASE)\
 $(LIB_BE_FAIL_RELEASE)\
 $(LIB_BEL_FAIL_RELEASE)\
 $(LIB_BER_FAIL_RELEASE)\
 $(LIB_BEC_FILE_RELEASE)\
 $(LIB_BEC_FILE_WC_RELEASE)\
 $(LIB_BE_FILE_RELEASE)\
 $(LIB_BEL_FILE_RELEASE)\
 $(LIB_BER_FILE_RELEASE)\
 $(LIB_BEC_FPRINTF_RELEASE)\
 $(LIB_BEC_FPRINTF_WC_RELEASE)\
 $(LIB_BE_FPRINTF_RELEASE)\
 $(LIB_BEL_FPRINTF_RELEASE)\
 $(LIB_BER_FPRINTF_RELEASE)\
 $(LIB_BEC_NULL_RELEASE)\
 $(LIB_BE_NULL_RELEASE)\
 $(LIB_BEL_NULL_RELEASE)\
 $(LIB_BER_NULL_RELEASE)\
 \

TARGETS_LIB_BE=\
 $(LIB_BE_LRSPLIT_DEBUG)\
 $(LIB_BE_N_DEBUG)\
 $(LIB_BEC_TEST_DEBUG)\
 $(LIB_BE_TEST_DEBUG)\
 $(LIB_BEL_TEST_DEBUG)\
 $(LIB_BER_TEST_DEBUG)\
 $(LIB_BEC_FAIL_DEBUG)\
 $(LIB_BE_FAIL_DEBUG)\
 $(LIB_BEL_FAIL_DEBUG)\
 $(LIB_BER_FAIL_DEBUG)\
 $(LIB_BEC_FILE_DEBUG)\
 $(LIB_BEC_FILE_WC_DEBUG)\
 $(LIB_BE_FILE_DEBUG)\
 $(LIB_BEL_FILE_DEBUG)\
 $(LIB_BER_FILE_DEBUG)\
 $(LIB_BEC_FPRINTF_DEBUG)\
 $(LIB_BEC_FPRINTF_WC_DEBUG)\
 $(LIB_BE_FPRINTF_DEBUG)\
 $(LIB_BEL_FPRINTF_DEBUG)\
 $(LIB_BER_FPRINTF_DEBUG)\
 $(LIB_BEC_NULL_DEBUG)\
 $(LIB_BE_NULL_DEBUG)\
 $(LIB_BEL_NULL_DEBUG)\
 $(LIB_BER_NULL_DEBUG)\
 $(LIB_BEC_COMERROBJ_DEBUG)\
 $(LIB_BEC_COMERROBJWC_DEBUG)\
 $(LIB_BE_COMERROBJ_DEBUG)\
 $(LIB_BEL_COMERROBJ_DEBUG)\
 $(LIB_BER_COMERROBJ_DEBUG)\
 $(LIB_BEC_SPEECH_DEBUG)\
 $(LIB_BEC_SPEECHWC_DEBUG)\
 $(LIB_BE_SPEECH_DEBUG)\
 $(LIB_BEL_SPEECH_DEBUG)\
 $(LIB_BER_SPEECH_DEBUG)\
 $(LIB_BEC_WINEVLOG_DEBUG)\
 $(LIB_BE_WINEVLOG_DEBUG)\
 $(LIB_BEL_WINEVLOG_DEBUG)\
 $(LIB_BER_WINEVLOG_DEBUG)\
 $(LIB_BEC_WINMSGBX_DEBUG)\
 $(LIB_BE_WINMSGBX_DEBUG)\
 $(LIB_BEL_WINMSGBX_DEBUG)\
 $(LIB_BER_WINMSGBX_DEBUG)\
 $(LIB_BEC_W32CONS_DEBUG)\
 $(LIB_BEC_W32CONSWC_DEBUG)\
 $(LIB_BE_W32CONS_DEBUG)\
 $(LIB_BEL_W32CONS_DEBUG)\
 $(LIB_BER_W32CONS_DEBUG)\
 $(LIB_BEC_WINDBGR_DEBUG)\
 $(LIB_BEC_WINDBGR_WC_DEBUG)\
 $(LIB_BE_WINDBGR_DEBUG)\
 $(LIB_BEL_WINDBGR_DEBUG)\
 $(LIB_BER_WINDBGR_DEBUG)\
 $(LIB_BEC_WINSYSLOG_DEBUG)\
 $(LIB_BEC_WINSYSLOG_WC_DEBUG)\
 $(LIB_BE_WINSYSLOG_DEBUG)\
 $(LIB_BEL_WINSYSLOG_DEBUG)\
 $(LIB_BER_WINSYSLOG_DEBUG)\
 $(LIB_BE_LRSPLIT_RELEASE)\
 $(LIB_BE_N_RELEASE)\
 $(LIB_BEC_TEST_RELEASE)\
 $(LIB_BE_TEST_RELEASE)\
 $(LIB_BEL_TEST_RELEASE)\
 $(LIB_BER_TEST_RELEASE)\
 $(LIB_BEC_FAIL_RELEASE)\
 $(LIB_BE_FAIL_RELEASE)\
 $(LIB_BEL_FAIL_RELEASE)\
 $(LIB_BER_FAIL_RELEASE)\
 $(LIB_BEC_FILE_RELEASE)\
 $(LIB_BEC_FILE_WC_RELEASE)\
 $(LIB_BE_FILE_RELEASE)\
 $(LIB_BEL_FILE_RELEASE)\
 $(LIB_BER_FILE_RELEASE)\
 $(LIB_BEC_FPRINTF_RELEASE)\
 $(LIB_BEC_FPRINTF_WC_RELEASE)\
 $(LIB_BE_FPRINTF_RELEASE)\
 $(LIB_BEL_FPRINTF_RELEASE)\
 $(LIB_BER_FPRINTF_RELEASE)\
 $(LIB_BEC_NULL_RELEASE)\
 $(LIB_BE_NULL_RELEASE)\
 $(LIB_BEL_NULL_RELEASE)\
 $(LIB_BER_NULL_RELEASE)\
 $(LIB_BEC_COMERROBJ_RELEASE)\
 $(LIB_BEC_COMERROBJWC_RELEASE)\
 $(LIB_BE_COMERROBJ_RELEASE)\
 $(LIB_BEL_COMERROBJ_RELEASE)\
 $(LIB_BER_COMERROBJ_RELEASE)\
 $(LIB_BEC_SPEECH_RELEASE)\
 $(LIB_BEC_SPEECHWC_RELEASE)\
 $(LIB_BE_SPEECH_RELEASE)\
 $(LIB_BEL_SPEECH_RELEASE)\
 $(LIB_BER_SPEECH_RELEASE)\
 $(LIB_BEC_WINEVLOG_RELEASE)\
 $(LIB_BE_WINEVLOG_RELEASE)\
 $(LIB_BEL_WINEVLOG_RELEASE)\
 $(LIB_BER_WINEVLOG_RELEASE)\
 $(LIB_BEC_WINMSGBX_RELEASE)\
 $(LIB_BE_WINMSGBX_RELEASE)\
 $(LIB_BEL_WINMSGBX_RELEASE)\
 $(LIB_BER_WINMSGBX_RELEASE)\
 $(LIB_BEC_W32CONS_RELEASE)\
 $(LIB_BEC_W32CONSWC_RELEASE)\
 $(LIB_BE_W32CONS_RELEASE)\
 $(LIB_BEL_W32CONS_RELEASE)\
 $(LIB_BER_W32CONS_RELEASE)\
 $(LIB_BEC_WINDBGR_RELEASE)\
 $(LIB_BEC_WINDBGR_WC_RELEASE)\
 $(LIB_BE_WINDBGR_RELEASE)\
 $(LIB_BEL_WINDBGR_RELEASE)\
 $(LIB_BER_WINDBGR_RELEASE)\
 $(LIB_BEC_WINSYSLOG_RELEASE)\
 $(LIB_BEC_WINSYSLOG_WC_RELEASE)\
 $(LIB_BE_WINSYSLOG_RELEASE)\
 $(LIB_BEL_WINSYSLOG_RELEASE)\
 $(LIB_BER_WINSYSLOG_RELEASE)\
 \

TARGETS_LIB_ACE=

TARGETS_LIB_ALL=\
 $(TARGETS_LIB_UTIL)\
 $(TARGETS_LIB_CORE)\
 $(TARGETS_LIB_APPL)\
 $(TARGETS_LIB_FE)\
 $(TARGETS_LIB_BE)\
 $(TARGETS_LIB_ACE)\
 \

TARGETS_LIB_B64=\
 $(LIB_B64_DEBUG)\
 $(LIB_B64_RELEASE)\
 \

TARGETS_LIB_SHWILD=\
 $(LIB_SHWILD_DEBUG)\
 $(LIB_SHWILD_RELEASE)\
 \

TARGETS_LIB_XTESTS=\
 $(LIB_XTESTS_DEBUG)\
 $(LIB_XTESTS_RELEASE)\
 \

########################################
# Programs

TARGETS_EXAMPLE_C=\
 $(TARG_EX_C_0_DEBUG)\
 $(TARG_EX_C_1_DEBUG)\
 $(TARG_EX_C_2_DEBUG)\
 $(TARG_EX_C_3_DEBUG)\
 $(TARG_EX_C_4_DEBUG)\
 $(TARG_EX_C_5_DEBUG)\
 $(TARG_EX_C_6_DEBUG)\
 $(TARG_EX_C_7_DEBUG)\
 $(TARG_EX_C_0_RELEASE)\
 $(TARG_EX_C_1_RELEASE)\
 $(TARG_EX_C_2_RELEASE)\
 $(TARG_EX_C_3_RELEASE)\
 $(TARG_EX_C_4_RELEASE)\
 $(TARG_EX_C_5_RELEASE)\
 $(TARG_EX_C_6_RELEASE)\
 $(TARG_EX_C_7_RELEASE)\
 \

TARGETS_EXAMPLE_CPP=\
 $(TARG_EX_P_be_01_DEBUG)\
 $(TARG_EX_P_be_02_DEBUG)\
 $(TARG_EX_P_be_03_DEBUG)\
 $(TARG_EX_P_be_04_DEBUG)\
 $(TARG_EX_P_be_05_DEBUG)\
 $(TARG_EX_P_be_06_DEBUG)\
 $(TARG_EX_P_cp_01_DEBUG)\
 $(TARG_EX_P_cp_02_DEBUG)\
 $(TARG_EX_P_fm_01_DEBUG)\
 $(TARG_EX_P_fm_02_DEBUG)\
 $(TARG_EX_P_fm_03_DEBUG)\
 $(TARG_EX_P_fe_01_DEBUG)\
 $(TARG_EX_P_in_01_DEBUG)\
 $(TARG_EX_P_in_02_DEBUG)\
 $(TARG_EX_P_in_03_DEBUG)\
 $(TARG_EX_P_in_04_DEBUG)\
 $(TARG_EX_P_in_05_DEBUG)\
 $(TARG_EX_P_in_06_DEBUG)\
 $(TARG_EX_P_in_07_DEBUG)\
 $(TARG_EX_P_in_08_DEBUG)\
 $(TARG_EX_P_in_09_DEBUG)\
 $(TARG_EX_P_in_10_DEBUG)\
 $(TARG_EX_P_in_11_DEBUG)\
 $(TARG_EX_P_in_12_DEBUG)\
 $(TARG_EX_P_in_13_DEBUG)\
 $(TARG_EX_P_in_14_DEBUG)\
 $(TARG_EX_P_in_15_DEBUG)\
 $(TARG_EX_P_in_16_DEBUG)\
 $(TARG_EX_P_in_17_DEBUG)\
 $(TARG_EX_P_ms_01_DEBUG)\
 $(TARG_EX_P_ms_02_DEBUG)\
 $(TARG_EX_P_ms_03_DEBUG)\
 $(TARG_EX_P_ms_04_DEBUG)\
 $(TARG_EX_P_ms_05_DEBUG)\
 $(TARG_EX_P_ms_06_DEBUG)\
 $(TARG_EX_P_ut_01_DEBUG)\
 $(TARG_EX_P_be_01_RELEASE)\
 $(TARG_EX_P_be_02_RELEASE)\
 $(TARG_EX_P_be_03_RELEASE)\
 $(TARG_EX_P_be_04_RELEASE)\
 $(TARG_EX_P_be_05_RELEASE)\
 $(TARG_EX_P_be_06_RELEASE)\
 $(TARG_EX_P_cp_01_RELEASE)\
 $(TARG_EX_P_cp_02_RELEASE)\
 $(TARG_EX_P_fm_01_RELEASE)\
 $(TARG_EX_P_fm_02_RELEASE)\
 $(TARG_EX_P_fm_03_RELEASE)\
 $(TARG_EX_P_fe_01_RELEASE)\
 $(TARG_EX_P_in_01_RELEASE)\
 $(TARG_EX_P_in_02_RELEASE)\
 $(TARG_EX_P_in_03_RELEASE)\
 $(TARG_EX_P_in_04_RELEASE)\
 $(TARG_EX_P_in_05_RELEASE)\
 $(TARG_EX_P_in_06_RELEASE)\
 $(TARG_EX_P_in_07_RELEASE)\
 $(TARG_EX_P_in_08_RELEASE)\
 $(TARG_EX_P_in_09_RELEASE)\
 $(TARG_EX_P_in_10_RELEASE)\
 $(TARG_EX_P_in_11_RELEASE)\
 $(TARG_EX_P_in_12_RELEASE)\
 $(TARG_EX_P_in_13_RELEASE)\
 $(TARG_EX_P_in_14_RELEASE)\
 $(TARG_EX_P_in_15_RELEASE)\
 $(TARG_EX_P_in_16_RELEASE)\
 $(TARG_EX_P_in_17_RELEASE)\
 $(TARG_EX_P_ms_01_RELEASE)\
 $(TARG_EX_P_ms_02_RELEASE)\
 $(TARG_EX_P_ms_03_RELEASE)\
 $(TARG_EX_P_ms_04_RELEASE)\
 $(TARG_EX_P_ms_05_RELEASE)\
 $(TARG_EX_P_ms_06_RELEASE)\
 $(TARG_EX_P_ut_01_RELEASE)\
 \

TARGETS_EXAMPLE=\
 $(TARGETS_EXAMPLE_C)\
 $(TARGETS_EXAMPLE_CPP)\
 \


TARGETS_SAMPLE=\
 \


TARGETS_TEST_COMPONENT=\
 $(TARG_TEST_COMP_core_initseq_DEBUG)\
 $(TARG_TEST_COMP_custsev_DEBUG)\
 $(TARG_TEST_COMP_ins_args_DEBUG)\
 $(TARG_TEST_COMP_ins_b64_DEBUG)\
 $(TARG_TEST_COMP_ins_bool_DEBUG)\
 $(TARG_TEST_COMP_ins_hostid_DEBUG)\
 $(TARG_TEST_COMP_ins_int_DEBUG)\
 $(TARG_TEST_COMP_ins_pad_DEBUG)\
 $(TARG_TEST_COMP_ins_ptr_DEBUG)\
 $(TARG_TEST_COMP_ins_prid_DEBUG)\
 $(TARG_TEST_COMP_ins_real_DEBUG)\
 $(TARG_TEST_COMP_ins_slice_DEBUG)\
 $(TARG_TEST_COMP_ins_thrid_DEBUG)\
 $(TARG_TEST_COMP_ins_vbool_DEBUG)\
 $(TARG_TEST_COMP_ins_w2m_DEBUG)\
 $(TARG_TEST_COMP_log_1_DEBUG)\
 $(TARG_TEST_COMP_pan_puts_DEBUG)\
 $(TARG_TEST_COMP_pan_prtf_DEBUG)\
 $(TARG_TEST_COMP_core_initseq_RELEASE)\
 $(TARG_TEST_COMP_custsev_RELEASE)\
 $(TARG_TEST_COMP_ins_args_RELEASE)\
 $(TARG_TEST_COMP_ins_b64_RELEASE)\
 $(TARG_TEST_COMP_ins_bool_RELEASE)\
 $(TARG_TEST_COMP_ins_hostid_RELEASE)\
 $(TARG_TEST_COMP_ins_int_RELEASE)\
 $(TARG_TEST_COMP_ins_pad_RELEASE)\
 $(TARG_TEST_COMP_ins_ptr_RELEASE)\
 $(TARG_TEST_COMP_ins_prid_RELEASE)\
 $(TARG_TEST_COMP_ins_real_RELEASE)\
 $(TARG_TEST_COMP_ins_slice_RELEASE)\
 $(TARG_TEST_COMP_ins_thrid_RELEASE)\
 $(TARG_TEST_COMP_ins_vbool_RELEASE)\
 $(TARG_TEST_COMP_ins_w2m_RELEASE)\
 $(TARG_TEST_COMP_log_1_RELEASE)\
 $(TARG_TEST_COMP_pan_puts_RELEASE)\
 $(TARG_TEST_COMP_pan_prtf_RELEASE)\
 \

TARGETS_TEST_PERFORMANCE=\
 $(TARG_TEST_PERF_ins_w2m_DEBUG)\
 $(TARG_TEST_PERF_ins_w2m_RELEASE)\
 \

TARGETS_TEST_SCRATCH_STD=\
 $(TARG_TEST_DEBUG_APIwBE_FPRINTF)\
 $(TARG_TEST_DEBUG_APIwBE_COMERROBJ)\
 $(TARG_TEST_DEBUG_APIwBE_WINEVLOG)\
 $(TARG_TEST_DEBUG_APIwBE_W32CONS)\
 $(TARG_TEST_DEBUG_APIwBE_WINDBGR)\
 $(TARG_TEST_DEBUG_APIwBE_WINSYSLOG)\
 $(TARG_TEST_DEBUG_APIwBELR_2_WINDBGRFPRINTF)\
 $(TARG_TEST_DEBUG_APIwBELR_3_WINDBGRWINSYSLOG)\
 $(TARG_TEST_FE_DEBUG)\
 $(TARG_TEST_BE_N_FAIL_DEBUG)\
 $(TARG_TEST_FE_SIMPLE_WC_DEBUG)\
 $(TARG_TEST_RELEASE_APIwBE_FPRINTF)\
 $(TARG_TEST_RELEASE_APIwBE_COMERROBJ)\
 $(TARG_TEST_RELEASE_APIwBE_WINEVLOG)\
 $(TARG_TEST_RELEASE_APIwBE_W32CONS)\
 $(TARG_TEST_RELEASE_APIwBE_WINDBGR)\
 $(TARG_TEST_RELEASE_APIwBE_WINSYSLOG)\
 $(TARG_TEST_RELEASE_APIwBELR_2_WINDBGRFPRINTF)\
 $(TARG_TEST_RELEASE_APIwBELR_3_WINDBGRWINSYSLOG)\
 $(TARG_TEST_FE_RELEASE)\
 $(TARG_TEST_BE_N_FAIL_RELEASE)\
 $(TARG_TEST_FE_SIMPLE_WC_RELEASE)\

TARGETS_TEST_SCRATCH=\
 $(TARGETS_TEST_SCRATCH_STD)\
 \
# $(TARG_TEST_SCRT_file_DEBUG)\
# $(TARG_TEST_SCRT_file_RELEASE)\
 \
# $(TARG_TEST_SCRT_file_DEBUG)\
# $(TARG_TEST_SCRT_file_RELEASE)\
 \

TARGETS_TEST_UNIT=\
 $(TARG_TEST_UNIT_lvls_dyninit_DEBUG)\
 $(TARG_TEST_UNIT_lvls_vals_DEBUG)\
 $(TARG_TEST_UNIT_getcurrtm_DEBUG)\
 $(TARG_TEST_UNIT_getversion_DEBUG)\
 $(TARG_TEST_UNIT_gethostname_DEBUG)\
 $(TARG_TEST_UNIT_onbailout_DEBUG)\
 $(TARG_TEST_UNIT_strnlen_DEBUG)\
 \
 $(TARG_TEST_UNIT_bec_CEO_DEBUG)\
 $(TARG_TEST_UNIT_bec_file_DEBUG)\
 $(TARG_TEST_UNIT_bec_fprintf_DEBUG)\
 $(TARG_TEST_UNIT_bec_fail_DEBUG)\
 $(TARG_TEST_UNIT_be_fail_DEBUG)\
 $(TARG_TEST_UNIT_be_lrsplit_DEBUG)\
 $(TARG_TEST_UNIT_be_N_DEBUG)\
 \
 $(TARG_TEST_UNIT_fe_all_DEBUG)\
 $(TARG_TEST_UNIT_fe_fail_DEBUG)\
 $(TARG_TEST_UNIT_fe_N_DEBUG)\
 $(TARG_TEST_UNIT_fe_null_DEBUG)\
 $(TARG_TEST_UNIT_fe_simple_DEBUG)\
 $(TARG_TEST_UNIT_fe_WinReg_DEBUG)\
 $(TARG_TEST_UNIT_ins_int_DEBUG)\
 $(TARG_TEST_UNIT_ins_w2m_DEBUG)\
 $(TARG_TEST_UNIT_lvls_dyninit_RELEASE)\
 $(TARG_TEST_UNIT_lvls_vals_RELEASE)\
 $(TARG_TEST_UNIT_getcurrtm_RELEASE)\
 $(TARG_TEST_UNIT_getversion_RELEASE)\
 $(TARG_TEST_UNIT_gethostname_RELEASE)\
 $(TARG_TEST_UNIT_onbailout_RELEASE)\
 $(TARG_TEST_UNIT_strnlen_RELEASE)\
 \
 $(TARG_TEST_UNIT_bec_CEO_RELEASE)\
 $(TARG_TEST_UNIT_bec_file_RELEASE)\
 $(TARG_TEST_UNIT_bec_fprintf_RELEASE)\
 $(TARG_TEST_UNIT_bec_fail_RELEASE)\
 $(TARG_TEST_UNIT_be_fail_RELEASE)\
 $(TARG_TEST_UNIT_be_lrsplit_RELEASE)\
 $(TARG_TEST_UNIT_be_N_RELEASE)\
 \
 $(TARG_TEST_UNIT_fe_all_RELEASE)\
 $(TARG_TEST_UNIT_fe_fail_RELEASE)\
 $(TARG_TEST_UNIT_fe_N_RELEASE)\
 $(TARG_TEST_UNIT_fe_null_RELEASE)\
 $(TARG_TEST_UNIT_fe_simple_RELEASE)\
 $(TARG_TEST_UNIT_fe_WinReg_RELEASE)\
 $(TARG_TEST_UNIT_ins_int_RELEASE)\
 $(TARG_TEST_UNIT_ins_w2m_RELEASE)\
 \

############################################################
# Third-party libraries

LIBS_3PTY_COMERROBJ				=	advapi32.lib ole32.lib oleaut32.lib user32.lib
LIBS_3PTY_FPRINTF				=	advapi32.lib user32.lib
LIBS_3PTY_SYSLOG				=	advapi32.lib user32.lib
LIBS_3PTY_SPEECH				=	advapi32.lib ole32.lib oleaut32.lib user32.lib
LIBS_3PTY_WINEVLOG				=	advapi32.lib user32.lib
LIBS_3PTY_WINMSGBX				=	advapi32.lib user32.lib
LIBS_3PTY_W32CONS				=	advapi32.lib user32.lib
LIBS_3PTY_WINDBGR				=	advapi32.lib user32.lib
LIBS_3PTY_WINSYSLOG				=	advapi32.lib user32.lib ws2_32.lib
LIBS_3PTY_WINDBGRFPRINTF		=	advapi32.lib user32.lib
LIBS_3PTY_WINDBGRWINSYSLOG		=	advapi32.lib user32.lib ws2_32.lib
LIBS_3PTY_BE_N_FAIL				=	advapi32.lib user32.lib
LIBS_3PTY_TEST_UNIT				=	advapi32.lib user32.lib

############################################################################
# External Targets (invokable by make)

# primary

_default:                   build

usage:
	@echo "make [usage | compile | build | test | clean | ace | prep] - default is 'build'"
	@echo "details:"
	@echo "    default(=build)"
	@echo "    compile.libs.util"
	@echo "    compile.libs.core"
	@echo "    compile.ace"
	@echo "    compile.examples"
	@echo "    compile.test"
	@echo "    compile(=compile.libs.util+compile.libs.core+compile.example+compile.test)"
	@echo "    compile.all(=compile.libs.util+compile.libs.core+compile.example+compile.test+compile.ace)"
	@echo "    build.libs.util"
	@echo "    build.libs.core"
	@echo "    build.libs.fe"
	@echo "    build.libs.be"
	@echo "    build.libs.ace"
	@echo "    build.libs(=build.libs.util+build.libs.core+build.libs.fe+build.libs.be)"
	@echo "    build.libs.all(=build.libs.util+build.libs.core+build.libs.fe+build.libs.be+build.libs.ace)"
	@echo "    build.examples"
	@echo "    build.test.std"
	@echo "    build.test.ace"
	@echo "    build.test(=build.test.std)"
	@echo "    build.test.all(=build.test.std+build.test.ace)"
	@echo "    build.util(=build.libs.util)"
	@echo "    build.core(=build.libs.core)"
	@echo "    build.std(=build.libs.util+build.libs.core+build.libs.fe+build.libs.be+build.test.std)"
	@echo "    build.ace(=build.libs.ace+build.test.ace)"
	@echo "    build(=build.libs+build.examples+build.samples+build.test)"
	@echo "    build.all(=build.libs.all+build.examples+build.samples+build.test.all)"
	@echo "    test.std"
	@echo "    test.ace"
	@echo "    test(=test.std)"
	@echo "    test.all(=test.std+test.ace)"
	@echo "    std=build.std+test.std"
	@echo "    ace=build.ace+test.ace"
	@echo "    all=core+std+ace"
	@echo "    prep: removes \r from all .h and .c files, on UNIX-only"


# infrastructure



prep.files.for.os:	$(PROJ_MAKEFILE_NAME)\
	\

prep:	prep.files.for.os


check.lib.dir:		$(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)

check.bin.dir:		$(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_BIN_DIR)\NUL mkdir $(PROJ_BIN_DIR)


# compile targets

compile.libs.util:          $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_UTIL)

compile.libs.core:          $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_CORE)

compile.libs.fe:            $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_FE)

compile.libs.be:            $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_BE)

compile.libs:                                                           compile.libs.util compile.libs.core compile.libs.fe compile.libs.be

compile.libs.b64:           $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_B64)

compile.libs.shwild:        $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_SHWILD)

compile.libs.xtests:        $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_XTESTS)

compile.libs.3pty:                                                      compile.libs.b64 compile.libs.shwild compile.libs.xtests

compile.libs.all:                                                       compile.libs compile.libs.3pty

compile.ex.c:               $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_EXAMPLE_C)

compile.ex.cpp:             $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_EXAMPLE_CPP)

compile.examples:           $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_EXAMPLE)

compile.samples:            $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_SAMPLE)

compile.test.component:     $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_TEST_COMPONENT)

compile.test.performance:   $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_TEST_PERFORMANCE)

compile.test.scratch:       $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_TEST_SCRATCH)

compile.test.unit:          $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_TEST_UNIT)

compile.test:                                                           compile.test.unit compile.test.component compile.test.performance compile.test.scratch

compile:                                                                compile.libs compile.libs.3pty compile.examples compile.test

compile.all:                                                            compile.libs compile.examples compile.test


# build targets

build.libs.util:            check.lib.dir           prep.files.for.os   $(TARGETS_LIB_UTIL)

build.libs.core:            check.lib.dir           prep.files.for.os   $(TARGETS_LIB_CORE)

build.libs.appl:            check.lib.dir           prep.files.for.os   $(TARGETS_LIB_APPL)

build.libs.fe:              check.lib.dir           prep.files.for.os   $(TARGETS_LIB_FE)

build.libs.be:              check.lib.dir           prep.files.for.os   $(TARGETS_LIB_BE)

build.libs.fe.cmn:          check.lib.dir           prep.files.for.os   $(TARGETS_LIB_FE_CMN)

build.libs.be.cmn:          check.lib.dir           prep.files.for.os   $(TARGETS_LIB_BE_CMN)

build.libs.b64:             check.lib.dir           prep.files.for.os   $(TARGETS_LIB_B64)

build.libs.shwild:          check.lib.dir           prep.files.for.os   $(TARGETS_LIB_SHWILD)

build.libs.xtests:          check.lib.dir           prep.files.for.os   $(TARGETS_LIB_XTESTS)

build.libs.3pty:            check.lib.dir                               build.libs.b64 build.libs.shwild build.libs.xtests

# obsolete
build.libs.std:                                                         check.lib.dir build.libs.fe build.libs.be

build.libs:                                                             build.libs.util build.libs.core build.libs.appl build.libs.fe build.libs.be

build.libs.all:                                                         build.libs

# These names - i.e. build.ex.?? - has to be short (rather than build.examples.??), otherwise GCC's
# make gets confused.
#
# Can't use C++ (or C++) because DMC++'s make can't handle it
build.ex.c:                 check.bin.dir           prep.files.for.os   $(TARGETS_EXAMPLE_C)

build.ex.cpp:               check.bin.dir           prep.files.for.os   $(TARGETS_EXAMPLE_CPP)

build.examples:             check.bin.dir           prep.files.for.os   $(TARGETS_EXAMPLE)

build.samples:              check.bin.dir           prep.files.for.os   $(TARGETS_SAMPLE)

build.test.component:       check.bin.dir           prep.files.for.os   $(TARGETS_TEST_COMPONENT)

build.test.performance:     check.bin.dir           prep.files.for.os   $(TARGETS_TEST_PERFORMANCE)

build.test.scratch:         check.bin.dir           prep.files.for.os   $(TARGETS_TEST_SCRATCH)

build.test.unit:            check.bin.dir           prep.files.for.os   $(TARGETS_TEST_UNIT)

build.test:                                                             build.test.unit build.test.component build.test.performance build.test.scratch

build.test.std:             check.bin.dir           prep.files.for.os   $(TARGETS_TEST_SCRATCH_STD)

build.test.std:             check.bin.dir           prep.files.for.os   $(TARGETS_TEST_SCRATCH_STD)

build.std:                                                              build.libs.util build.libs.core build.libs.appl build.libs.fe build.libs.be build.test.std

build.test.all:                                                         build.test.std


build:                                                                  build.libs build.examples build.samples build.test

build.cmn:                                                              build.libs.util build.libs.core build.libs.appl build.libs.fe.cmn build.libs.be.cmn

build.common:                                                           build.cmn

build.all:                                                              build.libs.all build.examples build.samples build.test.all


# test targets

test.component_:
	@echo Running Test:Component programs ...
	$(TARG_TEST_COMP_core_initseq_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_core_initseq_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_custsev_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_custsev_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_args_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_args_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_b64_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_b64_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_bool_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_bool_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_hostid_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_hostid_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_int_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_int_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_pad_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_pad_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_ptr_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_ptr_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_prid_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_prid_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_real_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_real_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_thrid_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_thrid_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_vbool_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_vbool_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_w2m_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_w2m_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_log_1_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_log_1_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_puts_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_puts_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_prtf_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_prtf_RELEASE) --verbosity=$(TEST_VERBOSITY)

test.performance_:
	@echo Running Test:Performance programs ...
	$(TARG_TEST_PERF_ins_w2m_DEBUG) on
	$(TARG_TEST_PERF_ins_w2m_DEBUG) off
	$(TARG_TEST_PERF_ins_w2m_RELEASE) on
	$(TARG_TEST_PERF_ins_w2m_RELEASE) off

test.scratch_:
	@echo Running Test:Scratch programs ...
	$(TARG_TEST_DEBUG_APIwBE_FPRINTF)
	$(TARG_TEST_DEBUG_APIwBE_COMERROBJ)
	$(TARG_TEST_DEBUG_APIwBE_WINEVLOG)
	$(TARG_TEST_DEBUG_APIwBE_W32CONS)
	$(TARG_TEST_DEBUG_APIwBE_WINDBGR)
	$(TARG_TEST_DEBUG_APIwBE_WINSYSLOG)
	$(TARG_TEST_DEBUG_APIwBELR_2_WINDBGRFPRINTF)
	$(TARG_TEST_DEBUG_APIwBELR_3_WINDBGRWINSYSLOG)
	$(TARG_TEST_FE_DEBUG)
	$(TARG_TEST_DEBUG_APIwBE_FPRINTF)
	$(TARG_TEST_BE_N_FAIL_DEBUG)
	$(TARG_TEST_FE_SIMPLE_WC_DEBUG)
	$(TARG_TEST_RELEASE_APIwBE_FPRINTF)
	$(TARG_TEST_RELEASE_APIwBE_COMERROBJ)
	$(TARG_TEST_RELEASE_APIwBE_WINEVLOG)
	$(TARG_TEST_RELEASE_APIwBE_W32CONS)
	$(TARG_TEST_RELEASE_APIwBE_WINDBGR)
	$(TARG_TEST_RELEASE_APIwBE_WINSYSLOG)
	$(TARG_TEST_RELEASE_APIwBELR_2_WINDBGRFPRINTF)
	$(TARG_TEST_RELEASE_APIwBELR_3_WINDBGRWINSYSLOG)
	$(TARG_TEST_FE_RELEASE)
	$(TARG_TEST_RELEASE_APIwBE_FPRINTF)
	$(TARG_TEST_BE_N_FAIL_RELEASE)
	$(TARG_TEST_FE_SIMPLE_WC_RELEASE)

test.unit_:
	@echo Running Test:Unit programs ...
	$(TARG_TEST_UNIT_lvls_dyninit_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_dyninit_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_vals_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_vals_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getcurrtm_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getcurrtm_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getversion_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getversion_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_gethostname_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_gethostname_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_onbailout_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_onbailout_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_strnlen_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_strnlen_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_CEO_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_CEO_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_file_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_file_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fprintf_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fprintf_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fail_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fail_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_fail_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_fail_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_lrsplit_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_lrsplit_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_N_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_N_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_all_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_all_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_fail_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_fail_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_N_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_N_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_null_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_null_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_simple_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_simple_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_WinReg_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_WinReg_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_int_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_int_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_w2m_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_w2m_RELEASE) --verbosity=$(TEST_VERBOSITY)

test.examples_:
	@echo Running Example programs ...
	$(TARG_EX_C_0_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_1_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_2_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_3_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_4_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_5_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_7_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_0_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_1_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_2_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_3_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_4_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_5_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_7_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_01_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_02_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_03_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_04_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_05_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_06_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_01_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_02_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_01_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_02_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_03_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fe_01_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_01_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_02_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_03_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_04_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_05_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_06_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_07_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_08_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_09_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_10_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_11_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_13_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_14_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_15_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_16_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_17_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_01_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_02_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_03_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_04_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_05_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_06_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ut_01_DEBUG) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_01_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_02_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_03_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_04_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_05_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_06_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_01_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_02_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_01_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_02_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_03_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fe_01_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_01_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_02_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_03_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_04_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_05_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_06_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_07_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_08_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_09_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_10_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_11_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_13_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_14_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_15_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_16_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_17_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_01_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_02_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_03_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_04_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_05_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_06_RELEASE) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ut_01_RELEASE) --verbosity=$(TEST_VERBOSITY)

test.component:		build.test.component test.component_

test.performance:	build.test.performance test.performance_

test.scratch:		build.test.scratch test.scratch_

test.unit:			build.test.unit test.unit_

test.examples:		build.examples test.examples_

test:                       test.unit test.component

test.all:                   test.scratch test.examples test.performance test


# clean targets

clean.libs.core:
	@echo Cleaning Core libraries ...
	@-if exist util.*.obj $(RM) util.*.obj
	@-if exist core.*.obj $(RM) core.*.obj
	@-if exist inserters.*.obj $(RM) inserters.*.obj
	@-if exist $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).core.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).core.$(COMP_TAG)*.lib
	@-if exist $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).util.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).util.$(COMP_TAG)*.lib

clean.libs.be:
	@echo Cleaning Back-end libraries ...
	@-if exist bec.*.obj $(RM) bec.*.obj
	@-if exist bel.*.obj $(RM) bel.*.obj
	@-if exist ber.*.obj $(RM) ber.*.obj
	@-if exist be.*.obj $(RM) be.*.obj
	@-if exist $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.*.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bec.*.$(COMP_TAG)*.lib
	@-if exist $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.*.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).bel.*.$(COMP_TAG)*.lib
	@-if exist $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.*.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).ber.*.$(COMP_TAG)*.lib
	@-if exist $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.*.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).be.*.$(COMP_TAG)*.lib


clean.libs.fe:
	@echo Cleaning Front-end libraries ...
	@-if exist fe.*.obj $(RM) fe.*.obj
	@-if exist $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.*.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(PROJ_TAG_VER).fe.*.$(COMP_TAG)*.lib

clean.libs.b64:
	@echo Cleaning b64 libraries ...
	@-if exist b64.*.obj $(RM) b64.*.obj
	@-if exist $(PROJ_LIB_DIR)\$(B64_TAG_VER).pan.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(B64_TAG_VER).pan.$(COMP_TAG)*.lib

clean.libs.shwild:
	@echo Cleaning shwild libraries ...
	@-if exist shwild.*.obj $(RM) shwild.*.obj
	@-if exist $(PROJ_LIB_DIR)\$(SHWILD_TAG_VER).pan.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(SHWILD_TAG_VER).pan.$(COMP_TAG)*.lib

clean.libs.xtests:
	@echo Cleaning xTests libraries ...
	@-if exist xtests.*.obj $(RM) xtests.*.obj
	@-if exist $(PROJ_LIB_DIR)\$(XTESTS_TAG_VER).pan.$(COMP_TAG)*.lib $(RM) $(PROJ_LIB_DIR)\$(XTESTS_TAG_VER).pan.$(COMP_TAG)*.lib


clean.examples:
	@echo Cleaning examples ...
	@-if exist ex.*.obj $(RM) ex.*.obj
	@-if exist $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.*.$(COMP_TAG)*.exe $(RM) $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).ex.*.$(COMP_TAG)*.exe

clean.samples:
	@echo Cleaning samples ...

clean.samples.all:	clean.samples

#	@-$(RM) $(TARGETS_SAMPLE)

clean.test.unit:
	@echo Cleaning unit tests ...
	@-if exist test.unit.*.obj $(RM) test.unit.*.obj
	@-if exist $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.*.$(COMP_TAG)*.exe $(RM) $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.unit.*.$(COMP_TAG)*.exe

clean.test.component:
	@echo Cleaning component tests ...
	@-if exist test.component.*.obj $(RM) test.component.*.obj
	@-if exist $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.*.$(COMP_TAG)*.exe $(RM) $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.component.*.$(COMP_TAG)*.exe

clean.test.performance:
	@echo Cleaning performance tests ...
	@-if exist test.performance.*.obj $(RM) test.performance.*.obj
	@-if exist $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.performance.*.$(COMP_TAG)*.exe $(RM) $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.performance.*.$(COMP_TAG)*.exe

clean.test.scratch:
	@echo Cleaning scratch tests ...
	@-if exist test.*.obj $(RM) test.*.obj
	@-if exist $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.*.$(COMP_TAG)*.exe $(RM) $(PROJ_BIN_DIR)\$(PROJ_TAG_VER).test.*.$(COMP_TAG)*.exe

clean.unittest.std:
	@echo Cleaning standard unittests ...


clean.libs: 		clean.libs.core clean.libs.be clean.libs.fe

clean.libs.3pty:	clean.libs.b64 clean.libs.shwild clean.libs.xtests

clean.libs.all: 	clean.libs clean.libs.3pty

clean.test:			clean.test.unit clean.test.component clean.test.scratch clean.test.performance

clean.test.all:		clean.test clean.libs.b64 clean.libs.shwild clean.libs.xtests

clean.unittest:		clean.unittest.std

clean.unittest.all:	clean.unittest.std

clean.std:			clean.libs clean.test

clean.logs:
	@echo Cleaning any log files created in testing ...
	@-if exist *.log $(RM) *.log

clean:				clean.libs clean.libs.3pty clean.examples clean.samples clean.test clean.unittest clean.logs

clean.all:			clean.libs.all clean.examples clean.samples.all clean.test.all clean.unittest.all

# obsolete targets
target:				build.core

test-all:			test.all

############################################################################
# build targets

##################################################
# Libraries

########################################
# Main library - 'pantheios.X.util.a'

$(LIB_UTIL_DEBUG): $(OBJ_UTIL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_UTIL_DEBUG)

$(OBJ_utl_bailout_DEBUG): $(PROJ_SRC_DIR)\util\bailout.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\bailout.c
$(OBJ_utl_init_code_strs_DEBUG): $(PROJ_SRC_DIR)\util\init_code_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\init_code_strings.c
$(OBJ_utl_sev_strs_DEBUG): $(PROJ_SRC_DIR)\util\severity_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\severity_strings.c
$(OBJ_utl_time_DEBUG): $(PROJ_SRC_DIR)\util\time.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\time.cpp
$(OBJ_utl_host_name_DEBUG): $(PROJ_SRC_DIR)\util\hostname.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\hostname.c
$(OBJ_utl_snprintf_DEBUG): $(PROJ_SRC_DIR)\util\snprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\snprintf.c
$(OBJ_utl_strdup_DEBUG): $(PROJ_SRC_DIR)\util\strdup.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\strdup.c
$(OBJ_utl_procid_DEBUG): $(PROJ_SRC_DIR)\util\processid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\processid.c
$(OBJ_utl_thread_DEBUG): $(PROJ_SRC_DIR)\util\threadid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\threadid.c
$(OBJ_utl_be_context_DEBUG): $(PROJ_SRC_DIR)\util\be.context.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\be.context.cpp
$(OBJ_utl_core_apidefs_DEBUG): $(PROJ_SRC_DIR)\util\core.apidefs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\core.apidefs.cpp
$(OBJ_utl_be_parse_DEBUG): $(PROJ_SRC_DIR)\util\be.parse.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\be.parse.cpp
$(OBJ_utl_strnlenp_DEBUG): $(PROJ_SRC_DIR)\util\strnlen.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\strnlen.cpp
$(OBJ_utl_strnlen_DEBUG): $(PROJ_SRC_DIR)\util\util.strnlen.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\util.strnlen.c

$(LIB_UTIL_RELEASE): $(OBJ_UTIL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_UTIL_RELEASE)

$(OBJ_utl_bailout_RELEASE): $(PROJ_SRC_DIR)\util\bailout.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\bailout.c
$(OBJ_utl_init_code_strs_RELEASE): $(PROJ_SRC_DIR)\util\init_code_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\init_code_strings.c
$(OBJ_utl_sev_strs_RELEASE): $(PROJ_SRC_DIR)\util\severity_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\severity_strings.c
$(OBJ_utl_time_RELEASE): $(PROJ_SRC_DIR)\util\time.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\time.cpp
$(OBJ_utl_host_name_RELEASE): $(PROJ_SRC_DIR)\util\hostname.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\hostname.c
$(OBJ_utl_snprintf_RELEASE): $(PROJ_SRC_DIR)\util\snprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\snprintf.c
$(OBJ_utl_strdup_RELEASE): $(PROJ_SRC_DIR)\util\strdup.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\strdup.c
$(OBJ_utl_procid_RELEASE): $(PROJ_SRC_DIR)\util\processid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\processid.c
$(OBJ_utl_thread_RELEASE): $(PROJ_SRC_DIR)\util\threadid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\threadid.c
$(OBJ_utl_be_context_RELEASE): $(PROJ_SRC_DIR)\util\be.context.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\be.context.cpp
$(OBJ_utl_core_apidefs_RELEASE): $(PROJ_SRC_DIR)\util\core.apidefs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\core.apidefs.cpp
$(OBJ_utl_be_parse_RELEASE): $(PROJ_SRC_DIR)\util\be.parse.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\be.parse.cpp
$(OBJ_utl_strnlenp_RELEASE): $(PROJ_SRC_DIR)\util\strnlen.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\strnlen.cpp
$(OBJ_utl_strnlen_RELEASE): $(PROJ_SRC_DIR)\util\util.strnlen.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\util\util.strnlen.c


########################################
# Main library - 'pantheios.X.core.a'

$(LIB_CORE_DEBUG): $(OBJ_CORE_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_CORE_DEBUG)

$(OBJ_cr_api_DEBUG): $(PROJ_SRC_DIR)\core\api.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.cpp
$(OBJ_cr_api_dep_DEBUG): $(PROJ_SRC_DIR)\core\api.deprecated.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.deprecated.c
$(OBJ_cr_api_exit_DEBUG): $(PROJ_SRC_DIR)\core\api.exitprocess.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.exitprocess.cpp
$(OBJ_cr_api_logprintf_DEBUG): $(PROJ_SRC_DIR)\core\logprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\logprintf.c
$(OBJ_cr_api_logputs_DEBUG): $(PROJ_SRC_DIR)\core\api.logputs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.logputs.cpp
$(OBJ_cr_auto_DEBUG): $(PROJ_SRC_DIR)\core\auto.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\auto.cpp

$(OBJ_ins_args_DEBUG): $(PROJ_SRC_DIR)\inserters\args.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\args.cpp
$(OBJ_ins_b64_DEBUG): $(PROJ_SRC_DIR)\inserters\b64.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\b64.cpp
$(OBJ_ins_blob_DEBUG): $(PROJ_SRC_DIR)\inserters\blob.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\blob.cpp
$(OBJ_ins_bool_DEBUG): $(PROJ_SRC_DIR)\inserters\boolean.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\boolean.cpp
$(OBJ_ins_excp_DEBUG): $(PROJ_SRC_DIR)\inserters\exception.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\exception.cpp
$(OBJ_ins_host_DEBUG): $(PROJ_SRC_DIR)\inserters\hostid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\hostid.cpp
$(OBJ_ins_int_DEBUG): $(PROJ_SRC_DIR)\inserters\integer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\integer.cpp
$(OBJ_ins_ptr_DEBUG): $(PROJ_SRC_DIR)\inserters\pointer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\pointer.cpp
$(OBJ_ins_prid_DEBUG): $(PROJ_SRC_DIR)\inserters\processid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\processid.cpp
$(OBJ_ins_real_DEBUG): $(PROJ_SRC_DIR)\inserters\real.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\real.cpp
$(OBJ_ins_slice_DEBUG): $(PROJ_SRC_DIR)\inserters\slice.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\slice.cpp
$(OBJ_ins_thrid_DEBUG): $(PROJ_SRC_DIR)\inserters\threadid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\threadid.cpp
$(OBJ_ins_w2m_DEBUG): $(PROJ_SRC_DIR)\inserters\w2m.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\w2m.cpp

$(LIB_CORE_RELEASE): $(OBJ_CORE_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_CORE_RELEASE)

$(OBJ_cr_api_RELEASE): $(PROJ_SRC_DIR)\core\api.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.cpp
$(OBJ_cr_api_dep_RELEASE): $(PROJ_SRC_DIR)\core\api.deprecated.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.deprecated.c
$(OBJ_cr_api_exit_RELEASE): $(PROJ_SRC_DIR)\core\api.exitprocess.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.exitprocess.cpp
$(OBJ_cr_api_logprintf_RELEASE): $(PROJ_SRC_DIR)\core\logprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\logprintf.c
$(OBJ_cr_api_logputs_RELEASE): $(PROJ_SRC_DIR)\core\api.logputs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\api.logputs.cpp
$(OBJ_cr_auto_RELEASE): $(PROJ_SRC_DIR)\core\auto.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\core\auto.cpp

$(OBJ_ins_args_RELEASE): $(PROJ_SRC_DIR)\inserters\args.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\args.cpp
$(OBJ_ins_b64_RELEASE): $(PROJ_SRC_DIR)\inserters\b64.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\b64.cpp
$(OBJ_ins_blob_RELEASE): $(PROJ_SRC_DIR)\inserters\blob.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\blob.cpp
$(OBJ_ins_bool_RELEASE): $(PROJ_SRC_DIR)\inserters\boolean.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\boolean.cpp
$(OBJ_ins_excp_RELEASE): $(PROJ_SRC_DIR)\inserters\exception.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\exception.cpp
$(OBJ_ins_host_RELEASE): $(PROJ_SRC_DIR)\inserters\hostid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\hostid.cpp
$(OBJ_ins_int_RELEASE): $(PROJ_SRC_DIR)\inserters\integer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\integer.cpp
$(OBJ_ins_ptr_RELEASE): $(PROJ_SRC_DIR)\inserters\pointer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\pointer.cpp
$(OBJ_ins_prid_RELEASE): $(PROJ_SRC_DIR)\inserters\processid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\processid.cpp
$(OBJ_ins_real_RELEASE): $(PROJ_SRC_DIR)\inserters\real.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\real.cpp
$(OBJ_ins_slice_RELEASE): $(PROJ_SRC_DIR)\inserters\slice.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\slice.cpp
$(OBJ_ins_thrid_RELEASE): $(PROJ_SRC_DIR)\inserters\threadid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\threadid.cpp
$(OBJ_ins_w2m_RELEASE): $(PROJ_SRC_DIR)\inserters\w2m.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\inserters\w2m.cpp

########################################
# Main library - 'pantheios.X.appl.a'

$(LIB_APPL_DEBUG): $(OBJ_APPL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_APPL_DEBUG)

$(LIB_APPL_RELEASE): $(OBJ_APPL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_APPL_RELEASE)

########################################
# Backend util components

########################################
# Backend common stock library - 'pantheios.bec.stock.a'

########################################
# Backend library - 'pantheios.be.lrsplit.a'

$(LIB_BE_LRSPLIT_DEBUG): $(OBJ_BE_LRSPLIT_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_LRSPLIT_DEBUG)

$(OBJ_be_lrsplit_DEBUG): $(PROJ_SRC_DIR)\backends\be.lrsplit.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be.lrsplit.c

$(LIB_BE_LRSPLIT_RELEASE): $(OBJ_BE_LRSPLIT_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_LRSPLIT_RELEASE)

$(OBJ_be_lrsplit_RELEASE): $(PROJ_SRC_DIR)\backends\be.lrsplit.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be.lrsplit.c

########################################
# Backend library - 'pantheios.be.N.a'

$(LIB_BE_N_DEBUG): $(OBJ_BE_N_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_N_DEBUG)

$(OBJ_be_N_DEBUG): $(PROJ_SRC_DIR)\backends\be.N.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be.N.c

$(LIB_BE_N_RELEASE): $(OBJ_BE_N_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_N_RELEASE)

$(OBJ_be_N_RELEASE): $(PROJ_SRC_DIR)\backends\be.N.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be.N.c

########################################
# Back-end library - 'pantheios.bec.test.a'

$(LIB_BEC_TEST_DEBUG): $(OBJ_BEC_TEST_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_TEST_DEBUG)

$(OBJ_bec_test_DEBUG): $(PROJ_SRC_DIR)\backends\bec.test.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.test.cpp

$(LIB_BEC_TEST_RELEASE): $(OBJ_BEC_TEST_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_TEST_RELEASE)

$(OBJ_bec_test_RELEASE): $(PROJ_SRC_DIR)\backends\bec.test.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.test.cpp

########################################
# Back-end library - 'pantheios.be.test.a'

$(LIB_BE_TEST_DEBUG): $(OBJ_BE_TEST_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_TEST_DEBUG)

$(OBJ_be_test_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.test.c

$(LIB_BE_TEST_RELEASE): $(OBJ_BE_TEST_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_TEST_RELEASE)

$(OBJ_be_test_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.test.c

########################################
# Back-end library - 'pantheios.bel.test.a'

$(LIB_BEL_TEST_DEBUG): $(OBJ_BEL_TEST_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_TEST_DEBUG)

$(OBJ_bel_test_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.test.c

$(LIB_BEL_TEST_RELEASE): $(OBJ_BEL_TEST_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_TEST_RELEASE)

$(OBJ_bel_test_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.test.c

########################################
# Back-end library - 'pantheios.ber.test.a'

$(LIB_BER_TEST_DEBUG): $(OBJ_BER_TEST_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_TEST_DEBUG)

$(OBJ_ber_test_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.test.c

$(LIB_BER_TEST_RELEASE): $(OBJ_BER_TEST_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_TEST_RELEASE)

$(OBJ_ber_test_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.test.c

########################################
# Backend library - 'pantheios.bec.fail.a'

$(LIB_BEC_FAIL_DEBUG): $(OBJ_BEC_FAIL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FAIL_DEBUG)

$(OBJ_bec_fail_DEBUG): $(PROJ_SRC_DIR)\backends\bec.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.fail.c

$(LIB_BEC_FAIL_RELEASE): $(OBJ_BEC_FAIL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FAIL_RELEASE)

$(OBJ_bec_fail_RELEASE): $(PROJ_SRC_DIR)\backends\bec.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.fail.c


########################################
# Backend library - 'pantheios.be.fail.a'

$(LIB_BE_FAIL_DEBUG): $(OBJ_BE_FAIL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_FAIL_DEBUG)

$(OBJ_be_fail_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.fail.c

$(LIB_BE_FAIL_RELEASE): $(OBJ_BE_FAIL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_FAIL_RELEASE)

$(OBJ_be_fail_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.fail.c


########################################
# Backend library - 'pantheios.bel.fail.a'

$(LIB_BEL_FAIL_DEBUG): $(OBJ_BEL_FAIL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_FAIL_DEBUG)

$(OBJ_bel_fail_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.fail.c

$(LIB_BEL_FAIL_RELEASE): $(OBJ_BEL_FAIL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_FAIL_RELEASE)

$(OBJ_bel_fail_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.fail.c


########################################
# Backend library - 'pantheios.ber.fail.a'

$(LIB_BER_FAIL_DEBUG): $(OBJ_BER_FAIL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_FAIL_DEBUG)

$(OBJ_ber_fail_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.fail.c

$(LIB_BER_FAIL_RELEASE): $(OBJ_BER_FAIL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_FAIL_RELEASE)

$(OBJ_ber_fail_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.fail.c


########################################
# Backend library - 'pantheios.bec.file.a'

$(LIB_BEC_FILE_DEBUG): $(OBJ_BEC_FILE_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FILE_DEBUG)

$(OBJ_bec_file_DEBUG): $(PROJ_SRC_DIR)\backends\bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.file.cpp

$(LIB_BEC_FILE_RELEASE): $(OBJ_BEC_FILE_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FILE_RELEASE)

$(OBJ_bec_file_RELEASE): $(PROJ_SRC_DIR)\backends\bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.file.cpp


########################################
# Backend library - 'pantheios.bec.file.WithCallback.a'

$(LIB_BEC_FILE_WC_DEBUG): $(OBJ_BEC_FILE_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FILE_WC_DEBUG)

$(OBJ_bec_file_WC_DEBUG): $(PROJ_SRC_DIR)\backends\bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.file.cpp

$(LIB_BEC_FILE_WC_RELEASE): $(OBJ_BEC_FILE_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FILE_WC_RELEASE)

$(OBJ_bec_file_WC_RELEASE): $(PROJ_SRC_DIR)\backends\bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.file.cpp


########################################
# Backend library - 'pantheios.be.file.a'

$(LIB_BE_FILE_DEBUG): $(OBJ_BE_FILE_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_FILE_DEBUG)

$(OBJ_be_file_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.file.c

$(LIB_BE_FILE_RELEASE): $(OBJ_BE_FILE_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_FILE_RELEASE)

$(OBJ_be_file_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.file.c


########################################
# Backend library - 'pantheios.bel.file.a'

$(LIB_BEL_FILE_DEBUG): $(OBJ_BEL_FILE_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_FILE_DEBUG)

$(OBJ_bel_file_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.file.c

$(LIB_BEL_FILE_RELEASE): $(OBJ_BEL_FILE_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_FILE_RELEASE)

$(OBJ_bel_file_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.file.c


########################################
# Backend library - 'pantheios.ber.file.a'

$(LIB_BER_FILE_DEBUG): $(OBJ_BER_FILE_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_FILE_DEBUG)

$(OBJ_ber_file_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.file.c

$(LIB_BER_FILE_RELEASE): $(OBJ_BER_FILE_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_FILE_RELEASE)

$(OBJ_ber_file_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.file.c


########################################
# Backend library - 'pantheios.bec.fprintf.a'

$(LIB_BEC_FPRINTF_DEBUG): $(OBJ_BEC_FPRINTF_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FPRINTF_DEBUG)

$(OBJ_bec_fprintf_DEBUG): $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp

$(LIB_BEC_FPRINTF_RELEASE): $(OBJ_BEC_FPRINTF_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FPRINTF_RELEASE)

$(OBJ_bec_fprintf_RELEASE): $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp


########################################
# Backend library - 'pantheios.bec.fprintf.WithCallback.a'

$(LIB_BEC_FPRINTF_WC_DEBUG): $(OBJ_BEC_FPRINTF_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FPRINTF_WC_DEBUG)

$(OBJ_bec_fprintf_WC_DEBUG): $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp

$(LIB_BEC_FPRINTF_WC_RELEASE): $(OBJ_BEC_FPRINTF_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_FPRINTF_WC_RELEASE)

$(OBJ_bec_fprintf_WC_RELEASE): $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.fprintf.cpp


########################################
# Backend library - 'pantheios.be.fprintf.a'

$(LIB_BE_FPRINTF_DEBUG): $(OBJ_BE_FPRINTF_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_FPRINTF_DEBUG)

$(OBJ_be_fprintf_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.fprintf.c

$(LIB_BE_FPRINTF_RELEASE): $(OBJ_BE_FPRINTF_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_FPRINTF_RELEASE)

$(OBJ_be_fprintf_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.fprintf.c


########################################
# Backend library - 'pantheios.bel.fprintf.a'

$(LIB_BEL_FPRINTF_DEBUG): $(OBJ_BEL_FPRINTF_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_FPRINTF_DEBUG)

$(OBJ_bel_fprintf_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.fprintf.c

$(LIB_BEL_FPRINTF_RELEASE): $(OBJ_BEL_FPRINTF_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_FPRINTF_RELEASE)

$(OBJ_bel_fprintf_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.fprintf.c


########################################
# Backend library - 'pantheios.ber.fprintf.a'

$(LIB_BER_FPRINTF_DEBUG): $(OBJ_BER_FPRINTF_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_FPRINTF_DEBUG)

$(OBJ_ber_fprintf_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.fprintf.c

$(LIB_BER_FPRINTF_RELEASE): $(OBJ_BER_FPRINTF_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_FPRINTF_RELEASE)

$(OBJ_ber_fprintf_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.fprintf.c


########################################
# Backend library - 'pantheios.bec.null.a'

$(LIB_BEC_NULL_DEBUG): $(OBJ_BEC_NULL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_NULL_DEBUG)

$(OBJ_bec_null_DEBUG): $(PROJ_SRC_DIR)\backends\bec.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.null.c

$(LIB_BEC_NULL_RELEASE): $(OBJ_BEC_NULL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_NULL_RELEASE)

$(OBJ_bec_null_RELEASE): $(PROJ_SRC_DIR)\backends\bec.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.null.c


########################################
# Backend library - 'pantheios.be.null.a'

$(LIB_BE_NULL_DEBUG): $(OBJ_BE_NULL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_NULL_DEBUG)

$(OBJ_be_null_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.null.c

$(LIB_BE_NULL_RELEASE): $(OBJ_BE_NULL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_NULL_RELEASE)

$(OBJ_be_null_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.null.c


########################################
# Backend library - 'pantheios.bel.null.a'

$(LIB_BEL_NULL_DEBUG): $(OBJ_BEL_NULL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_NULL_DEBUG)

$(OBJ_bel_null_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.null.c

$(LIB_BEL_NULL_RELEASE): $(OBJ_BEL_NULL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_NULL_RELEASE)

$(OBJ_bel_null_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.null.c


########################################
# Backend library - 'pantheios.ber.null.a'

$(LIB_BER_NULL_DEBUG): $(OBJ_BER_NULL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_NULL_DEBUG)

$(OBJ_ber_null_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.null.c

$(LIB_BER_NULL_RELEASE): $(OBJ_BER_NULL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_NULL_RELEASE)

$(OBJ_ber_null_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.null.c


########################################
# Backend library - 'pantheios.bec.COMErrorObject.a'

$(LIB_BEC_COMERROBJ_DEBUG): $(OBJ_BEC_COMERROBJ_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_COMERROBJ_DEBUG)

$(OBJ_bec_COMERROBJ_DEBUG): $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp

$(LIB_BEC_COMERROBJ_RELEASE): $(OBJ_BEC_COMERROBJ_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_COMERROBJ_RELEASE)

$(OBJ_bec_COMERROBJ_RELEASE): $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp


########################################
# Backend library - 'pantheios.bec.COMErrorObject.WithCallback.a'

$(LIB_BEC_COMERROBJWC_DEBUG): $(OBJ_BEC_COMERROBJWC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_COMERROBJWC_DEBUG)

$(OBJ_bec_COMERROBJ_WC_DEBUG): $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp

$(LIB_BEC_COMERROBJWC_RELEASE): $(OBJ_BEC_COMERROBJWC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_COMERROBJWC_RELEASE)

$(OBJ_bec_COMERROBJ_WC_RELEASE): $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.COMErrorObject.cpp


########################################
# Backend library - 'pantheios.be.COMErrorObject.a'

$(LIB_BE_COMERROBJ_DEBUG): $(OBJ_BE_COMERROBJ_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_COMERROBJ_DEBUG)

$(OBJ_be_COMERROBJ_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.COMErrorObject.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.COMErrorObject.c

$(LIB_BE_COMERROBJ_RELEASE): $(OBJ_BE_COMERROBJ_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_COMERROBJ_RELEASE)

$(OBJ_be_COMERROBJ_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.COMErrorObject.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.COMErrorObject.c


########################################
# Backend library - 'pantheios.bel.COMErrorObject.a'

$(LIB_BEL_COMERROBJ_DEBUG): $(OBJ_BEL_COMERROBJ_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_COMERROBJ_DEBUG)

$(OBJ_bel_COMERROBJ_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.COMErrorObject.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.COMErrorObject.c

$(LIB_BEL_COMERROBJ_RELEASE): $(OBJ_BEL_COMERROBJ_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_COMERROBJ_RELEASE)

$(OBJ_bel_COMERROBJ_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.COMErrorObject.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.COMErrorObject.c


########################################
# Backend library - 'pantheios.ber.COMErrorObject.a'

$(LIB_BER_COMERROBJ_DEBUG): $(OBJ_BER_COMERROBJ_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_COMERROBJ_DEBUG)

$(OBJ_ber_COMERROBJ_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.COMErrorObject.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.COMErrorObject.c

$(LIB_BER_COMERROBJ_RELEASE): $(OBJ_BER_COMERROBJ_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_COMERROBJ_RELEASE)

$(OBJ_ber_COMERROBJ_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.COMErrorObject.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.COMErrorObject.c


########################################
# Backend library - 'pantheios.bec.speech.a'

$(LIB_BEC_SPEECH_DEBUG): $(OBJ_BEC_SPEECH_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_SPEECH_DEBUG)

$(OBJ_bec_SPEECH_DEBUG): $(PROJ_SRC_DIR)\backends\bec.speech.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.speech.cpp

$(LIB_BEC_SPEECH_RELEASE): $(OBJ_BEC_SPEECH_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_SPEECH_RELEASE)

$(OBJ_bec_SPEECH_RELEASE): $(PROJ_SRC_DIR)\backends\bec.speech.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.speech.cpp


########################################
# Backend library - 'pantheios.bec.speech.WithCallback.a'

$(LIB_BEC_SPEECHWC_DEBUG): $(OBJ_BEC_SPEECHWC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_SPEECHWC_DEBUG)

$(OBJ_bec_SPEECH_WC_DEBUG): $(PROJ_SRC_DIR)\backends\bec.speech.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.speech.cpp

$(LIB_BEC_SPEECHWC_RELEASE): $(OBJ_BEC_SPEECHWC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_SPEECHWC_RELEASE)

$(OBJ_bec_SPEECH_WC_RELEASE): $(PROJ_SRC_DIR)\backends\bec.speech.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.speech.cpp


########################################
# Backend library - 'pantheios.be.speech.a'

$(LIB_BE_SPEECH_DEBUG): $(OBJ_BE_SPEECH_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_SPEECH_DEBUG)

$(OBJ_be_SPEECH_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.speech.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.speech.c

$(LIB_BE_SPEECH_RELEASE): $(OBJ_BE_SPEECH_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_SPEECH_RELEASE)

$(OBJ_be_SPEECH_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.speech.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.speech.c


########################################
# Backend library - 'pantheios.bel.speech.a'

$(LIB_BEL_SPEECH_DEBUG): $(OBJ_BEL_SPEECH_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_SPEECH_DEBUG)

$(OBJ_bel_SPEECH_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.speech.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.speech.c

$(LIB_BEL_SPEECH_RELEASE): $(OBJ_BEL_SPEECH_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_SPEECH_RELEASE)

$(OBJ_bel_SPEECH_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.speech.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.speech.c


########################################
# Backend library - 'pantheios.ber.speech.a'

$(LIB_BER_SPEECH_DEBUG): $(OBJ_BER_SPEECH_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_SPEECH_DEBUG)

$(OBJ_ber_SPEECH_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.speech.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.speech.c

$(LIB_BER_SPEECH_RELEASE): $(OBJ_BER_SPEECH_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_SPEECH_RELEASE)

$(OBJ_ber_SPEECH_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.speech.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.speech.c


########################################
# Backend library - 'pantheios.bec.WindowsEventLog.a'

$(LIB_BEC_WINEVLOG_DEBUG): $(OBJ_BEC_WINEVLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINEVLOG_DEBUG)

$(OBJ_bec_WINEVLOG_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsEventLog.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsEventLog.cpp

$(LIB_BEC_WINEVLOG_RELEASE): $(OBJ_BEC_WINEVLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINEVLOG_RELEASE)

$(OBJ_bec_WINEVLOG_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsEventLog.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsEventLog.cpp


########################################
# Backend library - 'pantheios.be.WindowsEventLog.a'

$(LIB_BE_WINEVLOG_DEBUG): $(OBJ_BE_WINEVLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINEVLOG_DEBUG)

$(OBJ_be_WINEVLOG_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.WindowsEventLog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsEventLog.c

$(LIB_BE_WINEVLOG_RELEASE): $(OBJ_BE_WINEVLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINEVLOG_RELEASE)

$(OBJ_be_WINEVLOG_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.WindowsEventLog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsEventLog.c


########################################
# Backend library - 'pantheios.bel.WindowsEventLog.a'

$(LIB_BEL_WINEVLOG_DEBUG): $(OBJ_BEL_WINEVLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINEVLOG_DEBUG)

$(OBJ_bel_WINEVLOG_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsEventLog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsEventLog.c

$(LIB_BEL_WINEVLOG_RELEASE): $(OBJ_BEL_WINEVLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINEVLOG_RELEASE)

$(OBJ_bel_WINEVLOG_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsEventLog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsEventLog.c


########################################
# Backend library - 'pantheios.ber.WindowsEventLog.a'

$(LIB_BER_WINEVLOG_DEBUG): $(OBJ_BER_WINEVLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINEVLOG_DEBUG)

$(OBJ_ber_WINEVLOG_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsEventLog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsEventLog.c

$(LIB_BER_WINEVLOG_RELEASE): $(OBJ_BER_WINEVLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINEVLOG_RELEASE)

$(OBJ_ber_WINEVLOG_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsEventLog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsEventLog.c


########################################
# Backend library - 'pantheios.bec.WindowsMessageBox.a'

$(LIB_BEC_WINMSGBX_DEBUG): $(OBJ_BEC_WINMSGBX_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINMSGBX_DEBUG)

$(OBJ_bec_WINMSGBX_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsMessageBox.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsMessageBox.cpp

$(LIB_BEC_WINMSGBX_RELEASE): $(OBJ_BEC_WINMSGBX_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINMSGBX_RELEASE)

$(OBJ_bec_WINMSGBX_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsMessageBox.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsMessageBox.cpp


########################################
# Backend library - 'pantheios.be.WindowsMessageBox.a'

$(LIB_BE_WINMSGBX_DEBUG): $(OBJ_BE_WINMSGBX_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINMSGBX_DEBUG)

$(OBJ_be_WINMSGBX_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.WindowsMessageBox.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsMessageBox.c

$(LIB_BE_WINMSGBX_RELEASE): $(OBJ_BE_WINMSGBX_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINMSGBX_RELEASE)

$(OBJ_be_WINMSGBX_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.WindowsMessageBox.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsMessageBox.c


########################################
# Backend library - 'pantheios.bel.WindowsMessageBox.a'

$(LIB_BEL_WINMSGBX_DEBUG): $(OBJ_BEL_WINMSGBX_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINMSGBX_DEBUG)

$(OBJ_bel_WINMSGBX_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsMessageBox.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsMessageBox.c

$(LIB_BEL_WINMSGBX_RELEASE): $(OBJ_BEL_WINMSGBX_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINMSGBX_RELEASE)

$(OBJ_bel_WINMSGBX_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsMessageBox.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsMessageBox.c


########################################
# Backend library - 'pantheios.ber.WindowsMessageBox.a'

$(LIB_BER_WINMSGBX_DEBUG): $(OBJ_BER_WINMSGBX_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINMSGBX_DEBUG)

$(OBJ_ber_WINMSGBX_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsMessageBox.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsMessageBox.c

$(LIB_BER_WINMSGBX_RELEASE): $(OBJ_BER_WINMSGBX_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINMSGBX_RELEASE)

$(OBJ_ber_WINMSGBX_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsMessageBox.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsMessageBox.c


########################################
# Backend library - 'pantheios.bec.WindowsConsole.a'

$(LIB_BEC_W32CONS_DEBUG): $(OBJ_BEC_W32CONS_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_W32CONS_DEBUG)

$(OBJ_bec_W32CONS_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp

$(LIB_BEC_W32CONS_RELEASE): $(OBJ_BEC_W32CONS_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_W32CONS_RELEASE)

$(OBJ_bec_W32CONS_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp


########################################
# Backend library - 'pantheios.bec.WindowsConsole.WithCallback.a'

$(LIB_BEC_W32CONSWC_DEBUG): $(OBJ_BEC_W32CONSWC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_W32CONSWC_DEBUG)

$(OBJ_bec_W32CONS_WC_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp

$(LIB_BEC_W32CONSWC_RELEASE): $(OBJ_BEC_W32CONSWC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_W32CONSWC_RELEASE)

$(OBJ_bec_W32CONS_WC_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsConsole.cpp


########################################
# Backend library - 'pantheios.be.WindowsConsole.a'

$(LIB_BE_W32CONS_DEBUG): $(OBJ_BE_W32CONS_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_W32CONS_DEBUG)

$(OBJ_be_W32CONS_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.WindowsConsole.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsConsole.c

$(LIB_BE_W32CONS_RELEASE): $(OBJ_BE_W32CONS_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_W32CONS_RELEASE)

$(OBJ_be_W32CONS_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.WindowsConsole.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsConsole.c


########################################
# Backend library - 'pantheios.bel.WindowsConsole.a'

$(LIB_BEL_W32CONS_DEBUG): $(OBJ_BEL_W32CONS_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_W32CONS_DEBUG)

$(OBJ_bel_W32CONS_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsConsole.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsConsole.c

$(LIB_BEL_W32CONS_RELEASE): $(OBJ_BEL_W32CONS_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_W32CONS_RELEASE)

$(OBJ_bel_W32CONS_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsConsole.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsConsole.c


########################################
# Backend library - 'pantheios.ber.WindowsConsole.a'

$(LIB_BER_W32CONS_DEBUG): $(OBJ_BER_W32CONS_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_W32CONS_DEBUG)

$(OBJ_ber_W32CONS_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsConsole.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsConsole.c

$(LIB_BER_W32CONS_RELEASE): $(OBJ_BER_W32CONS_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_W32CONS_RELEASE)

$(OBJ_ber_W32CONS_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsConsole.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsConsole.c


########################################
# Backend library - 'pantheios.bec.WindowsDebugger.a'

$(LIB_BEC_WINDBGR_DEBUG): $(OBJ_BEC_WINDBGR_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINDBGR_DEBUG)

$(OBJ_bec_WINDBGR_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp

$(LIB_BEC_WINDBGR_RELEASE): $(OBJ_BEC_WINDBGR_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINDBGR_RELEASE)

$(OBJ_bec_WINDBGR_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp


########################################
# Backend library - 'pantheios.bec.WindowsDebugger.WithCallback.a'

$(LIB_BEC_WINDBGR_WC_DEBUG): $(OBJ_BEC_WINDBGR_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINDBGR_WC_DEBUG)

$(OBJ_bec_WINDBGR_WC_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp

$(LIB_BEC_WINDBGR_WC_RELEASE): $(OBJ_BEC_WINDBGR_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINDBGR_WC_RELEASE)

$(OBJ_bec_WINDBGR_WC_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsDebugger.cpp


########################################
# Backend library - 'pantheios.be.WindowsDebugger.a'

$(LIB_BE_WINDBGR_DEBUG): $(OBJ_BE_WINDBGR_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINDBGR_DEBUG)

$(OBJ_be_WINDBGR_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.WindowsDebugger.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsDebugger.c

$(LIB_BE_WINDBGR_RELEASE): $(OBJ_BE_WINDBGR_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINDBGR_RELEASE)

$(OBJ_be_WINDBGR_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.WindowsDebugger.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsDebugger.c


########################################
# Backend library - 'pantheios.bel.WindowsDebugger.a'

$(LIB_BEL_WINDBGR_DEBUG): $(OBJ_BEL_WINDBGR_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINDBGR_DEBUG)

$(OBJ_bel_WINDBGR_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsDebugger.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsDebugger.c

$(LIB_BEL_WINDBGR_RELEASE): $(OBJ_BEL_WINDBGR_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINDBGR_RELEASE)

$(OBJ_bel_WINDBGR_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsDebugger.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsDebugger.c


########################################
# Backend library - 'pantheios.ber.WindowsDebugger.a'

$(LIB_BER_WINDBGR_DEBUG): $(OBJ_BER_WINDBGR_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINDBGR_DEBUG)

$(OBJ_ber_WINDBGR_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsDebugger.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsDebugger.c

$(LIB_BER_WINDBGR_RELEASE): $(OBJ_BER_WINDBGR_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINDBGR_RELEASE)

$(OBJ_ber_WINDBGR_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsDebugger.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsDebugger.c


########################################
# Backend library - 'pantheios.bec.WindowsSyslog.a'

$(LIB_BEC_WINSYSLOG_DEBUG): $(OBJ_BEC_WINSYSLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINSYSLOG_DEBUG)

$(OBJ_bec_WINSYSLOG_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp

$(LIB_BEC_WINSYSLOG_RELEASE): $(OBJ_BEC_WINSYSLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINSYSLOG_RELEASE)

$(OBJ_bec_WINSYSLOG_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp


########################################
# Backend library - 'pantheios.bec.WindowsSyslog.WithCallback.a'

$(LIB_BEC_WINSYSLOG_WC_DEBUG): $(OBJ_BEC_WINSYSLOG_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINSYSLOG_WC_DEBUG)

$(OBJ_bec_WINSYSLOG_WC_DEBUG): $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp

$(LIB_BEC_WINSYSLOG_WC_RELEASE): $(OBJ_BEC_WINSYSLOG_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEC_WINSYSLOG_WC_RELEASE)

$(OBJ_bec_WINSYSLOG_WC_RELEASE): $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bec.WindowsSyslog.cpp


########################################
# Backend library - 'pantheios.be.WindowsSyslog.a'

$(LIB_BE_WINSYSLOG_DEBUG): $(OBJ_BE_WINSYSLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINSYSLOG_DEBUG)

$(OBJ_be_WINSYSLOG_DEBUG): $(PROJ_SRC_DIR)\backends\be\be.WindowsSyslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsSyslog.c

$(LIB_BE_WINSYSLOG_RELEASE): $(OBJ_BE_WINSYSLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BE_WINSYSLOG_RELEASE)

$(OBJ_be_WINSYSLOG_RELEASE): $(PROJ_SRC_DIR)\backends\be\be.WindowsSyslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\be\be.WindowsSyslog.c


########################################
# Backend library - 'pantheios.bel.WindowsSyslog.a'

$(LIB_BEL_WINSYSLOG_DEBUG): $(OBJ_BEL_WINSYSLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINSYSLOG_DEBUG)

$(OBJ_bel_WINSYSLOG_DEBUG): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsSyslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsSyslog.c

$(LIB_BEL_WINSYSLOG_RELEASE): $(OBJ_BEL_WINSYSLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BEL_WINSYSLOG_RELEASE)

$(OBJ_bel_WINSYSLOG_RELEASE): $(PROJ_SRC_DIR)\backends\bel\bel.WindowsSyslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\bel\bel.WindowsSyslog.c


########################################
# Backend library - 'pantheios.ber.WindowsSyslog.a'

$(LIB_BER_WINSYSLOG_DEBUG): $(OBJ_BER_WINSYSLOG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINSYSLOG_DEBUG)

$(OBJ_ber_WINSYSLOG_DEBUG): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsSyslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsSyslog.c

$(LIB_BER_WINSYSLOG_RELEASE): $(OBJ_BER_WINSYSLOG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_BER_WINSYSLOG_RELEASE)

$(OBJ_ber_WINSYSLOG_RELEASE): $(PROJ_SRC_DIR)\backends\ber\ber.WindowsSyslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\backends\ber\ber.WindowsSyslog.c

########################################
# Front-end library - 'pantheios.fe.all.a'

$(LIB_FE_ALL_DEBUG): $(OBJ_FE_ALL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_ALL_DEBUG)

$(OBJ_fe_all_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.all.c

$(LIB_FE_ALL_RELEASE): $(OBJ_FE_ALL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_ALL_RELEASE)

$(OBJ_fe_all_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.all.c

########################################
# Front-end library - 'pantheios.fe.all.WithCallback.a'

$(LIB_FE_ALL_WC_DEBUG): $(OBJ_FE_ALL_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_ALL_WC_DEBUG)

$(OBJ_fe_all_WC_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.all.c

$(LIB_FE_ALL_WC_RELEASE): $(OBJ_FE_ALL_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_ALL_WC_RELEASE)

$(OBJ_fe_all_WC_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.all.c

########################################
# Front-end library - 'pantheios.fe.fail.a'

$(LIB_FE_FAIL_DEBUG): $(OBJ_FE_FAIL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_FAIL_DEBUG)

$(OBJ_fe_fail_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.fail.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.fail.c

$(LIB_FE_FAIL_RELEASE): $(OBJ_FE_FAIL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_FAIL_RELEASE)

$(OBJ_fe_fail_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.fail.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.fail.c

########################################
# Front-end library - 'pantheios.fe.N.a'

$(LIB_FE_N_DEBUG): $(OBJ_FE_N_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_N_DEBUG)

$(OBJ_fe_N_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.N.c

$(LIB_FE_N_RELEASE): $(OBJ_FE_N_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_N_RELEASE)

$(OBJ_fe_N_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.N.c

########################################
# Front-end library - 'pantheios.fe.N.WithCallback.a'

$(LIB_FE_N_WC_DEBUG): $(OBJ_FE_N_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_N_WC_DEBUG)

$(OBJ_fe_N_WC_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.N.c

$(LIB_FE_N_WC_RELEASE): $(OBJ_FE_N_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_N_WC_RELEASE)

$(OBJ_fe_N_WC_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.N.c

########################################
# Front-end library - 'pantheios.fe.null.a'

$(LIB_FE_NULL_DEBUG): $(OBJ_FE_NULL_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_NULL_DEBUG)

$(OBJ_fe_null_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.null.c

$(LIB_FE_NULL_RELEASE): $(OBJ_FE_NULL_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_NULL_RELEASE)

$(OBJ_fe_null_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.null.c

########################################
# Front-end library - 'pantheios.fe.null.WithCallback.a'

$(LIB_FE_NULL_WC_DEBUG): $(OBJ_FE_NULL_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_NULL_WC_DEBUG)

$(OBJ_fe_null_WC_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.null.c

$(LIB_FE_NULL_WC_RELEASE): $(OBJ_FE_NULL_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_NULL_WC_RELEASE)

$(OBJ_fe_null_WC_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.null.c

########################################
# Front-end library - 'pantheios.fe.simple.a'

$(LIB_FE_SIMPLE_DEBUG): $(OBJ_FE_SIMPLE_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_SIMPLE_DEBUG)

$(OBJ_fe_simple_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.simple.c

$(LIB_FE_SIMPLE_RELEASE): $(OBJ_FE_SIMPLE_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_SIMPLE_RELEASE)

$(OBJ_fe_simple_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.simple.c

########################################
# Front-end library - 'pantheios.fe.simple.WithCallback.a'

$(LIB_FE_SIMPLE_WC_DEBUG): $(OBJ_FE_SIMPLE_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_SIMPLE_WC_DEBUG)

$(OBJ_fe_simple_WC_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.simple.c

$(LIB_FE_SIMPLE_WC_RELEASE): $(OBJ_FE_SIMPLE_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_SIMPLE_WC_RELEASE)

$(OBJ_fe_simple_WC_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.simple.c

########################################
# Front-end library - 'pantheios.fe.WindowsRegistry.a'

$(LIB_FE_WINREG_DEBUG): $(OBJ_FE_WINREG_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_WINREG_DEBUG)

$(OBJ_fe_winreg_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp

$(LIB_FE_WINREG_RELEASE): $(OBJ_FE_WINREG_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_WINREG_RELEASE)

$(OBJ_fe_winreg_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp


########################################
# Front-end library - 'pantheios.fe.WindowsRegistry.WithCallback.a'

$(LIB_FE_WINREG_WC_DEBUG): $(OBJ_FE_WINREG_WC_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_WINREG_WC_DEBUG)

$(OBJ_fe_winreg_WC_DEBUG): $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_DEBUG) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp

$(LIB_FE_WINREG_WC_RELEASE): $(OBJ_FE_WINREG_WC_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_FE_WINREG_WC_RELEASE)

$(OBJ_fe_winreg_WC_RELEASE): $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_RELEASE) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) --output-path=$@ $(PROJ_SRC_DIR)\frontends\fe.WindowsRegistry.cpp


########################################
# b64 library - 'b64.X.pan.core.a'

$(LIB_B64_DEBUG): $(OBJ_B64_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_B64_DEBUG)

$(OBJ_b64_b64_DEBUG): $(B64_SRC_DIR)\b64.c $(B64_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_B64_ARGS_DEBUG) $(CC_B64_INCLUDES_DEBUG) --output-path=$@ $(B64_SRC_DIR)\b64.c

$(LIB_B64_RELEASE): $(OBJ_B64_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_B64_RELEASE)

$(OBJ_b64_b64_RELEASE): $(B64_SRC_DIR)\b64.c $(B64_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_B64_ARGS_RELEASE) $(CC_B64_INCLUDES_RELEASE) --output-path=$@ $(B64_SRC_DIR)\b64.c

########################################
# shwild library - 'shwild.X.pan.core.a'

$(LIB_SHWILD_DEBUG): $(OBJ_SHWILD_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_SHWILD_DEBUG)

$(OBJ_shw_api_DEBUG): $(SHWILD_SRC_DIR)\api.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_DEBUG) $(CPPC_SHWILD_INCLUDES_DEBUG) --output-path=$@ $(SHWILD_SRC_DIR)\api.cpp
$(OBJ_shw_matches_DEBUG): $(SHWILD_SRC_DIR)\matches.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_DEBUG) $(CPPC_SHWILD_INCLUDES_DEBUG) --output-path=$@ $(SHWILD_SRC_DIR)\matches.cpp
$(OBJ_shw_pattern_DEBUG): $(SHWILD_SRC_DIR)\pattern.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_DEBUG) $(CPPC_SHWILD_INCLUDES_DEBUG) --output-path=$@ $(SHWILD_SRC_DIR)\pattern.cpp

$(LIB_SHWILD_RELEASE): $(OBJ_SHWILD_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_SHWILD_RELEASE)

$(OBJ_shw_api_RELEASE): $(SHWILD_SRC_DIR)\api.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_RELEASE) $(CPPC_SHWILD_INCLUDES_RELEASE) --output-path=$@ $(SHWILD_SRC_DIR)\api.cpp
$(OBJ_shw_matches_RELEASE): $(SHWILD_SRC_DIR)\matches.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_RELEASE) $(CPPC_SHWILD_INCLUDES_RELEASE) --output-path=$@ $(SHWILD_SRC_DIR)\matches.cpp
$(OBJ_shw_pattern_RELEASE): $(SHWILD_SRC_DIR)\pattern.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_RELEASE) $(CPPC_SHWILD_INCLUDES_RELEASE) --output-path=$@ $(SHWILD_SRC_DIR)\pattern.cpp

########################################
# xTests library - 'xtests.X.pan.core.a'

$(LIB_XTESTS_DEBUG): $(OBJ_XTESTS_DEBUG) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_XTESTS_DEBUG)

$(OBJ_xts_core_DEBUG): $(XTESTS_SRC_DIR)\xtests.core.cpp $(XTESTS_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_XTESTS_ARGS_DEBUG) $(CPPC_XTESTS_INCLUDES_DEBUG) --output-path=$@ $(XTESTS_SRC_DIR)\xtests.core.cpp

$(LIB_XTESTS_RELEASE): $(OBJ_XTESTS_RELEASE) $(PROJ_MAKEFILE_NAME)
	@-if not exist $(PROJ_LIB_DIR)\NUL mkdir $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) $@ $(OBJ_XTESTS_RELEASE)

$(OBJ_xts_core_RELEASE): $(XTESTS_SRC_DIR)\xtests.core.cpp $(XTESTS_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_XTESTS_ARGS_RELEASE) $(CPPC_XTESTS_INCLUDES_RELEASE) --output-path=$@ $(XTESTS_SRC_DIR)\xtests.core.cpp

##################################################
# Examples

#  object files

$(OBJ_EX_P_be_01_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.callback\example.cpp.backends.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.callback\example.cpp.backends.callback.cpp
$(OBJ_EX_P_be_02_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file\example.cpp.backends.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file\example.cpp.backends.file.cpp
$(OBJ_EX_P_be_03_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.callback\example.cpp.backends.file.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.callback\example.cpp.backends.file.callback.cpp
$(OBJ_EX_P_be_04_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.lrsplit\example.cpp.backends.file.lrsplit.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.lrsplit\example.cpp.backends.file.lrsplit.cpp
$(OBJ_EX_P_be_05_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.1\example.cpp.backends.mx.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.1\example.cpp.backends.mx.1.cpp
$(OBJ_EX_P_be_06_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.2\example.cpp.backends.mx.2.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.2\example.cpp.backends.mx.2.cpp
$(OBJ_EX_P_cp_01_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_ASSERT\example.cpp.contract.PANTHEIOS_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_ASSERT\example.cpp.contract.PANTHEIOS_ASSERT.cpp
$(OBJ_EX_P_cp_02_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp
$(OBJ_EX_P_fm_01_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.highres\example.cpp.format.highres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.highres\example.cpp.format.highres.cpp
$(OBJ_EX_P_fm_02_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.lowres\example.cpp.format.lowres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.lowres\example.cpp.format.lowres.cpp
$(OBJ_EX_P_fm_03_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.pad\example.cpp.format.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.pad\example.cpp.format.pad.cpp
$(OBJ_EX_P_fe_01_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\frontends\example.cpp.frontends.custom\example.cpp.frontends.custom.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\frontends\example.cpp.frontends.custom\example.cpp.frontends.custom.cpp
$(OBJ_EX_P_in_01_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.args\example.cpp.inserter.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.args\example.cpp.inserter.args.cpp
$(OBJ_EX_P_in_02_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b\example.cpp.inserter.b.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b\example.cpp.inserter.b.cpp
$(OBJ_EX_P_in_03_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b64\example.cpp.inserter.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b64\example.cpp.inserter.b64.cpp
$(OBJ_EX_P_in_04_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.blob\example.cpp.inserter.blob.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.blob\example.cpp.inserter.blob.cpp
$(OBJ_EX_P_in_05_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.character\example.cpp.inserter.character.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.character\example.cpp.inserter.character.cpp
$(OBJ_EX_P_in_06_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hex_ptr\example.cpp.inserter.hex_ptr.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hex_ptr\example.cpp.inserter.hex_ptr.cpp
$(OBJ_EX_P_in_07_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hostid\example.cpp.inserter.hostid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hostid\example.cpp.inserter.hostid.cpp
$(OBJ_EX_P_in_08_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.i\example.cpp.inserter.i.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.i\example.cpp.inserter.i.cpp
$(OBJ_EX_P_in_09_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.integer\example.cpp.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.integer\example.cpp.inserter.integer.cpp
$(OBJ_EX_P_in_10_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.p\example.cpp.inserter.p.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.p\example.cpp.inserter.p.cpp
$(OBJ_EX_P_in_11_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.pointer\example.cpp.inserter.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.pointer\example.cpp.inserter.pointer.cpp
$(OBJ_EX_P_in_12_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.processid\example.cpp.inserter.processid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.processid\example.cpp.inserter.processid.cpp
$(OBJ_EX_P_in_13_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.real\example.cpp.inserter.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.real\example.cpp.inserter.real.cpp
$(OBJ_EX_P_in_14_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.threadid\example.cpp.inserter.threadid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.threadid\example.cpp.inserter.threadid.cpp
$(OBJ_EX_P_in_15_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xi\example.cpp.inserter.xi.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xi\example.cpp.inserter.xi.cpp
$(OBJ_EX_P_in_16_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xp\example.cpp.inserter.xp.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xp\example.cpp.inserter.xp.cpp
$(OBJ_EX_P_in_17_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.w2m\example.cpp.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.w2m\example.cpp.inserter.w2m.cpp
$(OBJ_EX_P_ms_01_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.101\example.cpp.misc.101.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.101\example.cpp.misc.101.cpp
$(OBJ_EX_P_ms_02_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.custom_type_1\example.cpp.misc.custom_type_1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.custom_type_1\example.cpp.misc.custom_type_1.cpp
$(OBJ_EX_P_ms_03_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.hetero1\example.cpp.misc.hetero1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.hetero1\example.cpp.misc.hetero1.cpp
$(OBJ_EX_P_ms_04_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.no_namespace\example.cpp.misc.no_namespace.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.no_namespace\example.cpp.misc.no_namespace.cpp
$(OBJ_EX_P_ms_05_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.strings\example.cpp.misc.strings.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.strings\example.cpp.misc.strings.cpp
$(OBJ_EX_P_ms_06_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.extended_severity_information\example.cpp.misc.extended_severity_information.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.extended_severity_information\example.cpp.misc.extended_severity_information.cpp
$(OBJ_EX_P_ut_01_DEBUG):	$(PROJ_EXAMPLES_DIR)\cpp\util\example.cpp.util.strdup\example.cpp.util.strdup.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\util\example.cpp.util.strdup\example.cpp.util.strdup.cpp
$(OBJ_EX_C_0_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\example.c.101\example.c.101.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.101\example.c.101.c
$(OBJ_EX_C_1_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\example.c.log_n\example.c.log_n.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.log_n\example.c.log_n.c
$(OBJ_EX_C_2_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\example.c.core.pantheios_logprintf\example.c.core.pantheios_logprintf.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.core.pantheios_logprintf\example.c.core.pantheios_logprintf.c
$(OBJ_EX_C_3_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\example.c.N\example.c.N.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.N\example.c.N.c
$(OBJ_EX_C_4_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\util\example.c.util.gethostname\example.c.util.gethostname.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\util\example.c.util.gethostname\example.c.util.gethostname.c
$(OBJ_EX_C_5_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\util\example.c.util.getcurrenttime\example.c.util.getcurrenttime.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\util\example.c.util.getcurrenttime\example.c.util.getcurrenttime.c
$(OBJ_EX_C_6_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\example.c.assert\example.c.assert.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.assert\example.c.assert.c
$(OBJ_EX_C_7_DEBUG):	$(PROJ_EXAMPLES_DIR)\c\example.c.extended_severity\example.c.extended_severity.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.extended_severity\example.c.extended_severity.c

$(OBJ_EX_P_be_01_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.callback\example.cpp.backends.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.callback\example.cpp.backends.callback.cpp
$(OBJ_EX_P_be_02_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file\example.cpp.backends.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file\example.cpp.backends.file.cpp
$(OBJ_EX_P_be_03_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.callback\example.cpp.backends.file.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.callback\example.cpp.backends.file.callback.cpp
$(OBJ_EX_P_be_04_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.lrsplit\example.cpp.backends.file.lrsplit.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.file.lrsplit\example.cpp.backends.file.lrsplit.cpp
$(OBJ_EX_P_be_05_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.1\example.cpp.backends.mx.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.1\example.cpp.backends.mx.1.cpp
$(OBJ_EX_P_be_06_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.2\example.cpp.backends.mx.2.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\backends\example.cpp.backends.mx.2\example.cpp.backends.mx.2.cpp
$(OBJ_EX_P_cp_01_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_ASSERT\example.cpp.contract.PANTHEIOS_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_ASSERT\example.cpp.contract.PANTHEIOS_ASSERT.cpp
$(OBJ_EX_P_cp_02_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\contract\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT\example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp
$(OBJ_EX_P_fm_01_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.highres\example.cpp.format.highres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.highres\example.cpp.format.highres.cpp
$(OBJ_EX_P_fm_02_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.lowres\example.cpp.format.lowres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.lowres\example.cpp.format.lowres.cpp
$(OBJ_EX_P_fm_03_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.pad\example.cpp.format.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\format\example.cpp.format.pad\example.cpp.format.pad.cpp
$(OBJ_EX_P_fe_01_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\frontends\example.cpp.frontends.custom\example.cpp.frontends.custom.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\frontends\example.cpp.frontends.custom\example.cpp.frontends.custom.cpp
$(OBJ_EX_P_in_01_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.args\example.cpp.inserter.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.args\example.cpp.inserter.args.cpp
$(OBJ_EX_P_in_02_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b\example.cpp.inserter.b.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b\example.cpp.inserter.b.cpp
$(OBJ_EX_P_in_03_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b64\example.cpp.inserter.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.b64\example.cpp.inserter.b64.cpp
$(OBJ_EX_P_in_04_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.blob\example.cpp.inserter.blob.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.blob\example.cpp.inserter.blob.cpp
$(OBJ_EX_P_in_05_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.character\example.cpp.inserter.character.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.character\example.cpp.inserter.character.cpp
$(OBJ_EX_P_in_06_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hex_ptr\example.cpp.inserter.hex_ptr.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hex_ptr\example.cpp.inserter.hex_ptr.cpp
$(OBJ_EX_P_in_07_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hostid\example.cpp.inserter.hostid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.hostid\example.cpp.inserter.hostid.cpp
$(OBJ_EX_P_in_08_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.i\example.cpp.inserter.i.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.i\example.cpp.inserter.i.cpp
$(OBJ_EX_P_in_09_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.integer\example.cpp.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.integer\example.cpp.inserter.integer.cpp
$(OBJ_EX_P_in_10_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.p\example.cpp.inserter.p.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.p\example.cpp.inserter.p.cpp
$(OBJ_EX_P_in_11_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.pointer\example.cpp.inserter.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.pointer\example.cpp.inserter.pointer.cpp
$(OBJ_EX_P_in_12_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.processid\example.cpp.inserter.processid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.processid\example.cpp.inserter.processid.cpp
$(OBJ_EX_P_in_13_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.real\example.cpp.inserter.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.real\example.cpp.inserter.real.cpp
$(OBJ_EX_P_in_14_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.threadid\example.cpp.inserter.threadid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.threadid\example.cpp.inserter.threadid.cpp
$(OBJ_EX_P_in_15_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xi\example.cpp.inserter.xi.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xi\example.cpp.inserter.xi.cpp
$(OBJ_EX_P_in_16_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xp\example.cpp.inserter.xp.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.xp\example.cpp.inserter.xp.cpp
$(OBJ_EX_P_in_17_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.w2m\example.cpp.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\inserters\example.cpp.inserter.w2m\example.cpp.inserter.w2m.cpp
$(OBJ_EX_P_ms_01_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.101\example.cpp.misc.101.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.101\example.cpp.misc.101.cpp
$(OBJ_EX_P_ms_02_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.custom_type_1\example.cpp.misc.custom_type_1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.custom_type_1\example.cpp.misc.custom_type_1.cpp
$(OBJ_EX_P_ms_03_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.hetero1\example.cpp.misc.hetero1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.hetero1\example.cpp.misc.hetero1.cpp
$(OBJ_EX_P_ms_04_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.no_namespace\example.cpp.misc.no_namespace.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.no_namespace\example.cpp.misc.no_namespace.cpp
$(OBJ_EX_P_ms_05_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.strings\example.cpp.misc.strings.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.strings\example.cpp.misc.strings.cpp
$(OBJ_EX_P_ms_06_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.extended_severity_information\example.cpp.misc.extended_severity_information.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\misc\example.cpp.misc.extended_severity_information\example.cpp.misc.extended_severity_information.cpp
$(OBJ_EX_P_ut_01_RELEASE):	$(PROJ_EXAMPLES_DIR)\cpp\util\example.cpp.util.strdup\example.cpp.util.strdup.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\cpp\util\example.cpp.util.strdup\example.cpp.util.strdup.cpp
$(OBJ_EX_C_0_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\example.c.101\example.c.101.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.101\example.c.101.c
$(OBJ_EX_C_1_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\example.c.log_n\example.c.log_n.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.log_n\example.c.log_n.c
$(OBJ_EX_C_2_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\example.c.core.pantheios_logprintf\example.c.core.pantheios_logprintf.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.core.pantheios_logprintf\example.c.core.pantheios_logprintf.c
$(OBJ_EX_C_3_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\example.c.N\example.c.N.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.N\example.c.N.c
$(OBJ_EX_C_4_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\util\example.c.util.gethostname\example.c.util.gethostname.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\util\example.c.util.gethostname\example.c.util.gethostname.c
$(OBJ_EX_C_5_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\util\example.c.util.getcurrenttime\example.c.util.getcurrenttime.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\util\example.c.util.getcurrenttime\example.c.util.getcurrenttime.c
$(OBJ_EX_C_6_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\example.c.assert\example.c.assert.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.assert\example.c.assert.c
$(OBJ_EX_C_7_RELEASE):	$(PROJ_EXAMPLES_DIR)\c\example.c.extended_severity\example.c.extended_severity.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_EXAMPLES_DIR)\c\example.c.extended_severity\example.c.extended_severity.c


# executables

$(TARG_EX_P_be_01_DEBUG): \
			$(OBJ_EX_P_be_01_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_W32CONS_DEBUG)\
			$(LIB_BEC_W32CONS_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_be_01_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_W32CONS_DEBUG)\
		$(LIB_BEC_W32CONS_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_be_02_DEBUG): \
			$(OBJ_EX_P_be_02_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FILE_DEBUG)\
			$(LIB_BEC_FILE_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_be_02_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FILE_DEBUG)\
		$(LIB_BEC_FILE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_be_03_DEBUG): \
			$(OBJ_EX_P_be_03_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FILE_DEBUG)\
			$(LIB_BEC_FILE_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_be_03_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FILE_DEBUG)\
		$(LIB_BEC_FILE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_be_04_DEBUG): \
			$(OBJ_EX_P_be_04_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FILE_DEBUG)\
			$(LIB_BEC_FILE_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_be_04_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FILE_DEBUG)\
		$(LIB_BEC_FILE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_be_05_DEBUG): \
			$(OBJ_EX_P_be_05_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_N_DEBUG)\
			$(LIB_BEC_W32CONS_DEBUG)\
			$(LIB_BEC_WINSYSLOG_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_be_05_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_N_DEBUG)\
		$(LIB_BEC_W32CONS_DEBUG)\
		$(LIB_BEC_WINSYSLOG_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_be_06_DEBUG): \
			$(OBJ_EX_P_be_06_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_N_DEBUG)\
			$(LIB_BEC_W32CONS_DEBUG)\
			$(LIB_BEC_WINSYSLOG_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_be_06_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_N_DEBUG)\
		$(LIB_BEC_W32CONS_DEBUG)\
		$(LIB_BEC_WINSYSLOG_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_cp_01_DEBUG): \
			$(OBJ_EX_P_cp_01_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_cp_01_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_cp_02_DEBUG): \
			$(OBJ_EX_P_cp_02_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_cp_02_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_ms_06_DEBUG): \
			$(OBJ_EX_P_ms_06_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_ms_06_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_fm_01_DEBUG): \
			$(OBJ_EX_P_fm_01_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_fm_01_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_fm_02_DEBUG): \
			$(OBJ_EX_P_fm_02_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_fm_02_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_fm_03_DEBUG): \
			$(OBJ_EX_P_fm_03_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_fm_03_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_fe_01_DEBUG): \
			$(OBJ_EX_P_fe_01_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_fe_01_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_01_DEBUG): \
			$(OBJ_EX_P_in_01_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_01_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_02_DEBUG): \
			$(OBJ_EX_P_in_02_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_02_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_03_DEBUG): \
			$(OBJ_EX_P_in_03_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_03_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_04_DEBUG): \
			$(OBJ_EX_P_in_04_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_04_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_05_DEBUG): \
			$(OBJ_EX_P_in_05_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_05_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_06_DEBUG): \
			$(OBJ_EX_P_in_06_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_06_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_07_DEBUG): \
			$(OBJ_EX_P_in_07_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_07_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_08_DEBUG): \
			$(OBJ_EX_P_in_08_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_08_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_09_DEBUG): \
			$(OBJ_EX_P_in_09_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_09_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_10_DEBUG): \
			$(OBJ_EX_P_in_10_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_10_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_11_DEBUG): \
			$(OBJ_EX_P_in_11_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_11_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_12_DEBUG): \
			$(OBJ_EX_P_in_12_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_12_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_13_DEBUG): \
			$(OBJ_EX_P_in_13_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_13_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_14_DEBUG): \
			$(OBJ_EX_P_in_14_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_14_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_15_DEBUG): \
			$(OBJ_EX_P_in_15_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_15_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_16_DEBUG): \
			$(OBJ_EX_P_in_16_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_16_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_in_17_DEBUG): \
			$(OBJ_EX_P_in_17_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_in_17_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_ms_01_DEBUG): \
			$(OBJ_EX_P_ms_01_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_ms_01_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_ms_02_DEBUG): \
			$(OBJ_EX_P_ms_02_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_ms_02_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_ms_03_DEBUG): \
			$(OBJ_EX_P_ms_03_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_ms_03_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_ms_04_DEBUG): \
			$(OBJ_EX_P_ms_04_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_ms_04_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_ms_05_DEBUG): \
			$(OBJ_EX_P_ms_05_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_ms_05_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_ut_01_DEBUG): \
			$(OBJ_EX_P_ut_01_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_P_ut_01_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_0_DEBUG): \
			$(OBJ_EX_C_0_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_0_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_1_DEBUG): \
			$(OBJ_EX_C_1_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_1_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_2_DEBUG): \
			$(OBJ_EX_C_2_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_2_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_3_DEBUG): \
			$(OBJ_EX_C_3_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_N_DEBUG)\
			$(LIB_BE_N_DEBUG)\
			$(LIB_BEC_FILE_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_BEC_NULL_DEBUG)\
			$(LIB_BEC_WINSYSLOG_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_3_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_N_DEBUG)\
		$(LIB_BE_N_DEBUG)\
		$(LIB_BEC_FILE_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_BEC_NULL_DEBUG)\
		$(LIB_BEC_WINSYSLOG_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_4_DEBUG): \
			$(OBJ_EX_C_4_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_4_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_5_DEBUG): \
			$(OBJ_EX_C_5_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_5_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_6_DEBUG): \
			$(OBJ_EX_C_6_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_6_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_C_7_DEBUG): \
			$(OBJ_EX_C_7_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_EX_C_7_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(TARG_EX_P_be_01_RELEASE): \
			$(OBJ_EX_P_be_01_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_W32CONS_RELEASE)\
			$(LIB_BEC_W32CONS_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_be_01_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_W32CONS_RELEASE)\
		$(LIB_BEC_W32CONS_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_be_02_RELEASE): \
			$(OBJ_EX_P_be_02_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FILE_RELEASE)\
			$(LIB_BEC_FILE_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_be_02_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FILE_RELEASE)\
		$(LIB_BEC_FILE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_be_03_RELEASE): \
			$(OBJ_EX_P_be_03_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FILE_RELEASE)\
			$(LIB_BEC_FILE_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_be_03_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FILE_RELEASE)\
		$(LIB_BEC_FILE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_be_04_RELEASE): \
			$(OBJ_EX_P_be_04_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FILE_RELEASE)\
			$(LIB_BEC_FILE_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_be_04_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FILE_RELEASE)\
		$(LIB_BEC_FILE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_be_05_RELEASE): \
			$(OBJ_EX_P_be_05_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_N_RELEASE)\
			$(LIB_BEC_W32CONS_RELEASE)\
			$(LIB_BEC_WINSYSLOG_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_be_05_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_N_RELEASE)\
		$(LIB_BEC_W32CONS_RELEASE)\
		$(LIB_BEC_WINSYSLOG_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_be_06_RELEASE): \
			$(OBJ_EX_P_be_06_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_N_RELEASE)\
			$(LIB_BEC_W32CONS_RELEASE)\
			$(LIB_BEC_WINSYSLOG_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_be_06_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_N_RELEASE)\
		$(LIB_BEC_W32CONS_RELEASE)\
		$(LIB_BEC_WINSYSLOG_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_cp_01_RELEASE): \
			$(OBJ_EX_P_cp_01_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_cp_01_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_cp_02_RELEASE): \
			$(OBJ_EX_P_cp_02_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_cp_02_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_ms_06_RELEASE): \
			$(OBJ_EX_P_ms_06_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_ms_06_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_fm_01_RELEASE): \
			$(OBJ_EX_P_fm_01_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_fm_01_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_fm_02_RELEASE): \
			$(OBJ_EX_P_fm_02_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_fm_02_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_fm_03_RELEASE): \
			$(OBJ_EX_P_fm_03_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_fm_03_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_fe_01_RELEASE): \
			$(OBJ_EX_P_fe_01_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_fe_01_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_01_RELEASE): \
			$(OBJ_EX_P_in_01_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_01_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_02_RELEASE): \
			$(OBJ_EX_P_in_02_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_02_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_03_RELEASE): \
			$(OBJ_EX_P_in_03_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_03_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_04_RELEASE): \
			$(OBJ_EX_P_in_04_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_04_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_05_RELEASE): \
			$(OBJ_EX_P_in_05_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_05_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_06_RELEASE): \
			$(OBJ_EX_P_in_06_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_06_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_07_RELEASE): \
			$(OBJ_EX_P_in_07_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_07_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_08_RELEASE): \
			$(OBJ_EX_P_in_08_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_08_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_09_RELEASE): \
			$(OBJ_EX_P_in_09_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_09_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_10_RELEASE): \
			$(OBJ_EX_P_in_10_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_10_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_11_RELEASE): \
			$(OBJ_EX_P_in_11_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_11_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_12_RELEASE): \
			$(OBJ_EX_P_in_12_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_12_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_13_RELEASE): \
			$(OBJ_EX_P_in_13_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_13_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_14_RELEASE): \
			$(OBJ_EX_P_in_14_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_14_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_15_RELEASE): \
			$(OBJ_EX_P_in_15_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_15_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_16_RELEASE): \
			$(OBJ_EX_P_in_16_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_16_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_in_17_RELEASE): \
			$(OBJ_EX_P_in_17_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_in_17_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_ms_01_RELEASE): \
			$(OBJ_EX_P_ms_01_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_ms_01_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_ms_02_RELEASE): \
			$(OBJ_EX_P_ms_02_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_ms_02_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_ms_03_RELEASE): \
			$(OBJ_EX_P_ms_03_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_ms_03_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_ms_04_RELEASE): \
			$(OBJ_EX_P_ms_04_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_ms_04_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_ms_05_RELEASE): \
			$(OBJ_EX_P_ms_05_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_ms_05_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_P_ut_01_RELEASE): \
			$(OBJ_EX_P_ut_01_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_P_ut_01_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_0_RELEASE): \
			$(OBJ_EX_C_0_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_0_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_1_RELEASE): \
			$(OBJ_EX_C_1_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_1_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_2_RELEASE): \
			$(OBJ_EX_C_2_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_2_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_3_RELEASE): \
			$(OBJ_EX_C_3_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_N_RELEASE)\
			$(LIB_BE_N_RELEASE)\
			$(LIB_BEC_FILE_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_BEC_NULL_RELEASE)\
			$(LIB_BEC_WINSYSLOG_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_3_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_N_RELEASE)\
		$(LIB_BE_N_RELEASE)\
		$(LIB_BEC_FILE_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_BEC_NULL_RELEASE)\
		$(LIB_BEC_WINSYSLOG_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_4_RELEASE): \
			$(OBJ_EX_C_4_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_4_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_5_RELEASE): \
			$(OBJ_EX_C_5_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_5_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_6_RELEASE): \
			$(OBJ_EX_C_6_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_6_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(TARG_EX_C_7_RELEASE): \
			$(OBJ_EX_C_7_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_EX_C_7_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)


##################################################
# Tests

##############################
# Component Tests

$(TARG_TEST_COMP_core_initseq_DEBUG):	\
			$(OBJ_TEST_COMP_core_initseq_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_core_initseq_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_core_initseq_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.core.initialisation_sequence\test.component.core.initialisation_sequence.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.core.initialisation_sequence\test.component.core.initialisation_sequence.c

$(TARG_TEST_COMP_core_initseq_RELEASE):	\
			$(OBJ_TEST_COMP_core_initseq_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_core_initseq_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_core_initseq_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.core.initialisation_sequence\test.component.core.initialisation_sequence.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.core.initialisation_sequence\test.component.core.initialisation_sequence.c

$(TARG_TEST_COMP_custsev_DEBUG):	\
			$(OBJ_TEST_COMP_custsev_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_custsev_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_custsev_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.custom_severity\test.component.custom_severity.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.custom_severity\test.component.custom_severity.cpp

$(TARG_TEST_COMP_custsev_RELEASE):	\
			$(OBJ_TEST_COMP_custsev_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_custsev_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_custsev_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.custom_severity\test.component.custom_severity.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.custom_severity\test.component.custom_severity.cpp

$(TARG_TEST_COMP_ins_args_DEBUG):	\
			$(OBJ_TEST_COMP_ins_args_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_args_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_args_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.args\test.component.inserters.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.args\test.component.inserters.args.cpp

$(TARG_TEST_COMP_ins_args_RELEASE):	\
			$(OBJ_TEST_COMP_ins_args_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_args_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_args_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.args\test.component.inserters.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.args\test.component.inserters.args.cpp

$(TARG_TEST_COMP_ins_b64_DEBUG):	\
			$(OBJ_TEST_COMP_ins_b64_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_B64_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_b64_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_B64_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_b64_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.b64\test.component.inserters.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.b64\test.component.inserters.b64.cpp

$(TARG_TEST_COMP_ins_b64_RELEASE):	\
			$(OBJ_TEST_COMP_ins_b64_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_B64_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_b64_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_B64_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_b64_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.b64\test.component.inserters.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.b64\test.component.inserters.b64.cpp

$(TARG_TEST_COMP_ins_bool_DEBUG):	\
			$(OBJ_TEST_COMP_ins_bool_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_bool_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_bool_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.boolean\test.component.inserters.boolean.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.boolean\test.component.inserters.boolean.cpp

$(TARG_TEST_COMP_ins_bool_RELEASE):	\
			$(OBJ_TEST_COMP_ins_bool_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_bool_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_bool_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.boolean\test.component.inserters.boolean.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.boolean\test.component.inserters.boolean.cpp

$(TARG_TEST_COMP_ins_hostid_DEBUG):	\
			$(OBJ_TEST_COMP_ins_hostid_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_hostid_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_hostid_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.hostId\test.component.inserters.hostId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.hostId\test.component.inserters.hostId.cpp

$(TARG_TEST_COMP_ins_hostid_RELEASE):	\
			$(OBJ_TEST_COMP_ins_hostid_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_hostid_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_hostid_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.hostId\test.component.inserters.hostId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.hostId\test.component.inserters.hostId.cpp

$(TARG_TEST_COMP_ins_int_DEBUG):	\
			$(OBJ_TEST_COMP_ins_int_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_int_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_int_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.integer\test.component.inserters.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.integer\test.component.inserters.integer.cpp

$(TARG_TEST_COMP_ins_int_RELEASE):	\
			$(OBJ_TEST_COMP_ins_int_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_int_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_int_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.integer\test.component.inserters.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.integer\test.component.inserters.integer.cpp

$(TARG_TEST_COMP_ins_pad_DEBUG):	\
			$(OBJ_TEST_COMP_ins_pad_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_pad_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_pad_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pad\test.component.inserters.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pad\test.component.inserters.pad.cpp

$(TARG_TEST_COMP_ins_pad_RELEASE):	\
			$(OBJ_TEST_COMP_ins_pad_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_pad_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_pad_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pad\test.component.inserters.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pad\test.component.inserters.pad.cpp

$(TARG_TEST_COMP_ins_ptr_DEBUG):	\
			$(OBJ_TEST_COMP_ins_ptr_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_ptr_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_ptr_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pointer\test.component.inserters.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pointer\test.component.inserters.pointer.cpp

$(TARG_TEST_COMP_ins_ptr_RELEASE):	\
			$(OBJ_TEST_COMP_ins_ptr_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_ptr_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_ptr_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pointer\test.component.inserters.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.pointer\test.component.inserters.pointer.cpp

$(TARG_TEST_COMP_ins_prid_DEBUG):	\
			$(OBJ_TEST_COMP_ins_prid_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_prid_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_prid_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.processId\test.component.inserters.processId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.processId\test.component.inserters.processId.cpp

$(TARG_TEST_COMP_ins_prid_RELEASE):	\
			$(OBJ_TEST_COMP_ins_prid_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_prid_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_prid_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.processId\test.component.inserters.processId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.processId\test.component.inserters.processId.cpp

$(TARG_TEST_COMP_ins_real_DEBUG):	\
			$(OBJ_TEST_COMP_ins_real_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_real_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_real_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.real\test.component.inserters.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.real\test.component.inserters.real.cpp

$(TARG_TEST_COMP_ins_real_RELEASE):	\
			$(OBJ_TEST_COMP_ins_real_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_real_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_real_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.real\test.component.inserters.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.real\test.component.inserters.real.cpp

$(TARG_TEST_COMP_ins_slice_DEBUG):	\
			$(OBJ_TEST_COMP_ins_slice_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_slice_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_slice_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.slice\test.component.inserters.slice.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.slice\test.component.inserters.slice.cpp

$(TARG_TEST_COMP_ins_slice_RELEASE):	\
			$(OBJ_TEST_COMP_ins_slice_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_slice_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_slice_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.slice\test.component.inserters.slice.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.slice\test.component.inserters.slice.cpp

$(TARG_TEST_COMP_ins_thrid_DEBUG):	\
			$(OBJ_TEST_COMP_ins_thrid_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_thrid_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_thrid_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.threadId\test.component.inserters.threadId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.threadId\test.component.inserters.threadId.cpp

$(TARG_TEST_COMP_ins_thrid_RELEASE):	\
			$(OBJ_TEST_COMP_ins_thrid_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_thrid_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_thrid_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.threadId\test.component.inserters.threadId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.threadId\test.component.inserters.threadId.cpp

$(TARG_TEST_COMP_ins_vbool_DEBUG):	\
			$(OBJ_TEST_COMP_ins_vbool_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_vbool_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_vbool_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.variant_bool\test.component.inserters.variant_bool.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.variant_bool\test.component.inserters.variant_bool.cpp

$(TARG_TEST_COMP_ins_vbool_RELEASE):	\
			$(OBJ_TEST_COMP_ins_vbool_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_vbool_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_vbool_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.variant_bool\test.component.inserters.variant_bool.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.variant_bool\test.component.inserters.variant_bool.cpp

$(TARG_TEST_COMP_ins_w2m_DEBUG):	\
			$(OBJ_TEST_COMP_ins_w2m_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_ins_w2m_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_ins_w2m_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.w2m\test.component.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.w2m\test.component.inserters.w2m.cpp

$(TARG_TEST_COMP_ins_w2m_RELEASE):	\
			$(OBJ_TEST_COMP_ins_w2m_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_ins_w2m_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_ins_w2m_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.w2m\test.component.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.inserters.w2m\test.component.inserters.w2m.cpp

$(TARG_TEST_COMP_log_1_DEBUG):	\
			$(OBJ_TEST_COMP_log_1_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_log_1_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_log_1_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.log.1\test.component.log.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.log.1\test.component.log.1.cpp

$(TARG_TEST_COMP_log_1_RELEASE):	\
			$(OBJ_TEST_COMP_log_1_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_log_1_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_log_1_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.log.1\test.component.log.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.log.1\test.component.log.1.cpp

$(TARG_TEST_COMP_pan_puts_DEBUG):	\
			$(OBJ_TEST_COMP_pan_puts_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_pan_puts_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_pan_puts_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logputs\test.component.core.pantheios_logputs.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logputs\test.component.core.pantheios_logputs.cpp

$(TARG_TEST_COMP_pan_puts_RELEASE):	\
			$(OBJ_TEST_COMP_pan_puts_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_pan_puts_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_pan_puts_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logputs\test.component.core.pantheios_logputs.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logputs\test.component.core.pantheios_logputs.cpp

$(TARG_TEST_COMP_pan_prtf_DEBUG):	\
			$(OBJ_TEST_COMP_pan_prtf_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_BE_TEST_DEBUG)\
			$(LIB_BEC_TEST_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_COMP_pan_prtf_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_BE_TEST_DEBUG)\
		$(LIB_BEC_TEST_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_COMP_pan_prtf_DEBUG):	$(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logprintf\test.component.core.pantheios_logprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logprintf\test.component.core.pantheios_logprintf.cpp

$(TARG_TEST_COMP_pan_prtf_RELEASE):	\
			$(OBJ_TEST_COMP_pan_prtf_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_BE_TEST_RELEASE)\
			$(LIB_BEC_TEST_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_COMP_pan_prtf_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_BE_TEST_RELEASE)\
		$(LIB_BEC_TEST_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_COMP_pan_prtf_RELEASE):	$(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logprintf\test.component.core.pantheios_logprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_COMPONENT_DIR)\test.component.core.pantheios_logprintf\test.component.core.pantheios_logprintf.cpp

##############################
# Performance Tests

$(TARG_TEST_PERF_ins_w2m_DEBUG):	\
			$(OBJ_TEST_PERF_ins_w2m_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_NULL_DEBUG)\
			$(LIB_BEC_NULL_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_PERF_ins_w2m_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_NULL_DEBUG)\
		$(LIB_BEC_NULL_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_PERF_ins_w2m_DEBUG):	$(PROJ_TEST_PERFORMANCE_DIR)\test.performance.inserters.w2m\test.performance.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_PERFORMANCE_DIR)\test.performance.inserters.w2m\test.performance.inserters.w2m.cpp

$(TARG_TEST_PERF_ins_w2m_RELEASE):	\
			$(OBJ_TEST_PERF_ins_w2m_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_NULL_RELEASE)\
			$(LIB_BEC_NULL_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_PERF_ins_w2m_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_NULL_RELEASE)\
		$(LIB_BEC_NULL_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_PERF_ins_w2m_RELEASE):	$(PROJ_TEST_PERFORMANCE_DIR)\test.performance.inserters.w2m\test.performance.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_PERFORMANCE_DIR)\test.performance.inserters.w2m\test.performance.inserters.w2m.cpp

##############################
# Scratch Tests

#  object files

$(OBJ_TEST_API_DEBUG):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.api\test.scratch.api.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.api\test.scratch.api.cpp
$(OBJ_TEST_API_RELEASE):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.api\test.scratch.api.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.api\test.scratch.api.cpp

# executables


$(TARG_TEST_DEBUG_APIwBE_FPRINTF):	\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_FPRINTF)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_DEBUG_APIwBE_COMERROBJ):		\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_COMERROBJ_DEBUG)\
			$(LIB_BEC_COMERROBJ_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_COMERROBJ_DEBUG)\
		$(LIB_BEC_COMERROBJ_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_COMERROBJ)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_DEBUG_APIwBE_WINEVLOG):	\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_WINEVLOG_DEBUG)\
			$(LIB_BEC_WINEVLOG_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_WINEVLOG_DEBUG)\
		$(LIB_BEC_WINEVLOG_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINEVLOG)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_DEBUG_APIwBE_W32CONS):	\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_W32CONS_DEBUG)\
			$(LIB_BEC_W32CONS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_W32CONS_DEBUG)\
		$(LIB_BEC_W32CONS_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_W32CONS)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_DEBUG_APIwBE_WINDBGR):	\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_WINDBGR_DEBUG)\
			$(LIB_BEC_WINDBGR_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_WINDBGR_DEBUG)\
		$(LIB_BEC_WINDBGR_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINDBGR)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_DEBUG_APIwBE_WINSYSLOG):		\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_WINSYSLOG_DEBUG)\
			$(LIB_BEC_WINSYSLOG_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_WINSYSLOG_DEBUG)\
		$(LIB_BEC_WINSYSLOG_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_DEBUG_APIwBELR_2_WINDBGRFPRINTF):	\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_LRSPLIT_DEBUG)\
			$(LIB_BEL_WINDBGR_DEBUG)\
			$(LIB_BEC_WINDBGR_DEBUG)\
			$(LIB_BER_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_LRSPLIT_DEBUG)\
		$(LIB_BEL_WINDBGR_DEBUG)\
		$(LIB_BEC_WINDBGR_DEBUG)\
		$(LIB_BER_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINDBGRFPRINTF)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_DEBUG_APIwBELR_3_WINDBGRWINSYSLOG):	\
			$(OBJ_TEST_API_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_LRSPLIT_DEBUG)\
			$(LIB_BEL_WINDBGR_DEBUG)\
			$(LIB_BEC_WINDBGR_DEBUG)\
			$(LIB_BER_WINSYSLOG_DEBUG)\
			$(LIB_BEC_WINSYSLOG_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_API_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_LRSPLIT_DEBUG)\
		$(LIB_BEL_WINDBGR_DEBUG)\
		$(LIB_BEC_WINDBGR_DEBUG)\
		$(LIB_BER_WINSYSLOG_DEBUG)\
		$(LIB_BEC_WINSYSLOG_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_WINDBGRWINSYSLOG)\
		$(TARGET_LIB_DEBUG)

$(TARG_TEST_FE_DEBUG): \
			$(OBJ_TEST_FE_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_FE_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_FE_DEBUG):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe\test.scratch.fe.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe\test.scratch.fe.cpp

$(TARG_TEST_BE_N_FAIL_DEBUG):	\
			$(OBJ_TEST_BE_N_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_N_DEBUG)\
			$(LIB_BE_N_DEBUG)\
			$(LIB_BEC_FAIL_DEBUG)\
			$(LIB_BEC_FILE_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_BE_N_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_N_DEBUG)\
		$(LIB_BE_N_DEBUG)\
		$(LIB_BEC_FAIL_DEBUG)\
		$(LIB_BEC_FILE_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_BE_N_DEBUG):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.be.N.fail\test.scratch.be.N.fail.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.be.N.fail\test.scratch.be.N.fail.cpp


$(TARG_TEST_FE_SIMPLE_WC_DEBUG):	\
			$(OBJ_TEST_FE_SIMPLE_WC_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_SIMPLE_WC_DEBUG)\
			$(LIB_BE_FPRINTF_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_FE_SIMPLE_WC_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_FE_SIMPLE_WC_DEBUG)\
		$(LIB_BE_FPRINTF_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_FE_SIMPLE_WC_DEBUG):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe.simple.WithCallback\test.scratch.fe.simple.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe.simple.WithCallback\test.scratch.fe.simple.WithCallback.cpp



$(TARG_TEST_RELEASE_APIwBE_FPRINTF):	\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_FPRINTF)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_RELEASE_APIwBE_COMERROBJ):		\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_COMERROBJ_RELEASE)\
			$(LIB_BEC_COMERROBJ_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_COMERROBJ_RELEASE)\
		$(LIB_BEC_COMERROBJ_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_COMERROBJ)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_RELEASE_APIwBE_WINEVLOG):	\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_WINEVLOG_RELEASE)\
			$(LIB_BEC_WINEVLOG_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_WINEVLOG_RELEASE)\
		$(LIB_BEC_WINEVLOG_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINEVLOG)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_RELEASE_APIwBE_W32CONS):	\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_W32CONS_RELEASE)\
			$(LIB_BEC_W32CONS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_W32CONS_RELEASE)\
		$(LIB_BEC_W32CONS_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_W32CONS)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_RELEASE_APIwBE_WINDBGR):	\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_WINDBGR_RELEASE)\
			$(LIB_BEC_WINDBGR_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_WINDBGR_RELEASE)\
		$(LIB_BEC_WINDBGR_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINDBGR)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_RELEASE_APIwBE_WINSYSLOG):		\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_WINSYSLOG_RELEASE)\
			$(LIB_BEC_WINSYSLOG_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_WINSYSLOG_RELEASE)\
		$(LIB_BEC_WINSYSLOG_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINSYSLOG)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_RELEASE_APIwBELR_2_WINDBGRFPRINTF):	\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_LRSPLIT_RELEASE)\
			$(LIB_BEL_WINDBGR_RELEASE)\
			$(LIB_BEC_WINDBGR_RELEASE)\
			$(LIB_BER_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_LRSPLIT_RELEASE)\
		$(LIB_BEL_WINDBGR_RELEASE)\
		$(LIB_BEC_WINDBGR_RELEASE)\
		$(LIB_BER_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINDBGRFPRINTF)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_RELEASE_APIwBELR_3_WINDBGRWINSYSLOG):	\
			$(OBJ_TEST_API_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_LRSPLIT_RELEASE)\
			$(LIB_BEL_WINDBGR_RELEASE)\
			$(LIB_BEC_WINDBGR_RELEASE)\
			$(LIB_BER_WINSYSLOG_RELEASE)\
			$(LIB_BEC_WINSYSLOG_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_API_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_LRSPLIT_RELEASE)\
		$(LIB_BEL_WINDBGR_RELEASE)\
		$(LIB_BEC_WINDBGR_RELEASE)\
		$(LIB_BER_WINSYSLOG_RELEASE)\
		$(LIB_BEC_WINSYSLOG_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_WINDBGRWINSYSLOG)\
		$(TARGET_LIB_RELEASE)

$(TARG_TEST_FE_RELEASE): \
			$(OBJ_TEST_FE_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_FE_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_FE_RELEASE):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe\test.scratch.fe.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe\test.scratch.fe.cpp

$(TARG_TEST_BE_N_FAIL_RELEASE):	\
			$(OBJ_TEST_BE_N_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_N_RELEASE)\
			$(LIB_BE_N_RELEASE)\
			$(LIB_BEC_FAIL_RELEASE)\
			$(LIB_BEC_FILE_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_BE_N_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_N_RELEASE)\
		$(LIB_BE_N_RELEASE)\
		$(LIB_BEC_FAIL_RELEASE)\
		$(LIB_BEC_FILE_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_BE_N_RELEASE):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.be.N.fail\test.scratch.be.N.fail.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.be.N.fail\test.scratch.be.N.fail.cpp


$(TARG_TEST_FE_SIMPLE_WC_RELEASE):	\
			$(OBJ_TEST_FE_SIMPLE_WC_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_SIMPLE_WC_RELEASE)\
			$(LIB_BE_FPRINTF_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_FE_SIMPLE_WC_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_FE_SIMPLE_WC_RELEASE)\
		$(LIB_BE_FPRINTF_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_FE_SIMPLE_WC_RELEASE):	$(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe.simple.WithCallback\test.scratch.fe.simple.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_SCRATCH_DIR)\test.scratch.fe.simple.WithCallback\test.scratch.fe.simple.WithCallback.cpp


##############################
# Unit Tests

$(TARG_TEST_UNIT_lvls_dyninit_DEBUG):	\
			$(OBJ_TEST_UNIT_lvls_dyninit_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_lvls_dyninit_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_lvls_dyninit_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.levels.dynamic_initialisation\test.unit.levels.dynamic_initialisation.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.levels.dynamic_initialisation\test.unit.levels.dynamic_initialisation.cpp

$(TARG_TEST_UNIT_lvls_dyninit_RELEASE):	\
			$(OBJ_TEST_UNIT_lvls_dyninit_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_lvls_dyninit_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_lvls_dyninit_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.levels.dynamic_initialisation\test.unit.levels.dynamic_initialisation.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.levels.dynamic_initialisation\test.unit.levels.dynamic_initialisation.cpp

$(TARG_TEST_UNIT_lvls_vals_DEBUG):	\
			$(OBJ_TEST_UNIT_lvls_vals_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_lvls_vals_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_lvls_vals_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.levels.values\test.unit.levels.values.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.levels.values\test.unit.levels.values.c

$(TARG_TEST_UNIT_lvls_vals_RELEASE):	\
			$(OBJ_TEST_UNIT_lvls_vals_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_lvls_vals_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_lvls_vals_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.levels.values\test.unit.levels.values.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.levels.values\test.unit.levels.values.c

$(TARG_TEST_UNIT_getcurrtm_DEBUG):	\
			$(OBJ_TEST_UNIT_getcurrtm_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_getcurrtm_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_getcurrtm_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.getcurrenttime\test.unit.util.getcurrenttime.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.getcurrenttime\test.unit.util.getcurrenttime.cpp

$(TARG_TEST_UNIT_getcurrtm_RELEASE):	\
			$(OBJ_TEST_UNIT_getcurrtm_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_getcurrtm_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_getcurrtm_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.getcurrenttime\test.unit.util.getcurrenttime.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.getcurrenttime\test.unit.util.getcurrenttime.cpp

$(TARG_TEST_UNIT_getversion_DEBUG):	\
			$(OBJ_TEST_UNIT_getversion_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_FAIL_DEBUG)\
			$(LIB_BE_FAIL_DEBUG)\
			$(LIB_BEC_FAIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_getversion_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_FE_FAIL_DEBUG)\
		$(LIB_BE_FAIL_DEBUG)\
		$(LIB_BEC_FAIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_getversion_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.getversion\test.unit.getversion.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.getversion\test.unit.getversion.c

$(TARG_TEST_UNIT_getversion_RELEASE):	\
			$(OBJ_TEST_UNIT_getversion_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_FAIL_RELEASE)\
			$(LIB_BE_FAIL_RELEASE)\
			$(LIB_BEC_FAIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_getversion_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_FE_FAIL_RELEASE)\
		$(LIB_BE_FAIL_RELEASE)\
		$(LIB_BEC_FAIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_getversion_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.getversion\test.unit.getversion.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.getversion\test.unit.getversion.c

$(TARG_TEST_UNIT_gethostname_DEBUG):	\
			$(OBJ_TEST_UNIT_gethostname_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_gethostname_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_gethostname_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.gethostname\test.unit.util.gethostname.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.gethostname\test.unit.util.gethostname.cpp

$(TARG_TEST_UNIT_gethostname_RELEASE):	\
			$(OBJ_TEST_UNIT_gethostname_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_gethostname_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_gethostname_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.gethostname\test.unit.util.gethostname.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.gethostname\test.unit.util.gethostname.cpp

$(TARG_TEST_UNIT_onbailout_DEBUG):	\
			$(OBJ_TEST_UNIT_onbailout_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_onbailout_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_onbailout_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.onbailout\test.unit.util.onbailout.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.onbailout\test.unit.util.onbailout.cpp

$(TARG_TEST_UNIT_onbailout_RELEASE):	\
			$(OBJ_TEST_UNIT_onbailout_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_onbailout_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_onbailout_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.onbailout\test.unit.util.onbailout.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.onbailout\test.unit.util.onbailout.cpp

$(TARG_TEST_UNIT_strnlen_DEBUG):	\
			$(OBJ_TEST_UNIT_strnlen_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_strnlen_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_strnlen_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.strnlen\test.unit.util.strnlen.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.strnlen\test.unit.util.strnlen.cpp

$(TARG_TEST_UNIT_strnlen_RELEASE):	\
			$(OBJ_TEST_UNIT_strnlen_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_strnlen_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_strnlen_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.util.strnlen\test.unit.util.strnlen.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.util.strnlen\test.unit.util.strnlen.cpp

$(TARG_TEST_UNIT_bec_CEO_DEBUG):	\
			$(OBJ_TEST_UNIT_bec_CEO_DEBUG)\
			$(LIB_BEC_COMERROBJ_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_bec_CEO_DEBUG)\
		$(LIB_BEC_COMERROBJ_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_bec_CEO_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.COMErrorObject\test.unit.bec.COMErrorObject.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.COMErrorObject\test.unit.bec.COMErrorObject.cpp

$(TARG_TEST_UNIT_bec_CEO_RELEASE):	\
			$(OBJ_TEST_UNIT_bec_CEO_RELEASE)\
			$(LIB_BEC_COMERROBJ_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_bec_CEO_RELEASE)\
		$(LIB_BEC_COMERROBJ_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_bec_CEO_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.COMErrorObject\test.unit.bec.COMErrorObject.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.COMErrorObject\test.unit.bec.COMErrorObject.cpp

$(TARG_TEST_UNIT_bec_file_DEBUG):	\
			$(OBJ_TEST_UNIT_bec_file_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_BEC_FILE_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_NULL_DEBUG)\
			$(LIB_BEC_NULL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_bec_file_DEBUG)\
		$(LIB_BEC_FILE_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_NULL_DEBUG)\
		$(LIB_BEC_NULL_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_bec_file_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.file\test.unit.bec.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.file\test.unit.bec.file.cpp

$(TARG_TEST_UNIT_bec_file_RELEASE):	\
			$(OBJ_TEST_UNIT_bec_file_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_BEC_FILE_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_NULL_RELEASE)\
			$(LIB_BEC_NULL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_bec_file_RELEASE)\
		$(LIB_BEC_FILE_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_NULL_RELEASE)\
		$(LIB_BEC_NULL_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_bec_file_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.file\test.unit.bec.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.file\test.unit.bec.file.cpp

$(TARG_TEST_UNIT_bec_fprintf_DEBUG):	\
			$(OBJ_TEST_UNIT_bec_fprintf_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_BEC_FPRINTF_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_BE_NULL_DEBUG)\
			$(LIB_BEC_NULL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_bec_fprintf_DEBUG)\
		$(LIB_BEC_FPRINTF_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_BE_NULL_DEBUG)\
		$(LIB_BEC_NULL_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_bec_fprintf_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.fprintf\test.unit.bec.fprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.fprintf\test.unit.bec.fprintf.cpp

$(TARG_TEST_UNIT_bec_fprintf_RELEASE):	\
			$(OBJ_TEST_UNIT_bec_fprintf_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_BEC_FPRINTF_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_BE_NULL_RELEASE)\
			$(LIB_BEC_NULL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_bec_fprintf_RELEASE)\
		$(LIB_BEC_FPRINTF_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_BE_NULL_RELEASE)\
		$(LIB_BEC_NULL_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_bec_fprintf_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.fprintf\test.unit.bec.fprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.fprintf\test.unit.bec.fprintf.cpp

$(TARG_TEST_UNIT_bec_fail_DEBUG):	\
			$(OBJ_TEST_UNIT_bec_fail_DEBUG)\
			$(LIB_BEC_FAIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_bec_fail_DEBUG)\
		$(LIB_BEC_FAIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_bec_fail_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.fail\test.unit.bec.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.fail\test.unit.bec.fail.c

$(TARG_TEST_UNIT_bec_fail_RELEASE):	\
			$(OBJ_TEST_UNIT_bec_fail_RELEASE)\
			$(LIB_BEC_FAIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_bec_fail_RELEASE)\
		$(LIB_BEC_FAIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_bec_fail_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.bec.fail\test.unit.bec.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.bec.fail\test.unit.bec.fail.c

$(TARG_TEST_UNIT_be_fail_DEBUG):	\
			$(OBJ_TEST_UNIT_be_fail_DEBUG)\
			$(LIB_BE_FAIL_DEBUG)\
			$(LIB_BEC_FAIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_be_fail_DEBUG)\
		$(LIB_BE_FAIL_DEBUG)\
		$(LIB_BEC_FAIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_be_fail_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.be.fail\test.unit.be.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.be.fail\test.unit.be.fail.c

$(TARG_TEST_UNIT_be_fail_RELEASE):	\
			$(OBJ_TEST_UNIT_be_fail_RELEASE)\
			$(LIB_BE_FAIL_RELEASE)\
			$(LIB_BEC_FAIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_be_fail_RELEASE)\
		$(LIB_BE_FAIL_RELEASE)\
		$(LIB_BEC_FAIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_be_fail_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.be.fail\test.unit.be.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.be.fail\test.unit.be.fail.c

#
$(TARG_TEST_UNIT_be_lrsplit_DEBUG):	\
			$(OBJ_TEST_UNIT_be_lrsplit_DEBUG)\
			$(LIB_BE_LRSPLIT_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_be_lrsplit_DEBUG)\
		$(LIB_BE_LRSPLIT_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_be_lrsplit_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.be.lrsplit\test.unit.be.lrsplit.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.be.lrsplit\test.unit.be.lrsplit.c

$(TARG_TEST_UNIT_be_lrsplit_RELEASE):	\
			$(OBJ_TEST_UNIT_be_lrsplit_RELEASE)\
			$(LIB_BE_LRSPLIT_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_be_lrsplit_RELEASE)\
		$(LIB_BE_LRSPLIT_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_be_lrsplit_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.be.lrsplit\test.unit.be.lrsplit.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.be.lrsplit\test.unit.be.lrsplit.c

$(TARG_TEST_UNIT_be_N_DEBUG):	\
			$(OBJ_TEST_UNIT_be_N_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_BE_N_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_be_N_DEBUG)\
		$(LIB_BE_N_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_be_N_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.be.N\test.unit.be.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.be.N\test.unit.be.N.c

$(TARG_TEST_UNIT_be_N_RELEASE):	\
			$(OBJ_TEST_UNIT_be_N_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_BE_N_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_be_N_RELEASE)\
		$(LIB_BE_N_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_be_N_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.be.N\test.unit.be.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.be.N\test.unit.be.N.c

$(TARG_TEST_UNIT_fe_all_DEBUG):	\
			$(OBJ_TEST_UNIT_fe_all_DEBUG)\
			$(LIB_FE_ALL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_fe_all_DEBUG)\
		$(LIB_FE_ALL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_fe_all_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.all\test.unit.fe.all.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.all\test.unit.fe.all.c

$(TARG_TEST_UNIT_fe_all_RELEASE):	\
			$(OBJ_TEST_UNIT_fe_all_RELEASE)\
			$(LIB_FE_ALL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_fe_all_RELEASE)\
		$(LIB_FE_ALL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_fe_all_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.all\test.unit.fe.all.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.all\test.unit.fe.all.c

$(TARG_TEST_UNIT_fe_fail_DEBUG):	\
			$(OBJ_TEST_UNIT_fe_fail_DEBUG)\
			$(LIB_FE_FAIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_fe_fail_DEBUG)\
		$(LIB_FE_FAIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_fe_fail_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.fail\test.unit.fe.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.fail\test.unit.fe.fail.c

$(TARG_TEST_UNIT_fe_fail_RELEASE):	\
			$(OBJ_TEST_UNIT_fe_fail_RELEASE)\
			$(LIB_FE_FAIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_fe_fail_RELEASE)\
		$(LIB_FE_FAIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_fe_fail_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.fail\test.unit.fe.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.fail\test.unit.fe.fail.c

$(TARG_TEST_UNIT_fe_N_DEBUG):	\
			$(OBJ_TEST_UNIT_fe_N_DEBUG)\
			$(LIB_FE_N_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_fe_N_DEBUG)\
		$(LIB_FE_N_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_fe_N_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.N\test.unit.fe.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.N\test.unit.fe.N.c

$(TARG_TEST_UNIT_fe_N_RELEASE):	\
			$(OBJ_TEST_UNIT_fe_N_RELEASE)\
			$(LIB_FE_N_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_fe_N_RELEASE)\
		$(LIB_FE_N_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_fe_N_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.N\test.unit.fe.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.N\test.unit.fe.N.c

$(TARG_TEST_UNIT_fe_null_DEBUG):	\
			$(OBJ_TEST_UNIT_fe_null_DEBUG)\
			$(LIB_FE_NULL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_fe_null_DEBUG)\
		$(LIB_FE_NULL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_fe_null_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.null\test.unit.fe.null.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.null\test.unit.fe.null.c

$(TARG_TEST_UNIT_fe_null_RELEASE):	\
			$(OBJ_TEST_UNIT_fe_null_RELEASE)\
			$(LIB_FE_NULL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_fe_null_RELEASE)\
		$(LIB_FE_NULL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_fe_null_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.null\test.unit.fe.null.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.null\test.unit.fe.null.c

$(TARG_TEST_UNIT_fe_simple_DEBUG):	\
			$(OBJ_TEST_UNIT_fe_simple_DEBUG)\
			$(LIB_FE_SIMPLE_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_fe_simple_DEBUG)\
		$(LIB_FE_SIMPLE_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_fe_simple_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.simple\test.unit.fe.simple.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_DEBUG) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.simple\test.unit.fe.simple.c

$(TARG_TEST_UNIT_fe_simple_RELEASE):	\
			$(OBJ_TEST_UNIT_fe_simple_RELEASE)\
			$(LIB_FE_SIMPLE_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_fe_simple_RELEASE)\
		$(LIB_FE_SIMPLE_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_fe_simple_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.simple\test.unit.fe.simple.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_RELEASE) $(CC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.simple\test.unit.fe.simple.c

$(TARG_TEST_UNIT_fe_WinReg_DEBUG):	\
			$(OBJ_TEST_UNIT_fe_WinReg_DEBUG)\
			$(LIB_FE_WINREG_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(LIB_SHWILD_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_fe_WinReg_DEBUG)\
		$(LIB_FE_WINREG_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIB_SHWILD_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_fe_WinReg_DEBUG):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.WindowsRegistry\test.unit.fe.WindowsRegistry.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.WindowsRegistry\test.unit.fe.WindowsRegistry.cpp

$(TARG_TEST_UNIT_fe_WinReg_RELEASE):	\
			$(OBJ_TEST_UNIT_fe_WinReg_RELEASE)\
			$(LIB_FE_WINREG_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(LIB_SHWILD_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_fe_WinReg_RELEASE)\
		$(LIB_FE_WINREG_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIB_SHWILD_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_fe_WinReg_RELEASE):	$(PROJ_TEST_UNIT_DIR)\test.unit.fe.WindowsRegistry\test.unit.fe.WindowsRegistry.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\test.unit.fe.WindowsRegistry\test.unit.fe.WindowsRegistry.cpp

$(TARG_TEST_UNIT_ins_int_DEBUG):	\
			$(OBJ_TEST_UNIT_ins_int_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_ins_int_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_ins_int_DEBUG):	$(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.integer\test.unit.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.integer\test.unit.inserter.integer.cpp

$(TARG_TEST_UNIT_ins_int_RELEASE):	\
			$(OBJ_TEST_UNIT_ins_int_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_ins_int_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_ins_int_RELEASE):	$(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.integer\test.unit.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.integer\test.unit.inserter.integer.cpp

$(TARG_TEST_UNIT_ins_w2m_DEBUG):	\
			$(OBJ_TEST_UNIT_ins_w2m_DEBUG)\
			$(LIB_CORE_DEBUG)\
			$(LIB_UTIL_DEBUG)\
			$(LIB_FE_NULL_DEBUG)\
			$(LIB_BE_NULL_DEBUG)\
			$(LIB_BEC_NULL_DEBUG)\
			$(LIB_XTESTS_DEBUG)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_DEBUG) $@ \
		$(OBJ_TEST_UNIT_ins_w2m_DEBUG)\
		$(LIB_CORE_DEBUG)\
		$(LIB_UTIL_DEBUG)\
		$(LIB_FE_NULL_DEBUG)\
		$(LIB_BE_NULL_DEBUG)\
		$(LIB_BEC_NULL_DEBUG)\
		$(LIB_XTESTS_DEBUG)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_DEBUG)

$(OBJ_TEST_UNIT_ins_w2m_DEBUG):	$(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.w2m\test.unit.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_DEBUG) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.w2m\test.unit.inserter.w2m.cpp

$(TARG_TEST_UNIT_ins_w2m_RELEASE):	\
			$(OBJ_TEST_UNIT_ins_w2m_RELEASE)\
			$(LIB_CORE_RELEASE)\
			$(LIB_UTIL_RELEASE)\
			$(LIB_FE_NULL_RELEASE)\
			$(LIB_BE_NULL_RELEASE)\
			$(LIB_BEC_NULL_RELEASE)\
			$(LIB_XTESTS_RELEASE)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_RELEASE) $@ \
		$(OBJ_TEST_UNIT_ins_w2m_RELEASE)\
		$(LIB_CORE_RELEASE)\
		$(LIB_UTIL_RELEASE)\
		$(LIB_FE_NULL_RELEASE)\
		$(LIB_BE_NULL_RELEASE)\
		$(LIB_BEC_NULL_RELEASE)\
		$(LIB_XTESTS_RELEASE)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_RELEASE)

$(OBJ_TEST_UNIT_ins_w2m_RELEASE):	$(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.w2m\test.unit.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_RELEASE) $(CPPC_INCLUDES) --output-path=$@ $(PROJ_TEST_UNIT_DIR)\inserters\test.unit.inserter.w2m\test.unit.inserter.w2m.cpp

############################### end of file ################################
