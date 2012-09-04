############################################################################
# File: 	makefile
#
# Purpose:	Makefile for the Pantheios project, for the GCC 3.2
#			compiler, on UNIX
#
# Created:	15th Feburary 2005
# Updated:	31st July 2012
#
############################################################################


############################################################################
# Environment check
#

ifndef STLSOFT
$(error STLSOFT must be defined)
endif # !STLSOFT

############################################################################
# Symbols

########################################
# Project & compiler version

COMP_TAG			=	gcc32
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

PROJ_BASE_DIR			=	../..
PROJ_BIN_DIR			=	$(PROJ_BASE_DIR)/bin
PROJ_INCLUDE_DIR		=	$(PROJ_BASE_DIR)/include
PROJ_LIB_DIR			=	$(PROJ_BASE_DIR)/lib
PROJ_OBJ_DIR			=	.
PROJ_SRC_DIR			=	$(PROJ_BASE_DIR)/src
PROJ_BACKENDS_DIR		=	$(PROJ_BASE_DIR)/backends
PROJ_FRONTENDS_DIR		=	$(PROJ_BASE_DIR)/frontends
PROJ_EXAMPLES_DIR		=	$(PROJ_BASE_DIR)/examples
PROJ_SAMPLES_DIR        =   $(PROJ_BASE_DIR)/samples
PROJ_TEST_ROOT_DIR		=	$(PROJ_BASE_DIR)/test
PROJ_TEST_COMPONENT_DIR	=	$(PROJ_TEST_ROOT_DIR)/component
PROJ_TEST_PERFORMANCE_DIR	=	$(PROJ_TEST_ROOT_DIR)/performance
PROJ_TEST_SCRATCH_DIR	=	$(PROJ_TEST_ROOT_DIR)/scratch
PROJ_TEST_UNIT_DIR		=	$(PROJ_TEST_ROOT_DIR)/unit

B64_INCLUDE_DIR			=   $(PROJ_BASE_DIR)/include
B64_SRC_DIR				=   $(PROJ_SRC_DIR)/b64

SHWILD_INCLUDE_DIR      =   $(PROJ_BASE_DIR)/include
SHWILD_SRC_DIR          =   $(PROJ_SRC_DIR)/shwild

XTESTS_INCLUDE_DIR      =   $(PROJ_BASE_DIR)/include
XTESTS_SRC_DIR          =   $(PROJ_SRC_DIR)/xtests

########################################
# tools

CC                      =   gcc
CPPC                    =   g++
AR                      =   ar
LD                      =   g++
RM                      =   rm -f
CP                      =   cp
RC                      =   

# Includes

INCLUDES_LEADER 	=	

PROJ_INCLUDE		=	-I$(PROJ_INCLUDE_DIR)

ENV_INCLUDES 	=

COMMON_INCLUDES		=

CC_INCLUDES_BASE=\
$(INCLUDES_LEADER)\
$(PROJ_INCLUDE)\
-I"$(STLSOFT)/include"\
$(COMMON_INCLUDES)\
$(ENV_INCLUDES)\
 \

CC_INCLUDES=\
$(CC_INCLUDES_BASE)\
 \


ifdef ACE
CC_INCLUDES_ACE=\
$(CC_INCLUDES_BASE)\
-I"$(ACE)"\
 \

endif # ACE

CPPC_INCLUDES_BASE=\
$(INCLUDES_LEADER)\
$(PROJ_INCLUDE)\
-I"$(STLSOFT)/include"\
$(COMMON_INCLUDES)\
$(ENV_INCLUDES)\
 \

CPPC_INCLUDES=\
$(CPPC_INCLUDES_BASE)\
 \

ifdef ACE
CPPC_INCLUDES_ACE=\
$(CPPC_INCLUDES_BASE)\
-I"$(ACE)"\
 \

endif # ACE


# b64

CC_B64_INCLUDES_BASE =\
$(CC_INCLUDES_BASE)\
 \

CC_B64_INCLUDES_MT_D =\
$(CC_B64_INCLUDES_BASE)\
 \

CC_B64_INCLUDES_MT_R =\
$(CC_B64_INCLUDES_BASE)\
 \

CC_B64_INCLUDES_ST_D =\
$(CC_B64_INCLUDES_BASE)\
 \

CC_B64_INCLUDES_ST_R =\
$(CC_B64_INCLUDES_BASE)\
 \


CPPC_B64_INCLUDES_BASE =\
$(CPPC_INCLUDES_BASE)\
 \

CPPC_B64_INCLUDES_MT_D =\
$(CPPC_B64_INCLUDES_BASE)\
 \

CPPC_B64_INCLUDES_MT_R =\
$(CPPC_B64_INCLUDES_BASE)\
 \

CPPC_B64_INCLUDES_ST_D =\
$(CPPC_B64_INCLUDES_BASE)\
 \

CPPC_B64_INCLUDES_ST_R =\
$(CPPC_B64_INCLUDES_BASE)\
 \


# shwild

CPPC_SHWILD_INCLUDES_BASE =\
$(CPPC_INCLUDES_BASE)\
 \

CPPC_SHWILD_INCLUDES_MT_D =\
$(CPPC_SHWILD_INCLUDES_BASE)\
 \

CPPC_SHWILD_INCLUDES_MT_R =\
$(CPPC_SHWILD_INCLUDES_BASE)\
 \

CPPC_SHWILD_INCLUDES_ST_D =\
$(CPPC_SHWILD_INCLUDES_BASE)\
 \

CPPC_SHWILD_INCLUDES_ST_R =\
$(CPPC_SHWILD_INCLUDES_BASE)\
 \


# xTests

CPPC_XTESTS_INCLUDES_BASE =\
$(CPPC_INCLUDES_BASE)\
 \

CPPC_XTESTS_INCLUDES_MT_D =\
$(CPPC_XTESTS_INCLUDES_BASE)\
 \

CPPC_XTESTS_INCLUDES_MT_R =\
$(CPPC_XTESTS_INCLUDES_BASE)\
 \

CPPC_XTESTS_INCLUDES_ST_D =\
$(CPPC_XTESTS_INCLUDES_BASE)\
 \

CPPC_XTESTS_INCLUDES_ST_R =\
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

TARGET_LIB_MT_D	= -lpthread
TARGET_LIB_MT_R	= -lpthread
TARGET_LIB_ST_D	= 
TARGET_LIB_ST_R	= 

# #defines
#
# - OS
# - 

OS_DEFS				=	-Dunix

COMMON_DEFS 		=

CUSTOM_DEFS			=

CC_CUSTOM_DEFS		=	$(CUSTOM_DEFS)

CPPC_CUSTOM_DEFS	=	$(CUSTOM_DEFS)

CC_DEFS 			=	$(COMMON_DEFS) $(OS_DEFS) $(CC_CUSTOM_DEFS)

CPPC_DEFS			=	$(COMMON_DEFS) $(OS_DEFS) $(CPPC_CUSTOM_DEFS)

# CC flags

CC_F_NOLOGO 			=	
CC_F_COMPILE_ONLY		=	-c
CC_F_WARNING_MAX		=	-Wall -pedantic -Wno-long-long -Wno-unused-value -Wno-unused-function -Wundef -Wshadow
CC_F_WARNING_ERR		=	-Werror
CC_F_WARNINGS_LIMIT		=	
CC_F_MINIMUM_SIZE		=	-O1
#CC_F_MAXIMUM_SPEED		=	-O3
CC_APP_F_WARNING_MAX	=	-Wall           -Wno-long-long -Wno-unused-value -Wno-unused-function -Wundef -Wshadow
CC_APP_F_WARNING_ERR	=	$(CC_F_WARNING_ERR)
CC_F_ADDITIONAL			=	

# CPPC flags

CPPC_F_NOLOGO			=	
CPPC_F_COMPILE_ONLY 	=	-c
CPPC_F_WARNING_MAX		=	-Wall -pedantic -Wno-long-long -Wno-unused-value -Wno-unused-function
CPPC_F_WARNING_ERR		=	-Werror
CPPC_F_EXCEPTIONS_ON	=	
CPPC_F_EXCEPTIONS_OFF	=	
CPPC_F_EXCEPTIONS		=	$(CPPC_F_EXCEPTIONS_ON)
CPPC_F_RTTI_ON			=	
CPPC_F_RTTI_OFF 		=	
CPPC_F_RTTI				=	$(CPPC_F_RTTI_ON)
CPPC_F_WARNINGS_LIMIT	=	
CPPC_F_MINIMUM_SIZE 	=	-O1
CPPC_F_MAXIMUM_SPEED	=	-O3
CPPC_F_OPTIMISED		=	$(CPPC_F_MAXIMUM_SPEED)
CPPC_APP_F_WARNING_MAX	=	-Wall           -Wno-long-long -Wno-unused-value -Wno-unused-function
CPPC_APP_F_WARNING_ERR	=	$(CPPC_F_WARNING_ERR)
CPPC_F_ADDITIONAL		=	



CC_TOOL_FLAGS		=

CPPC_TOOL_FLAGS 	=



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
CC_LIB_ARGS_MT_D			=	$(CC_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CC_LIB_ARGS_MT_R			=	$(CC_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CC_LIB_ARGS_ST_D			=	$(CC_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CC_LIB_ARGS_ST_R			=	$(CC_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS) -DPANTHEIOS_NO_AUTO_INIT

CPPC_LIB_ARGS_MT_D			=	$(CPPC_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CPPC_LIB_ARGS_MT_R			=	$(CPPC_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CPPC_LIB_ARGS_ST_D			=	$(CPPC_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS) -DPANTHEIOS_NO_AUTO_INIT
CPPC_LIB_ARGS_ST_R			=	$(CPPC_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS) -DPANTHEIOS_NO_AUTO_INIT


# - b64
CC_B64_ARGS_MT_D			=	$(CC_B64_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_B64_ARGS_MT_R			=	$(CC_B64_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS)
CC_B64_ARGS_ST_D			=	$(CC_B64_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_B64_ARGS_ST_R			=	$(CC_B64_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS)

CPPC_B64_ARGS_MT_D			=	$(CPPC_B64_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_B64_ARGS_MT_R			=	$(CPPC_B64_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS)
CPPC_B64_ARGS_ST_D			=	$(CPPC_B64_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_B64_ARGS_ST_R			=	$(CPPC_B64_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS)


# - shwild
CC_SHWILD_ARGS_MT_D			=	$(CC_SHWILD_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_SHWILD_ARGS_MT_R			=	$(CC_SHWILD_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS)
CC_SHWILD_ARGS_ST_D			=	$(CC_SHWILD_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_SHWILD_ARGS_ST_R			=	$(CC_SHWILD_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS)

CPPC_SHWILD_ARGS_MT_D		=	$(CPPC_SHWILD_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_SHWILD_ARGS_MT_R		=	$(CPPC_SHWILD_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS)
CPPC_SHWILD_ARGS_ST_D		=	$(CPPC_SHWILD_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_SHWILD_ARGS_ST_R		=	$(CPPC_SHWILD_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS)


# - xTests
CC_XTESTS_ARGS_MT_D			=	$(CC_XTESTS_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_XTESTS_ARGS_MT_R			=	$(CC_XTESTS_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS)
CC_XTESTS_ARGS_ST_D			=	$(CC_XTESTS_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_XTESTS_ARGS_ST_R			=	$(CC_XTESTS_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS)

CPPC_XTESTS_ARGS_MT_D			=	$(CPPC_XTESTS_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_XTESTS_ARGS_MT_R			=	$(CPPC_XTESTS_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS)
CPPC_XTESTS_ARGS_ST_D			=	$(CPPC_XTESTS_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_XTESTS_ARGS_ST_R			=	$(CPPC_XTESTS_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS)


# - Examples
CC_EXAMPLE_ARGS_MT_D		=	$(CC_EXAMPLE_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_EXAMPLE_ARGS_MT_R		=	$(CC_EXAMPLE_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS)
CC_EXAMPLE_ARGS_ST_D		=	$(CC_EXAMPLE_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_EXAMPLE_ARGS_ST_R		=	$(CC_EXAMPLE_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS)

CPPC_EXAMPLE_ARGS_MT_D		=	$(CPPC_EXAMPLE_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_EXAMPLE_ARGS_MT_R		=	$(CPPC_EXAMPLE_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS)
CPPC_EXAMPLE_ARGS_ST_D		=	$(CPPC_EXAMPLE_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_EXAMPLE_ARGS_ST_R		=	$(CPPC_EXAMPLE_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS)


# - Test (Component)
CC_TEST_COMPONENT_ARGS_MT_D			=	$(CC_TEST_COMPONENT_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_COMPONENT_ARGS_MT_R			=	$(CC_TEST_COMPONENT_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS)
CC_TEST_COMPONENT_ARGS_ST_D			=	$(CC_TEST_COMPONENT_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_COMPONENT_ARGS_ST_R			=	$(CC_TEST_COMPONENT_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS)

CPPC_TEST_COMPONENT_ARGS_MT_D			=	$(CPPC_TEST_COMPONENT_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_COMPONENT_ARGS_MT_R			=	$(CPPC_TEST_COMPONENT_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS)
CPPC_TEST_COMPONENT_ARGS_ST_D			=	$(CPPC_TEST_COMPONENT_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_COMPONENT_ARGS_ST_R			=	$(CPPC_TEST_COMPONENT_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS)


# - Test (Scratch)
CC_TEST_SCRATCH_ARGS_MT_D			=	$(CC_TEST_SCRATCH_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_SCRATCH_ARGS_MT_R			=	$(CC_TEST_SCRATCH_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS)
CC_TEST_SCRATCH_ARGS_ST_D			=	$(CC_TEST_SCRATCH_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_SCRATCH_ARGS_ST_R			=	$(CC_TEST_SCRATCH_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS)

CPPC_TEST_SCRATCH_ARGS_MT_D			=	$(CPPC_TEST_SCRATCH_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_SCRATCH_ARGS_MT_R			=	$(CPPC_TEST_SCRATCH_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS)
CPPC_TEST_SCRATCH_ARGS_ST_D			=	$(CPPC_TEST_SCRATCH_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_SCRATCH_ARGS_ST_R			=	$(CPPC_TEST_SCRATCH_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS)


# - Test (Unit)
CC_TEST_UNIT_ARGS_MT_D			=	$(CC_TEST_UNIT_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_UNIT_ARGS_MT_R			=	$(CC_TEST_UNIT_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CC_DEFS)
CC_TEST_UNIT_ARGS_ST_D			=	$(CC_TEST_UNIT_FLAGS) -D_DEBUG -UNDEBUG  $(CC_DEFS)
CC_TEST_UNIT_ARGS_ST_R			=	$(CC_TEST_UNIT_FLAGS) -DNDEBUG -U_DEBUG  $(CC_DEFS)

CPPC_TEST_UNIT_ARGS_MT_D			=	$(CPPC_TEST_UNIT_FLAGS) -D_REENTRANT -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_UNIT_ARGS_MT_R			=	$(CPPC_TEST_UNIT_FLAGS) -D_REENTRANT -U_DEBUG -DNDEBUG  $(CPPC_DEFS)
CPPC_TEST_UNIT_ARGS_ST_D			=	$(CPPC_TEST_UNIT_FLAGS) -D_DEBUG -UNDEBUG  $(CPPC_DEFS)
CPPC_TEST_UNIT_ARGS_ST_R			=	$(CPPC_TEST_UNIT_FLAGS) -DNDEBUG -U_DEBUG  $(CPPC_DEFS)


# AR args

AR_ARGS 			=	


# LD flags

LD_ARGS 		=	

LD_F_NOLOGO = 

# LD args

LD_ARGS_MT_D =  $(LD_ARGS) $(LD_F_NOLOGO) $(LIBPATHS) $(COMMON_LIB)
LD_ARGS_MT_R =  $(LD_ARGS) $(LD_F_NOLOGO) $(LIBPATHS) $(COMMON_LIB)
LD_ARGS_ST_D =  $(LD_ARGS) $(LD_F_NOLOGO) $(LIBPATHS) $(COMMON_LIB)
LD_ARGS_ST_R =  $(LD_ARGS) $(LD_F_NOLOGO) $(LIBPATHS) $(COMMON_LIB)

############################################################################
# Headers

CORE_HEADERS=\
 $(PROJ_INCLUDE_DIR)/pantheios/pantheios.h\
 $(PROJ_INCLUDE_DIR)/pantheios/backend.h\
 $(PROJ_INCLUDE_DIR)/pantheios/init_codes.h\
 $(PROJ_INCLUDE_DIR)/pantheios/frontend.h\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/pantheios.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/trace.h\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/args.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/b.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/b64.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/blob.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/boolean.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/ch.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/character.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/exception.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/fmt.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/hex_ptr.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/hostid.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/integer.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/m2w.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/p.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/pad.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/pointer.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/processid.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/real.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/slice.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/threadid.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/w2m.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/xi.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/inserters/xp.hpp\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/internal/initialiser.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/internal/stock_levels.hpp\
 $(PROJ_INCLUDE_DIR)/pantheios/internal/nox.h\
 $(PROJ_INCLUDE_DIR)/pantheios/internal/safestr.h\
 $(PROJ_INCLUDE_DIR)/pantheios/internal/threading.h\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/quality/contract.h\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/internal/generated/log_dispatch_functions.h\
 $(PROJ_INCLUDE_DIR)/pantheios/internal/generated/log_functions.c\
 $(PROJ_INCLUDE_DIR)/pantheios/internal/generated/log_functions.h\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/severity/levels.hpp\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/util/backends/arguments.h\
 $(PROJ_INCLUDE_DIR)/pantheios/util/backends/context.hpp\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/util/core/apidefs.hpp\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/util/memory/auto_buffer_selector.hpp\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/util/severity/ACE.h\
 $(PROJ_INCLUDE_DIR)/pantheios/util/severity/syslog.h\
 $(PROJ_INCLUDE_DIR)/pantheios/util/severity/WindowsEventLog.h\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/util/string/snprintf.h\
 $(PROJ_INCLUDE_DIR)/pantheios/util/string/strdup.h\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/util/system/hostname.h\
 $(PROJ_INCLUDE_DIR)/pantheios/util/system/processid.h\
 $(PROJ_INCLUDE_DIR)/pantheios/util/system/threadid.h\
 \
 $(PROJ_INCLUDE_DIR)/pantheios/util/time/currenttime.h\
 \
 \

BE_HEADERS=\
 $(PROJ_INCLUDE_DIR)/pantheios/pantheios.h \
 $(PROJ_INCLUDE_DIR)/pantheios/backend.h \
 \

FE_HEADERS=\
 $(PROJ_INCLUDE_DIR)/pantheios/pantheios.h \
 $(PROJ_INCLUDE_DIR)/pantheios/frontend.h \
 \

TEST_C_HEADERS=\
 $(PROJ_INCLUDE_DIR)/pantheios/backend.h\
 $(PROJ_INCLUDE_DIR)/pantheios/frontend.h\
 $(PROJ_INCLUDE_DIR)/pantheios/pantheios.h\
 \

TEST_CPP_HEADERS=\
 $(CORE_HEADERS)\
 $(PROJ_INCLUDE_DIR)/pantheios/backends/bec.test.h\
 \

B64_HEADERS=\
 $(PROJ_INCLUDE_DIR)/b64/b64.h\
 $(PROJ_INCLUDE_DIR)/b64/b64.hpp\
 \

SHWILD_HEADERS=\
 $(PROJ_INCLUDE_DIR)/shwild/shwild.h\
 $(PROJ_INCLUDE_DIR)/shwild/shwild.hpp\
 \

XTESTS_HEADERS=\
 $(PROJ_INCLUDE_DIR)/xtests/xtests.h\
 \

############################################################################
# Names

############################################################
# Targets

########################################
# Libraries

#  util
LIB_UTIL_MT_D				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).util.$(COMP_ARCH_OS_TAG).mt.debug.a

#  core
LIB_CORE_MT_D				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).core.$(COMP_ARCH_OS_TAG).mt.debug.a

#  appl
LIB_APPL_MT_D				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).appl.$(COMP_ARCH_OS_TAG).mt.debug.a

#  front-end(s)
LIB_FE_ALL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_ALL_WC_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.WithCallback.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_FAIL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.fail.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_N_MT_D				=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_N_WC_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.WithCallback.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_NULL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_NULL_WC_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.WithCallback.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_SIMPLE_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_FE_SIMPLE_WC_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.WithCallback.$(COMP_ARCH_OS_TAG).mt.debug.a

#  back-end(s)
LIB_BEC_STOCK_MT_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.stock.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_LRSPLIT_MT_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.lrsplit.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_N_MT_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.N.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_TEST_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.test.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_TEST_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.test.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEL_TEST_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.test.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BER_TEST_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.test.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_FAIL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fail.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_FAIL_MT_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fail.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEL_FAIL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fail.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BER_FAIL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fail.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_FILE_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_FILE_WC_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.WithCallback.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_FILE_MT_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.file.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEL_FILE_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.file.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BER_FILE_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.file.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_FPRINTF_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_FPRINTF_WC_MT_D	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.WithCallback.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_FPRINTF_MT_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fprintf.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEL_FPRINTF_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fprintf.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BER_FPRINTF_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fprintf.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_NULL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.null.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_NULL_MT_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.null.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEL_NULL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.null.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BER_NULL_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.null.$(COMP_ARCH_OS_TAG).mt.debug.a
ifdef ACE
LIB_BEC_ACE_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.ACE.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_ACE_MT_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.ACE.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEL_ACE_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.ACE.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BER_ACE_MT_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.ACE.$(COMP_ARCH_OS_TAG).mt.debug.a
endif # ACE
LIB_BEC_SYSLOG_MT_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BEC_SYSLOGWC_MT_D 	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.WithCallback.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BE_SYSLOG_MT_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.syslog.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_BER_SYSLOG_MT_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.syslog.$(COMP_ARCH_OS_TAG).mt.debug.a

#  util
LIB_UTIL_MT_R				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).util.$(COMP_ARCH_OS_TAG).mt.a

#  core
LIB_CORE_MT_R				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).core.$(COMP_ARCH_OS_TAG).mt.a

#  appl
LIB_APPL_MT_R				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).appl.$(COMP_ARCH_OS_TAG).mt.a

#  front-end(s)
LIB_FE_ALL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_ALL_WC_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.WithCallback.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_FAIL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.fail.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_N_MT_R				=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_N_WC_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.WithCallback.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_NULL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_NULL_WC_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.WithCallback.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_SIMPLE_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.$(COMP_ARCH_OS_TAG).mt.a
LIB_FE_SIMPLE_WC_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.WithCallback.$(COMP_ARCH_OS_TAG).mt.a

#  back-end(s)
LIB_BEC_STOCK_MT_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.stock.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_LRSPLIT_MT_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.lrsplit.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_N_MT_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.N.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_TEST_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.test.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_TEST_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.test.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEL_TEST_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.test.$(COMP_ARCH_OS_TAG).mt.a
LIB_BER_TEST_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.test.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_FAIL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fail.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_FAIL_MT_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fail.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEL_FAIL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fail.$(COMP_ARCH_OS_TAG).mt.a
LIB_BER_FAIL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fail.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_FILE_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_FILE_WC_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.WithCallback.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_FILE_MT_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.file.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEL_FILE_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.file.$(COMP_ARCH_OS_TAG).mt.a
LIB_BER_FILE_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.file.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_FPRINTF_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_FPRINTF_WC_MT_R	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.WithCallback.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_FPRINTF_MT_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fprintf.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEL_FPRINTF_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fprintf.$(COMP_ARCH_OS_TAG).mt.a
LIB_BER_FPRINTF_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fprintf.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_NULL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.null.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_NULL_MT_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.null.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEL_NULL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.null.$(COMP_ARCH_OS_TAG).mt.a
LIB_BER_NULL_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.null.$(COMP_ARCH_OS_TAG).mt.a
ifdef ACE
LIB_BEC_ACE_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.ACE.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_ACE_MT_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.ACE.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEL_ACE_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.ACE.$(COMP_ARCH_OS_TAG).mt.a
LIB_BER_ACE_MT_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.ACE.$(COMP_ARCH_OS_TAG).mt.a
endif # ACE
LIB_BEC_SYSLOG_MT_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.$(COMP_ARCH_OS_TAG).mt.a
LIB_BEC_SYSLOGWC_MT_R 	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.WithCallback.$(COMP_ARCH_OS_TAG).mt.a
LIB_BE_SYSLOG_MT_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.syslog.$(COMP_ARCH_OS_TAG).mt.a
LIB_BER_SYSLOG_MT_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.syslog.$(COMP_ARCH_OS_TAG).mt.a

#  util
LIB_UTIL_ST_D				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).util.$(COMP_ARCH_OS_TAG).debug.a

#  core
LIB_CORE_ST_D				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).core.$(COMP_ARCH_OS_TAG).debug.a

#  appl
LIB_APPL_ST_D				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).appl.$(COMP_ARCH_OS_TAG).debug.a

#  front-end(s)
LIB_FE_ALL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_ALL_WC_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.WithCallback.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_FAIL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.fail.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_N_ST_D				=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_N_WC_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.WithCallback.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_NULL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_NULL_WC_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.WithCallback.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_SIMPLE_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.$(COMP_ARCH_OS_TAG).debug.a
LIB_FE_SIMPLE_WC_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.WithCallback.$(COMP_ARCH_OS_TAG).debug.a

#  back-end(s)
LIB_BEC_STOCK_ST_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.stock.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_LRSPLIT_ST_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.lrsplit.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_N_ST_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.N.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_TEST_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.test.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_TEST_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.test.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEL_TEST_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.test.$(COMP_ARCH_OS_TAG).debug.a
LIB_BER_TEST_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.test.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_FAIL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fail.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_FAIL_ST_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fail.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEL_FAIL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fail.$(COMP_ARCH_OS_TAG).debug.a
LIB_BER_FAIL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fail.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_FILE_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_FILE_WC_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.WithCallback.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_FILE_ST_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.file.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEL_FILE_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.file.$(COMP_ARCH_OS_TAG).debug.a
LIB_BER_FILE_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.file.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_FPRINTF_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_FPRINTF_WC_ST_D	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.WithCallback.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_FPRINTF_ST_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fprintf.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEL_FPRINTF_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fprintf.$(COMP_ARCH_OS_TAG).debug.a
LIB_BER_FPRINTF_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fprintf.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_NULL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.null.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_NULL_ST_D 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.null.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEL_NULL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.null.$(COMP_ARCH_OS_TAG).debug.a
LIB_BER_NULL_ST_D			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.null.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_SYSLOG_ST_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.$(COMP_ARCH_OS_TAG).debug.a
LIB_BEC_SYSLOGWC_ST_D 	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.WithCallback.$(COMP_ARCH_OS_TAG).debug.a
LIB_BE_SYSLOG_ST_D		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.syslog.$(COMP_ARCH_OS_TAG).debug.a
LIB_BER_SYSLOG_ST_D 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.syslog.$(COMP_ARCH_OS_TAG).debug.a

#  util
LIB_UTIL_ST_R				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).util.$(COMP_ARCH_OS_TAG).a

#  core
LIB_CORE_ST_R				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).core.$(COMP_ARCH_OS_TAG).a

#  appl
LIB_APPL_ST_R				=   $(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).appl.$(COMP_ARCH_OS_TAG).a

#  front-end(s)
LIB_FE_ALL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.$(COMP_ARCH_OS_TAG).a
LIB_FE_ALL_WC_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.all.WithCallback.$(COMP_ARCH_OS_TAG).a
LIB_FE_FAIL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.fail.$(COMP_ARCH_OS_TAG).a
LIB_FE_N_ST_R				=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.$(COMP_ARCH_OS_TAG).a
LIB_FE_N_WC_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.N.WithCallback.$(COMP_ARCH_OS_TAG).a
LIB_FE_NULL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.$(COMP_ARCH_OS_TAG).a
LIB_FE_NULL_WC_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.null.WithCallback.$(COMP_ARCH_OS_TAG).a
LIB_FE_SIMPLE_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.$(COMP_ARCH_OS_TAG).a
LIB_FE_SIMPLE_WC_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).fe.simple.WithCallback.$(COMP_ARCH_OS_TAG).a

#  back-end(s)
LIB_BEC_STOCK_ST_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.stock.$(COMP_ARCH_OS_TAG).a
LIB_BE_LRSPLIT_ST_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.lrsplit.$(COMP_ARCH_OS_TAG).a
LIB_BE_N_ST_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.N.$(COMP_ARCH_OS_TAG).a
LIB_BEC_TEST_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.test.$(COMP_ARCH_OS_TAG).a
LIB_BE_TEST_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.test.$(COMP_ARCH_OS_TAG).a
LIB_BEL_TEST_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.test.$(COMP_ARCH_OS_TAG).a
LIB_BER_TEST_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.test.$(COMP_ARCH_OS_TAG).a
LIB_BEC_FAIL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fail.$(COMP_ARCH_OS_TAG).a
LIB_BE_FAIL_ST_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fail.$(COMP_ARCH_OS_TAG).a
LIB_BEL_FAIL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fail.$(COMP_ARCH_OS_TAG).a
LIB_BER_FAIL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fail.$(COMP_ARCH_OS_TAG).a
LIB_BEC_FILE_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.$(COMP_ARCH_OS_TAG).a
LIB_BEC_FILE_WC_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.file.WithCallback.$(COMP_ARCH_OS_TAG).a
LIB_BE_FILE_ST_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.file.$(COMP_ARCH_OS_TAG).a
LIB_BEL_FILE_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.file.$(COMP_ARCH_OS_TAG).a
LIB_BER_FILE_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.file.$(COMP_ARCH_OS_TAG).a
LIB_BEC_FPRINTF_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.$(COMP_ARCH_OS_TAG).a
LIB_BEC_FPRINTF_WC_ST_R	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.fprintf.WithCallback.$(COMP_ARCH_OS_TAG).a
LIB_BE_FPRINTF_ST_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.fprintf.$(COMP_ARCH_OS_TAG).a
LIB_BEL_FPRINTF_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.fprintf.$(COMP_ARCH_OS_TAG).a
LIB_BER_FPRINTF_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.fprintf.$(COMP_ARCH_OS_TAG).a
LIB_BEC_NULL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.null.$(COMP_ARCH_OS_TAG).a
LIB_BE_NULL_ST_R 			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.null.$(COMP_ARCH_OS_TAG).a
LIB_BEL_NULL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bel.null.$(COMP_ARCH_OS_TAG).a
LIB_BER_NULL_ST_R			=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.null.$(COMP_ARCH_OS_TAG).a
LIB_BEC_SYSLOG_ST_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.$(COMP_ARCH_OS_TAG).a
LIB_BEC_SYSLOGWC_ST_R 	=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).bec.syslog.WithCallback.$(COMP_ARCH_OS_TAG).a
LIB_BE_SYSLOG_ST_R		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).be.syslog.$(COMP_ARCH_OS_TAG).a
LIB_BER_SYSLOG_ST_R 		=	$(PROJ_LIB_DIR)/lib$(PROJ_TAG_VER).ber.syslog.$(COMP_ARCH_OS_TAG).a


# b64
LIB_B64_MT_D			=   $(PROJ_LIB_DIR)/lib$(B64_TAG_VER).pan.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_B64_MT_R			=   $(PROJ_LIB_DIR)/lib$(B64_TAG_VER).pan.$(COMP_ARCH_OS_TAG).mt.a
LIB_B64_ST_D			=   $(PROJ_LIB_DIR)/lib$(B64_TAG_VER).pan.$(COMP_ARCH_OS_TAG).debug.a
LIB_B64_ST_R			=   $(PROJ_LIB_DIR)/lib$(B64_TAG_VER).pan.$(COMP_ARCH_OS_TAG).a

# shwild
LIB_SHWILD_MT_D         =   $(PROJ_LIB_DIR)/lib$(SHWILD_TAG_VER).pan.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_SHWILD_MT_R         =   $(PROJ_LIB_DIR)/lib$(SHWILD_TAG_VER).pan.$(COMP_ARCH_OS_TAG).mt.a
LIB_SHWILD_ST_D         =   $(PROJ_LIB_DIR)/lib$(SHWILD_TAG_VER).pan.$(COMP_ARCH_OS_TAG).debug.a
LIB_SHWILD_ST_R         =   $(PROJ_LIB_DIR)/lib$(SHWILD_TAG_VER).pan.$(COMP_ARCH_OS_TAG).a

# xtests
LIB_XTESTS_MT_D         =   $(PROJ_LIB_DIR)/lib$(XTESTS_TAG_VER).pan.$(COMP_ARCH_OS_TAG).mt.debug.a
LIB_XTESTS_MT_R         =   $(PROJ_LIB_DIR)/lib$(XTESTS_TAG_VER).pan.$(COMP_ARCH_OS_TAG).mt.a
LIB_XTESTS_ST_D         =   $(PROJ_LIB_DIR)/lib$(XTESTS_TAG_VER).pan.$(COMP_ARCH_OS_TAG).debug.a
LIB_XTESTS_ST_R         =   $(PROJ_LIB_DIR)/lib$(XTESTS_TAG_VER).pan.$(COMP_ARCH_OS_TAG).a

########################################
# Examples

TARG_EX_P_be_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_be_02_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.02.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_be_03_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.03.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_be_04_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.04.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_be_05_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.05.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_be_06_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.06.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_cp_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_cp_02_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.02.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_fm_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_fm_02_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.02.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_fm_03_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.03.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_fe_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fe.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_02_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.02.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_03_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.03.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_04_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.04.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_05_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.05.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_06_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.06.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_07_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.07.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_08_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.08.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_09_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.09.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_10_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.10.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_11_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.11.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_12_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.12.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_13_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.13.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_14_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.14.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_15_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.15.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_16_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.16.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_in_17_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.17.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_ms_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_ms_02_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.02.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_ms_03_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.03.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_ms_04_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.04.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_ms_05_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.05.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_ms_06_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.06.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_tr_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_tr_02_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.02.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_ut_01_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ut.01.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_0_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.0.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_1_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.1.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_2_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.2.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_3_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.3.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_4_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.4.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_5_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.5.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_6_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.6.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_C_7_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.7.$(COMP_ARCH_OS_TAG).mt.debug
TARG_EX_P_be_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_be_02_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.02.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_be_03_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.03.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_be_04_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.04.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_be_05_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.05.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_be_06_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.06.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_cp_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_cp_02_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.02.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_fm_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_fm_02_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.02.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_fm_03_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.03.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_fe_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fe.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_02_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.02.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_03_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.03.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_04_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.04.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_05_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.05.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_06_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.06.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_07_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.07.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_08_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.08.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_09_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.09.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_10_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.10.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_11_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.11.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_12_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.12.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_13_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.13.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_14_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.14.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_15_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.15.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_16_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.16.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_in_17_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.17.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_ms_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_ms_02_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.02.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_ms_03_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.03.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_ms_04_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.04.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_ms_05_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.05.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_ms_06_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.06.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_tr_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_tr_02_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.02.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_ut_01_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ut.01.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_0_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.0.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_1_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.1.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_2_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.2.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_3_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.3.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_4_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.4.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_5_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.5.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_6_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.6.$(COMP_ARCH_OS_TAG).mt
TARG_EX_C_7_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.7.$(COMP_ARCH_OS_TAG).mt
TARG_EX_P_be_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_be_02_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.02.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_be_03_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.03.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_be_04_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.04.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_be_05_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.05.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_be_06_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.06.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_cp_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_cp_02_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.02.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_fm_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_fm_02_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.02.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_fm_03_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.03.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_fe_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fe.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_02_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.02.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_03_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.03.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_04_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.04.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_05_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.05.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_06_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.06.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_07_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.07.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_08_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.08.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_09_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.09.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_10_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.10.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_11_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.11.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_12_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.12.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_13_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.13.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_14_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.14.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_15_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.15.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_16_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.16.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_in_17_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.17.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_ms_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_ms_02_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.02.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_ms_03_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.03.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_ms_04_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.04.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_ms_05_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.05.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_ms_06_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.06.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_tr_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_tr_02_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.02.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_ut_01_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ut.01.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_0_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.0.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_1_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.1.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_2_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.2.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_3_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.3.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_4_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.4.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_5_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.5.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_6_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.6.$(COMP_ARCH_OS_TAG).debug
TARG_EX_C_7_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.7.$(COMP_ARCH_OS_TAG).debug
TARG_EX_P_be_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.01.$(COMP_ARCH_OS_TAG)
TARG_EX_P_be_02_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.02.$(COMP_ARCH_OS_TAG)
TARG_EX_P_be_03_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.03.$(COMP_ARCH_OS_TAG)
TARG_EX_P_be_04_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.04.$(COMP_ARCH_OS_TAG)
TARG_EX_P_be_05_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.05.$(COMP_ARCH_OS_TAG)
TARG_EX_P_be_06_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.be.06.$(COMP_ARCH_OS_TAG)
TARG_EX_P_cp_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.01.$(COMP_ARCH_OS_TAG)
TARG_EX_P_cp_02_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.cp.02.$(COMP_ARCH_OS_TAG)
TARG_EX_P_fm_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.01.$(COMP_ARCH_OS_TAG)
TARG_EX_P_fm_02_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.02.$(COMP_ARCH_OS_TAG)
TARG_EX_P_fm_03_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fm.03.$(COMP_ARCH_OS_TAG)
TARG_EX_P_fe_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.fe.01.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.01.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_02_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.02.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_03_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.03.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_04_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.04.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_05_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.05.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_06_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.06.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_07_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.07.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_08_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.08.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_09_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.09.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_10_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.10.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_11_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.11.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_12_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.12.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_13_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.13.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_14_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.14.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_15_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.15.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_16_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.16.$(COMP_ARCH_OS_TAG)
TARG_EX_P_in_17_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.in.17.$(COMP_ARCH_OS_TAG)
TARG_EX_P_ms_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.01.$(COMP_ARCH_OS_TAG)
TARG_EX_P_ms_02_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.02.$(COMP_ARCH_OS_TAG)
TARG_EX_P_ms_03_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.03.$(COMP_ARCH_OS_TAG)
TARG_EX_P_ms_04_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.04.$(COMP_ARCH_OS_TAG)
TARG_EX_P_ms_05_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.05.$(COMP_ARCH_OS_TAG)
TARG_EX_P_ms_06_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ms.06.$(COMP_ARCH_OS_TAG)
TARG_EX_P_tr_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.01.$(COMP_ARCH_OS_TAG)
TARG_EX_P_tr_02_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.tr.02.$(COMP_ARCH_OS_TAG)
TARG_EX_P_ut_01_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.cpp.ut.01.$(COMP_ARCH_OS_TAG)
TARG_EX_C_0_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.0.$(COMP_ARCH_OS_TAG)
TARG_EX_C_1_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.1.$(COMP_ARCH_OS_TAG)
TARG_EX_C_2_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.2.$(COMP_ARCH_OS_TAG)
TARG_EX_C_3_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.3.$(COMP_ARCH_OS_TAG)
TARG_EX_C_4_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.4.$(COMP_ARCH_OS_TAG)
TARG_EX_C_5_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.5.$(COMP_ARCH_OS_TAG)
TARG_EX_C_6_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.6.$(COMP_ARCH_OS_TAG)
TARG_EX_C_7_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).ex.c.7.$(COMP_ARCH_OS_TAG)

########################################
# Tests

##############################
# Component Tests

TARG_TEST_COMP_core_initseq_MT_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.init_sequence.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_core_initseq_MT_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.init_sequence.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_core_initseq_ST_D	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.init_sequence.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_core_initseq_ST_R	=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.init_sequence.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_custsev_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.custom_severity.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_custsev_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.custom_severity.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_custsev_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.custom_severity.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_custsev_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.custom_severity.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_filemt_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.be.file.threading.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_filemt_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.be.file.threading.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_args_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.args.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_args_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.args.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_args_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.args.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_args_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.args.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_b64_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.b64.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_b64_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.b64.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_b64_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.b64.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_b64_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.b64.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_bool_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.boolean.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_bool_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.boolean.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_bool_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.boolean.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_bool_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.boolean.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_hostid_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.hostId.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_hostid_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.hostId.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_hostid_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.hostId.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_hostid_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.hostId.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_int_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.integer.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_int_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.integer.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_int_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.integer.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_int_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.integer.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_pad_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pad.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_pad_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pad.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_pad_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pad.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_pad_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pad.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_ptr_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pointer.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_ptr_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pointer.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_ptr_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pointer.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_ptr_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.pointer.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_prid_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.processId.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_prid_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.processId.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_prid_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.processId.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_prid_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.processId.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_real_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.real.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_real_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.real.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_real_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.real.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_real_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.real.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_slice_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.slice.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_slice_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.slice.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_slice_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.slice.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_slice_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.slice.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_thrid_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.threadId.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_thrid_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.threadId.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_thrid_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.threadId.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_thrid_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.threadId.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_ins_w2m_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.w2m.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_ins_w2m_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.w2m.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_ins_w2m_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.w2m.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_ins_w2m_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserters.w2m.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_log_1_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.log.1.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_log_1_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.log.1.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_log_1_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.log.1.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_log_1_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.log.1.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_pan_puts_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logputs.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_pan_puts_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logputs.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_pan_puts_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logputs.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_pan_puts_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logputs.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_pan_prtf_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logprintf.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_pan_prtf_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logprintf.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_pan_prtf_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logprintf.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_pan_prtf_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.core.pan_logprintf.$(COMP_ARCH_OS_TAG)
TARG_TEST_COMP_trace_1_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.trace.1.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_COMP_trace_1_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.trace.1.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_COMP_trace_1_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.trace.1.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_COMP_trace_1_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.trace.1.$(COMP_ARCH_OS_TAG)

##############################
# Performance Tests

TARG_TEST_PERF_ins_w2m_MT_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserter.w2m.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_PERF_ins_w2m_MT_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserter.w2m.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_PERF_ins_w2m_ST_D		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserter.w2m.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_PERF_ins_w2m_ST_R		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.component.inserter.w2m.$(COMP_ARCH_OS_TAG)

##############################
# Scratch Tests

# test programs
#
# - be.ACE
# - be.fprintf
# - be.syslog / be.WindowsDebugger
#
# - be.lrsplit & fprintf + syslog / WindowsSyslog
#
# The strange naming conventions of these targets are to placate some
# make utilities, which have a limit to the identifier length.
ifdef ACE
TEST_SCRT_ACE_MT_D						=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.ACE.$(COMP_ARCH_OS_TAG).mt.debug
endif # ACE
TARG_TEST_MT_D_APIwBE_FPRINTF					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.fprintf.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_SCRT_BE_SYSLOG_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_SCRT_BE_SYSLOGWC_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.WC.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_MT_D_APIwBELR_1_FPRINTF_SYSLOG		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.l.fprintf-r.syslog.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_FE_MT_D 						=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_BE_N_FAIL_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.N.fail.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_FE_SIMPLE_WC_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.simple.WC.$(COMP_ARCH_OS_TAG).mt.debug
ifdef ACE
TEST_SCRT_ACE_MT_R						=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.ACE.$(COMP_ARCH_OS_TAG).mt
endif # ACE
TARG_TEST_MT_R_APIwBE_FPRINTF					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.fprintf.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_SCRT_BE_SYSLOG_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_SCRT_BE_SYSLOGWC_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.WC.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_MT_R_APIwBELR_1_FPRINTF_SYSLOG		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.l.fprintf-r.syslog.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_FE_MT_R 						=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_BE_N_FAIL_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.N.fail.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_FE_SIMPLE_WC_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.simple.WC.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_ST_D_APIwBE_FPRINTF					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.fprintf.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_SCRT_BE_SYSLOG_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_SCRT_BE_SYSLOGWC_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.WC.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_ST_D_APIwBELR_1_FPRINTF_SYSLOG		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.l.fprintf-r.syslog.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_FE_ST_D 						=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_BE_N_FAIL_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.N.fail.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_FE_SIMPLE_WC_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.simple.WC.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_ST_R_APIwBE_FPRINTF					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.fprintf.$(COMP_ARCH_OS_TAG)
TARG_TEST_SCRT_BE_SYSLOG_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.$(COMP_ARCH_OS_TAG)
TARG_TEST_SCRT_BE_SYSLOGWC_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.syslog.WC.$(COMP_ARCH_OS_TAG)
TARG_TEST_ST_R_APIwBELR_1_FPRINTF_SYSLOG		=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.l.fprintf-r.syslog.$(COMP_ARCH_OS_TAG)
TARG_TEST_FE_ST_R 						=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.$(COMP_ARCH_OS_TAG)
TARG_TEST_BE_N_FAIL_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.be.N.fail.$(COMP_ARCH_OS_TAG)
TARG_TEST_FE_SIMPLE_WC_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.scratch.fe.simple.WC.$(COMP_ARCH_OS_TAG)

##############################
# Unit Tests
#
# NOTE: dynamic_initialisation is contracted, as scares CodeWarrior's linker (for being longer than 63 chars)

TARG_TEST_UNIT_lvls_dyninit_MT_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.dynamic_init.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_lvls_dyninit_MT_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.dynamic_init.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_lvls_dyninit_ST_D			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.dynamic_init.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_lvls_dyninit_ST_R			=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.dynamic_init.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_lvls_vals_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.values.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_lvls_vals_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.values.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_lvls_vals_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.values.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_lvls_vals_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.levels.values.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_getcurrtm_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.getcurrenttime.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_getcurrtm_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.getcurrenttime.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_getcurrtm_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.getcurrenttime.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_getcurrtm_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.getcurrenttime.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_getversion_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.getversion.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_getversion_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.getversion.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_getversion_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.getversion.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_getversion_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.getversion.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_gethostname_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.gethostname.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_gethostname_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.gethostname.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_gethostname_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.gethostname.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_gethostname_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.gethostname.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_onbailout_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.onbailout.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_strnlen_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.strnlen.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_onbailout_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.onbailout.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_strnlen_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.strnlen.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_onbailout_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.onbailout.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_strnlen_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.strnlen.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_onbailout_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.onbailout.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_strnlen_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.util.strnlen.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_bec_file_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.file.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_bec_file_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.file.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_bec_file_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.file.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_bec_file_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.file.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_bec_fprintf_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fprintf.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_bec_fprintf_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fprintf.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_bec_fprintf_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fprintf.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_bec_fprintf_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fprintf.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_bec_fail_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fail.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_bec_fail_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fail.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_bec_fail_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fail.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_bec_fail_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.bec.fail.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_be_fail_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_be_fail_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_be_fail_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_be_fail_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_be_fail_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_be_fail_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_be_fail_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_be_fail_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.fail.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_be_lrsplit_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.lrsplit.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_be_lrsplit_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.lrsplit.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_be_lrsplit_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.lrsplit.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_be_lrsplit_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.lrsplit.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_be_N_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.N.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_be_N_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.N.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_be_N_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.N.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_be_N_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.be.N.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_fe_all_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.all.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_fe_all_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.all.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_fe_all_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.all.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_fe_all_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.all.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_fe_fail_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.fail.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_fe_fail_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.fail.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_fe_fail_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.fail.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_fe_fail_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.fail.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_fe_N_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.N.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_fe_N_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.N.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_fe_N_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.N.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_fe_N_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.N.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_fe_null_MT_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.null.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_fe_null_MT_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.null.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_fe_null_ST_D					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.null.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_fe_null_ST_R					=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.null.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_fe_simple_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.simple.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_fe_simple_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.simple.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_fe_simple_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.simple.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_fe_simple_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.fe.simple.$(COMP_ARCH_OS_TAG)

TARG_TEST_UNIT_ins_int_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.integer.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_ins_w2m_MT_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.w2m.$(COMP_ARCH_OS_TAG).mt.debug
TARG_TEST_UNIT_ins_int_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.integer.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_ins_w2m_MT_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.w2m.$(COMP_ARCH_OS_TAG).mt
TARG_TEST_UNIT_ins_int_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.integer.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_ins_w2m_ST_D				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.w2m.$(COMP_ARCH_OS_TAG).debug
TARG_TEST_UNIT_ins_int_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.integer.$(COMP_ARCH_OS_TAG)
TARG_TEST_UNIT_ins_w2m_ST_R				=	$(PROJ_BIN_DIR)/$(PROJ_TAG_VER).test.unit.inserter.w2m.$(COMP_ARCH_OS_TAG)

############################################################
# Objects

########################################
# Libraries

# core

OBJ_utl_bailout_MT_D		= $(PROJ_OBJ_DIR)/util.bailout.mt.debug.o
OBJ_utl_init_code_strs_MT_D	= $(PROJ_OBJ_DIR)/util.init_err_strings.mt.debug.o
OBJ_utl_sev_strs_MT_D		= $(PROJ_OBJ_DIR)/util.sev_strings.mt.debug.o
OBJ_utl_snprintf_MT_D 	= $(PROJ_OBJ_DIR)/util.snprintf.mt.debug.o
OBJ_utl_strdup_MT_D 		= $(PROJ_OBJ_DIR)/util.strdup.mt.debug.o
OBJ_utl_procid_MT_D 		= $(PROJ_OBJ_DIR)/util.procid.mt.debug.o
OBJ_utl_thread_MT_D 		= $(PROJ_OBJ_DIR)/util.thread.mt.debug.o
OBJ_utl_be_context_MT_D	= $(PROJ_OBJ_DIR)/util.be_context.mt.debug.o
OBJ_utl_core_apidefs_MT_D	= $(PROJ_OBJ_DIR)/util.core_apidefs.mt.debug.o
OBJ_utl_be_parse_MT_D		= $(PROJ_OBJ_DIR)/util.be_parse.mt.debug.o
OBJ_utl_strnlen_MT_D		= $(PROJ_OBJ_DIR)/util.strnlen.mt.debug.o
OBJ_utl_strnlenp_MT_D		= $(PROJ_OBJ_DIR)/util.strnlen.c++.mt.debug.o
OBJ_utl_time_MT_D			= $(PROJ_OBJ_DIR)/util.time.mt.debug.o
OBJ_utl_host_name_MT_D	= $(PROJ_OBJ_DIR)/util.host_name.mt.debug.o
OBJ_cr_api_MT_D 			= $(PROJ_OBJ_DIR)/core.api.mt.debug.o
OBJ_cr_api_dep_MT_D 		= $(PROJ_OBJ_DIR)/core.api.deprecated.mt.debug.o
OBJ_cr_api_exit_MT_D 		= $(PROJ_OBJ_DIR)/core.api.exitprocess.mt.debug.o
OBJ_cr_api_logprintf_MT_D	= $(PROJ_OBJ_DIR)/core.api.logprintf.mt.debug.o
OBJ_cr_api_logputs_MT_D	= $(PROJ_OBJ_DIR)/core.api.logputs.mt.debug.o
OBJ_cr_auto_MT_D 			= $(PROJ_OBJ_DIR)/core.auto.mt.debug.o
OBJ_ins_args_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.args.mt.debug.o
OBJ_ins_b64_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.b64.mt.debug.o
OBJ_ins_blob_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.blob.mt.debug.o
OBJ_ins_bool_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.boolean.mt.debug.o
OBJ_ins_excp_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.exception.mt.debug.o
OBJ_ins_host_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.hostid.mt.debug.o
OBJ_ins_int_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.integer.mt.debug.o
OBJ_ins_ptr_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.pointer.mt.debug.o
OBJ_ins_prid_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.processid.mt.debug.o
OBJ_ins_real_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.real.mt.debug.o
OBJ_ins_slice_MT_D	 	= $(PROJ_OBJ_DIR)/inserters.slice.mt.debug.o
OBJ_ins_thrid_MT_D	 	= $(PROJ_OBJ_DIR)/inserters.threadid.mt.debug.o
OBJ_ins_w2m_MT_D	 		= $(PROJ_OBJ_DIR)/inserters.w2m.mt.debug.o
#PDB_bec_test_MT_D 		= $(PROJ_OBJ_DIR)/bec.test.mt.debug.pdb
OBJ_bec_test_MT_D 		= $(PROJ_OBJ_DIR)/bec.test.mt.debug.o
OBJ_be_test_MT_D 			= $(PROJ_OBJ_DIR)/be.test.mt.debug.o
OBJ_bel_test_MT_D 		= $(PROJ_OBJ_DIR)/bel.test.mt.debug.o
OBJ_ber_test_MT_D 		= $(PROJ_OBJ_DIR)/ber.test.mt.debug.o
OBJ_be_lrsplit_MT_D		= $(PROJ_OBJ_DIR)/be.lrsplit.mt.debug.o
OBJ_be_N_MT_D				= $(PROJ_OBJ_DIR)/be.N.mt.debug.o
OBJ_bec_fail_MT_D			= $(PROJ_OBJ_DIR)/bec.fail.mt.debug.o
OBJ_be_fail_MT_D			= $(PROJ_OBJ_DIR)/be.fail.mt.debug.o
OBJ_bel_fail_MT_D			= $(PROJ_OBJ_DIR)/bel.fail.mt.debug.o
OBJ_ber_fail_MT_D			= $(PROJ_OBJ_DIR)/ber.fail.mt.debug.o
OBJ_bec_file_MT_D			= $(PROJ_OBJ_DIR)/bec.file.mt.debug.o
OBJ_bec_file_WC_MT_D		= $(PROJ_OBJ_DIR)/bec.file.WithCallback.mt.debug.o
OBJ_be_file_MT_D			= $(PROJ_OBJ_DIR)/be.file.mt.debug.o
OBJ_bel_file_MT_D			= $(PROJ_OBJ_DIR)/bel.file.mt.debug.o
OBJ_ber_file_MT_D			= $(PROJ_OBJ_DIR)/ber.file.mt.debug.o
OBJ_bec_fprintf_MT_D		= $(PROJ_OBJ_DIR)/bec.fprintf.mt.debug.o
OBJ_bec_fprintf_WC_MT_D	= $(PROJ_OBJ_DIR)/bec.fprintf.WithCallback.mt.debug.o
OBJ_be_fprintf_MT_D		= $(PROJ_OBJ_DIR)/be.fprintf.mt.debug.o
OBJ_bel_fprintf_MT_D		= $(PROJ_OBJ_DIR)/bel.fprintf.mt.debug.o
OBJ_ber_fprintf_MT_D		= $(PROJ_OBJ_DIR)/ber.fprintf.mt.debug.o
OBJ_bec_null_MT_D			= $(PROJ_OBJ_DIR)/bec.null.mt.debug.o
OBJ_be_null_MT_D			= $(PROJ_OBJ_DIR)/be.null.mt.debug.o
OBJ_bel_null_MT_D			= $(PROJ_OBJ_DIR)/bel.null.mt.debug.o
OBJ_ber_null_MT_D			= $(PROJ_OBJ_DIR)/ber.null.mt.debug.o
ifdef ACE
OBJ_bec_ACE_MT_D			= $(PROJ_OBJ_DIR)/bec.ACE.mt.debug.o
OBJ_be_ACE_MT_D			= $(PROJ_OBJ_DIR)/be.ACE.mt.debug.o
OBJ_bel_ACE_MT_D			= $(PROJ_OBJ_DIR)/bel.ACE.mt.debug.o
OBJ_ber_ACE_MT_D			= $(PROJ_OBJ_DIR)/ber.ACE.mt.debug.o
endif # ACE
OBJ_bec_SYSLOG_MT_D		= $(PROJ_OBJ_DIR)/bec.syslog.mt.debug.o
OBJ_bec_SYSLOG_WC_MT_D	= $(PROJ_OBJ_DIR)/bec.syslog.WithCallback.mt.debug.o
OBJ_be_SYSLOG_MT_D 		= $(PROJ_OBJ_DIR)/be.syslog.mt.debug.o
OBJ_ber_SYSLOG_MT_D		= $(PROJ_OBJ_DIR)/ber.syslog.mt.debug.o
OBJ_fe_all_MT_D 			= $(PROJ_OBJ_DIR)/fe.all.mt.debug.o
OBJ_fe_all_WC_MT_D 		= $(PROJ_OBJ_DIR)/fe.all.WithCallback.mt.debug.o
OBJ_fe_fail_MT_D 			= $(PROJ_OBJ_DIR)/fe.fail.mt.debug.o
OBJ_fe_N_MT_D 			= $(PROJ_OBJ_DIR)/fe.N.mt.debug.o
OBJ_fe_N_WC_MT_D 			= $(PROJ_OBJ_DIR)/fe.N.WithCallback.mt.debug.o
OBJ_fe_null_MT_D 			= $(PROJ_OBJ_DIR)/fe.null.mt.debug.o
OBJ_fe_null_WC_MT_D 		= $(PROJ_OBJ_DIR)/fe.null.WithCallback.mt.debug.o
OBJ_fe_simple_MT_D 		= $(PROJ_OBJ_DIR)/fe.simple.mt.debug.o
OBJ_fe_simple_WC_MT_D 	= $(PROJ_OBJ_DIR)/fe.simple.WithCallback.mt.debug.o

OBJ_UTIL_MT_D=\
 $(OBJ_utl_bailout_MT_D)\
 $(OBJ_utl_init_code_strs_MT_D)\
 $(OBJ_utl_sev_strs_MT_D)\
 $(OBJ_utl_snprintf_MT_D)\
 $(OBJ_utl_strdup_MT_D)\
 $(OBJ_utl_procid_MT_D)\
 $(OBJ_utl_thread_MT_D)\
 $(OBJ_utl_be_context_MT_D)\
 $(OBJ_utl_core_apidefs_MT_D)\
 $(OBJ_utl_be_parse_MT_D)\
 $(OBJ_utl_strnlen_MT_D)\
 $(OBJ_utl_strnlenp_MT_D)\
 $(OBJ_utl_time_MT_D)\
 $(OBJ_utl_host_name_MT_D)\
 \

OBJ_CORE_MT_D=\
 $(OBJ_cr_api_MT_D)\
 $(OBJ_cr_api_dep_MT_D)\
 $(OBJ_cr_api_exit_MT_D)\
 $(OBJ_cr_api_logprintf_MT_D)\
 $(OBJ_cr_api_logputs_MT_D)\
 $(OBJ_cr_auto_MT_D)\
 $(OBJ_ins_args_MT_D)\
 $(OBJ_ins_b64_MT_D)\
 $(OBJ_ins_blob_MT_D)\
 $(OBJ_ins_bool_MT_D)\
 $(OBJ_ins_excp_MT_D)\
 $(OBJ_ins_host_MT_D)\
 $(OBJ_ins_int_MT_D)\
 $(OBJ_ins_ptr_MT_D)\
 $(OBJ_ins_prid_MT_D)\
 $(OBJ_ins_real_MT_D)\
 $(OBJ_ins_slice_MT_D)\
 $(OBJ_ins_thrid_MT_D)\
 $(OBJ_ins_w2m_MT_D)\
 \

OBJ_APPL_MT_D=\
 $(OBJ_ins_args_MT_D)\
 $(OBJ_ins_b64_MT_D)\
 $(OBJ_ins_blob_MT_D)\
 $(OBJ_ins_bool_MT_D)\
 $(OBJ_ins_excp_MT_D)\
 $(OBJ_ins_host_MT_D)\
 $(OBJ_ins_int_MT_D)\
 $(OBJ_ins_ptr_MT_D)\
 $(OBJ_ins_prid_MT_D)\
 $(OBJ_ins_real_MT_D)\
 $(OBJ_ins_slice_MT_D)\
 $(OBJ_ins_thrid_MT_D)\
 $(OBJ_ins_w2m_MT_D)\
 \


OBJ_BE_LRSPLIT_MT_D=\
 $(OBJ_be_lrsplit_MT_D)\
 \

OBJ_BE_N_MT_D=\
 $(OBJ_be_N_MT_D)\
 \

OBJ_BEC_TEST_MT_D=\
 $(OBJ_bec_test_MT_D)\
 \


OBJ_BE_TEST_MT_D=\
 $(OBJ_be_test_MT_D)\
 \


OBJ_BEL_TEST_MT_D=\
 $(OBJ_bel_test_MT_D)\
 \


OBJ_BER_TEST_MT_D=\
 $(OBJ_ber_test_MT_D)\
 \


OBJ_BEC_FAIL_MT_D=\
 $(OBJ_bec_fail_MT_D)\
 \

OBJ_BE_FAIL_MT_D=\
 $(OBJ_be_fail_MT_D)\
 \

OBJ_BEL_FAIL_MT_D=\
 $(OBJ_bel_fail_MT_D)\
 \

OBJ_BER_FAIL_MT_D=\
 $(OBJ_ber_fail_MT_D)\
 \

OBJ_BEC_FILE_MT_D=\
 $(OBJ_bec_file_MT_D)\
 \

OBJ_BEC_FILE_WC_MT_D=\
 $(OBJ_bec_file_WC_MT_D)\
 \

OBJ_BE_FILE_MT_D=\
 $(OBJ_be_file_MT_D)\
 \

OBJ_BEL_FILE_MT_D=\
 $(OBJ_bel_file_MT_D)\
 \

OBJ_BER_FILE_MT_D=\
 $(OBJ_ber_file_MT_D)\
 \

OBJ_BEC_FPRINTF_MT_D=\
 $(OBJ_bec_fprintf_MT_D)\
 \

OBJ_BEC_FPRINTF_WC_MT_D=\
 $(OBJ_bec_fprintf_WC_MT_D)\
 \

OBJ_BE_FPRINTF_MT_D=\
 $(OBJ_be_fprintf_MT_D)\
 \

OBJ_BEL_FPRINTF_MT_D=\
 $(OBJ_bel_fprintf_MT_D)\
 \

OBJ_BER_FPRINTF_MT_D=\
 $(OBJ_ber_fprintf_MT_D)\
 \

OBJ_BEC_NULL_MT_D=\
 $(OBJ_bec_null_MT_D)\
 \

OBJ_BE_NULL_MT_D=\
 $(OBJ_be_null_MT_D)\
 \

OBJ_BEL_NULL_MT_D=\
 $(OBJ_bel_null_MT_D)\
 \

OBJ_BER_NULL_MT_D=\
 $(OBJ_ber_null_MT_D)\
 \

ifdef ACE
OBJ_BEC_ACE_MT_D=\
 $(OBJ_bec_ACE_MT_D)\
 \

OBJ_BE_ACE_MT_D=\
 $(OBJ_be_ACE_MT_D)\
 \

OBJ_BEL_ACE_MT_D=\
 $(OBJ_bel_ACE_MT_D)\
 \

OBJ_BER_ACE_MT_D=\
 $(OBJ_ber_ACE_MT_D)\
 \

endif # ACE
OBJ_BEC_SYSLOG_MT_D=\
 $(OBJ_bec_SYSLOG_MT_D)\
 \

OBJ_BEC_SYSLOGWC_MT_D=\
 $(OBJ_bec_SYSLOG_WC_MT_D)\
 \

OBJ_BE_SYSLOG_MT_D=\
 $(OBJ_be_SYSLOG_MT_D)\
 \

OBJ_BER_SYSLOG_MT_D=\
 $(OBJ_ber_SYSLOG_MT_D)\
 \

OBJ_FE_ALL_MT_D=\
 $(OBJ_fe_all_MT_D)\
 \

OBJ_FE_ALL_WC_MT_D=\
 $(OBJ_fe_all_WC_MT_D)\
 \

OBJ_FE_FAIL_MT_D=\
 $(OBJ_fe_fail_MT_D)\
 \

OBJ_FE_N_MT_D=\
 $(OBJ_fe_N_MT_D)\
 \

OBJ_FE_N_WC_MT_D=\
 $(OBJ_fe_N_WC_MT_D)\
 \

OBJ_FE_NULL_MT_D=\
 $(OBJ_fe_null_MT_D)\
 \

OBJ_FE_NULL_WC_MT_D=\
 $(OBJ_fe_null_WC_MT_D)\
 \

OBJ_FE_SIMPLE_MT_D=\
 $(OBJ_fe_simple_MT_D)\
 \

OBJ_FE_SIMPLE_WC_MT_D=\
 $(OBJ_fe_simple_WC_MT_D)\
 \

OBJ_utl_bailout_MT_R		= $(PROJ_OBJ_DIR)/util.bailout.mt.o
OBJ_utl_init_code_strs_MT_R	= $(PROJ_OBJ_DIR)/util.init_err_strings.mt.o
OBJ_utl_sev_strs_MT_R		= $(PROJ_OBJ_DIR)/util.sev_strings.mt.o
OBJ_utl_snprintf_MT_R 	= $(PROJ_OBJ_DIR)/util.snprintf.mt.o
OBJ_utl_strdup_MT_R 		= $(PROJ_OBJ_DIR)/util.strdup.mt.o
OBJ_utl_procid_MT_R 		= $(PROJ_OBJ_DIR)/util.procid.mt.o
OBJ_utl_thread_MT_R 		= $(PROJ_OBJ_DIR)/util.thread.mt.o
OBJ_utl_be_context_MT_R	= $(PROJ_OBJ_DIR)/util.be_context.mt.o
OBJ_utl_core_apidefs_MT_R	= $(PROJ_OBJ_DIR)/util.core_apidefs.mt.o
OBJ_utl_be_parse_MT_R		= $(PROJ_OBJ_DIR)/util.be_parse.mt.o
OBJ_utl_strnlen_MT_R		= $(PROJ_OBJ_DIR)/util.strnlen.mt.o
OBJ_utl_strnlenp_MT_R		= $(PROJ_OBJ_DIR)/util.strnlen.c++.mt.o
OBJ_utl_time_MT_R			= $(PROJ_OBJ_DIR)/util.time.mt.o
OBJ_utl_host_name_MT_R	= $(PROJ_OBJ_DIR)/util.host_name.mt.o
OBJ_cr_api_MT_R 			= $(PROJ_OBJ_DIR)/core.api.mt.o
OBJ_cr_api_dep_MT_R 		= $(PROJ_OBJ_DIR)/core.api.deprecated.mt.o
OBJ_cr_api_exit_MT_R 		= $(PROJ_OBJ_DIR)/core.api.exitprocess.mt.o
OBJ_cr_api_logprintf_MT_R	= $(PROJ_OBJ_DIR)/core.api.logprintf.mt.o
OBJ_cr_api_logputs_MT_R	= $(PROJ_OBJ_DIR)/core.api.logputs.mt.o
OBJ_cr_auto_MT_R 			= $(PROJ_OBJ_DIR)/core.auto.mt.o
OBJ_ins_args_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.args.mt.o
OBJ_ins_b64_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.b64.mt.o
OBJ_ins_blob_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.blob.mt.o
OBJ_ins_bool_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.boolean.mt.o
OBJ_ins_excp_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.exception.mt.o
OBJ_ins_host_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.hostid.mt.o
OBJ_ins_int_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.integer.mt.o
OBJ_ins_ptr_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.pointer.mt.o
OBJ_ins_prid_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.processid.mt.o
OBJ_ins_real_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.real.mt.o
OBJ_ins_slice_MT_R	 	= $(PROJ_OBJ_DIR)/inserters.slice.mt.o
OBJ_ins_thrid_MT_R	 	= $(PROJ_OBJ_DIR)/inserters.threadid.mt.o
OBJ_ins_w2m_MT_R	 		= $(PROJ_OBJ_DIR)/inserters.w2m.mt.o
#PDB_bec_test_MT_R 		= $(PROJ_OBJ_DIR)/bec.test.mt.pdb
OBJ_bec_test_MT_R 		= $(PROJ_OBJ_DIR)/bec.test.mt.o
OBJ_be_test_MT_R 			= $(PROJ_OBJ_DIR)/be.test.mt.o
OBJ_bel_test_MT_R 		= $(PROJ_OBJ_DIR)/bel.test.mt.o
OBJ_ber_test_MT_R 		= $(PROJ_OBJ_DIR)/ber.test.mt.o
OBJ_be_lrsplit_MT_R		= $(PROJ_OBJ_DIR)/be.lrsplit.mt.o
OBJ_be_N_MT_R				= $(PROJ_OBJ_DIR)/be.N.mt.o
OBJ_bec_fail_MT_R			= $(PROJ_OBJ_DIR)/bec.fail.mt.o
OBJ_be_fail_MT_R			= $(PROJ_OBJ_DIR)/be.fail.mt.o
OBJ_bel_fail_MT_R			= $(PROJ_OBJ_DIR)/bel.fail.mt.o
OBJ_ber_fail_MT_R			= $(PROJ_OBJ_DIR)/ber.fail.mt.o
OBJ_bec_file_MT_R			= $(PROJ_OBJ_DIR)/bec.file.mt.o
OBJ_bec_file_WC_MT_R		= $(PROJ_OBJ_DIR)/bec.file.WithCallback.mt.o
OBJ_be_file_MT_R			= $(PROJ_OBJ_DIR)/be.file.mt.o
OBJ_bel_file_MT_R			= $(PROJ_OBJ_DIR)/bel.file.mt.o
OBJ_ber_file_MT_R			= $(PROJ_OBJ_DIR)/ber.file.mt.o
OBJ_bec_fprintf_MT_R		= $(PROJ_OBJ_DIR)/bec.fprintf.mt.o
OBJ_bec_fprintf_WC_MT_R	= $(PROJ_OBJ_DIR)/bec.fprintf.WithCallback.mt.o
OBJ_be_fprintf_MT_R		= $(PROJ_OBJ_DIR)/be.fprintf.mt.o
OBJ_bel_fprintf_MT_R		= $(PROJ_OBJ_DIR)/bel.fprintf.mt.o
OBJ_ber_fprintf_MT_R		= $(PROJ_OBJ_DIR)/ber.fprintf.mt.o
OBJ_bec_null_MT_R			= $(PROJ_OBJ_DIR)/bec.null.mt.o
OBJ_be_null_MT_R			= $(PROJ_OBJ_DIR)/be.null.mt.o
OBJ_bel_null_MT_R			= $(PROJ_OBJ_DIR)/bel.null.mt.o
OBJ_ber_null_MT_R			= $(PROJ_OBJ_DIR)/ber.null.mt.o
ifdef ACE
OBJ_bec_ACE_MT_R			= $(PROJ_OBJ_DIR)/bec.ACE.mt.o
OBJ_be_ACE_MT_R			= $(PROJ_OBJ_DIR)/be.ACE.mt.o
OBJ_bel_ACE_MT_R			= $(PROJ_OBJ_DIR)/bel.ACE.mt.o
OBJ_ber_ACE_MT_R			= $(PROJ_OBJ_DIR)/ber.ACE.mt.o
endif # ACE
OBJ_bec_SYSLOG_MT_R		= $(PROJ_OBJ_DIR)/bec.syslog.mt.o
OBJ_bec_SYSLOG_WC_MT_R	= $(PROJ_OBJ_DIR)/bec.syslog.WithCallback.mt.o
OBJ_be_SYSLOG_MT_R 		= $(PROJ_OBJ_DIR)/be.syslog.mt.o
OBJ_ber_SYSLOG_MT_R		= $(PROJ_OBJ_DIR)/ber.syslog.mt.o
OBJ_fe_all_MT_R 			= $(PROJ_OBJ_DIR)/fe.all.mt.o
OBJ_fe_all_WC_MT_R 		= $(PROJ_OBJ_DIR)/fe.all.WithCallback.mt.o
OBJ_fe_fail_MT_R 			= $(PROJ_OBJ_DIR)/fe.fail.mt.o
OBJ_fe_N_MT_R 			= $(PROJ_OBJ_DIR)/fe.N.mt.o
OBJ_fe_N_WC_MT_R 			= $(PROJ_OBJ_DIR)/fe.N.WithCallback.mt.o
OBJ_fe_null_MT_R 			= $(PROJ_OBJ_DIR)/fe.null.mt.o
OBJ_fe_null_WC_MT_R 		= $(PROJ_OBJ_DIR)/fe.null.WithCallback.mt.o
OBJ_fe_simple_MT_R 		= $(PROJ_OBJ_DIR)/fe.simple.mt.o
OBJ_fe_simple_WC_MT_R 	= $(PROJ_OBJ_DIR)/fe.simple.WithCallback.mt.o

OBJ_UTIL_MT_R=\
 $(OBJ_utl_bailout_MT_R)\
 $(OBJ_utl_init_code_strs_MT_R)\
 $(OBJ_utl_sev_strs_MT_R)\
 $(OBJ_utl_snprintf_MT_R)\
 $(OBJ_utl_strdup_MT_R)\
 $(OBJ_utl_procid_MT_R)\
 $(OBJ_utl_thread_MT_R)\
 $(OBJ_utl_be_context_MT_R)\
 $(OBJ_utl_core_apidefs_MT_R)\
 $(OBJ_utl_be_parse_MT_R)\
 $(OBJ_utl_strnlen_MT_R)\
 $(OBJ_utl_strnlenp_MT_R)\
 $(OBJ_utl_time_MT_R)\
 $(OBJ_utl_host_name_MT_R)\
 \

OBJ_CORE_MT_R=\
 $(OBJ_cr_api_MT_R)\
 $(OBJ_cr_api_dep_MT_R)\
 $(OBJ_cr_api_exit_MT_R)\
 $(OBJ_cr_api_logprintf_MT_R)\
 $(OBJ_cr_api_logputs_MT_R)\
 $(OBJ_cr_auto_MT_R)\
 $(OBJ_ins_args_MT_R)\
 $(OBJ_ins_b64_MT_R)\
 $(OBJ_ins_blob_MT_R)\
 $(OBJ_ins_bool_MT_R)\
 $(OBJ_ins_excp_MT_R)\
 $(OBJ_ins_host_MT_R)\
 $(OBJ_ins_int_MT_R)\
 $(OBJ_ins_ptr_MT_R)\
 $(OBJ_ins_prid_MT_R)\
 $(OBJ_ins_real_MT_R)\
 $(OBJ_ins_slice_MT_R)\
 $(OBJ_ins_thrid_MT_R)\
 $(OBJ_ins_w2m_MT_R)\
 \

OBJ_APPL_MT_R=\
 $(OBJ_ins_args_MT_R)\
 $(OBJ_ins_b64_MT_R)\
 $(OBJ_ins_blob_MT_R)\
 $(OBJ_ins_bool_MT_R)\
 $(OBJ_ins_excp_MT_R)\
 $(OBJ_ins_host_MT_R)\
 $(OBJ_ins_int_MT_R)\
 $(OBJ_ins_ptr_MT_R)\
 $(OBJ_ins_prid_MT_R)\
 $(OBJ_ins_real_MT_R)\
 $(OBJ_ins_slice_MT_R)\
 $(OBJ_ins_thrid_MT_R)\
 $(OBJ_ins_w2m_MT_R)\
 \


OBJ_BE_LRSPLIT_MT_R=\
 $(OBJ_be_lrsplit_MT_R)\
 \

OBJ_BE_N_MT_R=\
 $(OBJ_be_N_MT_R)\
 \

OBJ_BEC_TEST_MT_R=\
 $(OBJ_bec_test_MT_R)\
 \


OBJ_BE_TEST_MT_R=\
 $(OBJ_be_test_MT_R)\
 \


OBJ_BEL_TEST_MT_R=\
 $(OBJ_bel_test_MT_R)\
 \


OBJ_BER_TEST_MT_R=\
 $(OBJ_ber_test_MT_R)\
 \


OBJ_BEC_FAIL_MT_R=\
 $(OBJ_bec_fail_MT_R)\
 \

OBJ_BE_FAIL_MT_R=\
 $(OBJ_be_fail_MT_R)\
 \

OBJ_BEL_FAIL_MT_R=\
 $(OBJ_bel_fail_MT_R)\
 \

OBJ_BER_FAIL_MT_R=\
 $(OBJ_ber_fail_MT_R)\
 \

OBJ_BEC_FILE_MT_R=\
 $(OBJ_bec_file_MT_R)\
 \

OBJ_BEC_FILE_WC_MT_R=\
 $(OBJ_bec_file_WC_MT_R)\
 \

OBJ_BE_FILE_MT_R=\
 $(OBJ_be_file_MT_R)\
 \

OBJ_BEL_FILE_MT_R=\
 $(OBJ_bel_file_MT_R)\
 \

OBJ_BER_FILE_MT_R=\
 $(OBJ_ber_file_MT_R)\
 \

OBJ_BEC_FPRINTF_MT_R=\
 $(OBJ_bec_fprintf_MT_R)\
 \

OBJ_BEC_FPRINTF_WC_MT_R=\
 $(OBJ_bec_fprintf_WC_MT_R)\
 \

OBJ_BE_FPRINTF_MT_R=\
 $(OBJ_be_fprintf_MT_R)\
 \

OBJ_BEL_FPRINTF_MT_R=\
 $(OBJ_bel_fprintf_MT_R)\
 \

OBJ_BER_FPRINTF_MT_R=\
 $(OBJ_ber_fprintf_MT_R)\
 \

OBJ_BEC_NULL_MT_R=\
 $(OBJ_bec_null_MT_R)\
 \

OBJ_BE_NULL_MT_R=\
 $(OBJ_be_null_MT_R)\
 \

OBJ_BEL_NULL_MT_R=\
 $(OBJ_bel_null_MT_R)\
 \

OBJ_BER_NULL_MT_R=\
 $(OBJ_ber_null_MT_R)\
 \

ifdef ACE
OBJ_BEC_ACE_MT_R=\
 $(OBJ_bec_ACE_MT_R)\
 \

OBJ_BE_ACE_MT_R=\
 $(OBJ_be_ACE_MT_R)\
 \

OBJ_BEL_ACE_MT_R=\
 $(OBJ_bel_ACE_MT_R)\
 \

OBJ_BER_ACE_MT_R=\
 $(OBJ_ber_ACE_MT_R)\
 \

endif # ACE
OBJ_BEC_SYSLOG_MT_R=\
 $(OBJ_bec_SYSLOG_MT_R)\
 \

OBJ_BEC_SYSLOGWC_MT_R=\
 $(OBJ_bec_SYSLOG_WC_MT_R)\
 \

OBJ_BE_SYSLOG_MT_R=\
 $(OBJ_be_SYSLOG_MT_R)\
 \

OBJ_BER_SYSLOG_MT_R=\
 $(OBJ_ber_SYSLOG_MT_R)\
 \

OBJ_FE_ALL_MT_R=\
 $(OBJ_fe_all_MT_R)\
 \

OBJ_FE_ALL_WC_MT_R=\
 $(OBJ_fe_all_WC_MT_R)\
 \

OBJ_FE_FAIL_MT_R=\
 $(OBJ_fe_fail_MT_R)\
 \

OBJ_FE_N_MT_R=\
 $(OBJ_fe_N_MT_R)\
 \

OBJ_FE_N_WC_MT_R=\
 $(OBJ_fe_N_WC_MT_R)\
 \

OBJ_FE_NULL_MT_R=\
 $(OBJ_fe_null_MT_R)\
 \

OBJ_FE_NULL_WC_MT_R=\
 $(OBJ_fe_null_WC_MT_R)\
 \

OBJ_FE_SIMPLE_MT_R=\
 $(OBJ_fe_simple_MT_R)\
 \

OBJ_FE_SIMPLE_WC_MT_R=\
 $(OBJ_fe_simple_WC_MT_R)\
 \

OBJ_utl_bailout_ST_D		= $(PROJ_OBJ_DIR)/util.bailout.debug.o
OBJ_utl_init_code_strs_ST_D	= $(PROJ_OBJ_DIR)/util.init_err_strings.debug.o
OBJ_utl_sev_strs_ST_D		= $(PROJ_OBJ_DIR)/util.sev_strings.debug.o
OBJ_utl_snprintf_ST_D 	= $(PROJ_OBJ_DIR)/util.snprintf.debug.o
OBJ_utl_strdup_ST_D 		= $(PROJ_OBJ_DIR)/util.strdup.debug.o
OBJ_utl_procid_ST_D 		= $(PROJ_OBJ_DIR)/util.procid.debug.o
OBJ_utl_thread_ST_D 		= $(PROJ_OBJ_DIR)/util.thread.debug.o
OBJ_utl_be_context_ST_D	= $(PROJ_OBJ_DIR)/util.be_context.debug.o
OBJ_utl_core_apidefs_ST_D	= $(PROJ_OBJ_DIR)/util.core_apidefs.debug.o
OBJ_utl_be_parse_ST_D		= $(PROJ_OBJ_DIR)/util.be_parse.debug.o
OBJ_utl_strnlen_ST_D		= $(PROJ_OBJ_DIR)/util.strnlen.debug.o
OBJ_utl_strnlenp_ST_D		= $(PROJ_OBJ_DIR)/util.strnlen.c++.debug.o
OBJ_utl_time_ST_D			= $(PROJ_OBJ_DIR)/util.time.debug.o
OBJ_utl_host_name_ST_D	= $(PROJ_OBJ_DIR)/util.host_name.debug.o
OBJ_cr_api_ST_D 			= $(PROJ_OBJ_DIR)/core.api.debug.o
OBJ_cr_api_dep_ST_D 		= $(PROJ_OBJ_DIR)/core.api.deprecated.debug.o
OBJ_cr_api_exit_ST_D 		= $(PROJ_OBJ_DIR)/core.api.exitprocess.debug.o
OBJ_cr_api_logprintf_ST_D	= $(PROJ_OBJ_DIR)/core.api.logprintf.debug.o
OBJ_cr_api_logputs_ST_D	= $(PROJ_OBJ_DIR)/core.api.logputs.debug.o
OBJ_cr_auto_ST_D 			= $(PROJ_OBJ_DIR)/core.auto.debug.o
OBJ_ins_args_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.args.debug.o
OBJ_ins_b64_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.b64.debug.o
OBJ_ins_blob_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.blob.debug.o
OBJ_ins_bool_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.boolean.debug.o
OBJ_ins_excp_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.exception.debug.o
OBJ_ins_host_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.hostid.debug.o
OBJ_ins_int_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.integer.debug.o
OBJ_ins_ptr_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.pointer.debug.o
OBJ_ins_prid_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.processid.debug.o
OBJ_ins_real_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.real.debug.o
OBJ_ins_slice_ST_D	 	= $(PROJ_OBJ_DIR)/inserters.slice.debug.o
OBJ_ins_thrid_ST_D	 	= $(PROJ_OBJ_DIR)/inserters.threadid.debug.o
OBJ_ins_w2m_ST_D	 		= $(PROJ_OBJ_DIR)/inserters.w2m.debug.o
#PDB_bec_test_ST_D 		= $(PROJ_OBJ_DIR)/bec.test.debug.pdb
OBJ_bec_test_ST_D 		= $(PROJ_OBJ_DIR)/bec.test.debug.o
OBJ_be_test_ST_D 			= $(PROJ_OBJ_DIR)/be.test.debug.o
OBJ_bel_test_ST_D 		= $(PROJ_OBJ_DIR)/bel.test.debug.o
OBJ_ber_test_ST_D 		= $(PROJ_OBJ_DIR)/ber.test.debug.o
OBJ_be_lrsplit_ST_D		= $(PROJ_OBJ_DIR)/be.lrsplit.debug.o
OBJ_be_N_ST_D				= $(PROJ_OBJ_DIR)/be.N.debug.o
OBJ_bec_fail_ST_D			= $(PROJ_OBJ_DIR)/bec.fail.debug.o
OBJ_be_fail_ST_D			= $(PROJ_OBJ_DIR)/be.fail.debug.o
OBJ_bel_fail_ST_D			= $(PROJ_OBJ_DIR)/bel.fail.debug.o
OBJ_ber_fail_ST_D			= $(PROJ_OBJ_DIR)/ber.fail.debug.o
OBJ_bec_file_ST_D			= $(PROJ_OBJ_DIR)/bec.file.debug.o
OBJ_bec_file_WC_ST_D		= $(PROJ_OBJ_DIR)/bec.file.WithCallback.debug.o
OBJ_be_file_ST_D			= $(PROJ_OBJ_DIR)/be.file.debug.o
OBJ_bel_file_ST_D			= $(PROJ_OBJ_DIR)/bel.file.debug.o
OBJ_ber_file_ST_D			= $(PROJ_OBJ_DIR)/ber.file.debug.o
OBJ_bec_fprintf_ST_D		= $(PROJ_OBJ_DIR)/bec.fprintf.debug.o
OBJ_bec_fprintf_WC_ST_D	= $(PROJ_OBJ_DIR)/bec.fprintf.WithCallback.debug.o
OBJ_be_fprintf_ST_D		= $(PROJ_OBJ_DIR)/be.fprintf.debug.o
OBJ_bel_fprintf_ST_D		= $(PROJ_OBJ_DIR)/bel.fprintf.debug.o
OBJ_ber_fprintf_ST_D		= $(PROJ_OBJ_DIR)/ber.fprintf.debug.o
OBJ_bec_null_ST_D			= $(PROJ_OBJ_DIR)/bec.null.debug.o
OBJ_be_null_ST_D			= $(PROJ_OBJ_DIR)/be.null.debug.o
OBJ_bel_null_ST_D			= $(PROJ_OBJ_DIR)/bel.null.debug.o
OBJ_ber_null_ST_D			= $(PROJ_OBJ_DIR)/ber.null.debug.o
ifdef ACE
OBJ_bec_ACE_ST_D			= $(PROJ_OBJ_DIR)/bec.ACE.debug.o
OBJ_be_ACE_ST_D			= $(PROJ_OBJ_DIR)/be.ACE.debug.o
OBJ_bel_ACE_ST_D			= $(PROJ_OBJ_DIR)/bel.ACE.debug.o
OBJ_ber_ACE_ST_D			= $(PROJ_OBJ_DIR)/ber.ACE.debug.o
endif # ACE
OBJ_bec_SYSLOG_ST_D		= $(PROJ_OBJ_DIR)/bec.syslog.debug.o
OBJ_bec_SYSLOG_WC_ST_D	= $(PROJ_OBJ_DIR)/bec.syslog.WithCallback.debug.o
OBJ_be_SYSLOG_ST_D 		= $(PROJ_OBJ_DIR)/be.syslog.debug.o
OBJ_ber_SYSLOG_ST_D		= $(PROJ_OBJ_DIR)/ber.syslog.debug.o
OBJ_fe_all_ST_D 			= $(PROJ_OBJ_DIR)/fe.all.debug.o
OBJ_fe_all_WC_ST_D 		= $(PROJ_OBJ_DIR)/fe.all.WithCallback.debug.o
OBJ_fe_fail_ST_D 			= $(PROJ_OBJ_DIR)/fe.fail.debug.o
OBJ_fe_N_ST_D 			= $(PROJ_OBJ_DIR)/fe.N.debug.o
OBJ_fe_N_WC_ST_D 			= $(PROJ_OBJ_DIR)/fe.N.WithCallback.debug.o
OBJ_fe_null_ST_D 			= $(PROJ_OBJ_DIR)/fe.null.debug.o
OBJ_fe_null_WC_ST_D 		= $(PROJ_OBJ_DIR)/fe.null.WithCallback.debug.o
OBJ_fe_simple_ST_D 		= $(PROJ_OBJ_DIR)/fe.simple.debug.o
OBJ_fe_simple_WC_ST_D 	= $(PROJ_OBJ_DIR)/fe.simple.WithCallback.debug.o

OBJ_UTIL_ST_D=\
 $(OBJ_utl_bailout_ST_D)\
 $(OBJ_utl_init_code_strs_ST_D)\
 $(OBJ_utl_sev_strs_ST_D)\
 $(OBJ_utl_snprintf_ST_D)\
 $(OBJ_utl_strdup_ST_D)\
 $(OBJ_utl_procid_ST_D)\
 $(OBJ_utl_thread_ST_D)\
 $(OBJ_utl_be_context_ST_D)\
 $(OBJ_utl_core_apidefs_ST_D)\
 $(OBJ_utl_be_parse_ST_D)\
 $(OBJ_utl_strnlen_ST_D)\
 $(OBJ_utl_strnlenp_ST_D)\
 $(OBJ_utl_time_ST_D)\
 $(OBJ_utl_host_name_ST_D)\
 \

OBJ_CORE_ST_D=\
 $(OBJ_cr_api_ST_D)\
 $(OBJ_cr_api_dep_ST_D)\
 $(OBJ_cr_api_exit_ST_D)\
 $(OBJ_cr_api_logprintf_ST_D)\
 $(OBJ_cr_api_logputs_ST_D)\
 $(OBJ_cr_auto_ST_D)\
 $(OBJ_ins_args_ST_D)\
 $(OBJ_ins_b64_ST_D)\
 $(OBJ_ins_blob_ST_D)\
 $(OBJ_ins_bool_ST_D)\
 $(OBJ_ins_excp_ST_D)\
 $(OBJ_ins_host_ST_D)\
 $(OBJ_ins_int_ST_D)\
 $(OBJ_ins_ptr_ST_D)\
 $(OBJ_ins_prid_ST_D)\
 $(OBJ_ins_real_ST_D)\
 $(OBJ_ins_slice_ST_D)\
 $(OBJ_ins_thrid_ST_D)\
 $(OBJ_ins_w2m_ST_D)\
 \

OBJ_APPL_ST_D=\
 $(OBJ_ins_args_ST_D)\
 $(OBJ_ins_b64_ST_D)\
 $(OBJ_ins_blob_ST_D)\
 $(OBJ_ins_bool_ST_D)\
 $(OBJ_ins_excp_ST_D)\
 $(OBJ_ins_host_ST_D)\
 $(OBJ_ins_int_ST_D)\
 $(OBJ_ins_ptr_ST_D)\
 $(OBJ_ins_prid_ST_D)\
 $(OBJ_ins_real_ST_D)\
 $(OBJ_ins_slice_ST_D)\
 $(OBJ_ins_thrid_ST_D)\
 $(OBJ_ins_w2m_ST_D)\
 \


OBJ_BE_LRSPLIT_ST_D=\
 $(OBJ_be_lrsplit_ST_D)\
 \

OBJ_BE_N_ST_D=\
 $(OBJ_be_N_ST_D)\
 \

OBJ_BEC_TEST_ST_D=\
 $(OBJ_bec_test_ST_D)\
 \


OBJ_BE_TEST_ST_D=\
 $(OBJ_be_test_ST_D)\
 \


OBJ_BEL_TEST_ST_D=\
 $(OBJ_bel_test_ST_D)\
 \


OBJ_BER_TEST_ST_D=\
 $(OBJ_ber_test_ST_D)\
 \


OBJ_BEC_FAIL_ST_D=\
 $(OBJ_bec_fail_ST_D)\
 \

OBJ_BE_FAIL_ST_D=\
 $(OBJ_be_fail_ST_D)\
 \

OBJ_BEL_FAIL_ST_D=\
 $(OBJ_bel_fail_ST_D)\
 \

OBJ_BER_FAIL_ST_D=\
 $(OBJ_ber_fail_ST_D)\
 \

OBJ_BEC_FILE_ST_D=\
 $(OBJ_bec_file_ST_D)\
 \

OBJ_BEC_FILE_WC_ST_D=\
 $(OBJ_bec_file_WC_ST_D)\
 \

OBJ_BE_FILE_ST_D=\
 $(OBJ_be_file_ST_D)\
 \

OBJ_BEL_FILE_ST_D=\
 $(OBJ_bel_file_ST_D)\
 \

OBJ_BER_FILE_ST_D=\
 $(OBJ_ber_file_ST_D)\
 \

OBJ_BEC_FPRINTF_ST_D=\
 $(OBJ_bec_fprintf_ST_D)\
 \

OBJ_BEC_FPRINTF_WC_ST_D=\
 $(OBJ_bec_fprintf_WC_ST_D)\
 \

OBJ_BE_FPRINTF_ST_D=\
 $(OBJ_be_fprintf_ST_D)\
 \

OBJ_BEL_FPRINTF_ST_D=\
 $(OBJ_bel_fprintf_ST_D)\
 \

OBJ_BER_FPRINTF_ST_D=\
 $(OBJ_ber_fprintf_ST_D)\
 \

OBJ_BEC_NULL_ST_D=\
 $(OBJ_bec_null_ST_D)\
 \

OBJ_BE_NULL_ST_D=\
 $(OBJ_be_null_ST_D)\
 \

OBJ_BEL_NULL_ST_D=\
 $(OBJ_bel_null_ST_D)\
 \

OBJ_BER_NULL_ST_D=\
 $(OBJ_ber_null_ST_D)\
 \

ifdef ACE
OBJ_BEC_ACE_ST_D=\
 $(OBJ_bec_ACE_ST_D)\
 \

OBJ_BE_ACE_ST_D=\
 $(OBJ_be_ACE_ST_D)\
 \

OBJ_BEL_ACE_ST_D=\
 $(OBJ_bel_ACE_ST_D)\
 \

OBJ_BER_ACE_ST_D=\
 $(OBJ_ber_ACE_ST_D)\
 \

endif # ACE
OBJ_BEC_SYSLOG_ST_D=\
 $(OBJ_bec_SYSLOG_ST_D)\
 \

OBJ_BEC_SYSLOGWC_ST_D=\
 $(OBJ_bec_SYSLOG_WC_ST_D)\
 \

OBJ_BE_SYSLOG_ST_D=\
 $(OBJ_be_SYSLOG_ST_D)\
 \

OBJ_BER_SYSLOG_ST_D=\
 $(OBJ_ber_SYSLOG_ST_D)\
 \

OBJ_FE_ALL_ST_D=\
 $(OBJ_fe_all_ST_D)\
 \

OBJ_FE_ALL_WC_ST_D=\
 $(OBJ_fe_all_WC_ST_D)\
 \

OBJ_FE_FAIL_ST_D=\
 $(OBJ_fe_fail_ST_D)\
 \

OBJ_FE_N_ST_D=\
 $(OBJ_fe_N_ST_D)\
 \

OBJ_FE_N_WC_ST_D=\
 $(OBJ_fe_N_WC_ST_D)\
 \

OBJ_FE_NULL_ST_D=\
 $(OBJ_fe_null_ST_D)\
 \

OBJ_FE_NULL_WC_ST_D=\
 $(OBJ_fe_null_WC_ST_D)\
 \

OBJ_FE_SIMPLE_ST_D=\
 $(OBJ_fe_simple_ST_D)\
 \

OBJ_FE_SIMPLE_WC_ST_D=\
 $(OBJ_fe_simple_WC_ST_D)\
 \

OBJ_utl_bailout_ST_R		= $(PROJ_OBJ_DIR)/util.bailout.o
OBJ_utl_init_code_strs_ST_R	= $(PROJ_OBJ_DIR)/util.init_err_strings.o
OBJ_utl_sev_strs_ST_R		= $(PROJ_OBJ_DIR)/util.sev_strings.o
OBJ_utl_snprintf_ST_R 	= $(PROJ_OBJ_DIR)/util.snprintf.o
OBJ_utl_strdup_ST_R 		= $(PROJ_OBJ_DIR)/util.strdup.o
OBJ_utl_procid_ST_R 		= $(PROJ_OBJ_DIR)/util.procid.o
OBJ_utl_thread_ST_R 		= $(PROJ_OBJ_DIR)/util.thread.o
OBJ_utl_be_context_ST_R	= $(PROJ_OBJ_DIR)/util.be_context.o
OBJ_utl_core_apidefs_ST_R	= $(PROJ_OBJ_DIR)/util.core_apidefs.o
OBJ_utl_be_parse_ST_R		= $(PROJ_OBJ_DIR)/util.be_parse.o
OBJ_utl_strnlen_ST_R		= $(PROJ_OBJ_DIR)/util.strnlen.o
OBJ_utl_strnlenp_ST_R		= $(PROJ_OBJ_DIR)/util.strnlen.c++.o
OBJ_utl_time_ST_R			= $(PROJ_OBJ_DIR)/util.time.o
OBJ_utl_host_name_ST_R	= $(PROJ_OBJ_DIR)/util.host_name.o
OBJ_cr_api_ST_R 			= $(PROJ_OBJ_DIR)/core.api.o
OBJ_cr_api_dep_ST_R 		= $(PROJ_OBJ_DIR)/core.api.deprecated.o
OBJ_cr_api_exit_ST_R 		= $(PROJ_OBJ_DIR)/core.api.exitprocess.o
OBJ_cr_api_logprintf_ST_R	= $(PROJ_OBJ_DIR)/core.api.logprintf.o
OBJ_cr_api_logputs_ST_R	= $(PROJ_OBJ_DIR)/core.api.logputs.o
OBJ_cr_auto_ST_R 			= $(PROJ_OBJ_DIR)/core.auto.o
OBJ_ins_args_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.args.o
OBJ_ins_b64_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.b64.o
OBJ_ins_blob_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.blob.o
OBJ_ins_bool_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.boolean.o
OBJ_ins_excp_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.exception.o
OBJ_ins_host_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.hostid.o
OBJ_ins_int_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.integer.o
OBJ_ins_ptr_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.pointer.o
OBJ_ins_prid_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.processid.o
OBJ_ins_real_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.real.o
OBJ_ins_slice_ST_R	 	= $(PROJ_OBJ_DIR)/inserters.slice.o
OBJ_ins_thrid_ST_R	 	= $(PROJ_OBJ_DIR)/inserters.threadid.o
OBJ_ins_w2m_ST_R	 		= $(PROJ_OBJ_DIR)/inserters.w2m.o
#PDB_bec_test_ST_R 		= $(PROJ_OBJ_DIR)/bec.test.pdb
OBJ_bec_test_ST_R 		= $(PROJ_OBJ_DIR)/bec.test.o
OBJ_be_test_ST_R 			= $(PROJ_OBJ_DIR)/be.test.o
OBJ_bel_test_ST_R 		= $(PROJ_OBJ_DIR)/bel.test.o
OBJ_ber_test_ST_R 		= $(PROJ_OBJ_DIR)/ber.test.o
OBJ_be_lrsplit_ST_R		= $(PROJ_OBJ_DIR)/be.lrsplit.o
OBJ_be_N_ST_R				= $(PROJ_OBJ_DIR)/be.N.o
OBJ_bec_fail_ST_R			= $(PROJ_OBJ_DIR)/bec.fail.o
OBJ_be_fail_ST_R			= $(PROJ_OBJ_DIR)/be.fail.o
OBJ_bel_fail_ST_R			= $(PROJ_OBJ_DIR)/bel.fail.o
OBJ_ber_fail_ST_R			= $(PROJ_OBJ_DIR)/ber.fail.o
OBJ_bec_file_ST_R			= $(PROJ_OBJ_DIR)/bec.file.o
OBJ_bec_file_WC_ST_R		= $(PROJ_OBJ_DIR)/bec.file.WithCallback.o
OBJ_be_file_ST_R			= $(PROJ_OBJ_DIR)/be.file.o
OBJ_bel_file_ST_R			= $(PROJ_OBJ_DIR)/bel.file.o
OBJ_ber_file_ST_R			= $(PROJ_OBJ_DIR)/ber.file.o
OBJ_bec_fprintf_ST_R		= $(PROJ_OBJ_DIR)/bec.fprintf.o
OBJ_bec_fprintf_WC_ST_R	= $(PROJ_OBJ_DIR)/bec.fprintf.WithCallback.o
OBJ_be_fprintf_ST_R		= $(PROJ_OBJ_DIR)/be.fprintf.o
OBJ_bel_fprintf_ST_R		= $(PROJ_OBJ_DIR)/bel.fprintf.o
OBJ_ber_fprintf_ST_R		= $(PROJ_OBJ_DIR)/ber.fprintf.o
OBJ_bec_null_ST_R			= $(PROJ_OBJ_DIR)/bec.null.o
OBJ_be_null_ST_R			= $(PROJ_OBJ_DIR)/be.null.o
OBJ_bel_null_ST_R			= $(PROJ_OBJ_DIR)/bel.null.o
OBJ_ber_null_ST_R			= $(PROJ_OBJ_DIR)/ber.null.o
ifdef ACE
OBJ_bec_ACE_ST_R			= $(PROJ_OBJ_DIR)/bec.ACE.o
OBJ_be_ACE_ST_R			= $(PROJ_OBJ_DIR)/be.ACE.o
OBJ_bel_ACE_ST_R			= $(PROJ_OBJ_DIR)/bel.ACE.o
OBJ_ber_ACE_ST_R			= $(PROJ_OBJ_DIR)/ber.ACE.o
endif # ACE
OBJ_bec_SYSLOG_ST_R		= $(PROJ_OBJ_DIR)/bec.syslog.o
OBJ_bec_SYSLOG_WC_ST_R	= $(PROJ_OBJ_DIR)/bec.syslog.WithCallback.o
OBJ_be_SYSLOG_ST_R 		= $(PROJ_OBJ_DIR)/be.syslog.o
OBJ_ber_SYSLOG_ST_R		= $(PROJ_OBJ_DIR)/ber.syslog.o
OBJ_fe_all_ST_R 			= $(PROJ_OBJ_DIR)/fe.all.o
OBJ_fe_all_WC_ST_R 		= $(PROJ_OBJ_DIR)/fe.all.WithCallback.o
OBJ_fe_fail_ST_R 			= $(PROJ_OBJ_DIR)/fe.fail.o
OBJ_fe_N_ST_R 			= $(PROJ_OBJ_DIR)/fe.N.o
OBJ_fe_N_WC_ST_R 			= $(PROJ_OBJ_DIR)/fe.N.WithCallback.o
OBJ_fe_null_ST_R 			= $(PROJ_OBJ_DIR)/fe.null.o
OBJ_fe_null_WC_ST_R 		= $(PROJ_OBJ_DIR)/fe.null.WithCallback.o
OBJ_fe_simple_ST_R 		= $(PROJ_OBJ_DIR)/fe.simple.o
OBJ_fe_simple_WC_ST_R 	= $(PROJ_OBJ_DIR)/fe.simple.WithCallback.o

OBJ_UTIL_ST_R=\
 $(OBJ_utl_bailout_ST_R)\
 $(OBJ_utl_init_code_strs_ST_R)\
 $(OBJ_utl_sev_strs_ST_R)\
 $(OBJ_utl_snprintf_ST_R)\
 $(OBJ_utl_strdup_ST_R)\
 $(OBJ_utl_procid_ST_R)\
 $(OBJ_utl_thread_ST_R)\
 $(OBJ_utl_be_context_ST_R)\
 $(OBJ_utl_core_apidefs_ST_R)\
 $(OBJ_utl_be_parse_ST_R)\
 $(OBJ_utl_strnlen_ST_R)\
 $(OBJ_utl_strnlenp_ST_R)\
 $(OBJ_utl_time_ST_R)\
 $(OBJ_utl_host_name_ST_R)\
 \

OBJ_CORE_ST_R=\
 $(OBJ_cr_api_ST_R)\
 $(OBJ_cr_api_dep_ST_R)\
 $(OBJ_cr_api_exit_ST_R)\
 $(OBJ_cr_api_logprintf_ST_R)\
 $(OBJ_cr_api_logputs_ST_R)\
 $(OBJ_cr_auto_ST_R)\
 $(OBJ_ins_args_ST_R)\
 $(OBJ_ins_b64_ST_R)\
 $(OBJ_ins_blob_ST_R)\
 $(OBJ_ins_bool_ST_R)\
 $(OBJ_ins_excp_ST_R)\
 $(OBJ_ins_host_ST_R)\
 $(OBJ_ins_int_ST_R)\
 $(OBJ_ins_ptr_ST_R)\
 $(OBJ_ins_prid_ST_R)\
 $(OBJ_ins_real_ST_R)\
 $(OBJ_ins_slice_ST_R)\
 $(OBJ_ins_thrid_ST_R)\
 $(OBJ_ins_w2m_ST_R)\
 \

OBJ_APPL_ST_R=\
 $(OBJ_ins_args_ST_R)\
 $(OBJ_ins_b64_ST_R)\
 $(OBJ_ins_blob_ST_R)\
 $(OBJ_ins_bool_ST_R)\
 $(OBJ_ins_excp_ST_R)\
 $(OBJ_ins_host_ST_R)\
 $(OBJ_ins_int_ST_R)\
 $(OBJ_ins_ptr_ST_R)\
 $(OBJ_ins_prid_ST_R)\
 $(OBJ_ins_real_ST_R)\
 $(OBJ_ins_slice_ST_R)\
 $(OBJ_ins_thrid_ST_R)\
 $(OBJ_ins_w2m_ST_R)\
 \


OBJ_BE_LRSPLIT_ST_R=\
 $(OBJ_be_lrsplit_ST_R)\
 \

OBJ_BE_N_ST_R=\
 $(OBJ_be_N_ST_R)\
 \

OBJ_BEC_TEST_ST_R=\
 $(OBJ_bec_test_ST_R)\
 \


OBJ_BE_TEST_ST_R=\
 $(OBJ_be_test_ST_R)\
 \


OBJ_BEL_TEST_ST_R=\
 $(OBJ_bel_test_ST_R)\
 \


OBJ_BER_TEST_ST_R=\
 $(OBJ_ber_test_ST_R)\
 \


OBJ_BEC_FAIL_ST_R=\
 $(OBJ_bec_fail_ST_R)\
 \

OBJ_BE_FAIL_ST_R=\
 $(OBJ_be_fail_ST_R)\
 \

OBJ_BEL_FAIL_ST_R=\
 $(OBJ_bel_fail_ST_R)\
 \

OBJ_BER_FAIL_ST_R=\
 $(OBJ_ber_fail_ST_R)\
 \

OBJ_BEC_FILE_ST_R=\
 $(OBJ_bec_file_ST_R)\
 \

OBJ_BEC_FILE_WC_ST_R=\
 $(OBJ_bec_file_WC_ST_R)\
 \

OBJ_BE_FILE_ST_R=\
 $(OBJ_be_file_ST_R)\
 \

OBJ_BEL_FILE_ST_R=\
 $(OBJ_bel_file_ST_R)\
 \

OBJ_BER_FILE_ST_R=\
 $(OBJ_ber_file_ST_R)\
 \

OBJ_BEC_FPRINTF_ST_R=\
 $(OBJ_bec_fprintf_ST_R)\
 \

OBJ_BEC_FPRINTF_WC_ST_R=\
 $(OBJ_bec_fprintf_WC_ST_R)\
 \

OBJ_BE_FPRINTF_ST_R=\
 $(OBJ_be_fprintf_ST_R)\
 \

OBJ_BEL_FPRINTF_ST_R=\
 $(OBJ_bel_fprintf_ST_R)\
 \

OBJ_BER_FPRINTF_ST_R=\
 $(OBJ_ber_fprintf_ST_R)\
 \

OBJ_BEC_NULL_ST_R=\
 $(OBJ_bec_null_ST_R)\
 \

OBJ_BE_NULL_ST_R=\
 $(OBJ_be_null_ST_R)\
 \

OBJ_BEL_NULL_ST_R=\
 $(OBJ_bel_null_ST_R)\
 \

OBJ_BER_NULL_ST_R=\
 $(OBJ_ber_null_ST_R)\
 \

ifdef ACE
OBJ_BEC_ACE_ST_R=\
 $(OBJ_bec_ACE_ST_R)\
 \

OBJ_BE_ACE_ST_R=\
 $(OBJ_be_ACE_ST_R)\
 \

OBJ_BEL_ACE_ST_R=\
 $(OBJ_bel_ACE_ST_R)\
 \

OBJ_BER_ACE_ST_R=\
 $(OBJ_ber_ACE_ST_R)\
 \

endif # ACE
OBJ_BEC_SYSLOG_ST_R=\
 $(OBJ_bec_SYSLOG_ST_R)\
 \

OBJ_BEC_SYSLOGWC_ST_R=\
 $(OBJ_bec_SYSLOG_WC_ST_R)\
 \

OBJ_BE_SYSLOG_ST_R=\
 $(OBJ_be_SYSLOG_ST_R)\
 \

OBJ_BER_SYSLOG_ST_R=\
 $(OBJ_ber_SYSLOG_ST_R)\
 \

OBJ_FE_ALL_ST_R=\
 $(OBJ_fe_all_ST_R)\
 \

OBJ_FE_ALL_WC_ST_R=\
 $(OBJ_fe_all_WC_ST_R)\
 \

OBJ_FE_FAIL_ST_R=\
 $(OBJ_fe_fail_ST_R)\
 \

OBJ_FE_N_ST_R=\
 $(OBJ_fe_N_ST_R)\
 \

OBJ_FE_N_WC_ST_R=\
 $(OBJ_fe_N_WC_ST_R)\
 \

OBJ_FE_NULL_ST_R=\
 $(OBJ_fe_null_ST_R)\
 \

OBJ_FE_NULL_WC_ST_R=\
 $(OBJ_fe_null_WC_ST_R)\
 \

OBJ_FE_SIMPLE_ST_R=\
 $(OBJ_fe_simple_ST_R)\
 \

OBJ_FE_SIMPLE_WC_ST_R=\
 $(OBJ_fe_simple_WC_ST_R)\
 \


# b64

OBJ_b64_b64_MT_D            = $(PROJ_OBJ_DIR)/b64.pan.b64.mt.debug.o

OBJ_B64_MT_D=\
 $(OBJ_b64_b64_MT_D)\
 \

OBJ_b64_b64_MT_R            = $(PROJ_OBJ_DIR)/b64.pan.b64.mt.o

OBJ_B64_MT_R=\
 $(OBJ_b64_b64_MT_R)\
 \

OBJ_b64_b64_ST_D            = $(PROJ_OBJ_DIR)/b64.pan.b64.debug.o

OBJ_B64_ST_D=\
 $(OBJ_b64_b64_ST_D)\
 \

OBJ_b64_b64_ST_R            = $(PROJ_OBJ_DIR)/b64.pan.b64.o

OBJ_B64_ST_R=\
 $(OBJ_b64_b64_ST_R)\
 \


# shwild

OBJ_shw_api_MT_D            = $(PROJ_OBJ_DIR)/shwild.pan.api.mt.debug.o
OBJ_shw_matches_MT_D        = $(PROJ_OBJ_DIR)/shwild.pan.matches.mt.debug.o
OBJ_shw_pattern_MT_D        = $(PROJ_OBJ_DIR)/shwild.pan.pattern.mt.debug.o

OBJ_SHWILD_MT_D=\
 $(OBJ_shw_api_MT_D)\
 $(OBJ_shw_matches_MT_D)\
 $(OBJ_shw_pattern_MT_D)\
 \

OBJ_shw_api_MT_R            = $(PROJ_OBJ_DIR)/shwild.pan.api.mt.o
OBJ_shw_matches_MT_R        = $(PROJ_OBJ_DIR)/shwild.pan.matches.mt.o
OBJ_shw_pattern_MT_R        = $(PROJ_OBJ_DIR)/shwild.pan.pattern.mt.o

OBJ_SHWILD_MT_R=\
 $(OBJ_shw_api_MT_R)\
 $(OBJ_shw_matches_MT_R)\
 $(OBJ_shw_pattern_MT_R)\
 \

OBJ_shw_api_ST_D            = $(PROJ_OBJ_DIR)/shwild.pan.api.debug.o
OBJ_shw_matches_ST_D        = $(PROJ_OBJ_DIR)/shwild.pan.matches.debug.o
OBJ_shw_pattern_ST_D        = $(PROJ_OBJ_DIR)/shwild.pan.pattern.debug.o

OBJ_SHWILD_ST_D=\
 $(OBJ_shw_api_ST_D)\
 $(OBJ_shw_matches_ST_D)\
 $(OBJ_shw_pattern_ST_D)\
 \

OBJ_shw_api_ST_R            = $(PROJ_OBJ_DIR)/shwild.pan.api.o
OBJ_shw_matches_ST_R        = $(PROJ_OBJ_DIR)/shwild.pan.matches.o
OBJ_shw_pattern_ST_R        = $(PROJ_OBJ_DIR)/shwild.pan.pattern.o

OBJ_SHWILD_ST_R=\
 $(OBJ_shw_api_ST_R)\
 $(OBJ_shw_matches_ST_R)\
 $(OBJ_shw_pattern_ST_R)\
 \


# xtests

OBJ_xts_core_MT_D           = $(PROJ_OBJ_DIR)/xtests.pan.core.api.mt.debug.o

OBJ_XTESTS_MT_D=\
 $(OBJ_xts_core_MT_D)\
 \

OBJ_xts_core_MT_R           = $(PROJ_OBJ_DIR)/xtests.pan.core.api.mt.o

OBJ_XTESTS_MT_R=\
 $(OBJ_xts_core_MT_R)\
 \

OBJ_xts_core_ST_D           = $(PROJ_OBJ_DIR)/xtests.pan.core.api.debug.o

OBJ_XTESTS_ST_D=\
 $(OBJ_xts_core_ST_D)\
 \

OBJ_xts_core_ST_R           = $(PROJ_OBJ_DIR)/xtests.pan.core.api.o

OBJ_XTESTS_ST_R=\
 $(OBJ_xts_core_ST_R)\
 \



########################################
# Examples

OBJ_EX_P_be_01_MT_D	=	./ex.cpp.be.01.mt.debug.o
OBJ_EX_P_be_02_MT_D	=	./ex.cpp.be.02.mt.debug.o
OBJ_EX_P_be_03_MT_D	=	./ex.cpp.be.03.mt.debug.o
OBJ_EX_P_be_04_MT_D	=	./ex.cpp.be.04.mt.debug.o
OBJ_EX_P_be_05_MT_D	=	./ex.cpp.be.05.mt.debug.o
OBJ_EX_P_be_06_MT_D	=	./ex.cpp.be.06.mt.debug.o
OBJ_EX_P_cp_01_MT_D	=	./ex.cpp.cp.01.mt.debug.o
OBJ_EX_P_cp_02_MT_D	=	./ex.cpp.cp.02.mt.debug.o
OBJ_EX_P_fm_01_MT_D	=	./ex.cpp.fm.01.mt.debug.o
OBJ_EX_P_fm_02_MT_D	=	./ex.cpp.fm.02.mt.debug.o
OBJ_EX_P_fm_03_MT_D	=	./ex.cpp.fm.03.mt.debug.o
OBJ_EX_P_fe_01_MT_D	=	./ex.cpp.fe.01.mt.debug.o
OBJ_EX_P_in_01_MT_D	=	./ex.cpp.in.01.mt.debug.o
OBJ_EX_P_in_02_MT_D	=	./ex.cpp.in.02.mt.debug.o
OBJ_EX_P_in_03_MT_D	=	./ex.cpp.in.03.mt.debug.o
OBJ_EX_P_in_04_MT_D	=	./ex.cpp.in.04.mt.debug.o
OBJ_EX_P_in_05_MT_D	=	./ex.cpp.in.05.mt.debug.o
OBJ_EX_P_in_06_MT_D	=	./ex.cpp.in.06.mt.debug.o
OBJ_EX_P_in_07_MT_D	=	./ex.cpp.in.07.mt.debug.o
OBJ_EX_P_in_08_MT_D	=	./ex.cpp.in.08.mt.debug.o
OBJ_EX_P_in_09_MT_D	=	./ex.cpp.in.09.mt.debug.o
OBJ_EX_P_in_10_MT_D	=	./ex.cpp.in.10.mt.debug.o
OBJ_EX_P_in_11_MT_D	=	./ex.cpp.in.11.mt.debug.o
OBJ_EX_P_in_12_MT_D	=	./ex.cpp.in.12.mt.debug.o
OBJ_EX_P_in_13_MT_D	=	./ex.cpp.in.13.mt.debug.o
OBJ_EX_P_in_14_MT_D	=	./ex.cpp.in.14.mt.debug.o
OBJ_EX_P_in_15_MT_D	=	./ex.cpp.in.15.mt.debug.o
OBJ_EX_P_in_16_MT_D	=	./ex.cpp.in.16.mt.debug.o
OBJ_EX_P_in_17_MT_D	=	./ex.cpp.in.17.mt.debug.o
OBJ_EX_P_ms_01_MT_D	=	./ex.cpp.m2.01.mt.debug.o
OBJ_EX_P_ms_02_MT_D	=	./ex.cpp.m2.02.mt.debug.o
OBJ_EX_P_ms_03_MT_D	=	./ex.cpp.m2.03.mt.debug.o
OBJ_EX_P_ms_04_MT_D	=	./ex.cpp.m2.04.mt.debug.o
OBJ_EX_P_ms_05_MT_D	=	./ex.cpp.m2.05.mt.debug.o
OBJ_EX_P_ms_06_MT_D	=	./ex.cpp.ms.06.mt.debug.o
OBJ_EX_P_tr_01_MT_D	=	./ex.cpp.tr.01.mt.debug.o
OBJ_EX_P_tr_02_MT_D	=	./ex.cpp.tr.02.mt.debug.o
OBJ_EX_P_ut_01_MT_D	=	./ex.cpp.ut.01.mt.debug.o
OBJ_EX_C_0_MT_D		=	./ex.c.0.mt.debug.o
OBJ_EX_C_1_MT_D		=	./ex.c.1.mt.debug.o
OBJ_EX_C_2_MT_D		=	./ex.c.2.mt.debug.o
OBJ_EX_C_3_MT_D		=	./ex.c.3.mt.debug.o
OBJ_EX_C_4_MT_D		=	./ex.c.4.mt.debug.o
OBJ_EX_C_5_MT_D		=	./ex.c.5.mt.debug.o
OBJ_EX_C_6_MT_D		=	./ex.c.6.mt.debug.o
OBJ_EX_C_7_MT_D		=	./ex.c.7.mt.debug.o
OBJ_EX_P_be_01_MT_R	=	./ex.cpp.be.01.mt.o
OBJ_EX_P_be_02_MT_R	=	./ex.cpp.be.02.mt.o
OBJ_EX_P_be_03_MT_R	=	./ex.cpp.be.03.mt.o
OBJ_EX_P_be_04_MT_R	=	./ex.cpp.be.04.mt.o
OBJ_EX_P_be_05_MT_R	=	./ex.cpp.be.05.mt.o
OBJ_EX_P_be_06_MT_R	=	./ex.cpp.be.06.mt.o
OBJ_EX_P_cp_01_MT_R	=	./ex.cpp.cp.01.mt.o
OBJ_EX_P_cp_02_MT_R	=	./ex.cpp.cp.02.mt.o
OBJ_EX_P_fm_01_MT_R	=	./ex.cpp.fm.01.mt.o
OBJ_EX_P_fm_02_MT_R	=	./ex.cpp.fm.02.mt.o
OBJ_EX_P_fm_03_MT_R	=	./ex.cpp.fm.03.mt.o
OBJ_EX_P_fe_01_MT_R	=	./ex.cpp.fe.01.mt.o
OBJ_EX_P_in_01_MT_R	=	./ex.cpp.in.01.mt.o
OBJ_EX_P_in_02_MT_R	=	./ex.cpp.in.02.mt.o
OBJ_EX_P_in_03_MT_R	=	./ex.cpp.in.03.mt.o
OBJ_EX_P_in_04_MT_R	=	./ex.cpp.in.04.mt.o
OBJ_EX_P_in_05_MT_R	=	./ex.cpp.in.05.mt.o
OBJ_EX_P_in_06_MT_R	=	./ex.cpp.in.06.mt.o
OBJ_EX_P_in_07_MT_R	=	./ex.cpp.in.07.mt.o
OBJ_EX_P_in_08_MT_R	=	./ex.cpp.in.08.mt.o
OBJ_EX_P_in_09_MT_R	=	./ex.cpp.in.09.mt.o
OBJ_EX_P_in_10_MT_R	=	./ex.cpp.in.10.mt.o
OBJ_EX_P_in_11_MT_R	=	./ex.cpp.in.11.mt.o
OBJ_EX_P_in_12_MT_R	=	./ex.cpp.in.12.mt.o
OBJ_EX_P_in_13_MT_R	=	./ex.cpp.in.13.mt.o
OBJ_EX_P_in_14_MT_R	=	./ex.cpp.in.14.mt.o
OBJ_EX_P_in_15_MT_R	=	./ex.cpp.in.15.mt.o
OBJ_EX_P_in_16_MT_R	=	./ex.cpp.in.16.mt.o
OBJ_EX_P_in_17_MT_R	=	./ex.cpp.in.17.mt.o
OBJ_EX_P_ms_01_MT_R	=	./ex.cpp.m2.01.mt.o
OBJ_EX_P_ms_02_MT_R	=	./ex.cpp.m2.02.mt.o
OBJ_EX_P_ms_03_MT_R	=	./ex.cpp.m2.03.mt.o
OBJ_EX_P_ms_04_MT_R	=	./ex.cpp.m2.04.mt.o
OBJ_EX_P_ms_05_MT_R	=	./ex.cpp.m2.05.mt.o
OBJ_EX_P_ms_06_MT_R	=	./ex.cpp.ms.06.mt.o
OBJ_EX_P_tr_01_MT_R	=	./ex.cpp.tr.01.mt.o
OBJ_EX_P_tr_02_MT_R	=	./ex.cpp.tr.02.mt.o
OBJ_EX_P_ut_01_MT_R	=	./ex.cpp.ut.01.mt.o
OBJ_EX_C_0_MT_R		=	./ex.c.0.mt.o
OBJ_EX_C_1_MT_R		=	./ex.c.1.mt.o
OBJ_EX_C_2_MT_R		=	./ex.c.2.mt.o
OBJ_EX_C_3_MT_R		=	./ex.c.3.mt.o
OBJ_EX_C_4_MT_R		=	./ex.c.4.mt.o
OBJ_EX_C_5_MT_R		=	./ex.c.5.mt.o
OBJ_EX_C_6_MT_R		=	./ex.c.6.mt.o
OBJ_EX_C_7_MT_R		=	./ex.c.7.mt.o
OBJ_EX_P_be_01_ST_D	=	./ex.cpp.be.01.debug.o
OBJ_EX_P_be_02_ST_D	=	./ex.cpp.be.02.debug.o
OBJ_EX_P_be_03_ST_D	=	./ex.cpp.be.03.debug.o
OBJ_EX_P_be_04_ST_D	=	./ex.cpp.be.04.debug.o
OBJ_EX_P_be_05_ST_D	=	./ex.cpp.be.05.debug.o
OBJ_EX_P_be_06_ST_D	=	./ex.cpp.be.06.debug.o
OBJ_EX_P_cp_01_ST_D	=	./ex.cpp.cp.01.debug.o
OBJ_EX_P_cp_02_ST_D	=	./ex.cpp.cp.02.debug.o
OBJ_EX_P_fm_01_ST_D	=	./ex.cpp.fm.01.debug.o
OBJ_EX_P_fm_02_ST_D	=	./ex.cpp.fm.02.debug.o
OBJ_EX_P_fm_03_ST_D	=	./ex.cpp.fm.03.debug.o
OBJ_EX_P_fe_01_ST_D	=	./ex.cpp.fe.01.debug.o
OBJ_EX_P_in_01_ST_D	=	./ex.cpp.in.01.debug.o
OBJ_EX_P_in_02_ST_D	=	./ex.cpp.in.02.debug.o
OBJ_EX_P_in_03_ST_D	=	./ex.cpp.in.03.debug.o
OBJ_EX_P_in_04_ST_D	=	./ex.cpp.in.04.debug.o
OBJ_EX_P_in_05_ST_D	=	./ex.cpp.in.05.debug.o
OBJ_EX_P_in_06_ST_D	=	./ex.cpp.in.06.debug.o
OBJ_EX_P_in_07_ST_D	=	./ex.cpp.in.07.debug.o
OBJ_EX_P_in_08_ST_D	=	./ex.cpp.in.08.debug.o
OBJ_EX_P_in_09_ST_D	=	./ex.cpp.in.09.debug.o
OBJ_EX_P_in_10_ST_D	=	./ex.cpp.in.10.debug.o
OBJ_EX_P_in_11_ST_D	=	./ex.cpp.in.11.debug.o
OBJ_EX_P_in_12_ST_D	=	./ex.cpp.in.12.debug.o
OBJ_EX_P_in_13_ST_D	=	./ex.cpp.in.13.debug.o
OBJ_EX_P_in_14_ST_D	=	./ex.cpp.in.14.debug.o
OBJ_EX_P_in_15_ST_D	=	./ex.cpp.in.15.debug.o
OBJ_EX_P_in_16_ST_D	=	./ex.cpp.in.16.debug.o
OBJ_EX_P_in_17_ST_D	=	./ex.cpp.in.17.debug.o
OBJ_EX_P_ms_01_ST_D	=	./ex.cpp.m2.01.debug.o
OBJ_EX_P_ms_02_ST_D	=	./ex.cpp.m2.02.debug.o
OBJ_EX_P_ms_03_ST_D	=	./ex.cpp.m2.03.debug.o
OBJ_EX_P_ms_04_ST_D	=	./ex.cpp.m2.04.debug.o
OBJ_EX_P_ms_05_ST_D	=	./ex.cpp.m2.05.debug.o
OBJ_EX_P_ms_06_ST_D	=	./ex.cpp.ms.06.debug.o
OBJ_EX_P_tr_01_ST_D	=	./ex.cpp.tr.01.debug.o
OBJ_EX_P_tr_02_ST_D	=	./ex.cpp.tr.02.debug.o
OBJ_EX_P_ut_01_ST_D	=	./ex.cpp.ut.01.debug.o
OBJ_EX_C_0_ST_D		=	./ex.c.0.debug.o
OBJ_EX_C_1_ST_D		=	./ex.c.1.debug.o
OBJ_EX_C_2_ST_D		=	./ex.c.2.debug.o
OBJ_EX_C_3_ST_D		=	./ex.c.3.debug.o
OBJ_EX_C_4_ST_D		=	./ex.c.4.debug.o
OBJ_EX_C_5_ST_D		=	./ex.c.5.debug.o
OBJ_EX_C_6_ST_D		=	./ex.c.6.debug.o
OBJ_EX_C_7_ST_D		=	./ex.c.7.debug.o
OBJ_EX_P_be_01_ST_R	=	./ex.cpp.be.01.o
OBJ_EX_P_be_02_ST_R	=	./ex.cpp.be.02.o
OBJ_EX_P_be_03_ST_R	=	./ex.cpp.be.03.o
OBJ_EX_P_be_04_ST_R	=	./ex.cpp.be.04.o
OBJ_EX_P_be_05_ST_R	=	./ex.cpp.be.05.o
OBJ_EX_P_be_06_ST_R	=	./ex.cpp.be.06.o
OBJ_EX_P_cp_01_ST_R	=	./ex.cpp.cp.01.o
OBJ_EX_P_cp_02_ST_R	=	./ex.cpp.cp.02.o
OBJ_EX_P_fm_01_ST_R	=	./ex.cpp.fm.01.o
OBJ_EX_P_fm_02_ST_R	=	./ex.cpp.fm.02.o
OBJ_EX_P_fm_03_ST_R	=	./ex.cpp.fm.03.o
OBJ_EX_P_fe_01_ST_R	=	./ex.cpp.fe.01.o
OBJ_EX_P_in_01_ST_R	=	./ex.cpp.in.01.o
OBJ_EX_P_in_02_ST_R	=	./ex.cpp.in.02.o
OBJ_EX_P_in_03_ST_R	=	./ex.cpp.in.03.o
OBJ_EX_P_in_04_ST_R	=	./ex.cpp.in.04.o
OBJ_EX_P_in_05_ST_R	=	./ex.cpp.in.05.o
OBJ_EX_P_in_06_ST_R	=	./ex.cpp.in.06.o
OBJ_EX_P_in_07_ST_R	=	./ex.cpp.in.07.o
OBJ_EX_P_in_08_ST_R	=	./ex.cpp.in.08.o
OBJ_EX_P_in_09_ST_R	=	./ex.cpp.in.09.o
OBJ_EX_P_in_10_ST_R	=	./ex.cpp.in.10.o
OBJ_EX_P_in_11_ST_R	=	./ex.cpp.in.11.o
OBJ_EX_P_in_12_ST_R	=	./ex.cpp.in.12.o
OBJ_EX_P_in_13_ST_R	=	./ex.cpp.in.13.o
OBJ_EX_P_in_14_ST_R	=	./ex.cpp.in.14.o
OBJ_EX_P_in_15_ST_R	=	./ex.cpp.in.15.o
OBJ_EX_P_in_16_ST_R	=	./ex.cpp.in.16.o
OBJ_EX_P_in_17_ST_R	=	./ex.cpp.in.17.o
OBJ_EX_P_ms_01_ST_R	=	./ex.cpp.m2.01.o
OBJ_EX_P_ms_02_ST_R	=	./ex.cpp.m2.02.o
OBJ_EX_P_ms_03_ST_R	=	./ex.cpp.m2.03.o
OBJ_EX_P_ms_04_ST_R	=	./ex.cpp.m2.04.o
OBJ_EX_P_ms_05_ST_R	=	./ex.cpp.m2.05.o
OBJ_EX_P_ms_06_ST_R	=	./ex.cpp.ms.06.o
OBJ_EX_P_tr_01_ST_R	=	./ex.cpp.tr.01.o
OBJ_EX_P_tr_02_ST_R	=	./ex.cpp.tr.02.o
OBJ_EX_P_ut_01_ST_R	=	./ex.cpp.ut.01.o
OBJ_EX_C_0_ST_R		=	./ex.c.0.o
OBJ_EX_C_1_ST_R		=	./ex.c.1.o
OBJ_EX_C_2_ST_R		=	./ex.c.2.o
OBJ_EX_C_3_ST_R		=	./ex.c.3.o
OBJ_EX_C_4_ST_R		=	./ex.c.4.o
OBJ_EX_C_5_ST_R		=	./ex.c.5.o
OBJ_EX_C_6_ST_R		=	./ex.c.6.o
OBJ_EX_C_7_ST_R		=	./ex.c.7.o

########################################
# Tests

##############################
# Component Tests

OBJ_TEST_COMP_core_initseq_MT_D	=	./test.component.core.init_sequence.mt.debug.o
OBJ_TEST_COMP_core_initseq_MT_R	=	./test.component.core.init_sequence.mt.o
OBJ_TEST_COMP_core_initseq_ST_D	=	./test.component.core.init_sequence.debug.o
OBJ_TEST_COMP_core_initseq_ST_R	=	./test.component.core.init_sequence.o
OBJ_TEST_COMP_custsev_MT_D	=	./test.component.custom_severity.mt.debug.o
OBJ_TEST_COMP_custsev_MT_R	=	./test.component.custom_severity.mt.o
OBJ_TEST_COMP_custsev_ST_D	=	./test.component.custom_severity.debug.o
OBJ_TEST_COMP_custsev_ST_R	=	./test.component.custom_severity.o
OBJ_TEST_COMP_filemt_MT_D	=	./test.component.be.file.threading.mt.debug.o
OBJ_TEST_COMP_filemt_MT_R	=	./test.component.be.file.threading.mt.o
OBJ_TEST_COMP_ins_args_MT_D	=	./test.component.inserters.args.mt.debug.o
OBJ_TEST_COMP_ins_args_MT_R	=	./test.component.inserters.args.mt.o
OBJ_TEST_COMP_ins_args_ST_D	=	./test.component.inserters.args.debug.o
OBJ_TEST_COMP_ins_args_ST_R	=	./test.component.inserters.args.o
OBJ_TEST_COMP_ins_b64_MT_D	=	./test.component.inserters.b64.mt.debug.o
OBJ_TEST_COMP_ins_b64_MT_R	=	./test.component.inserters.b64.mt.o
OBJ_TEST_COMP_ins_b64_ST_D	=	./test.component.inserters.b64.debug.o
OBJ_TEST_COMP_ins_b64_ST_R	=	./test.component.inserters.b64.o
OBJ_TEST_COMP_ins_bool_MT_D	=	./test.component.inserters.boolean.mt.debug.o
OBJ_TEST_COMP_ins_bool_MT_R	=	./test.component.inserters.boolean.mt.o
OBJ_TEST_COMP_ins_bool_ST_D	=	./test.component.inserters.boolean.debug.o
OBJ_TEST_COMP_ins_bool_ST_R	=	./test.component.inserters.boolean.o
OBJ_TEST_COMP_ins_hostid_MT_D	=	./test.component.inserters.hostId.mt.debug.o
OBJ_TEST_COMP_ins_hostid_MT_R	=	./test.component.inserters.hostId.mt.o
OBJ_TEST_COMP_ins_hostid_ST_D	=	./test.component.inserters.hostId.debug.o
OBJ_TEST_COMP_ins_hostid_ST_R	=	./test.component.inserters.hostId.o
OBJ_TEST_COMP_ins_int_MT_D	=	./test.component.inserters.integer.mt.debug.o
OBJ_TEST_COMP_ins_int_MT_R	=	./test.component.inserters.integer.mt.o
OBJ_TEST_COMP_ins_int_ST_D	=	./test.component.inserters.integer.debug.o
OBJ_TEST_COMP_ins_int_ST_R	=	./test.component.inserters.integer.o
OBJ_TEST_COMP_ins_pad_MT_D	=	./test.component.inserters.pad.mt.debug.o
OBJ_TEST_COMP_ins_pad_MT_R	=	./test.component.inserters.pad.mt.o
OBJ_TEST_COMP_ins_pad_ST_D	=	./test.component.inserters.pad.debug.o
OBJ_TEST_COMP_ins_pad_ST_R	=	./test.component.inserters.pad.o
OBJ_TEST_COMP_ins_ptr_MT_D	=	./test.component.inserters.pointer.mt.debug.o
OBJ_TEST_COMP_ins_ptr_MT_R	=	./test.component.inserters.pointer.mt.o
OBJ_TEST_COMP_ins_ptr_ST_D	=	./test.component.inserters.pointer.debug.o
OBJ_TEST_COMP_ins_ptr_ST_R	=	./test.component.inserters.pointer.o
OBJ_TEST_COMP_ins_prid_MT_D	=	./test.component.inserters.processId.mt.debug.o
OBJ_TEST_COMP_ins_prid_MT_R	=	./test.component.inserters.processId.mt.o
OBJ_TEST_COMP_ins_prid_ST_D	=	./test.component.inserters.processId.debug.o
OBJ_TEST_COMP_ins_prid_ST_R	=	./test.component.inserters.processId.o
OBJ_TEST_COMP_ins_real_MT_D	=	./test.component.inserters.real.mt.debug.o
OBJ_TEST_COMP_ins_real_MT_R	=	./test.component.inserters.real.mt.o
OBJ_TEST_COMP_ins_real_ST_D	=	./test.component.inserters.real.debug.o
OBJ_TEST_COMP_ins_real_ST_R	=	./test.component.inserters.real.o
OBJ_TEST_COMP_ins_slice_MT_D	=	./test.component.inserters.slice.mt.debug.o
OBJ_TEST_COMP_ins_slice_MT_R	=	./test.component.inserters.slice.mt.o
OBJ_TEST_COMP_ins_slice_ST_D	=	./test.component.inserters.slice.debug.o
OBJ_TEST_COMP_ins_slice_ST_R	=	./test.component.inserters.slice.o
OBJ_TEST_COMP_ins_thrid_MT_D	=	./test.component.inserters.threadId.mt.debug.o
OBJ_TEST_COMP_ins_thrid_MT_R	=	./test.component.inserters.threadId.mt.o
OBJ_TEST_COMP_ins_thrid_ST_D	=	./test.component.inserters.threadId.debug.o
OBJ_TEST_COMP_ins_thrid_ST_R	=	./test.component.inserters.threadId.o
OBJ_TEST_COMP_ins_w2m_MT_D	=	./test.component.inserters.w2m.mt.debug.o
OBJ_TEST_COMP_ins_w2m_MT_R	=	./test.component.inserters.w2m.mt.o
OBJ_TEST_COMP_ins_w2m_ST_D	=	./test.component.inserters.w2m.debug.o
OBJ_TEST_COMP_ins_w2m_ST_R	=	./test.component.inserters.w2m.o
OBJ_TEST_COMP_log_1_MT_D	=	./test.component.log.1.mt.debug.o
OBJ_TEST_COMP_log_1_MT_R	=	./test.component.log.1.mt.o
OBJ_TEST_COMP_log_1_ST_D	=	./test.component.log.1.debug.o
OBJ_TEST_COMP_log_1_ST_R	=	./test.component.log.1.o
OBJ_TEST_COMP_pan_puts_MT_D	=	./test.component.core.pan_logputs.mt.debug.o
OBJ_TEST_COMP_pan_puts_MT_R	=	./test.component.core.pan_logputs.mt.o
OBJ_TEST_COMP_pan_puts_ST_D	=	./test.component.core.pan_logputs.debug.o
OBJ_TEST_COMP_pan_puts_ST_R	=	./test.component.core.pan_logputs.o
OBJ_TEST_COMP_pan_prtf_MT_D	=	./test.component.core.pan_logprintf.mt.debug.o
OBJ_TEST_COMP_pan_prtf_MT_R	=	./test.component.core.pan_logprintf.mt.o
OBJ_TEST_COMP_pan_prtf_ST_D	=	./test.component.core.pan_logprintf.debug.o
OBJ_TEST_COMP_pan_prtf_ST_R	=	./test.component.core.pan_logprintf.o
OBJ_TEST_COMP_trace_1_MT_D	=	./test.component.trace.1.mt.debug.o
OBJ_TEST_COMP_trace_1_MT_R	=	./test.component.trace.1.mt.o
OBJ_TEST_COMP_trace_1_ST_D	=	./test.component.trace.1.debug.o
OBJ_TEST_COMP_trace_1_ST_R	=	./test.component.trace.1.o

##############################
# Performance Tests

OBJ_TEST_PERF_ins_w2m_MT_D		=	./test.performance.inserters.w2m.mt.debug.o
OBJ_TEST_PERF_ins_w2m_MT_R		=	./test.performance.inserters.w2m.mt.o
OBJ_TEST_PERF_ins_w2m_ST_D		=	./test.performance.inserters.w2m.debug.o
OBJ_TEST_PERF_ins_w2m_ST_R		=	./test.performance.inserters.w2m.o

##############################
# Scratch Tests

ifdef ACE
OBJ_SCRT_ACE_MT_D					=	./test.scratch.ACE.mt.debug.o
endif # ACE
OBJ_TEST_API_MT_D					=	./test.api.mt.debug.o
OBJ_TEST_FE_MT_D					=	./test.fe.mt.debug.o
OBJ_TEST_BE_N_MT_D				=	./test.be.N.fail.mt.debug.o
OBJ_TEST_FE_SIMPLE_WC_MT_D		=	./test.fe.simple.WithCallback.mt.debug.o
OBJ_TEST_SCRT_BE_SYSLOG_MT_D		=	./test.scratch.be.syslog.mt.debug.o
OBJ_TEST_SCRT_BE_SYSLOGWC_MT_D	=	./test.scratch.be.syslog.WithCallback.mt.debug.o
ifdef ACE
OBJ_SCRT_ACE_MT_R					=	./test.scratch.ACE.mt.o
endif # ACE
OBJ_TEST_API_MT_R					=	./test.api.mt.o
OBJ_TEST_FE_MT_R					=	./test.fe.mt.o
OBJ_TEST_BE_N_MT_R				=	./test.be.N.fail.mt.o
OBJ_TEST_FE_SIMPLE_WC_MT_R		=	./test.fe.simple.WithCallback.mt.o
OBJ_TEST_SCRT_BE_SYSLOG_MT_R		=	./test.scratch.be.syslog.mt.o
OBJ_TEST_SCRT_BE_SYSLOGWC_MT_R	=	./test.scratch.be.syslog.WithCallback.mt.o
ifdef ACE
OBJ_SCRT_ACE_ST_D					=	./test.scratch.ACE.debug.o
endif # ACE
OBJ_TEST_API_ST_D					=	./test.api.debug.o
OBJ_TEST_FE_ST_D					=	./test.fe.debug.o
OBJ_TEST_BE_N_ST_D				=	./test.be.N.fail.debug.o
OBJ_TEST_FE_SIMPLE_WC_ST_D		=	./test.fe.simple.WithCallback.debug.o
OBJ_TEST_SCRT_BE_SYSLOG_ST_D		=	./test.scratch.be.syslog.debug.o
OBJ_TEST_SCRT_BE_SYSLOGWC_ST_D	=	./test.scratch.be.syslog.WithCallback.debug.o
ifdef ACE
OBJ_SCRT_ACE_ST_R					=	./test.scratch.ACE.o
endif # ACE
OBJ_TEST_API_ST_R					=	./test.api.o
OBJ_TEST_FE_ST_R					=	./test.fe.o
OBJ_TEST_BE_N_ST_R				=	./test.be.N.fail.o
OBJ_TEST_FE_SIMPLE_WC_ST_R		=	./test.fe.simple.WithCallback.o
OBJ_TEST_SCRT_BE_SYSLOG_ST_R		=	./test.scratch.be.syslog.o
OBJ_TEST_SCRT_BE_SYSLOGWC_ST_R	=	./test.scratch.be.syslog.WithCallback.o

##############################
# Unit Tests

OBJ_TEST_UNIT_lvls_dyninit_MT_D	=	./test.unit.levels.dynamic_init.mt.debug.o
OBJ_TEST_UNIT_lvls_dyninit_MT_R	=	./test.unit.levels.dynamic_init.mt.o
OBJ_TEST_UNIT_lvls_dyninit_ST_D	=	./test.unit.levels.dynamic_init.debug.o
OBJ_TEST_UNIT_lvls_dyninit_ST_R	=	./test.unit.levels.dynamic_init.o
OBJ_TEST_UNIT_lvls_vals_MT_D	=	./test.unit.levels.values.mt.debug.o
OBJ_TEST_UNIT_lvls_vals_MT_R	=	./test.unit.levels.values.mt.o
OBJ_TEST_UNIT_lvls_vals_ST_D	=	./test.unit.levels.values.debug.o
OBJ_TEST_UNIT_lvls_vals_ST_R	=	./test.unit.levels.values.o
OBJ_TEST_UNIT_getcurrtm_MT_D	=	./test.unit.util.getcurrenttime.mt.debug.o
OBJ_TEST_UNIT_getcurrtm_MT_R	=	./test.unit.util.getcurrenttime.mt.o
OBJ_TEST_UNIT_getcurrtm_ST_D	=	./test.unit.util.getcurrenttime.debug.o
OBJ_TEST_UNIT_getcurrtm_ST_R	=	./test.unit.util.getcurrenttime.o
OBJ_TEST_UNIT_getversion_MT_D	=	./test.unit.getversion.mt.debug.o
OBJ_TEST_UNIT_getversion_MT_R	=	./test.unit.getversion.mt.o
OBJ_TEST_UNIT_getversion_ST_D	=	./test.unit.getversion.debug.o
OBJ_TEST_UNIT_getversion_ST_R	=	./test.unit.getversion.o
OBJ_TEST_UNIT_gethostname_MT_D	=	./test.unit.util.gethostname.mt.debug.o
OBJ_TEST_UNIT_gethostname_MT_R	=	./test.unit.util.gethostname.mt.o
OBJ_TEST_UNIT_gethostname_ST_D	=	./test.unit.util.gethostname.debug.o
OBJ_TEST_UNIT_gethostname_ST_R	=	./test.unit.util.gethostname.o
OBJ_TEST_UNIT_onbailout_MT_D	=	./test.unit.util.onbailout.mt.debug.o
OBJ_TEST_UNIT_strnlen_MT_D	=	./test.unit.util.strnlen.mt.debug.o
OBJ_TEST_UNIT_onbailout_MT_R	=	./test.unit.util.onbailout.mt.o
OBJ_TEST_UNIT_strnlen_MT_R	=	./test.unit.util.strnlen.mt.o
OBJ_TEST_UNIT_onbailout_ST_D	=	./test.unit.util.onbailout.debug.o
OBJ_TEST_UNIT_strnlen_ST_D	=	./test.unit.util.strnlen.debug.o
OBJ_TEST_UNIT_onbailout_ST_R	=	./test.unit.util.onbailout.o
OBJ_TEST_UNIT_strnlen_ST_R	=	./test.unit.util.strnlen.o
OBJ_TEST_UNIT_bec_file_MT_D		=	./test.unit.bec.file.mt.debug.o
OBJ_TEST_UNIT_bec_file_MT_R		=	./test.unit.bec.file.mt.o
OBJ_TEST_UNIT_bec_file_ST_D		=	./test.unit.bec.file.debug.o
OBJ_TEST_UNIT_bec_file_ST_R		=	./test.unit.bec.file.o
OBJ_TEST_UNIT_bec_fprintf_MT_D	=	./test.unit.bec.fprintf.mt.debug.o
OBJ_TEST_UNIT_bec_fprintf_MT_R	=	./test.unit.bec.fprintf.mt.o
OBJ_TEST_UNIT_bec_fprintf_ST_D	=	./test.unit.bec.fprintf.debug.o
OBJ_TEST_UNIT_bec_fprintf_ST_R	=	./test.unit.bec.fprintf.o
OBJ_TEST_UNIT_bec_fail_MT_D		=	./test.unit.bec.fail.mt.debug.o
OBJ_TEST_UNIT_bec_fail_MT_R		=	./test.unit.bec.fail.mt.o
OBJ_TEST_UNIT_bec_fail_ST_D		=	./test.unit.bec.fail.debug.o
OBJ_TEST_UNIT_bec_fail_ST_R		=	./test.unit.bec.fail.o
OBJ_TEST_UNIT_be_fail_MT_D		=	./test.unit.be.fail.mt.debug.o
OBJ_TEST_UNIT_be_fail_MT_R		=	./test.unit.be.fail.mt.o
OBJ_TEST_UNIT_be_fail_ST_D		=	./test.unit.be.fail.debug.o
OBJ_TEST_UNIT_be_fail_ST_R		=	./test.unit.be.fail.o
OBJ_TEST_UNIT_be_lrsplit_MT_D	=	./test.unit.be.lrsplit.mt.debug.o
OBJ_TEST_UNIT_be_lrsplit_MT_R	=	./test.unit.be.lrsplit.mt.o
OBJ_TEST_UNIT_be_lrsplit_ST_D	=	./test.unit.be.lrsplit.debug.o
OBJ_TEST_UNIT_be_lrsplit_ST_R	=	./test.unit.be.lrsplit.o
OBJ_TEST_UNIT_be_N_MT_D			=	./test.unit.be.N.mt.debug.o
OBJ_TEST_UNIT_be_N_MT_R			=	./test.unit.be.N.mt.o
OBJ_TEST_UNIT_be_N_ST_D			=	./test.unit.be.N.debug.o
OBJ_TEST_UNIT_be_N_ST_R			=	./test.unit.be.N.o
OBJ_TEST_UNIT_fe_all_MT_D		=	./test.unit.fe.all.mt.debug.o
OBJ_TEST_UNIT_fe_all_MT_R		=	./test.unit.fe.all.mt.o
OBJ_TEST_UNIT_fe_all_ST_D		=	./test.unit.fe.all.debug.o
OBJ_TEST_UNIT_fe_all_ST_R		=	./test.unit.fe.all.o
OBJ_TEST_UNIT_fe_fail_MT_D		=	./test.unit.fe.fail.mt.debug.o
OBJ_TEST_UNIT_fe_fail_MT_R		=	./test.unit.fe.fail.mt.o
OBJ_TEST_UNIT_fe_fail_ST_D		=	./test.unit.fe.fail.debug.o
OBJ_TEST_UNIT_fe_fail_ST_R		=	./test.unit.fe.fail.o
OBJ_TEST_UNIT_fe_N_MT_D			=	./test.unit.fe.N.mt.debug.o
OBJ_TEST_UNIT_fe_N_MT_R			=	./test.unit.fe.N.mt.o
OBJ_TEST_UNIT_fe_N_ST_D			=	./test.unit.fe.N.debug.o
OBJ_TEST_UNIT_fe_N_ST_R			=	./test.unit.fe.N.o
OBJ_TEST_UNIT_fe_null_MT_D		=	./test.unit.fe.null.mt.debug.o
OBJ_TEST_UNIT_fe_null_MT_R		=	./test.unit.fe.null.mt.o
OBJ_TEST_UNIT_fe_null_ST_D		=	./test.unit.fe.null.debug.o
OBJ_TEST_UNIT_fe_null_ST_R		=	./test.unit.fe.null.o
OBJ_TEST_UNIT_fe_simple_MT_D	=	./test.unit.fe.simple.mt.debug.o
OBJ_TEST_UNIT_fe_simple_MT_R	=	./test.unit.fe.simple.mt.o
OBJ_TEST_UNIT_fe_simple_ST_D	=	./test.unit.fe.simple.debug.o
OBJ_TEST_UNIT_fe_simple_ST_R	=	./test.unit.fe.simple.o

OBJ_TEST_UNIT_ins_int_MT_D	=	./test.unit.inserter.integer.mt.debug.o
OBJ_TEST_UNIT_ins_w2m_MT_D	=	./test.unit.inserter.w2m.mt.debug.o
OBJ_TEST_UNIT_ins_int_MT_R	=	./test.unit.inserter.integer.mt.o
OBJ_TEST_UNIT_ins_w2m_MT_R	=	./test.unit.inserter.w2m.mt.o
OBJ_TEST_UNIT_ins_int_ST_D	=	./test.unit.inserter.integer.debug.o
OBJ_TEST_UNIT_ins_w2m_ST_D	=	./test.unit.inserter.w2m.debug.o
OBJ_TEST_UNIT_ins_int_ST_R	=	./test.unit.inserter.integer.o
OBJ_TEST_UNIT_ins_w2m_ST_R	=	./test.unit.inserter.w2m.o

##################################################
# Target groups

########################################
# Objects

# util
OBJS_UTIL=\
 $(OBJ_UTIL_MT_D)\
 $(OBJ_UTIL_MT_R)\
 $(OBJ_UTIL_ST_D)\
 $(OBJ_UTIL_ST_R)\
 \

# core
OBJS_CORE=\
 $(OBJ_CORE_MT_D)\
 $(OBJ_CORE_MT_R)\
 $(OBJ_CORE_ST_D)\
 $(OBJ_CORE_ST_R)\
 \

# appl
OBJS_APPL=\
 $(OBJ_APPL_MT_D)\
 $(OBJ_APPL_MT_R)\
 $(OBJ_APPL_ST_D)\
 $(OBJ_APPL_ST_R)\
 \

# front-ends
OBJS_FE=\
 $(OBJ_FE_ALL_MT_D)\
 $(OBJ_FE_ALL_WC_MT_D)\
 $(OBJ_FE_FAIL_MT_D)\
 $(OBJ_FE_N_MT_D)\
 $(OBJ_FE_N_WC_MT_D)\
 $(OBJ_FE_NULL_MT_D)\
 $(OBJ_FE_NULL_WC_MT_D)\
 $(OBJ_FE_SIMPLE_MT_D)\
 $(OBJ_FE_SIMPLE_WC_MT_D)\
 $(OBJ_FE_ALL_MT_R)\
 $(OBJ_FE_ALL_WC_MT_R)\
 $(OBJ_FE_FAIL_MT_R)\
 $(OBJ_FE_N_MT_R)\
 $(OBJ_FE_N_WC_MT_R)\
 $(OBJ_FE_NULL_MT_R)\
 $(OBJ_FE_NULL_WC_MT_R)\
 $(OBJ_FE_SIMPLE_MT_R)\
 $(OBJ_FE_SIMPLE_WC_MT_R)\
 $(OBJ_FE_ALL_ST_D)\
 $(OBJ_FE_ALL_WC_ST_D)\
 $(OBJ_FE_FAIL_ST_D)\
 $(OBJ_FE_N_ST_D)\
 $(OBJ_FE_N_WC_ST_D)\
 $(OBJ_FE_NULL_ST_D)\
 $(OBJ_FE_NULL_WC_ST_D)\
 $(OBJ_FE_SIMPLE_ST_D)\
 $(OBJ_FE_SIMPLE_WC_ST_D)\
 $(OBJ_FE_ALL_ST_R)\
 $(OBJ_FE_ALL_WC_ST_R)\
 $(OBJ_FE_FAIL_ST_R)\
 $(OBJ_FE_N_ST_R)\
 $(OBJ_FE_N_WC_ST_R)\
 $(OBJ_FE_NULL_ST_R)\
 $(OBJ_FE_NULL_WC_ST_R)\
 $(OBJ_FE_SIMPLE_ST_R)\
 $(OBJ_FE_SIMPLE_WC_ST_R)\
 \

# back-ends
OBJS_BE=\
 $(OBJ_BE_LRSPLIT_MT_D)\
 $(OBJ_BE_N_MT_D)\
 $(OBJ_BEC_TEST_MT_D)\
 $(OBJ_BE_TEST_MT_D)\
 $(OBJ_BEL_TEST_MT_D)\
 $(OBJ_BER_TEST_MT_D)\
 $(OBJ_BEC_FAIL_MT_D)\
 $(OBJ_BE_FAIL_MT_D)\
 $(OBJ_BEL_FAIL_MT_D)\
 $(OBJ_BER_FAIL_MT_D)\
 $(OBJ_BEC_FILE_MT_D)\
 $(OBJ_BEC_FILE_WC_MT_D)\
 $(OBJ_BE_FILE_MT_D)\
 $(OBJ_BEL_FILE_MT_D)\
 $(OBJ_BER_FILE_MT_D)\
 $(OBJ_BEC_FPRINTF_MT_D)\
 $(OBJ_BEC_FPRINTF_WC_MT_D)\
 $(OBJ_BE_FPRINTF_MT_D)\
 $(OBJ_BEL_FPRINTF_MT_D)\
 $(OBJ_BER_FPRINTF_MT_D)\
 $(OBJ_BEC_NULL_MT_D)\
 $(OBJ_BE_NULL_MT_D)\
 $(OBJ_BEL_NULL_MT_D)\
 $(OBJ_BER_NULL_MT_D)\
 $(OBJ_BEC_SYSLOG_MT_D)\
 $(OBJ_BEC_SYSLOGWC_MT_D)\
 $(OBJ_BE_SYSLOG_MT_D)\
 $(OBJ_BER_SYSLOG_MT_D)\
 $(OBJ_BE_LRSPLIT_MT_R)\
 $(OBJ_BE_N_MT_R)\
 $(OBJ_BEC_TEST_MT_R)\
 $(OBJ_BE_TEST_MT_R)\
 $(OBJ_BEL_TEST_MT_R)\
 $(OBJ_BER_TEST_MT_R)\
 $(OBJ_BEC_FAIL_MT_R)\
 $(OBJ_BE_FAIL_MT_R)\
 $(OBJ_BEL_FAIL_MT_R)\
 $(OBJ_BER_FAIL_MT_R)\
 $(OBJ_BEC_FILE_MT_R)\
 $(OBJ_BEC_FILE_WC_MT_R)\
 $(OBJ_BE_FILE_MT_R)\
 $(OBJ_BEL_FILE_MT_R)\
 $(OBJ_BER_FILE_MT_R)\
 $(OBJ_BEC_FPRINTF_MT_R)\
 $(OBJ_BEC_FPRINTF_WC_MT_R)\
 $(OBJ_BE_FPRINTF_MT_R)\
 $(OBJ_BEL_FPRINTF_MT_R)\
 $(OBJ_BER_FPRINTF_MT_R)\
 $(OBJ_BEC_NULL_MT_R)\
 $(OBJ_BE_NULL_MT_R)\
 $(OBJ_BEL_NULL_MT_R)\
 $(OBJ_BER_NULL_MT_R)\
 $(OBJ_BEC_SYSLOG_MT_R)\
 $(OBJ_BEC_SYSLOGWC_MT_R)\
 $(OBJ_BE_SYSLOG_MT_R)\
 $(OBJ_BER_SYSLOG_MT_R)\
 $(OBJ_BE_LRSPLIT_ST_D)\
 $(OBJ_BE_N_ST_D)\
 $(OBJ_BEC_TEST_ST_D)\
 $(OBJ_BE_TEST_ST_D)\
 $(OBJ_BEL_TEST_ST_D)\
 $(OBJ_BER_TEST_ST_D)\
 $(OBJ_BEC_FAIL_ST_D)\
 $(OBJ_BE_FAIL_ST_D)\
 $(OBJ_BEL_FAIL_ST_D)\
 $(OBJ_BER_FAIL_ST_D)\
 $(OBJ_BEC_FILE_ST_D)\
 $(OBJ_BEC_FILE_WC_ST_D)\
 $(OBJ_BE_FILE_ST_D)\
 $(OBJ_BEL_FILE_ST_D)\
 $(OBJ_BER_FILE_ST_D)\
 $(OBJ_BEC_FPRINTF_ST_D)\
 $(OBJ_BEC_FPRINTF_WC_ST_D)\
 $(OBJ_BE_FPRINTF_ST_D)\
 $(OBJ_BEL_FPRINTF_ST_D)\
 $(OBJ_BER_FPRINTF_ST_D)\
 $(OBJ_BEC_NULL_ST_D)\
 $(OBJ_BE_NULL_ST_D)\
 $(OBJ_BEL_NULL_ST_D)\
 $(OBJ_BER_NULL_ST_D)\
 $(OBJ_BEC_SYSLOG_ST_D)\
 $(OBJ_BEC_SYSLOGWC_ST_D)\
 $(OBJ_BE_SYSLOG_ST_D)\
 $(OBJ_BER_SYSLOG_ST_D)\
 $(OBJ_BE_LRSPLIT_ST_R)\
 $(OBJ_BE_N_ST_R)\
 $(OBJ_BEC_TEST_ST_R)\
 $(OBJ_BE_TEST_ST_R)\
 $(OBJ_BEL_TEST_ST_R)\
 $(OBJ_BER_TEST_ST_R)\
 $(OBJ_BEC_FAIL_ST_R)\
 $(OBJ_BE_FAIL_ST_R)\
 $(OBJ_BEL_FAIL_ST_R)\
 $(OBJ_BER_FAIL_ST_R)\
 $(OBJ_BEC_FILE_ST_R)\
 $(OBJ_BEC_FILE_WC_ST_R)\
 $(OBJ_BE_FILE_ST_R)\
 $(OBJ_BEL_FILE_ST_R)\
 $(OBJ_BER_FILE_ST_R)\
 $(OBJ_BEC_FPRINTF_ST_R)\
 $(OBJ_BEC_FPRINTF_WC_ST_R)\
 $(OBJ_BE_FPRINTF_ST_R)\
 $(OBJ_BEL_FPRINTF_ST_R)\
 $(OBJ_BER_FPRINTF_ST_R)\
 $(OBJ_BEC_NULL_ST_R)\
 $(OBJ_BE_NULL_ST_R)\
 $(OBJ_BEL_NULL_ST_R)\
 $(OBJ_BER_NULL_ST_R)\
 $(OBJ_BEC_SYSLOG_ST_R)\
 $(OBJ_BEC_SYSLOGWC_ST_R)\
 $(OBJ_BE_SYSLOG_ST_R)\
 $(OBJ_BER_SYSLOG_ST_R)\
 \


ifdef ACE
OBJS_ACE=\
 $(OBJ_BEC_ACE_MT_D)\
 $(OBJ_BE_ACE_MT_D)\
 $(OBJ_BEL_ACE_MT_D)\
 $(OBJ_BER_ACE_MT_D)\
 $(OBJ_BEC_ACE_MT_R)\
 $(OBJ_BE_ACE_MT_R)\
 $(OBJ_BEL_ACE_MT_R)\
 $(OBJ_BER_ACE_MT_R)\
 \

endif # ACE
# b64
OBJS_B64=\
 $(OBJ_B64_MT_D)\
 $(OBJ_B64_MT_R)\
 $(OBJ_B64_ST_D)\
 $(OBJ_B64_ST_R)\
 \

# shwild
OBJS_SHWILD=\
 $(OBJ_SHWILD_MT_D)\
 $(OBJ_SHWILD_MT_R)\
 $(OBJ_SHWILD_ST_D)\
 $(OBJ_SHWILD_ST_R)\
 \

# xtests
OBJS_XTESTS=\
 $(OBJ_XTESTS_MT_D)\
 $(OBJ_XTESTS_MT_R)\
 $(OBJ_XTESTS_ST_D)\
 $(OBJ_XTESTS_ST_R)\
 \

# examples
OBJS_EXAMPLE_C=\
 $(OBJ_EX_C_0_MT_D)\
 $(OBJ_EX_C_1_MT_D)\
 $(OBJ_EX_C_2_MT_D)\
 $(OBJ_EX_C_3_MT_D)\
 $(OBJ_EX_C_4_MT_D)\
 $(OBJ_EX_C_5_MT_D)\
 $(OBJ_EX_C_6_MT_D)\
 $(OBJ_EX_C_7_MT_D)\
 $(OBJ_EX_C_0_MT_R)\
 $(OBJ_EX_C_1_MT_R)\
 $(OBJ_EX_C_2_MT_R)\
 $(OBJ_EX_C_3_MT_R)\
 $(OBJ_EX_C_4_MT_R)\
 $(OBJ_EX_C_5_MT_R)\
 $(OBJ_EX_C_6_MT_R)\
 $(OBJ_EX_C_7_MT_R)\
 $(OBJ_EX_C_0_ST_D)\
 $(OBJ_EX_C_1_ST_D)\
 $(OBJ_EX_C_2_ST_D)\
 $(OBJ_EX_C_3_ST_D)\
 $(OBJ_EX_C_4_ST_D)\
 $(OBJ_EX_C_5_ST_D)\
 $(OBJ_EX_C_6_ST_D)\
 $(OBJ_EX_C_7_ST_D)\
 $(OBJ_EX_C_0_ST_R)\
 $(OBJ_EX_C_1_ST_R)\
 $(OBJ_EX_C_2_ST_R)\
 $(OBJ_EX_C_3_ST_R)\
 $(OBJ_EX_C_4_ST_R)\
 $(OBJ_EX_C_5_ST_R)\
 $(OBJ_EX_C_6_ST_R)\
 $(OBJ_EX_C_7_ST_R)\
 \

OBJS_EXAMPLE_CPP=\
 $(OBJ_EX_P_be_01_MT_D)\
 $(OBJ_EX_P_be_02_MT_D)\
 $(OBJ_EX_P_be_03_MT_D)\
 $(OBJ_EX_P_be_04_MT_D)\
 $(OBJ_EX_P_be_05_MT_D)\
 $(OBJ_EX_P_be_06_MT_D)\
 $(OBJ_EX_P_cp_01_MT_D)\
 $(OBJ_EX_P_cp_02_MT_D)\
 $(OBJ_EX_P_fm_01_MT_D)\
 $(OBJ_EX_P_fm_02_MT_D)\
 $(OBJ_EX_P_fm_03_MT_D)\
 $(OBJ_EX_P_fe_01_MT_D)\
 $(OBJ_EX_P_in_01_MT_D)\
 $(OBJ_EX_P_in_02_MT_D)\
 $(OBJ_EX_P_in_03_MT_D)\
 $(OBJ_EX_P_in_04_MT_D)\
 $(OBJ_EX_P_in_05_MT_D)\
 $(OBJ_EX_P_in_06_MT_D)\
 $(OBJ_EX_P_in_07_MT_D)\
 $(OBJ_EX_P_in_08_MT_D)\
 $(OBJ_EX_P_in_09_MT_D)\
 $(OBJ_EX_P_in_10_MT_D)\
 $(OBJ_EX_P_in_11_MT_D)\
 $(OBJ_EX_P_in_12_MT_D)\
 $(OBJ_EX_P_in_13_MT_D)\
 $(OBJ_EX_P_in_14_MT_D)\
 $(OBJ_EX_P_in_15_MT_D)\
 $(OBJ_EX_P_in_16_MT_D)\
 $(OBJ_EX_P_in_17_MT_D)\
 $(OBJ_EX_P_ms_01_MT_D)\
 $(OBJ_EX_P_ms_02_MT_D)\
 $(OBJ_EX_P_ms_03_MT_D)\
 $(OBJ_EX_P_ms_04_MT_D)\
 $(OBJ_EX_P_ms_05_MT_D)\
 $(OBJ_EX_P_ms_06_MT_D)\
 $(OBJ_EX_P_tr_01_MT_D)\
 $(OBJ_EX_P_tr_02_MT_D)\
 $(OBJ_EX_P_ut_01_MT_D)\
 $(OBJ_EX_P_be_01_MT_R)\
 $(OBJ_EX_P_be_02_MT_R)\
 $(OBJ_EX_P_be_03_MT_R)\
 $(OBJ_EX_P_be_04_MT_R)\
 $(OBJ_EX_P_be_05_MT_R)\
 $(OBJ_EX_P_be_06_MT_R)\
 $(OBJ_EX_P_cp_01_MT_R)\
 $(OBJ_EX_P_cp_02_MT_R)\
 $(OBJ_EX_P_fm_01_MT_R)\
 $(OBJ_EX_P_fm_02_MT_R)\
 $(OBJ_EX_P_fm_03_MT_R)\
 $(OBJ_EX_P_fe_01_MT_R)\
 $(OBJ_EX_P_in_01_MT_R)\
 $(OBJ_EX_P_in_02_MT_R)\
 $(OBJ_EX_P_in_03_MT_R)\
 $(OBJ_EX_P_in_04_MT_R)\
 $(OBJ_EX_P_in_05_MT_R)\
 $(OBJ_EX_P_in_06_MT_R)\
 $(OBJ_EX_P_in_07_MT_R)\
 $(OBJ_EX_P_in_08_MT_R)\
 $(OBJ_EX_P_in_09_MT_R)\
 $(OBJ_EX_P_in_10_MT_R)\
 $(OBJ_EX_P_in_11_MT_R)\
 $(OBJ_EX_P_in_12_MT_R)\
 $(OBJ_EX_P_in_13_MT_R)\
 $(OBJ_EX_P_in_14_MT_R)\
 $(OBJ_EX_P_in_15_MT_R)\
 $(OBJ_EX_P_in_16_MT_R)\
 $(OBJ_EX_P_in_17_MT_R)\
 $(OBJ_EX_P_ms_01_MT_R)\
 $(OBJ_EX_P_ms_02_MT_R)\
 $(OBJ_EX_P_ms_03_MT_R)\
 $(OBJ_EX_P_ms_04_MT_R)\
 $(OBJ_EX_P_ms_05_MT_R)\
 $(OBJ_EX_P_ms_06_MT_R)\
 $(OBJ_EX_P_tr_01_MT_R)\
 $(OBJ_EX_P_tr_02_MT_R)\
 $(OBJ_EX_P_ut_01_MT_R)\
 $(OBJ_EX_P_be_01_ST_D)\
 $(OBJ_EX_P_be_02_ST_D)\
 $(OBJ_EX_P_be_03_ST_D)\
 $(OBJ_EX_P_be_04_ST_D)\
 $(OBJ_EX_P_be_05_ST_D)\
 $(OBJ_EX_P_be_06_ST_D)\
 $(OBJ_EX_P_cp_01_ST_D)\
 $(OBJ_EX_P_cp_02_ST_D)\
 $(OBJ_EX_P_fm_01_ST_D)\
 $(OBJ_EX_P_fm_02_ST_D)\
 $(OBJ_EX_P_fm_03_ST_D)\
 $(OBJ_EX_P_fe_01_ST_D)\
 $(OBJ_EX_P_in_01_ST_D)\
 $(OBJ_EX_P_in_02_ST_D)\
 $(OBJ_EX_P_in_03_ST_D)\
 $(OBJ_EX_P_in_04_ST_D)\
 $(OBJ_EX_P_in_05_ST_D)\
 $(OBJ_EX_P_in_06_ST_D)\
 $(OBJ_EX_P_in_07_ST_D)\
 $(OBJ_EX_P_in_08_ST_D)\
 $(OBJ_EX_P_in_09_ST_D)\
 $(OBJ_EX_P_in_10_ST_D)\
 $(OBJ_EX_P_in_11_ST_D)\
 $(OBJ_EX_P_in_12_ST_D)\
 $(OBJ_EX_P_in_13_ST_D)\
 $(OBJ_EX_P_in_14_ST_D)\
 $(OBJ_EX_P_in_15_ST_D)\
 $(OBJ_EX_P_in_16_ST_D)\
 $(OBJ_EX_P_in_17_ST_D)\
 $(OBJ_EX_P_ms_01_ST_D)\
 $(OBJ_EX_P_ms_02_ST_D)\
 $(OBJ_EX_P_ms_03_ST_D)\
 $(OBJ_EX_P_ms_04_ST_D)\
 $(OBJ_EX_P_ms_05_ST_D)\
 $(OBJ_EX_P_ms_06_ST_D)\
 $(OBJ_EX_P_tr_01_ST_D)\
 $(OBJ_EX_P_tr_02_ST_D)\
 $(OBJ_EX_P_ut_01_ST_D)\
 $(OBJ_EX_P_be_01_ST_R)\
 $(OBJ_EX_P_be_02_ST_R)\
 $(OBJ_EX_P_be_03_ST_R)\
 $(OBJ_EX_P_be_04_ST_R)\
 $(OBJ_EX_P_be_05_ST_R)\
 $(OBJ_EX_P_be_06_ST_R)\
 $(OBJ_EX_P_cp_01_ST_R)\
 $(OBJ_EX_P_cp_02_ST_R)\
 $(OBJ_EX_P_fm_01_ST_R)\
 $(OBJ_EX_P_fm_02_ST_R)\
 $(OBJ_EX_P_fm_03_ST_R)\
 $(OBJ_EX_P_fe_01_ST_R)\
 $(OBJ_EX_P_in_01_ST_R)\
 $(OBJ_EX_P_in_02_ST_R)\
 $(OBJ_EX_P_in_03_ST_R)\
 $(OBJ_EX_P_in_04_ST_R)\
 $(OBJ_EX_P_in_05_ST_R)\
 $(OBJ_EX_P_in_06_ST_R)\
 $(OBJ_EX_P_in_07_ST_R)\
 $(OBJ_EX_P_in_08_ST_R)\
 $(OBJ_EX_P_in_09_ST_R)\
 $(OBJ_EX_P_in_10_ST_R)\
 $(OBJ_EX_P_in_11_ST_R)\
 $(OBJ_EX_P_in_12_ST_R)\
 $(OBJ_EX_P_in_13_ST_R)\
 $(OBJ_EX_P_in_14_ST_R)\
 $(OBJ_EX_P_in_15_ST_R)\
 $(OBJ_EX_P_in_16_ST_R)\
 $(OBJ_EX_P_in_17_ST_R)\
 $(OBJ_EX_P_ms_01_ST_R)\
 $(OBJ_EX_P_ms_02_ST_R)\
 $(OBJ_EX_P_ms_03_ST_R)\
 $(OBJ_EX_P_ms_04_ST_R)\
 $(OBJ_EX_P_ms_05_ST_R)\
 $(OBJ_EX_P_ms_06_ST_R)\
 $(OBJ_EX_P_tr_01_ST_R)\
 $(OBJ_EX_P_tr_02_ST_R)\
 $(OBJ_EX_P_ut_01_ST_R)\
 \

OBJS_EXAMPLE=\
 $(OBJS_EXAMPLE_C)\
 $(OBJS_EXAMPLE_CPP)\
 \


# Component Tests
OBJS_TEST_COMPONENT=\
 $(OBJ_TEST_COMP_core_initseq_MT_D)\
 $(OBJ_TEST_COMP_custsev_MT_D)\
 $(OBJ_TEST_COMP_filemt_MT_D)\
 $(OBJ_TEST_COMP_ins_args_MT_D)\
 $(OBJ_TEST_COMP_ins_b64_MT_D)\
 $(OBJ_TEST_COMP_ins_bool_MT_D)\
 $(OBJ_TEST_COMP_ins_hostid_MT_D)\
 $(OBJ_TEST_COMP_ins_int_MT_D)\
 $(OBJ_TEST_COMP_ins_pad_MT_D)\
 $(OBJ_TEST_COMP_ins_ptr_MT_D)\
 $(OBJ_TEST_COMP_ins_prid_MT_D)\
 $(OBJ_TEST_COMP_ins_real_MT_D)\
 $(OBJ_TEST_COMP_ins_slice_MT_D)\
 $(OBJ_TEST_COMP_ins_thrid_MT_D)\
 $(OBJ_TEST_COMP_ins_w2m_MT_D)\
 $(OBJ_TEST_COMP_log_1_MT_D)\
 $(OBJ_TEST_COMP_pan_puts_MT_D)\
 $(OBJ_TEST_COMP_pan_prtf_MT_D)\
 $(OBJ_TEST_COMP_trace_1_MT_D)\
 $(OBJ_TEST_COMP_core_initseq_MT_R)\
 $(OBJ_TEST_COMP_custsev_MT_R)\
 $(OBJ_TEST_COMP_filemt_MT_R)\
 $(OBJ_TEST_COMP_ins_args_MT_R)\
 $(OBJ_TEST_COMP_ins_b64_MT_R)\
 $(OBJ_TEST_COMP_ins_bool_MT_R)\
 $(OBJ_TEST_COMP_ins_hostid_MT_R)\
 $(OBJ_TEST_COMP_ins_int_MT_R)\
 $(OBJ_TEST_COMP_ins_pad_MT_R)\
 $(OBJ_TEST_COMP_ins_ptr_MT_R)\
 $(OBJ_TEST_COMP_ins_prid_MT_R)\
 $(OBJ_TEST_COMP_ins_real_MT_R)\
 $(OBJ_TEST_COMP_ins_slice_MT_R)\
 $(OBJ_TEST_COMP_ins_thrid_MT_R)\
 $(OBJ_TEST_COMP_ins_w2m_MT_R)\
 $(OBJ_TEST_COMP_log_1_MT_R)\
 $(OBJ_TEST_COMP_pan_puts_MT_R)\
 $(OBJ_TEST_COMP_pan_prtf_MT_R)\
 $(OBJ_TEST_COMP_trace_1_MT_R)\
 $(OBJ_TEST_COMP_core_initseq_ST_D)\
 $(OBJ_TEST_COMP_custsev_ST_D)\
 $(OBJ_TEST_COMP_ins_args_ST_D)\
 $(OBJ_TEST_COMP_ins_b64_ST_D)\
 $(OBJ_TEST_COMP_ins_bool_ST_D)\
 $(OBJ_TEST_COMP_ins_hostid_ST_D)\
 $(OBJ_TEST_COMP_ins_int_ST_D)\
 $(OBJ_TEST_COMP_ins_pad_ST_D)\
 $(OBJ_TEST_COMP_ins_ptr_ST_D)\
 $(OBJ_TEST_COMP_ins_prid_ST_D)\
 $(OBJ_TEST_COMP_ins_real_ST_D)\
 $(OBJ_TEST_COMP_ins_slice_ST_D)\
 $(OBJ_TEST_COMP_ins_thrid_ST_D)\
 $(OBJ_TEST_COMP_ins_w2m_ST_D)\
 $(OBJ_TEST_COMP_log_1_ST_D)\
 $(OBJ_TEST_COMP_pan_puts_ST_D)\
 $(OBJ_TEST_COMP_pan_prtf_ST_D)\
 $(OBJ_TEST_COMP_trace_1_ST_D)\
 $(OBJ_TEST_COMP_core_initseq_ST_R)\
 $(OBJ_TEST_COMP_custsev_ST_R)\
 $(OBJ_TEST_COMP_ins_args_ST_R)\
 $(OBJ_TEST_COMP_ins_b64_ST_R)\
 $(OBJ_TEST_COMP_ins_bool_ST_R)\
 $(OBJ_TEST_COMP_ins_hostid_ST_R)\
 $(OBJ_TEST_COMP_ins_int_ST_R)\
 $(OBJ_TEST_COMP_ins_pad_ST_R)\
 $(OBJ_TEST_COMP_ins_ptr_ST_R)\
 $(OBJ_TEST_COMP_ins_prid_ST_R)\
 $(OBJ_TEST_COMP_ins_real_ST_R)\
 $(OBJ_TEST_COMP_ins_slice_ST_R)\
 $(OBJ_TEST_COMP_ins_thrid_ST_R)\
 $(OBJ_TEST_COMP_ins_w2m_ST_R)\
 $(OBJ_TEST_COMP_log_1_ST_R)\
 $(OBJ_TEST_COMP_pan_puts_ST_R)\
 $(OBJ_TEST_COMP_pan_prtf_ST_R)\
 $(OBJ_TEST_COMP_trace_1_ST_R)\
 \

# Performance Tests
OBJS_TEST_PERFORMANCE=\
 $(OBJ_TEST_PERF_ins_w2m_MT_D)\
 $(OBJ_TEST_PERF_ins_w2m_MT_R)\
 $(OBJ_TEST_PERF_ins_w2m_ST_D)\
 $(OBJ_TEST_PERF_ins_w2m_ST_R)\
 \

# Scratch Tests
OBJS_TEST_SCRATCH=\
 $(OBJ_TEST_API_MT_D)\
 $(OBJ_TEST_FE_MT_D)\
 $(OBJ_TEST_BE_N_MT_D)\
 $(OBJ_TEST_FE_SIMPLE_WC_MT_D)\
 $(OBJ_TEST_SCRT_BE_SYSLOG_MT_D)\
 $(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_D)\
 $(OBJ_TEST_API_MT_R)\
 $(OBJ_TEST_FE_MT_R)\
 $(OBJ_TEST_BE_N_MT_R)\
 $(OBJ_TEST_FE_SIMPLE_WC_MT_R)\
 $(OBJ_TEST_SCRT_BE_SYSLOG_MT_R)\
 $(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_R)\
 $(OBJ_TEST_API_ST_D)\
 $(OBJ_TEST_FE_ST_D)\
 $(OBJ_TEST_BE_N_ST_D)\
 $(OBJ_TEST_FE_SIMPLE_WC_ST_D)\
 $(OBJ_TEST_SCRT_BE_SYSLOG_ST_D)\
 $(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_D)\
 $(OBJ_TEST_API_ST_R)\
 $(OBJ_TEST_FE_ST_R)\
 $(OBJ_TEST_BE_N_ST_R)\
 $(OBJ_TEST_FE_SIMPLE_WC_ST_R)\
 $(OBJ_TEST_SCRT_BE_SYSLOG_ST_R)\
 $(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_R)\
 \

# Unit Tests
OBJS_TEST_UNIT=\
 $(OBJ_TEST_UNIT_lvls_dyninit_MT_D)\
 $(OBJ_TEST_UNIT_lvls_vals_MT_D)\
 $(OBJ_TEST_UNIT_getcurrtm_MT_D)\
 $(OBJ_TEST_UNIT_getversion_MT_D)\
 $(OBJ_TEST_UNIT_gethostname_MT_D)\
 $(OBJ_TEST_UNIT_onbailout_MT_D)\
 $(OBJ_TEST_UNIT_strnlen_MT_D)\
 \
 $(OBJ_TEST_UNIT_bec_file_MT_D)\
 $(OBJ_TEST_UNIT_bec_fprintf_MT_D)\
 $(OBJ_TEST_UNIT_bec_fail_MT_D)\
 $(OBJ_TEST_UNIT_be_fail_MT_D)\
 $(OBJ_TEST_UNIT_be_lrsplit_MT_D)\
 $(OBJ_TEST_UNIT_be_N_MT_D)\
 \
 $(OBJ_TEST_UNIT_fe_all_MT_D)\
 $(OBJ_TEST_UNIT_fe_fail_MT_D)\
 $(OBJ_TEST_UNIT_fe_N_MT_D)\
 $(OBJ_TEST_UNIT_fe_null_MT_D)\
 $(OBJ_TEST_UNIT_fe_simple_MT_D)\
 $(OBJ_TEST_UNIT_ins_int_MT_D)\
 $(OBJ_TEST_UNIT_ins_w2m_MT_D)\
 $(OBJ_TEST_UNIT_lvls_dyninit_MT_R)\
 $(OBJ_TEST_UNIT_lvls_vals_MT_R)\
 $(OBJ_TEST_UNIT_getcurrtm_MT_R)\
 $(OBJ_TEST_UNIT_getversion_MT_R)\
 $(OBJ_TEST_UNIT_gethostname_MT_R)\
 $(OBJ_TEST_UNIT_onbailout_MT_R)\
 $(OBJ_TEST_UNIT_strnlen_MT_R)\
 \
 $(OBJ_TEST_UNIT_bec_file_MT_R)\
 $(OBJ_TEST_UNIT_bec_fprintf_MT_R)\
 $(OBJ_TEST_UNIT_bec_fail_MT_R)\
 $(OBJ_TEST_UNIT_be_fail_MT_R)\
 $(OBJ_TEST_UNIT_be_lrsplit_MT_R)\
 $(OBJ_TEST_UNIT_be_N_MT_R)\
 \
 $(OBJ_TEST_UNIT_fe_all_MT_R)\
 $(OBJ_TEST_UNIT_fe_fail_MT_R)\
 $(OBJ_TEST_UNIT_fe_N_MT_R)\
 $(OBJ_TEST_UNIT_fe_null_MT_R)\
 $(OBJ_TEST_UNIT_fe_simple_MT_R)\
 $(OBJ_TEST_UNIT_ins_int_MT_R)\
 $(OBJ_TEST_UNIT_ins_w2m_MT_R)\
 $(OBJ_TEST_UNIT_lvls_dyninit_ST_D)\
 $(OBJ_TEST_UNIT_lvls_vals_ST_D)\
 $(OBJ_TEST_UNIT_getcurrtm_ST_D)\
 $(OBJ_TEST_UNIT_getversion_ST_D)\
 $(OBJ_TEST_UNIT_gethostname_ST_D)\
 $(OBJ_TEST_UNIT_onbailout_ST_D)\
 $(OBJ_TEST_UNIT_strnlen_ST_D)\
 \
 $(OBJ_TEST_UNIT_bec_file_ST_D)\
 $(OBJ_TEST_UNIT_bec_fprintf_ST_D)\
 $(OBJ_TEST_UNIT_bec_fail_ST_D)\
 $(OBJ_TEST_UNIT_be_fail_ST_D)\
 $(OBJ_TEST_UNIT_be_lrsplit_ST_D)\
 $(OBJ_TEST_UNIT_be_N_ST_D)\
 \
 $(OBJ_TEST_UNIT_fe_all_ST_D)\
 $(OBJ_TEST_UNIT_fe_fail_ST_D)\
 $(OBJ_TEST_UNIT_fe_N_ST_D)\
 $(OBJ_TEST_UNIT_fe_null_ST_D)\
 $(OBJ_TEST_UNIT_fe_simple_ST_D)\
 $(OBJ_TEST_UNIT_ins_int_ST_D)\
 $(OBJ_TEST_UNIT_ins_w2m_ST_D)\
 $(OBJ_TEST_UNIT_lvls_dyninit_ST_R)\
 $(OBJ_TEST_UNIT_lvls_vals_ST_R)\
 $(OBJ_TEST_UNIT_getcurrtm_ST_R)\
 $(OBJ_TEST_UNIT_getversion_ST_R)\
 $(OBJ_TEST_UNIT_gethostname_ST_R)\
 $(OBJ_TEST_UNIT_onbailout_ST_R)\
 $(OBJ_TEST_UNIT_strnlen_ST_R)\
 \
 $(OBJ_TEST_UNIT_bec_file_ST_R)\
 $(OBJ_TEST_UNIT_bec_fprintf_ST_R)\
 $(OBJ_TEST_UNIT_bec_fail_ST_R)\
 $(OBJ_TEST_UNIT_be_fail_ST_R)\
 $(OBJ_TEST_UNIT_be_lrsplit_ST_R)\
 $(OBJ_TEST_UNIT_be_N_ST_R)\
 \
 $(OBJ_TEST_UNIT_fe_all_ST_R)\
 $(OBJ_TEST_UNIT_fe_fail_ST_R)\
 $(OBJ_TEST_UNIT_fe_N_ST_R)\
 $(OBJ_TEST_UNIT_fe_null_ST_R)\
 $(OBJ_TEST_UNIT_fe_simple_ST_R)\
 $(OBJ_TEST_UNIT_ins_int_ST_R)\
 $(OBJ_TEST_UNIT_ins_w2m_ST_R)\
 \

########################################
# Libraries

# $(LIB_UTIL [ ` [ N a m e T a g ] ` ] )\

TARGETS_LIB_UTIL=\
 $(LIB_UTIL_MT_D)\
 $(LIB_UTIL_MT_R)\
 $(LIB_UTIL_ST_D)\
 $(LIB_UTIL_ST_R)\
 \

TARGETS_LIB_CORE=\
 $(LIB_CORE_MT_D)\
 $(LIB_CORE_MT_R)\
 $(LIB_CORE_ST_D)\
 $(LIB_CORE_ST_R)\
 \

TARGETS_LIB_APPL=\
 $(LIB_APPL_MT_D)\
 $(LIB_APPL_MT_R)\
 $(LIB_APPL_ST_D)\
 $(LIB_APPL_ST_R)\
 \

TARGETS_LIB_FE_CMN=\
 $(LIB_FE_ALL_MT_D)\
 $(LIB_FE_ALL_WC_MT_D)\
 $(LIB_FE_FAIL_MT_D)\
 $(LIB_FE_N_MT_D)\
 $(LIB_FE_N_WC_MT_D)\
 $(LIB_FE_NULL_MT_D)\
 $(LIB_FE_NULL_WC_MT_D)\
 $(LIB_FE_SIMPLE_MT_D)\
 $(LIB_FE_SIMPLE_WC_MT_D)\
 $(LIB_FE_ALL_MT_R)\
 $(LIB_FE_ALL_WC_MT_R)\
 $(LIB_FE_FAIL_MT_R)\
 $(LIB_FE_N_MT_R)\
 $(LIB_FE_N_WC_MT_R)\
 $(LIB_FE_NULL_MT_R)\
 $(LIB_FE_NULL_WC_MT_R)\
 $(LIB_FE_SIMPLE_MT_R)\
 $(LIB_FE_SIMPLE_WC_MT_R)\
 $(LIB_FE_ALL_ST_D)\
 $(LIB_FE_ALL_WC_ST_D)\
 $(LIB_FE_FAIL_ST_D)\
 $(LIB_FE_N_ST_D)\
 $(LIB_FE_N_WC_ST_D)\
 $(LIB_FE_NULL_ST_D)\
 $(LIB_FE_NULL_WC_ST_D)\
 $(LIB_FE_SIMPLE_ST_D)\
 $(LIB_FE_SIMPLE_WC_ST_D)\
 $(LIB_FE_ALL_ST_R)\
 $(LIB_FE_ALL_WC_ST_R)\
 $(LIB_FE_FAIL_ST_R)\
 $(LIB_FE_N_ST_R)\
 $(LIB_FE_N_WC_ST_R)\
 $(LIB_FE_NULL_ST_R)\
 $(LIB_FE_NULL_WC_ST_R)\
 $(LIB_FE_SIMPLE_ST_R)\
 $(LIB_FE_SIMPLE_WC_ST_R)\
 \

TARGETS_LIB_FE=\
 $(TARGETS_LIB_FE_CMN)\
 \

TARGETS_LIB_BE_CMN=\
 $(LIB_BE_LRSPLIT_MT_D)\
 $(LIB_BE_N_MT_D)\
 $(LIB_BEC_FAIL_MT_D)\
 $(LIB_BE_FAIL_MT_D)\
 $(LIB_BEL_FAIL_MT_D)\
 $(LIB_BER_FAIL_MT_D)\
 $(LIB_BEC_FILE_MT_D)\
 $(LIB_BEC_FILE_WC_MT_D)\
 $(LIB_BE_FILE_MT_D)\
 $(LIB_BEL_FILE_MT_D)\
 $(LIB_BER_FILE_MT_D)\
 $(LIB_BEC_FPRINTF_MT_D)\
 $(LIB_BEC_FPRINTF_WC_MT_D)\
 $(LIB_BE_FPRINTF_MT_D)\
 $(LIB_BEL_FPRINTF_MT_D)\
 $(LIB_BER_FPRINTF_MT_D)\
 $(LIB_BEC_NULL_MT_D)\
 $(LIB_BE_NULL_MT_D)\
 $(LIB_BEL_NULL_MT_D)\
 $(LIB_BER_NULL_MT_D)\
 $(LIB_BE_LRSPLIT_MT_R)\
 $(LIB_BE_N_MT_R)\
 $(LIB_BEC_FAIL_MT_R)\
 $(LIB_BE_FAIL_MT_R)\
 $(LIB_BEL_FAIL_MT_R)\
 $(LIB_BER_FAIL_MT_R)\
 $(LIB_BEC_FILE_MT_R)\
 $(LIB_BEC_FILE_WC_MT_R)\
 $(LIB_BE_FILE_MT_R)\
 $(LIB_BEL_FILE_MT_R)\
 $(LIB_BER_FILE_MT_R)\
 $(LIB_BEC_FPRINTF_MT_R)\
 $(LIB_BEC_FPRINTF_WC_MT_R)\
 $(LIB_BE_FPRINTF_MT_R)\
 $(LIB_BEL_FPRINTF_MT_R)\
 $(LIB_BER_FPRINTF_MT_R)\
 $(LIB_BEC_NULL_MT_R)\
 $(LIB_BE_NULL_MT_R)\
 $(LIB_BEL_NULL_MT_R)\
 $(LIB_BER_NULL_MT_R)\
 $(LIB_BE_LRSPLIT_ST_D)\
 $(LIB_BE_N_ST_D)\
 $(LIB_BEC_FAIL_ST_D)\
 $(LIB_BE_FAIL_ST_D)\
 $(LIB_BEL_FAIL_ST_D)\
 $(LIB_BER_FAIL_ST_D)\
 $(LIB_BEC_FILE_ST_D)\
 $(LIB_BEC_FILE_WC_ST_D)\
 $(LIB_BE_FILE_ST_D)\
 $(LIB_BEL_FILE_ST_D)\
 $(LIB_BER_FILE_ST_D)\
 $(LIB_BEC_FPRINTF_ST_D)\
 $(LIB_BEC_FPRINTF_WC_ST_D)\
 $(LIB_BE_FPRINTF_ST_D)\
 $(LIB_BEL_FPRINTF_ST_D)\
 $(LIB_BER_FPRINTF_ST_D)\
 $(LIB_BEC_NULL_ST_D)\
 $(LIB_BE_NULL_ST_D)\
 $(LIB_BEL_NULL_ST_D)\
 $(LIB_BER_NULL_ST_D)\
 $(LIB_BE_LRSPLIT_ST_R)\
 $(LIB_BE_N_ST_R)\
 $(LIB_BEC_FAIL_ST_R)\
 $(LIB_BE_FAIL_ST_R)\
 $(LIB_BEL_FAIL_ST_R)\
 $(LIB_BER_FAIL_ST_R)\
 $(LIB_BEC_FILE_ST_R)\
 $(LIB_BEC_FILE_WC_ST_R)\
 $(LIB_BE_FILE_ST_R)\
 $(LIB_BEL_FILE_ST_R)\
 $(LIB_BER_FILE_ST_R)\
 $(LIB_BEC_FPRINTF_ST_R)\
 $(LIB_BEC_FPRINTF_WC_ST_R)\
 $(LIB_BE_FPRINTF_ST_R)\
 $(LIB_BEL_FPRINTF_ST_R)\
 $(LIB_BER_FPRINTF_ST_R)\
 $(LIB_BEC_NULL_ST_R)\
 $(LIB_BE_NULL_ST_R)\
 $(LIB_BEL_NULL_ST_R)\
 $(LIB_BER_NULL_ST_R)\
 \

TARGETS_LIB_BE=\
 $(LIB_BE_LRSPLIT_MT_D)\
 $(LIB_BE_N_MT_D)\
 $(LIB_BEC_TEST_MT_D)\
 $(LIB_BE_TEST_MT_D)\
 $(LIB_BEL_TEST_MT_D)\
 $(LIB_BER_TEST_MT_D)\
 $(LIB_BEC_FAIL_MT_D)\
 $(LIB_BE_FAIL_MT_D)\
 $(LIB_BEL_FAIL_MT_D)\
 $(LIB_BER_FAIL_MT_D)\
 $(LIB_BEC_FILE_MT_D)\
 $(LIB_BEC_FILE_WC_MT_D)\
 $(LIB_BE_FILE_MT_D)\
 $(LIB_BEL_FILE_MT_D)\
 $(LIB_BER_FILE_MT_D)\
 $(LIB_BEC_FPRINTF_MT_D)\
 $(LIB_BEC_FPRINTF_WC_MT_D)\
 $(LIB_BE_FPRINTF_MT_D)\
 $(LIB_BEL_FPRINTF_MT_D)\
 $(LIB_BER_FPRINTF_MT_D)\
 $(LIB_BEC_NULL_MT_D)\
 $(LIB_BE_NULL_MT_D)\
 $(LIB_BEL_NULL_MT_D)\
 $(LIB_BER_NULL_MT_D)\
 $(LIB_BEC_SYSLOG_MT_D)\
 $(LIB_BEC_SYSLOGWC_MT_D)\
 $(LIB_BE_SYSLOG_MT_D)\
 $(LIB_BER_SYSLOG_MT_D)\
 $(LIB_BE_LRSPLIT_MT_R)\
 $(LIB_BE_N_MT_R)\
 $(LIB_BEC_TEST_MT_R)\
 $(LIB_BE_TEST_MT_R)\
 $(LIB_BEL_TEST_MT_R)\
 $(LIB_BER_TEST_MT_R)\
 $(LIB_BEC_FAIL_MT_R)\
 $(LIB_BE_FAIL_MT_R)\
 $(LIB_BEL_FAIL_MT_R)\
 $(LIB_BER_FAIL_MT_R)\
 $(LIB_BEC_FILE_MT_R)\
 $(LIB_BEC_FILE_WC_MT_R)\
 $(LIB_BE_FILE_MT_R)\
 $(LIB_BEL_FILE_MT_R)\
 $(LIB_BER_FILE_MT_R)\
 $(LIB_BEC_FPRINTF_MT_R)\
 $(LIB_BEC_FPRINTF_WC_MT_R)\
 $(LIB_BE_FPRINTF_MT_R)\
 $(LIB_BEL_FPRINTF_MT_R)\
 $(LIB_BER_FPRINTF_MT_R)\
 $(LIB_BEC_NULL_MT_R)\
 $(LIB_BE_NULL_MT_R)\
 $(LIB_BEL_NULL_MT_R)\
 $(LIB_BER_NULL_MT_R)\
 $(LIB_BEC_SYSLOG_MT_R)\
 $(LIB_BEC_SYSLOGWC_MT_R)\
 $(LIB_BE_SYSLOG_MT_R)\
 $(LIB_BER_SYSLOG_MT_R)\
 $(LIB_BE_LRSPLIT_ST_D)\
 $(LIB_BE_N_ST_D)\
 $(LIB_BEC_TEST_ST_D)\
 $(LIB_BE_TEST_ST_D)\
 $(LIB_BEL_TEST_ST_D)\
 $(LIB_BER_TEST_ST_D)\
 $(LIB_BEC_FAIL_ST_D)\
 $(LIB_BE_FAIL_ST_D)\
 $(LIB_BEL_FAIL_ST_D)\
 $(LIB_BER_FAIL_ST_D)\
 $(LIB_BEC_FILE_ST_D)\
 $(LIB_BEC_FILE_WC_ST_D)\
 $(LIB_BE_FILE_ST_D)\
 $(LIB_BEL_FILE_ST_D)\
 $(LIB_BER_FILE_ST_D)\
 $(LIB_BEC_FPRINTF_ST_D)\
 $(LIB_BEC_FPRINTF_WC_ST_D)\
 $(LIB_BE_FPRINTF_ST_D)\
 $(LIB_BEL_FPRINTF_ST_D)\
 $(LIB_BER_FPRINTF_ST_D)\
 $(LIB_BEC_NULL_ST_D)\
 $(LIB_BE_NULL_ST_D)\
 $(LIB_BEL_NULL_ST_D)\
 $(LIB_BER_NULL_ST_D)\
 $(LIB_BEC_SYSLOG_ST_D)\
 $(LIB_BEC_SYSLOGWC_ST_D)\
 $(LIB_BE_SYSLOG_ST_D)\
 $(LIB_BER_SYSLOG_ST_D)\
 $(LIB_BE_LRSPLIT_ST_R)\
 $(LIB_BE_N_ST_R)\
 $(LIB_BEC_TEST_ST_R)\
 $(LIB_BE_TEST_ST_R)\
 $(LIB_BEL_TEST_ST_R)\
 $(LIB_BER_TEST_ST_R)\
 $(LIB_BEC_FAIL_ST_R)\
 $(LIB_BE_FAIL_ST_R)\
 $(LIB_BEL_FAIL_ST_R)\
 $(LIB_BER_FAIL_ST_R)\
 $(LIB_BEC_FILE_ST_R)\
 $(LIB_BEC_FILE_WC_ST_R)\
 $(LIB_BE_FILE_ST_R)\
 $(LIB_BEL_FILE_ST_R)\
 $(LIB_BER_FILE_ST_R)\
 $(LIB_BEC_FPRINTF_ST_R)\
 $(LIB_BEC_FPRINTF_WC_ST_R)\
 $(LIB_BE_FPRINTF_ST_R)\
 $(LIB_BEL_FPRINTF_ST_R)\
 $(LIB_BER_FPRINTF_ST_R)\
 $(LIB_BEC_NULL_ST_R)\
 $(LIB_BE_NULL_ST_R)\
 $(LIB_BEL_NULL_ST_R)\
 $(LIB_BER_NULL_ST_R)\
 $(LIB_BEC_SYSLOG_ST_R)\
 $(LIB_BEC_SYSLOGWC_ST_R)\
 $(LIB_BE_SYSLOG_ST_R)\
 $(LIB_BER_SYSLOG_ST_R)\
 \

ifdef ACE
TARGETS_LIB_ACE=\
 $(LIB_BEC_ACE_MT_D)\
 $(LIB_BE_ACE_MT_D)\
 $(LIB_BEL_ACE_MT_D)\
 $(LIB_BER_ACE_MT_D)\
 $(LIB_BEC_ACE_MT_R)\
 $(LIB_BE_ACE_MT_R)\
 $(LIB_BEL_ACE_MT_R)\
 $(LIB_BER_ACE_MT_R)\
 \

else # ACE
TARGETS_LIB_ACE=

endif # ACE
TARGETS_LIB_ALL=\
 $(TARGETS_LIB_UTIL)\
 $(TARGETS_LIB_CORE)\
 $(TARGETS_LIB_APPL)\
 $(TARGETS_LIB_FE)\
 $(TARGETS_LIB_BE)\
 $(TARGETS_LIB_ACE)\
 \

TARGETS_LIB_B64=\
 $(LIB_B64_MT_D)\
 $(LIB_B64_MT_R)\
 $(LIB_B64_ST_D)\
 $(LIB_B64_ST_R)\
 \

TARGETS_LIB_SHWILD=\
 $(LIB_SHWILD_MT_D)\
 $(LIB_SHWILD_MT_R)\
 $(LIB_SHWILD_ST_D)\
 $(LIB_SHWILD_ST_R)\
 \

TARGETS_LIB_XTESTS=\
 $(LIB_XTESTS_MT_D)\
 $(LIB_XTESTS_MT_R)\
 $(LIB_XTESTS_ST_D)\
 $(LIB_XTESTS_ST_R)\
 \

########################################
# Programs

TARGETS_EXAMPLE_C=\
 $(TARG_EX_C_0_MT_D)\
 $(TARG_EX_C_1_MT_D)\
 $(TARG_EX_C_2_MT_D)\
 $(TARG_EX_C_3_MT_D)\
 $(TARG_EX_C_4_MT_D)\
 $(TARG_EX_C_5_MT_D)\
 $(TARG_EX_C_6_MT_D)\
 $(TARG_EX_C_7_MT_D)\
 $(TARG_EX_C_0_MT_R)\
 $(TARG_EX_C_1_MT_R)\
 $(TARG_EX_C_2_MT_R)\
 $(TARG_EX_C_3_MT_R)\
 $(TARG_EX_C_4_MT_R)\
 $(TARG_EX_C_5_MT_R)\
 $(TARG_EX_C_6_MT_R)\
 $(TARG_EX_C_7_MT_R)\
 $(TARG_EX_C_0_ST_D)\
 $(TARG_EX_C_1_ST_D)\
 $(TARG_EX_C_2_ST_D)\
 $(TARG_EX_C_3_ST_D)\
 $(TARG_EX_C_4_ST_D)\
 $(TARG_EX_C_5_ST_D)\
 $(TARG_EX_C_6_ST_D)\
 $(TARG_EX_C_7_ST_D)\
 $(TARG_EX_C_0_ST_R)\
 $(TARG_EX_C_1_ST_R)\
 $(TARG_EX_C_2_ST_R)\
 $(TARG_EX_C_3_ST_R)\
 $(TARG_EX_C_4_ST_R)\
 $(TARG_EX_C_5_ST_R)\
 $(TARG_EX_C_6_ST_R)\
 $(TARG_EX_C_7_ST_R)\
 \

TARGETS_EXAMPLE_CPP=\
 $(TARG_EX_P_be_01_MT_D)\
 $(TARG_EX_P_be_02_MT_D)\
 $(TARG_EX_P_be_03_MT_D)\
 $(TARG_EX_P_be_04_MT_D)\
 $(TARG_EX_P_be_05_MT_D)\
 $(TARG_EX_P_be_06_MT_D)\
 $(TARG_EX_P_cp_01_MT_D)\
 $(TARG_EX_P_cp_02_MT_D)\
 $(TARG_EX_P_fm_01_MT_D)\
 $(TARG_EX_P_fm_02_MT_D)\
 $(TARG_EX_P_fm_03_MT_D)\
 $(TARG_EX_P_fe_01_MT_D)\
 $(TARG_EX_P_in_01_MT_D)\
 $(TARG_EX_P_in_02_MT_D)\
 $(TARG_EX_P_in_03_MT_D)\
 $(TARG_EX_P_in_04_MT_D)\
 $(TARG_EX_P_in_05_MT_D)\
 $(TARG_EX_P_in_06_MT_D)\
 $(TARG_EX_P_in_07_MT_D)\
 $(TARG_EX_P_in_08_MT_D)\
 $(TARG_EX_P_in_09_MT_D)\
 $(TARG_EX_P_in_10_MT_D)\
 $(TARG_EX_P_in_11_MT_D)\
 $(TARG_EX_P_in_12_MT_D)\
 $(TARG_EX_P_in_13_MT_D)\
 $(TARG_EX_P_in_14_MT_D)\
 $(TARG_EX_P_in_15_MT_D)\
 $(TARG_EX_P_in_16_MT_D)\
 $(TARG_EX_P_in_17_MT_D)\
 $(TARG_EX_P_ms_01_MT_D)\
 $(TARG_EX_P_ms_02_MT_D)\
 $(TARG_EX_P_ms_03_MT_D)\
 $(TARG_EX_P_ms_04_MT_D)\
 $(TARG_EX_P_ms_05_MT_D)\
 $(TARG_EX_P_ms_06_MT_D)\
 $(TARG_EX_P_tr_01_MT_D)\
 $(TARG_EX_P_tr_02_MT_D)\
 $(TARG_EX_P_ut_01_MT_D)\
 $(TARG_EX_P_be_01_MT_R)\
 $(TARG_EX_P_be_02_MT_R)\
 $(TARG_EX_P_be_03_MT_R)\
 $(TARG_EX_P_be_04_MT_R)\
 $(TARG_EX_P_be_05_MT_R)\
 $(TARG_EX_P_be_06_MT_R)\
 $(TARG_EX_P_cp_01_MT_R)\
 $(TARG_EX_P_cp_02_MT_R)\
 $(TARG_EX_P_fm_01_MT_R)\
 $(TARG_EX_P_fm_02_MT_R)\
 $(TARG_EX_P_fm_03_MT_R)\
 $(TARG_EX_P_fe_01_MT_R)\
 $(TARG_EX_P_in_01_MT_R)\
 $(TARG_EX_P_in_02_MT_R)\
 $(TARG_EX_P_in_03_MT_R)\
 $(TARG_EX_P_in_04_MT_R)\
 $(TARG_EX_P_in_05_MT_R)\
 $(TARG_EX_P_in_06_MT_R)\
 $(TARG_EX_P_in_07_MT_R)\
 $(TARG_EX_P_in_08_MT_R)\
 $(TARG_EX_P_in_09_MT_R)\
 $(TARG_EX_P_in_10_MT_R)\
 $(TARG_EX_P_in_11_MT_R)\
 $(TARG_EX_P_in_12_MT_R)\
 $(TARG_EX_P_in_13_MT_R)\
 $(TARG_EX_P_in_14_MT_R)\
 $(TARG_EX_P_in_15_MT_R)\
 $(TARG_EX_P_in_16_MT_R)\
 $(TARG_EX_P_in_17_MT_R)\
 $(TARG_EX_P_ms_01_MT_R)\
 $(TARG_EX_P_ms_02_MT_R)\
 $(TARG_EX_P_ms_03_MT_R)\
 $(TARG_EX_P_ms_04_MT_R)\
 $(TARG_EX_P_ms_05_MT_R)\
 $(TARG_EX_P_ms_06_MT_R)\
 $(TARG_EX_P_tr_01_MT_R)\
 $(TARG_EX_P_tr_02_MT_R)\
 $(TARG_EX_P_ut_01_MT_R)\
 $(TARG_EX_P_be_01_ST_D)\
 $(TARG_EX_P_be_02_ST_D)\
 $(TARG_EX_P_be_03_ST_D)\
 $(TARG_EX_P_be_04_ST_D)\
 $(TARG_EX_P_be_05_ST_D)\
 $(TARG_EX_P_be_06_ST_D)\
 $(TARG_EX_P_cp_01_ST_D)\
 $(TARG_EX_P_cp_02_ST_D)\
 $(TARG_EX_P_fm_01_ST_D)\
 $(TARG_EX_P_fm_02_ST_D)\
 $(TARG_EX_P_fm_03_ST_D)\
 $(TARG_EX_P_fe_01_ST_D)\
 $(TARG_EX_P_in_01_ST_D)\
 $(TARG_EX_P_in_02_ST_D)\
 $(TARG_EX_P_in_03_ST_D)\
 $(TARG_EX_P_in_04_ST_D)\
 $(TARG_EX_P_in_05_ST_D)\
 $(TARG_EX_P_in_06_ST_D)\
 $(TARG_EX_P_in_07_ST_D)\
 $(TARG_EX_P_in_08_ST_D)\
 $(TARG_EX_P_in_09_ST_D)\
 $(TARG_EX_P_in_10_ST_D)\
 $(TARG_EX_P_in_11_ST_D)\
 $(TARG_EX_P_in_12_ST_D)\
 $(TARG_EX_P_in_13_ST_D)\
 $(TARG_EX_P_in_14_ST_D)\
 $(TARG_EX_P_in_15_ST_D)\
 $(TARG_EX_P_in_16_ST_D)\
 $(TARG_EX_P_in_17_ST_D)\
 $(TARG_EX_P_ms_01_ST_D)\
 $(TARG_EX_P_ms_02_ST_D)\
 $(TARG_EX_P_ms_03_ST_D)\
 $(TARG_EX_P_ms_04_ST_D)\
 $(TARG_EX_P_ms_05_ST_D)\
 $(TARG_EX_P_ms_06_ST_D)\
 $(TARG_EX_P_tr_01_ST_D)\
 $(TARG_EX_P_tr_02_ST_D)\
 $(TARG_EX_P_ut_01_ST_D)\
 $(TARG_EX_P_be_01_ST_R)\
 $(TARG_EX_P_be_02_ST_R)\
 $(TARG_EX_P_be_03_ST_R)\
 $(TARG_EX_P_be_04_ST_R)\
 $(TARG_EX_P_be_05_ST_R)\
 $(TARG_EX_P_be_06_ST_R)\
 $(TARG_EX_P_cp_01_ST_R)\
 $(TARG_EX_P_cp_02_ST_R)\
 $(TARG_EX_P_fm_01_ST_R)\
 $(TARG_EX_P_fm_02_ST_R)\
 $(TARG_EX_P_fm_03_ST_R)\
 $(TARG_EX_P_fe_01_ST_R)\
 $(TARG_EX_P_in_01_ST_R)\
 $(TARG_EX_P_in_02_ST_R)\
 $(TARG_EX_P_in_03_ST_R)\
 $(TARG_EX_P_in_04_ST_R)\
 $(TARG_EX_P_in_05_ST_R)\
 $(TARG_EX_P_in_06_ST_R)\
 $(TARG_EX_P_in_07_ST_R)\
 $(TARG_EX_P_in_08_ST_R)\
 $(TARG_EX_P_in_09_ST_R)\
 $(TARG_EX_P_in_10_ST_R)\
 $(TARG_EX_P_in_11_ST_R)\
 $(TARG_EX_P_in_12_ST_R)\
 $(TARG_EX_P_in_13_ST_R)\
 $(TARG_EX_P_in_14_ST_R)\
 $(TARG_EX_P_in_15_ST_R)\
 $(TARG_EX_P_in_16_ST_R)\
 $(TARG_EX_P_in_17_ST_R)\
 $(TARG_EX_P_ms_01_ST_R)\
 $(TARG_EX_P_ms_02_ST_R)\
 $(TARG_EX_P_ms_03_ST_R)\
 $(TARG_EX_P_ms_04_ST_R)\
 $(TARG_EX_P_ms_05_ST_R)\
 $(TARG_EX_P_ms_06_ST_R)\
 $(TARG_EX_P_tr_01_ST_R)\
 $(TARG_EX_P_tr_02_ST_R)\
 $(TARG_EX_P_ut_01_ST_R)\
 \

TARGETS_EXAMPLE=\
 $(TARGETS_EXAMPLE_C)\
 $(TARGETS_EXAMPLE_CPP)\
 \


TARGETS_SAMPLE=\
 \


TARGETS_TEST_COMPONENT=\
 $(TARG_TEST_COMP_core_initseq_MT_D)\
 $(TARG_TEST_COMP_custsev_MT_D)\
 $(TARG_TEST_COMP_filemt_MT_D)\
 $(TARG_TEST_COMP_ins_args_MT_D)\
 $(TARG_TEST_COMP_ins_b64_MT_D)\
 $(TARG_TEST_COMP_ins_bool_MT_D)\
 $(TARG_TEST_COMP_ins_hostid_MT_D)\
 $(TARG_TEST_COMP_ins_int_MT_D)\
 $(TARG_TEST_COMP_ins_pad_MT_D)\
 $(TARG_TEST_COMP_ins_ptr_MT_D)\
 $(TARG_TEST_COMP_ins_prid_MT_D)\
 $(TARG_TEST_COMP_ins_real_MT_D)\
 $(TARG_TEST_COMP_ins_slice_MT_D)\
 $(TARG_TEST_COMP_ins_thrid_MT_D)\
 $(TARG_TEST_COMP_ins_w2m_MT_D)\
 $(TARG_TEST_COMP_log_1_MT_D)\
 $(TARG_TEST_COMP_pan_puts_MT_D)\
 $(TARG_TEST_COMP_pan_prtf_MT_D)\
 $(TARG_TEST_COMP_trace_1_MT_D)\
 $(TARG_TEST_COMP_core_initseq_MT_R)\
 $(TARG_TEST_COMP_custsev_MT_R)\
 $(TARG_TEST_COMP_filemt_MT_R)\
 $(TARG_TEST_COMP_ins_args_MT_R)\
 $(TARG_TEST_COMP_ins_b64_MT_R)\
 $(TARG_TEST_COMP_ins_bool_MT_R)\
 $(TARG_TEST_COMP_ins_hostid_MT_R)\
 $(TARG_TEST_COMP_ins_int_MT_R)\
 $(TARG_TEST_COMP_ins_pad_MT_R)\
 $(TARG_TEST_COMP_ins_ptr_MT_R)\
 $(TARG_TEST_COMP_ins_prid_MT_R)\
 $(TARG_TEST_COMP_ins_real_MT_R)\
 $(TARG_TEST_COMP_ins_slice_MT_R)\
 $(TARG_TEST_COMP_ins_thrid_MT_R)\
 $(TARG_TEST_COMP_ins_w2m_MT_R)\
 $(TARG_TEST_COMP_log_1_MT_R)\
 $(TARG_TEST_COMP_pan_puts_MT_R)\
 $(TARG_TEST_COMP_pan_prtf_MT_R)\
 $(TARG_TEST_COMP_trace_1_MT_R)\
 $(TARG_TEST_COMP_core_initseq_ST_D)\
 $(TARG_TEST_COMP_custsev_ST_D)\
 $(TARG_TEST_COMP_ins_args_ST_D)\
 $(TARG_TEST_COMP_ins_b64_ST_D)\
 $(TARG_TEST_COMP_ins_bool_ST_D)\
 $(TARG_TEST_COMP_ins_hostid_ST_D)\
 $(TARG_TEST_COMP_ins_int_ST_D)\
 $(TARG_TEST_COMP_ins_pad_ST_D)\
 $(TARG_TEST_COMP_ins_ptr_ST_D)\
 $(TARG_TEST_COMP_ins_prid_ST_D)\
 $(TARG_TEST_COMP_ins_real_ST_D)\
 $(TARG_TEST_COMP_ins_slice_ST_D)\
 $(TARG_TEST_COMP_ins_thrid_ST_D)\
 $(TARG_TEST_COMP_ins_w2m_ST_D)\
 $(TARG_TEST_COMP_log_1_ST_D)\
 $(TARG_TEST_COMP_pan_puts_ST_D)\
 $(TARG_TEST_COMP_pan_prtf_ST_D)\
 $(TARG_TEST_COMP_trace_1_ST_D)\
 $(TARG_TEST_COMP_core_initseq_ST_R)\
 $(TARG_TEST_COMP_custsev_ST_R)\
 $(TARG_TEST_COMP_ins_args_ST_R)\
 $(TARG_TEST_COMP_ins_b64_ST_R)\
 $(TARG_TEST_COMP_ins_bool_ST_R)\
 $(TARG_TEST_COMP_ins_hostid_ST_R)\
 $(TARG_TEST_COMP_ins_int_ST_R)\
 $(TARG_TEST_COMP_ins_pad_ST_R)\
 $(TARG_TEST_COMP_ins_ptr_ST_R)\
 $(TARG_TEST_COMP_ins_prid_ST_R)\
 $(TARG_TEST_COMP_ins_real_ST_R)\
 $(TARG_TEST_COMP_ins_slice_ST_R)\
 $(TARG_TEST_COMP_ins_thrid_ST_R)\
 $(TARG_TEST_COMP_ins_w2m_ST_R)\
 $(TARG_TEST_COMP_log_1_ST_R)\
 $(TARG_TEST_COMP_pan_puts_ST_R)\
 $(TARG_TEST_COMP_pan_prtf_ST_R)\
 $(TARG_TEST_COMP_trace_1_ST_R)\
 \

TARGETS_TEST_PERFORMANCE=\
 $(TARG_TEST_PERF_ins_w2m_MT_D)\
 $(TARG_TEST_PERF_ins_w2m_MT_R)\
 $(TARG_TEST_PERF_ins_w2m_ST_D)\
 $(TARG_TEST_PERF_ins_w2m_ST_R)\
 \

TARGETS_TEST_SCRATCH_STD=\
 $(TARG_TEST_MT_D_APIwBE_FPRINTF)\
 $(TARG_TEST_SCRT_BE_SYSLOG_MT_D)\
 $(TARG_TEST_SCRT_BE_SYSLOGWC_MT_D)\
 $(TARG_TEST_MT_D_APIwBELR_1_FPRINTF_SYSLOG)\
 $(TARG_TEST_FE_MT_D)\
 $(TARG_TEST_BE_N_FAIL_MT_D)\
 $(TARG_TEST_FE_SIMPLE_WC_MT_D)\
 $(TARG_TEST_MT_R_APIwBE_FPRINTF)\
 $(TARG_TEST_SCRT_BE_SYSLOG_MT_R)\
 $(TARG_TEST_SCRT_BE_SYSLOGWC_MT_R)\
 $(TARG_TEST_MT_R_APIwBELR_1_FPRINTF_SYSLOG)\
 $(TARG_TEST_FE_MT_R)\
 $(TARG_TEST_BE_N_FAIL_MT_R)\
 $(TARG_TEST_FE_SIMPLE_WC_MT_R)\
 $(TARG_TEST_ST_D_APIwBE_FPRINTF)\
 $(TARG_TEST_SCRT_BE_SYSLOG_ST_D)\
 $(TARG_TEST_SCRT_BE_SYSLOGWC_ST_D)\
 $(TARG_TEST_ST_D_APIwBELR_1_FPRINTF_SYSLOG)\
 $(TARG_TEST_FE_ST_D)\
 $(TARG_TEST_BE_N_FAIL_ST_D)\
 $(TARG_TEST_FE_SIMPLE_WC_ST_D)\
 $(TARG_TEST_ST_R_APIwBE_FPRINTF)\
 $(TARG_TEST_SCRT_BE_SYSLOG_ST_R)\
 $(TARG_TEST_SCRT_BE_SYSLOGWC_ST_R)\
 $(TARG_TEST_ST_R_APIwBELR_1_FPRINTF_SYSLOG)\
 $(TARG_TEST_FE_ST_R)\
 $(TARG_TEST_BE_N_FAIL_ST_R)\
 $(TARG_TEST_FE_SIMPLE_WC_ST_R)\

ifdef ACE
TARGETS_TEST_SCRATCH_ACE=\
 $(TEST_SCRT_ACE_MT_D)\
 $(TEST_SCRT_ACE_MT_R)\
 \

endif # ACE
TARGETS_TEST_SCRATCH=\
 $(TARGETS_TEST_SCRATCH_STD)\
 \
# $(TARG_TEST_SCRT_file_MT_D)\
# $(TARG_TEST_SCRT_file_MT_R)\
# $(TARG_TEST_SCRT_file_ST_D)\
# $(TARG_TEST_SCRT_file_ST_R)\
 \
# $(TARG_TEST_SCRT_file_MT_D)\
# $(TARG_TEST_SCRT_file_MT_R)\
# $(TARG_TEST_SCRT_file_ST_D)\
# $(TARG_TEST_SCRT_file_ST_R)\
 \

TARGETS_TEST_UNIT=\
 $(TARG_TEST_UNIT_lvls_dyninit_MT_D)\
 $(TARG_TEST_UNIT_lvls_vals_MT_D)\
 $(TARG_TEST_UNIT_getcurrtm_MT_D)\
 $(TARG_TEST_UNIT_getversion_MT_D)\
 $(TARG_TEST_UNIT_gethostname_MT_D)\
 $(TARG_TEST_UNIT_onbailout_MT_D)\
 $(TARG_TEST_UNIT_strnlen_MT_D)\
 \
 $(TARG_TEST_UNIT_bec_file_MT_D)\
 $(TARG_TEST_UNIT_bec_fprintf_MT_D)\
 $(TARG_TEST_UNIT_bec_fail_MT_D)\
 $(TARG_TEST_UNIT_be_fail_MT_D)\
 $(TARG_TEST_UNIT_be_lrsplit_MT_D)\
 $(TARG_TEST_UNIT_be_N_MT_D)\
 \
 $(TARG_TEST_UNIT_fe_all_MT_D)\
 $(TARG_TEST_UNIT_fe_fail_MT_D)\
 $(TARG_TEST_UNIT_fe_N_MT_D)\
 $(TARG_TEST_UNIT_fe_null_MT_D)\
 $(TARG_TEST_UNIT_fe_simple_MT_D)\
 $(TARG_TEST_UNIT_ins_int_MT_D)\
 $(TARG_TEST_UNIT_ins_w2m_MT_D)\
 $(TARG_TEST_UNIT_lvls_dyninit_MT_R)\
 $(TARG_TEST_UNIT_lvls_vals_MT_R)\
 $(TARG_TEST_UNIT_getcurrtm_MT_R)\
 $(TARG_TEST_UNIT_getversion_MT_R)\
 $(TARG_TEST_UNIT_gethostname_MT_R)\
 $(TARG_TEST_UNIT_onbailout_MT_R)\
 $(TARG_TEST_UNIT_strnlen_MT_R)\
 \
 $(TARG_TEST_UNIT_bec_file_MT_R)\
 $(TARG_TEST_UNIT_bec_fprintf_MT_R)\
 $(TARG_TEST_UNIT_bec_fail_MT_R)\
 $(TARG_TEST_UNIT_be_fail_MT_R)\
 $(TARG_TEST_UNIT_be_lrsplit_MT_R)\
 $(TARG_TEST_UNIT_be_N_MT_R)\
 \
 $(TARG_TEST_UNIT_fe_all_MT_R)\
 $(TARG_TEST_UNIT_fe_fail_MT_R)\
 $(TARG_TEST_UNIT_fe_N_MT_R)\
 $(TARG_TEST_UNIT_fe_null_MT_R)\
 $(TARG_TEST_UNIT_fe_simple_MT_R)\
 $(TARG_TEST_UNIT_ins_int_MT_R)\
 $(TARG_TEST_UNIT_ins_w2m_MT_R)\
 $(TARG_TEST_UNIT_lvls_dyninit_ST_D)\
 $(TARG_TEST_UNIT_lvls_vals_ST_D)\
 $(TARG_TEST_UNIT_getcurrtm_ST_D)\
 $(TARG_TEST_UNIT_getversion_ST_D)\
 $(TARG_TEST_UNIT_gethostname_ST_D)\
 $(TARG_TEST_UNIT_onbailout_ST_D)\
 $(TARG_TEST_UNIT_strnlen_ST_D)\
 \
 $(TARG_TEST_UNIT_bec_file_ST_D)\
 $(TARG_TEST_UNIT_bec_fprintf_ST_D)\
 $(TARG_TEST_UNIT_bec_fail_ST_D)\
 $(TARG_TEST_UNIT_be_fail_ST_D)\
 $(TARG_TEST_UNIT_be_lrsplit_ST_D)\
 $(TARG_TEST_UNIT_be_N_ST_D)\
 \
 $(TARG_TEST_UNIT_fe_all_ST_D)\
 $(TARG_TEST_UNIT_fe_fail_ST_D)\
 $(TARG_TEST_UNIT_fe_N_ST_D)\
 $(TARG_TEST_UNIT_fe_null_ST_D)\
 $(TARG_TEST_UNIT_fe_simple_ST_D)\
 $(TARG_TEST_UNIT_ins_int_ST_D)\
 $(TARG_TEST_UNIT_ins_w2m_ST_D)\
 $(TARG_TEST_UNIT_lvls_dyninit_ST_R)\
 $(TARG_TEST_UNIT_lvls_vals_ST_R)\
 $(TARG_TEST_UNIT_getcurrtm_ST_R)\
 $(TARG_TEST_UNIT_getversion_ST_R)\
 $(TARG_TEST_UNIT_gethostname_ST_R)\
 $(TARG_TEST_UNIT_onbailout_ST_R)\
 $(TARG_TEST_UNIT_strnlen_ST_R)\
 \
 $(TARG_TEST_UNIT_bec_file_ST_R)\
 $(TARG_TEST_UNIT_bec_fprintf_ST_R)\
 $(TARG_TEST_UNIT_bec_fail_ST_R)\
 $(TARG_TEST_UNIT_be_fail_ST_R)\
 $(TARG_TEST_UNIT_be_lrsplit_ST_R)\
 $(TARG_TEST_UNIT_be_N_ST_R)\
 \
 $(TARG_TEST_UNIT_fe_all_ST_R)\
 $(TARG_TEST_UNIT_fe_fail_ST_R)\
 $(TARG_TEST_UNIT_fe_N_ST_R)\
 $(TARG_TEST_UNIT_fe_null_ST_R)\
 $(TARG_TEST_UNIT_fe_simple_ST_R)\
 $(TARG_TEST_UNIT_ins_int_ST_R)\
 $(TARG_TEST_UNIT_ins_w2m_ST_R)\
 \

############################################################
# Third-party libraries

ifdef ACE
LIBS_3PTY_ACE					=	-libace.a
endif # ACE
LIBS_3PTY_FPRINTF				=
LIBS_3PTY_SYSLOG				=
LIBS_3PTY_BE_N_FAIL				=
LIBS_3PTY_TEST_UNIT				=

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


STLSOFT_SENTINEL_HEADER		=	$(STLSOFT)/include/stlsoft/stlsoft.h
STLSOFT_PREP_FILE			=	$(STLSOFT)/include/stlsoft/internal/dos2unix.has.been.performed
PANTHEIOS_SENTINEL_HEADER	=	$(PROJ_INCLUDE_DIR)/pantheios/pantheios.h
PANTHEIOS_PREP_FILE			=	$(PROJ_INCLUDE_DIR)/pantheios/internal/dos2unix.has.been.performed

$(STLSOFT_PREP_FILE):	$(STLSOFT_SENTINEL_HEADER)
	@echo Ensuring all STLSoft C source files are in UNIX format
	@find $(STLSOFT) -name '*.h' | while read f ; do \
	  set -e ; sed 's/\r$$//' $$f > $$f.tmp ; mv $$f.tmp $$f ; \
	done
	@echo dos2unix.has.been.performed > $(STLSOFT_PREP_FILE)

$(PANTHEIOS_PREP_FILE):	$(PANTHEIOS_SENTINEL_HEADER)
	@echo Ensuring all Pantheios - and bundled library - C source files are in UNIX format
	@find $(PROJ_BASE_DIR) -name '*.[ch]' | while read f ; do \
	  set -e ; sed 's/\r$$//' $$f > $$f.tmp ; mv $$f.tmp $$f ; \
	done
	@echo dos2unix.has.been.performed > $(PANTHEIOS_PREP_FILE)

prep.stlsoft.for.os:	$(STLSOFT_PREP_FILE)

prep.pantheios.for.os:	$(PANTHEIOS_PREP_FILE)


prep.files.for.os:	$(PROJ_MAKEFILE_NAME)\
	prep.stlsoft.for.os\
	prep.pantheios.for.os\
	\

prep:	prep.files.for.os


check.lib.dir:		$(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)

check.bin.dir:		$(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_BIN_DIR)


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

ifdef ACE
compile.ace:                $(PROJ_MAKEFILE_NAME)   prep.files.for.os   $(OBJS_ACE)

endif # ACE
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

ifdef ACE
compile.all:                                                            compile.libs compile.examples compile.test compile.ace
else # ACE
compile.all:                                                            compile.libs compile.examples compile.test
endif # ACE


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

ifdef ACE
build.libs.ace:                                                         check.lib.dir $(TARGETS_LIB_ACE)

endif # ACE
build.libs:                                                             build.libs.util build.libs.core build.libs.appl build.libs.fe build.libs.be

ifdef ACE
build.libs.all:                                                         build.libs build.libs.ace
else # ACE
build.libs.all:                                                         build.libs
endif # ACE

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

ifdef ACE
build.test.ace:             check.bin.dir           prep.files.for.os   $(TARGETS_TEST_SCRATCH_ACE)

endif # ACE
build.test.std:             check.bin.dir           prep.files.for.os   $(TARGETS_TEST_SCRATCH_STD)

ifdef ACE
build.test.ace:             check.bin.dir           prep.files.for.os   $(TARGETS_TEST_SCRATCH_ACE)

endif # ACE
build.std:                                                              build.libs.util build.libs.core build.libs.appl build.libs.fe build.libs.be build.test.std

ifdef ACE
build.test.all:                                                         build.test.std build.test.ace
else # ACE
build.test.all:                                                         build.test.std
endif # ACE


ifdef ACE
build.ace:                                                              build.libs.ace build.test.ace

endif # ACE
build:                                                                  build.libs build.examples build.samples build.test

build.cmn:                                                              build.libs.util build.libs.core build.libs.appl build.libs.fe.cmn build.libs.be.cmn

build.common:                                                           build.cmn

build.all:                                                              build.libs.all build.examples build.samples build.test.all


# test targets

test.component_:
	@echo Running Test:Component programs ...
	$(TARG_TEST_COMP_core_initseq_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_core_initseq_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_core_initseq_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_core_initseq_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_custsev_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_custsev_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_custsev_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_custsev_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_filemt_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_filemt_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_args_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_args_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_args_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_args_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_b64_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_b64_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_b64_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_b64_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_bool_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_bool_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_bool_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_bool_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_hostid_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_hostid_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_hostid_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_hostid_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_int_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_int_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_int_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_int_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_pad_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_pad_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_pad_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_pad_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_ptr_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_ptr_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_ptr_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_ptr_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_prid_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_prid_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_prid_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_prid_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_real_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_real_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_real_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_real_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_thrid_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_thrid_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_thrid_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_thrid_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_w2m_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_w2m_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_w2m_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_ins_w2m_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_log_1_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_log_1_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_log_1_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_log_1_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_puts_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_puts_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_puts_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_puts_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_prtf_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_prtf_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_prtf_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_pan_prtf_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_trace_1_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_trace_1_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_trace_1_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_COMP_trace_1_ST_R) --verbosity=$(TEST_VERBOSITY)

test.performance_:
	@echo Running Test:Performance programs ...
	$(TARG_TEST_PERF_ins_w2m_MT_R) on
	$(TARG_TEST_PERF_ins_w2m_MT_R) off
	$(TARG_TEST_PERF_ins_w2m_ST_R) on
	$(TARG_TEST_PERF_ins_w2m_ST_R) off

test.scratch_:
	@echo Running Test:Scratch programs ...
	$(TARG_TEST_MT_D_APIwBE_FPRINTF)
	$(TARG_TEST_SCRT_BE_SYSLOG_MT_D)
	$(TARG_TEST_SCRT_BE_SYSLOGWC_MT_D)
	$(TARG_TEST_MT_D_APIwBELR_1_FPRINTF_SYSLOG)
	$(TARG_TEST_FE_MT_D)
	$(TARG_TEST_MT_D_APIwBE_FPRINTF)
	$(TARG_TEST_BE_N_FAIL_MT_D)
	$(TARG_TEST_FE_SIMPLE_WC_MT_D)
	$(TARG_TEST_MT_R_APIwBE_FPRINTF)
	$(TARG_TEST_SCRT_BE_SYSLOG_MT_R)
	$(TARG_TEST_SCRT_BE_SYSLOGWC_MT_R)
	$(TARG_TEST_MT_R_APIwBELR_1_FPRINTF_SYSLOG)
	$(TARG_TEST_FE_MT_R)
	$(TARG_TEST_MT_R_APIwBE_FPRINTF)
	$(TARG_TEST_BE_N_FAIL_MT_R)
	$(TARG_TEST_FE_SIMPLE_WC_MT_R)
	$(TARG_TEST_ST_D_APIwBE_FPRINTF)
	$(TARG_TEST_SCRT_BE_SYSLOG_ST_D)
	$(TARG_TEST_SCRT_BE_SYSLOGWC_ST_D)
	$(TARG_TEST_ST_D_APIwBELR_1_FPRINTF_SYSLOG)
	$(TARG_TEST_FE_ST_D)
	$(TARG_TEST_ST_D_APIwBE_FPRINTF)
	$(TARG_TEST_BE_N_FAIL_ST_D)
	$(TARG_TEST_FE_SIMPLE_WC_ST_D)
	$(TARG_TEST_ST_R_APIwBE_FPRINTF)
	$(TARG_TEST_SCRT_BE_SYSLOG_ST_R)
	$(TARG_TEST_SCRT_BE_SYSLOGWC_ST_R)
	$(TARG_TEST_ST_R_APIwBELR_1_FPRINTF_SYSLOG)
	$(TARG_TEST_FE_ST_R)
	$(TARG_TEST_ST_R_APIwBE_FPRINTF)
	$(TARG_TEST_BE_N_FAIL_ST_R)
	$(TARG_TEST_FE_SIMPLE_WC_ST_R)

test.unit_:
	@echo Running Test:Unit programs ...
	$(TARG_TEST_UNIT_lvls_dyninit_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_dyninit_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_dyninit_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_dyninit_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_vals_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_vals_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_vals_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_lvls_vals_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getcurrtm_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getcurrtm_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getcurrtm_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getcurrtm_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getversion_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getversion_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getversion_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_getversion_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_gethostname_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_gethostname_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_gethostname_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_gethostname_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_onbailout_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_onbailout_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_onbailout_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_onbailout_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_strnlen_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_strnlen_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_strnlen_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_strnlen_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_file_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_file_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_file_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_file_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fprintf_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fprintf_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fprintf_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fprintf_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fail_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fail_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fail_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_bec_fail_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_fail_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_fail_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_fail_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_fail_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_lrsplit_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_lrsplit_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_lrsplit_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_lrsplit_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_N_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_N_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_N_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_be_N_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_all_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_all_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_all_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_all_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_fail_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_fail_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_fail_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_fail_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_N_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_N_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_N_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_N_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_null_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_null_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_null_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_null_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_simple_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_simple_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_simple_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_fe_simple_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_int_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_int_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_int_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_int_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_w2m_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_w2m_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_w2m_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_TEST_UNIT_ins_w2m_ST_R) --verbosity=$(TEST_VERBOSITY)

test.examples_:
	@echo Running Example programs ...
	$(TARG_EX_C_0_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_1_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_2_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_3_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_4_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_5_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_7_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_0_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_1_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_2_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_3_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_4_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_5_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_7_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_0_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_1_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_2_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_3_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_4_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_5_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_7_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_0_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_1_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_2_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_3_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_4_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_5_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_C_7_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_02_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_03_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_04_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_05_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_06_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_02_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_02_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_03_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fe_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_02_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_03_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_04_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_05_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_06_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_07_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_08_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_09_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_10_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_11_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_13_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_14_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_15_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_16_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_17_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_02_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_03_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_04_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_05_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_06_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_02_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ut_01_MT_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_02_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_03_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_04_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_05_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_06_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_02_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_02_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_03_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fe_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_02_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_03_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_04_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_05_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_06_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_07_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_08_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_09_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_10_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_11_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_13_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_14_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_15_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_16_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_17_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_02_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_03_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_04_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_05_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_06_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_02_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ut_01_MT_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_02_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_03_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_04_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_05_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_06_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_02_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_02_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_03_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fe_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_02_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_03_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_04_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_05_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_06_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_07_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_08_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_09_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_10_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_11_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_13_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_14_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_15_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_16_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_17_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_02_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_03_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_04_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_05_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_06_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_02_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ut_01_ST_D) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_01_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_02_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_03_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_04_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_05_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_be_06_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_01_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_cp_02_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_01_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_02_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fm_03_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_fe_01_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_01_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_02_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_03_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_04_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_05_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_06_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_07_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_08_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_09_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_10_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_11_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_13_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_14_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_15_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_16_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_in_17_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_01_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_02_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_03_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_04_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_05_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ms_06_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_01_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_tr_02_ST_R) --verbosity=$(TEST_VERBOSITY)
	$(TARG_EX_P_ut_01_ST_R) --verbosity=$(TEST_VERBOSITY)

test.component:		build.test.component test.component_

test.performance:	build.test.performance test.performance_

test.scratch:		build.test.scratch test.scratch_

test.unit:			build.test.unit test.unit_

test.examples:		build.examples test.examples_

ifdef ACE
test.ace:			build.libs.ace build.libs.core $(TARGETS_TEST_SCRATCH_ACE)
	$(TEST_SCRT_ACE_MT_D)
	$(TEST_SCRT_ACE_MT_R)

endif # ACE
test:                       test.unit test.component

ifdef ACE
test.all:                   test.scratch test.examples test.performance test.ace test
else # ACE
test.all:                   test.scratch test.examples test.performance test
endif # ACE


# clean targets

clean.libs.core:
	@echo Cleaning Core libraries ...
	@-$(RM) $(LIB_UTIL_MT_D)
	@-$(RM) $(LIB_CORE_MT_D)
	@-$(RM) $(LIB_APPL_MT_D)
	@-$(RM) $(LIB_UTIL_MT_R)
	@-$(RM) $(LIB_CORE_MT_R)
	@-$(RM) $(LIB_APPL_MT_R)
	@-$(RM) $(LIB_UTIL_ST_D)
	@-$(RM) $(LIB_CORE_ST_D)
	@-$(RM) $(LIB_APPL_ST_D)
	@-$(RM) $(LIB_UTIL_ST_R)
	@-$(RM) $(LIB_CORE_ST_R)
	@-$(RM) $(LIB_APPL_ST_R)
	@-$(RM) $(OBJ_UTIL_MT_D)
	@-$(RM) $(OBJ_CORE_MT_D)
	@-$(RM) $(OBJ_APPL_MT_D)
	@-$(RM) $(OBJ_UTIL_MT_R)
	@-$(RM) $(OBJ_CORE_MT_R)
	@-$(RM) $(OBJ_APPL_MT_R)
	@-$(RM) $(OBJ_UTIL_ST_D)
	@-$(RM) $(OBJ_CORE_ST_D)
	@-$(RM) $(OBJ_APPL_ST_D)
	@-$(RM) $(OBJ_UTIL_ST_R)
	@-$(RM) $(OBJ_CORE_ST_R)
	@-$(RM) $(OBJ_APPL_ST_R)

clean.libs.be:
	@echo Cleaning Back-end libraries ...
	@-$(RM) $(LIB_BE_LRSPLIT_MT_D)
	@-$(RM) $(LIB_BE_N_MT_D)
	@-$(RM) $(LIB_BEC_TEST_MT_D)
	@-$(RM) $(LIB_BE_TEST_MT_D)
	@-$(RM) $(LIB_BEL_TEST_MT_D)
	@-$(RM) $(LIB_BER_TEST_MT_D)
	@-$(RM) $(LIB_BEC_FAIL_MT_D)
	@-$(RM) $(LIB_BE_FAIL_MT_D)
	@-$(RM) $(LIB_BEL_FAIL_MT_D)
	@-$(RM) $(LIB_BER_FAIL_MT_D)
	@-$(RM) $(LIB_BEC_FILE_MT_D)
	@-$(RM) $(LIB_BEC_FILE_WC_MT_D)
	@-$(RM) $(LIB_BE_FILE_MT_D)
	@-$(RM) $(LIB_BEL_FILE_MT_D)
	@-$(RM) $(LIB_BER_FILE_MT_D)
	@-$(RM) $(LIB_BEC_FPRINTF_MT_D)
	@-$(RM) $(LIB_BEC_FPRINTF_WC_MT_D)
	@-$(RM) $(LIB_BE_FPRINTF_MT_D)
	@-$(RM) $(LIB_BEL_FPRINTF_MT_D)
	@-$(RM) $(LIB_BER_FPRINTF_MT_D)
	@-$(RM) $(LIB_BEC_NULL_MT_D)
	@-$(RM) $(LIB_BE_NULL_MT_D)
	@-$(RM) $(LIB_BEL_NULL_MT_D)
	@-$(RM) $(LIB_BER_NULL_MT_D)
	@-$(RM) $(LIB_BEC_SYSLOG_MT_D)
	@-$(RM) $(LIB_BEC_SYSLOGWC_MT_D)
	@-$(RM) $(LIB_BE_SYSLOG_MT_D)
	@-$(RM) $(LIB_BER_SYSLOG_MT_D)
	@-$(RM) $(LIB_BE_LRSPLIT_MT_R)
	@-$(RM) $(LIB_BE_N_MT_R)
	@-$(RM) $(LIB_BEC_TEST_MT_R)
	@-$(RM) $(LIB_BE_TEST_MT_R)
	@-$(RM) $(LIB_BEL_TEST_MT_R)
	@-$(RM) $(LIB_BER_TEST_MT_R)
	@-$(RM) $(LIB_BEC_FAIL_MT_R)
	@-$(RM) $(LIB_BE_FAIL_MT_R)
	@-$(RM) $(LIB_BEL_FAIL_MT_R)
	@-$(RM) $(LIB_BER_FAIL_MT_R)
	@-$(RM) $(LIB_BEC_FILE_MT_R)
	@-$(RM) $(LIB_BEC_FILE_WC_MT_R)
	@-$(RM) $(LIB_BE_FILE_MT_R)
	@-$(RM) $(LIB_BEL_FILE_MT_R)
	@-$(RM) $(LIB_BER_FILE_MT_R)
	@-$(RM) $(LIB_BEC_FPRINTF_MT_R)
	@-$(RM) $(LIB_BEC_FPRINTF_WC_MT_R)
	@-$(RM) $(LIB_BE_FPRINTF_MT_R)
	@-$(RM) $(LIB_BEL_FPRINTF_MT_R)
	@-$(RM) $(LIB_BER_FPRINTF_MT_R)
	@-$(RM) $(LIB_BEC_NULL_MT_R)
	@-$(RM) $(LIB_BE_NULL_MT_R)
	@-$(RM) $(LIB_BEL_NULL_MT_R)
	@-$(RM) $(LIB_BER_NULL_MT_R)
	@-$(RM) $(LIB_BEC_SYSLOG_MT_R)
	@-$(RM) $(LIB_BEC_SYSLOGWC_MT_R)
	@-$(RM) $(LIB_BE_SYSLOG_MT_R)
	@-$(RM) $(LIB_BER_SYSLOG_MT_R)
	@-$(RM) $(LIB_BE_LRSPLIT_ST_D)
	@-$(RM) $(LIB_BE_N_ST_D)
	@-$(RM) $(LIB_BEC_TEST_ST_D)
	@-$(RM) $(LIB_BE_TEST_ST_D)
	@-$(RM) $(LIB_BEL_TEST_ST_D)
	@-$(RM) $(LIB_BER_TEST_ST_D)
	@-$(RM) $(LIB_BEC_FAIL_ST_D)
	@-$(RM) $(LIB_BE_FAIL_ST_D)
	@-$(RM) $(LIB_BEL_FAIL_ST_D)
	@-$(RM) $(LIB_BER_FAIL_ST_D)
	@-$(RM) $(LIB_BEC_FILE_ST_D)
	@-$(RM) $(LIB_BEC_FILE_WC_ST_D)
	@-$(RM) $(LIB_BE_FILE_ST_D)
	@-$(RM) $(LIB_BEL_FILE_ST_D)
	@-$(RM) $(LIB_BER_FILE_ST_D)
	@-$(RM) $(LIB_BEC_FPRINTF_ST_D)
	@-$(RM) $(LIB_BEC_FPRINTF_WC_ST_D)
	@-$(RM) $(LIB_BE_FPRINTF_ST_D)
	@-$(RM) $(LIB_BEL_FPRINTF_ST_D)
	@-$(RM) $(LIB_BER_FPRINTF_ST_D)
	@-$(RM) $(LIB_BEC_NULL_ST_D)
	@-$(RM) $(LIB_BE_NULL_ST_D)
	@-$(RM) $(LIB_BEL_NULL_ST_D)
	@-$(RM) $(LIB_BER_NULL_ST_D)
	@-$(RM) $(LIB_BEC_SYSLOG_ST_D)
	@-$(RM) $(LIB_BEC_SYSLOGWC_ST_D)
	@-$(RM) $(LIB_BE_SYSLOG_ST_D)
	@-$(RM) $(LIB_BER_SYSLOG_ST_D)
	@-$(RM) $(LIB_BE_LRSPLIT_ST_R)
	@-$(RM) $(LIB_BE_N_ST_R)
	@-$(RM) $(LIB_BEC_TEST_ST_R)
	@-$(RM) $(LIB_BE_TEST_ST_R)
	@-$(RM) $(LIB_BEL_TEST_ST_R)
	@-$(RM) $(LIB_BER_TEST_ST_R)
	@-$(RM) $(LIB_BEC_FAIL_ST_R)
	@-$(RM) $(LIB_BE_FAIL_ST_R)
	@-$(RM) $(LIB_BEL_FAIL_ST_R)
	@-$(RM) $(LIB_BER_FAIL_ST_R)
	@-$(RM) $(LIB_BEC_FILE_ST_R)
	@-$(RM) $(LIB_BEC_FILE_WC_ST_R)
	@-$(RM) $(LIB_BE_FILE_ST_R)
	@-$(RM) $(LIB_BEL_FILE_ST_R)
	@-$(RM) $(LIB_BER_FILE_ST_R)
	@-$(RM) $(LIB_BEC_FPRINTF_ST_R)
	@-$(RM) $(LIB_BEC_FPRINTF_WC_ST_R)
	@-$(RM) $(LIB_BE_FPRINTF_ST_R)
	@-$(RM) $(LIB_BEL_FPRINTF_ST_R)
	@-$(RM) $(LIB_BER_FPRINTF_ST_R)
	@-$(RM) $(LIB_BEC_NULL_ST_R)
	@-$(RM) $(LIB_BE_NULL_ST_R)
	@-$(RM) $(LIB_BEL_NULL_ST_R)
	@-$(RM) $(LIB_BER_NULL_ST_R)
	@-$(RM) $(LIB_BEC_SYSLOG_ST_R)
	@-$(RM) $(LIB_BEC_SYSLOGWC_ST_R)
	@-$(RM) $(LIB_BE_SYSLOG_ST_R)
	@-$(RM) $(LIB_BER_SYSLOG_ST_R)
	@-$(RM) $(OBJ_BE_LRSPLIT_MT_D)
	@-$(RM) $(OBJ_BE_N_MT_D)
	@-$(RM) $(OBJ_BEC_TEST_MT_D)
	@-$(RM) $(OBJ_BE_TEST_MT_D)
	@-$(RM) $(OBJ_BEL_TEST_MT_D)
	@-$(RM) $(OBJ_BER_TEST_MT_D)
	@-$(RM) $(OBJ_BEC_FAIL_MT_D)
	@-$(RM) $(OBJ_BE_FAIL_MT_D)
	@-$(RM) $(OBJ_BEL_FAIL_MT_D)
	@-$(RM) $(OBJ_BER_FAIL_MT_D)
	@-$(RM) $(OBJ_BEC_FILE_MT_D)
	@-$(RM) $(OBJ_BEC_FILE_WC_MT_D)
	@-$(RM) $(OBJ_BE_FILE_MT_D)
	@-$(RM) $(OBJ_BEL_FILE_MT_D)
	@-$(RM) $(OBJ_BER_FILE_MT_D)
	@-$(RM) $(OBJ_BEC_FPRINTF_MT_D)
	@-$(RM) $(OBJ_BEC_FPRINTF_WC_MT_D)
	@-$(RM) $(OBJ_BE_FPRINTF_MT_D)
	@-$(RM) $(OBJ_BEL_FPRINTF_MT_D)
	@-$(RM) $(OBJ_BER_FPRINTF_MT_D)
	@-$(RM) $(OBJ_BEC_NULL_MT_D)
	@-$(RM) $(OBJ_BE_NULL_MT_D)
	@-$(RM) $(OBJ_BEL_NULL_MT_D)
	@-$(RM) $(OBJ_BER_NULL_MT_D)
	@-$(RM) $(OBJ_BEC_SYSLOG_MT_D)
	@-$(RM) $(OBJ_BEC_SYSLOGWC_MT_D)
	@-$(RM) $(OBJ_BE_SYSLOG_MT_D)
	@-$(RM) $(OBJ_BER_SYSLOG_MT_D)
	@-$(RM) $(OBJ_BE_LRSPLIT_MT_R)
	@-$(RM) $(OBJ_BE_N_MT_R)
	@-$(RM) $(OBJ_BEC_TEST_MT_R)
	@-$(RM) $(OBJ_BE_TEST_MT_R)
	@-$(RM) $(OBJ_BEL_TEST_MT_R)
	@-$(RM) $(OBJ_BER_TEST_MT_R)
	@-$(RM) $(OBJ_BEC_FAIL_MT_R)
	@-$(RM) $(OBJ_BE_FAIL_MT_R)
	@-$(RM) $(OBJ_BEL_FAIL_MT_R)
	@-$(RM) $(OBJ_BER_FAIL_MT_R)
	@-$(RM) $(OBJ_BEC_FILE_MT_R)
	@-$(RM) $(OBJ_BEC_FILE_WC_MT_R)
	@-$(RM) $(OBJ_BE_FILE_MT_R)
	@-$(RM) $(OBJ_BEL_FILE_MT_R)
	@-$(RM) $(OBJ_BER_FILE_MT_R)
	@-$(RM) $(OBJ_BEC_FPRINTF_MT_R)
	@-$(RM) $(OBJ_BEC_FPRINTF_WC_MT_R)
	@-$(RM) $(OBJ_BE_FPRINTF_MT_R)
	@-$(RM) $(OBJ_BEL_FPRINTF_MT_R)
	@-$(RM) $(OBJ_BER_FPRINTF_MT_R)
	@-$(RM) $(OBJ_BEC_NULL_MT_R)
	@-$(RM) $(OBJ_BE_NULL_MT_R)
	@-$(RM) $(OBJ_BEL_NULL_MT_R)
	@-$(RM) $(OBJ_BER_NULL_MT_R)
	@-$(RM) $(OBJ_BEC_SYSLOG_MT_R)
	@-$(RM) $(OBJ_BEC_SYSLOGWC_MT_R)
	@-$(RM) $(OBJ_BE_SYSLOG_MT_R)
	@-$(RM) $(OBJ_BER_SYSLOG_MT_R)
	@-$(RM) $(OBJ_BE_LRSPLIT_ST_D)
	@-$(RM) $(OBJ_BE_N_ST_D)
	@-$(RM) $(OBJ_BEC_TEST_ST_D)
	@-$(RM) $(OBJ_BE_TEST_ST_D)
	@-$(RM) $(OBJ_BEL_TEST_ST_D)
	@-$(RM) $(OBJ_BER_TEST_ST_D)
	@-$(RM) $(OBJ_BEC_FAIL_ST_D)
	@-$(RM) $(OBJ_BE_FAIL_ST_D)
	@-$(RM) $(OBJ_BEL_FAIL_ST_D)
	@-$(RM) $(OBJ_BER_FAIL_ST_D)
	@-$(RM) $(OBJ_BEC_FILE_ST_D)
	@-$(RM) $(OBJ_BEC_FILE_WC_ST_D)
	@-$(RM) $(OBJ_BE_FILE_ST_D)
	@-$(RM) $(OBJ_BEL_FILE_ST_D)
	@-$(RM) $(OBJ_BER_FILE_ST_D)
	@-$(RM) $(OBJ_BEC_FPRINTF_ST_D)
	@-$(RM) $(OBJ_BEC_FPRINTF_WC_ST_D)
	@-$(RM) $(OBJ_BE_FPRINTF_ST_D)
	@-$(RM) $(OBJ_BEL_FPRINTF_ST_D)
	@-$(RM) $(OBJ_BER_FPRINTF_ST_D)
	@-$(RM) $(OBJ_BEC_NULL_ST_D)
	@-$(RM) $(OBJ_BE_NULL_ST_D)
	@-$(RM) $(OBJ_BEL_NULL_ST_D)
	@-$(RM) $(OBJ_BER_NULL_ST_D)
	@-$(RM) $(OBJ_BEC_SYSLOG_ST_D)
	@-$(RM) $(OBJ_BEC_SYSLOGWC_ST_D)
	@-$(RM) $(OBJ_BE_SYSLOG_ST_D)
	@-$(RM) $(OBJ_BER_SYSLOG_ST_D)
	@-$(RM) $(OBJ_BE_LRSPLIT_ST_R)
	@-$(RM) $(OBJ_BE_N_ST_R)
	@-$(RM) $(OBJ_BEC_TEST_ST_R)
	@-$(RM) $(OBJ_BE_TEST_ST_R)
	@-$(RM) $(OBJ_BEL_TEST_ST_R)
	@-$(RM) $(OBJ_BER_TEST_ST_R)
	@-$(RM) $(OBJ_BEC_FAIL_ST_R)
	@-$(RM) $(OBJ_BE_FAIL_ST_R)
	@-$(RM) $(OBJ_BEL_FAIL_ST_R)
	@-$(RM) $(OBJ_BER_FAIL_ST_R)
	@-$(RM) $(OBJ_BEC_FILE_ST_R)
	@-$(RM) $(OBJ_BEC_FILE_WC_ST_R)
	@-$(RM) $(OBJ_BE_FILE_ST_R)
	@-$(RM) $(OBJ_BEL_FILE_ST_R)
	@-$(RM) $(OBJ_BER_FILE_ST_R)
	@-$(RM) $(OBJ_BEC_FPRINTF_ST_R)
	@-$(RM) $(OBJ_BEC_FPRINTF_WC_ST_R)
	@-$(RM) $(OBJ_BE_FPRINTF_ST_R)
	@-$(RM) $(OBJ_BEL_FPRINTF_ST_R)
	@-$(RM) $(OBJ_BER_FPRINTF_ST_R)
	@-$(RM) $(OBJ_BEC_NULL_ST_R)
	@-$(RM) $(OBJ_BE_NULL_ST_R)
	@-$(RM) $(OBJ_BEL_NULL_ST_R)
	@-$(RM) $(OBJ_BER_NULL_ST_R)
	@-$(RM) $(OBJ_BEC_SYSLOG_ST_R)
	@-$(RM) $(OBJ_BEC_SYSLOGWC_ST_R)
	@-$(RM) $(OBJ_BE_SYSLOG_ST_R)
	@-$(RM) $(OBJ_BER_SYSLOG_ST_R)


clean.libs.fe:
	@echo Cleaning Front-end libraries ...
	@-$(RM) $(LIB_FE_ALL_MT_D)
	@-$(RM) $(LIB_FE_ALL_WC_MT_D)
	@-$(RM) $(LIB_FE_FAIL_MT_D)
	@-$(RM) $(LIB_FE_N_MT_D)
	@-$(RM) $(LIB_FE_N_WC_MT_D)
	@-$(RM) $(LIB_FE_NULL_MT_D)
	@-$(RM) $(LIB_FE_NULL_WC_MT_D)
	@-$(RM) $(LIB_FE_SIMPLE_MT_D)
	@-$(RM) $(LIB_FE_SIMPLE_WC_MT_D)
	@-$(RM) $(LIB_FE_ALL_MT_R)
	@-$(RM) $(LIB_FE_ALL_WC_MT_R)
	@-$(RM) $(LIB_FE_FAIL_MT_R)
	@-$(RM) $(LIB_FE_N_MT_R)
	@-$(RM) $(LIB_FE_N_WC_MT_R)
	@-$(RM) $(LIB_FE_NULL_MT_R)
	@-$(RM) $(LIB_FE_NULL_WC_MT_R)
	@-$(RM) $(LIB_FE_SIMPLE_MT_R)
	@-$(RM) $(LIB_FE_SIMPLE_WC_MT_R)
	@-$(RM) $(LIB_FE_ALL_ST_D)
	@-$(RM) $(LIB_FE_ALL_WC_ST_D)
	@-$(RM) $(LIB_FE_FAIL_ST_D)
	@-$(RM) $(LIB_FE_N_ST_D)
	@-$(RM) $(LIB_FE_N_WC_ST_D)
	@-$(RM) $(LIB_FE_NULL_ST_D)
	@-$(RM) $(LIB_FE_NULL_WC_ST_D)
	@-$(RM) $(LIB_FE_SIMPLE_ST_D)
	@-$(RM) $(LIB_FE_SIMPLE_WC_ST_D)
	@-$(RM) $(LIB_FE_ALL_ST_R)
	@-$(RM) $(LIB_FE_ALL_WC_ST_R)
	@-$(RM) $(LIB_FE_FAIL_ST_R)
	@-$(RM) $(LIB_FE_N_ST_R)
	@-$(RM) $(LIB_FE_N_WC_ST_R)
	@-$(RM) $(LIB_FE_NULL_ST_R)
	@-$(RM) $(LIB_FE_NULL_WC_ST_R)
	@-$(RM) $(LIB_FE_SIMPLE_ST_R)
	@-$(RM) $(LIB_FE_SIMPLE_WC_ST_R)
	@-$(RM) $(OBJ_FE_ALL_MT_D)
	@-$(RM) $(OBJ_FE_ALL_WC_MT_D)
	@-$(RM) $(OBJ_FE_FAIL_MT_D)
	@-$(RM) $(OBJ_FE_N_MT_D)
	@-$(RM) $(OBJ_FE_N_WC_MT_D)
	@-$(RM) $(OBJ_FE_NULL_MT_D)
	@-$(RM) $(OBJ_FE_NULL_WC_MT_D)
	@-$(RM) $(OBJ_FE_SIMPLE_MT_D)
	@-$(RM) $(OBJ_FE_SIMPLE_WC_MT_D)
	@-$(RM) $(OBJ_FE_ALL_MT_R)
	@-$(RM) $(OBJ_FE_ALL_WC_MT_R)
	@-$(RM) $(OBJ_FE_FAIL_MT_R)
	@-$(RM) $(OBJ_FE_N_MT_R)
	@-$(RM) $(OBJ_FE_N_WC_MT_R)
	@-$(RM) $(OBJ_FE_NULL_MT_R)
	@-$(RM) $(OBJ_FE_NULL_WC_MT_R)
	@-$(RM) $(OBJ_FE_SIMPLE_MT_R)
	@-$(RM) $(OBJ_FE_SIMPLE_WC_MT_R)
	@-$(RM) $(OBJ_FE_ALL_ST_D)
	@-$(RM) $(OBJ_FE_ALL_WC_ST_D)
	@-$(RM) $(OBJ_FE_FAIL_ST_D)
	@-$(RM) $(OBJ_FE_N_ST_D)
	@-$(RM) $(OBJ_FE_N_WC_ST_D)
	@-$(RM) $(OBJ_FE_NULL_ST_D)
	@-$(RM) $(OBJ_FE_NULL_WC_ST_D)
	@-$(RM) $(OBJ_FE_SIMPLE_ST_D)
	@-$(RM) $(OBJ_FE_SIMPLE_WC_ST_D)
	@-$(RM) $(OBJ_FE_ALL_ST_R)
	@-$(RM) $(OBJ_FE_ALL_WC_ST_R)
	@-$(RM) $(OBJ_FE_FAIL_ST_R)
	@-$(RM) $(OBJ_FE_N_ST_R)
	@-$(RM) $(OBJ_FE_N_WC_ST_R)
	@-$(RM) $(OBJ_FE_NULL_ST_R)
	@-$(RM) $(OBJ_FE_NULL_WC_ST_R)
	@-$(RM) $(OBJ_FE_SIMPLE_ST_R)
	@-$(RM) $(OBJ_FE_SIMPLE_WC_ST_R)

clean.libs.b64:
	@echo Cleaning b64 libraries ...
	@-$(RM) $(LIB_B64_MT_D)
	@-$(RM) $(LIB_B64_MT_R)
	@-$(RM) $(LIB_B64_ST_D)
	@-$(RM) $(LIB_B64_ST_R)
	@-$(RM) $(OBJ_B64_MT_D)
	@-$(RM) $(OBJ_B64_MT_R)
	@-$(RM) $(OBJ_B64_ST_D)
	@-$(RM) $(OBJ_B64_ST_R)

clean.libs.shwild:
	@echo Cleaning shwild libraries ...
	@-$(RM) $(LIB_SHWILD_MT_D)
	@-$(RM) $(LIB_SHWILD_MT_R)
	@-$(RM) $(LIB_SHWILD_ST_D)
	@-$(RM) $(LIB_SHWILD_ST_R)
	@-$(RM) $(OBJ_SHWILD_MT_D)
	@-$(RM) $(OBJ_SHWILD_MT_R)
	@-$(RM) $(OBJ_SHWILD_ST_D)
	@-$(RM) $(OBJ_SHWILD_ST_R)

clean.libs.xtests:
	@echo Cleaning xTests libraries ...
	@-$(RM) $(LIB_XTESTS_MT_D)
	@-$(RM) $(LIB_XTESTS_MT_R)
	@-$(RM) $(LIB_XTESTS_ST_D)
	@-$(RM) $(LIB_XTESTS_ST_R)
	@-$(RM) $(OBJ_XTESTS_MT_D)
	@-$(RM) $(OBJ_XTESTS_MT_R)
	@-$(RM) $(OBJ_XTESTS_ST_D)
	@-$(RM) $(OBJ_XTESTS_ST_R)

ifdef ACE
clean.libs.ace:
	@echo Cleaning ACE libraries ...
	@-$(RM) $(LIB_BEC_ACE_MT_D)
	@-$(RM) $(LIB_BE_ACE_MT_D)
	@-$(RM) $(LIB_BEL_ACE_MT_D)
	@-$(RM) $(LIB_BER_ACE_MT_D)
	@-$(RM) $(LIB_BEC_ACE_MT_R)
	@-$(RM) $(LIB_BE_ACE_MT_R)
	@-$(RM) $(LIB_BEL_ACE_MT_R)
	@-$(RM) $(LIB_BER_ACE_MT_R)
	@-$(RM) $(OBJ_BEC_ACE_MT_D)
	@-$(RM) $(OBJ_BE_ACE_MT_D)
	@-$(RM) $(OBJ_BEL_ACE_MT_D)
	@-$(RM) $(OBJ_BER_ACE_MT_D)
	@-$(RM) $(OBJ_BEC_ACE_MT_R)
	@-$(RM) $(OBJ_BE_ACE_MT_R)
	@-$(RM) $(OBJ_BEL_ACE_MT_R)
	@-$(RM) $(OBJ_BER_ACE_MT_R)
endif # ACE

clean.examples:
	@echo Cleaning examples ...
	@-$(RM) $(TARG_EX_P_be_01_MT_D)
	@-$(RM) $(TARG_EX_P_be_02_MT_D)
	@-$(RM) $(TARG_EX_P_be_03_MT_D)
	@-$(RM) $(TARG_EX_P_be_04_MT_D)
	@-$(RM) $(TARG_EX_P_be_05_MT_D)
	@-$(RM) $(TARG_EX_P_be_06_MT_D)
	@-$(RM) $(TARG_EX_P_cp_01_MT_D)
	@-$(RM) $(TARG_EX_P_cp_02_MT_D)
	@-$(RM) $(TARG_EX_P_fm_01_MT_D)
	@-$(RM) $(TARG_EX_P_fm_02_MT_D)
	@-$(RM) $(TARG_EX_P_fm_03_MT_D)
	@-$(RM) $(TARG_EX_P_fe_01_MT_D)
	@-$(RM) $(TARG_EX_P_in_01_MT_D)
	@-$(RM) $(TARG_EX_P_in_02_MT_D)
	@-$(RM) $(TARG_EX_P_in_03_MT_D)
	@-$(RM) $(TARG_EX_P_in_04_MT_D)
	@-$(RM) $(TARG_EX_P_in_05_MT_D)
	@-$(RM) $(TARG_EX_P_in_06_MT_D)
	@-$(RM) $(TARG_EX_P_in_07_MT_D)
	@-$(RM) $(TARG_EX_P_in_08_MT_D)
	@-$(RM) $(TARG_EX_P_in_09_MT_D)
	@-$(RM) $(TARG_EX_P_in_10_MT_D)
	@-$(RM) $(TARG_EX_P_in_11_MT_D)
	@-$(RM) $(TARG_EX_P_in_12_MT_D)
	@-$(RM) $(TARG_EX_P_in_13_MT_D)
	@-$(RM) $(TARG_EX_P_in_14_MT_D)
	@-$(RM) $(TARG_EX_P_in_15_MT_D)
	@-$(RM) $(TARG_EX_P_in_16_MT_D)
	@-$(RM) $(TARG_EX_P_in_17_MT_D)
	@-$(RM) $(TARG_EX_P_ms_01_MT_D)
	@-$(RM) $(TARG_EX_P_ms_02_MT_D)
	@-$(RM) $(TARG_EX_P_ms_03_MT_D)
	@-$(RM) $(TARG_EX_P_ms_04_MT_D)
	@-$(RM) $(TARG_EX_P_ms_05_MT_D)
	@-$(RM) $(TARG_EX_P_ms_06_MT_D)
	@-$(RM) $(TARG_EX_P_tr_01_MT_D)
	@-$(RM) $(TARG_EX_P_tr_02_MT_D)
	@-$(RM) $(TARG_EX_P_ut_01_MT_D)
	@-$(RM) $(TARG_EX_C_0_MT_D)
	@-$(RM) $(TARG_EX_C_1_MT_D)
	@-$(RM) $(TARG_EX_C_2_MT_D)
	@-$(RM) $(TARG_EX_C_3_MT_D)
	@-$(RM) $(TARG_EX_C_4_MT_D)
	@-$(RM) $(TARG_EX_C_5_MT_D)
	@-$(RM) $(TARG_EX_C_6_MT_D)
	@-$(RM) $(TARG_EX_C_7_MT_D)
	@-$(RM) $(TARG_EX_P_be_01_MT_R)
	@-$(RM) $(TARG_EX_P_be_02_MT_R)
	@-$(RM) $(TARG_EX_P_be_03_MT_R)
	@-$(RM) $(TARG_EX_P_be_04_MT_R)
	@-$(RM) $(TARG_EX_P_be_05_MT_R)
	@-$(RM) $(TARG_EX_P_be_06_MT_R)
	@-$(RM) $(TARG_EX_P_cp_01_MT_R)
	@-$(RM) $(TARG_EX_P_cp_02_MT_R)
	@-$(RM) $(TARG_EX_P_fm_01_MT_R)
	@-$(RM) $(TARG_EX_P_fm_02_MT_R)
	@-$(RM) $(TARG_EX_P_fm_03_MT_R)
	@-$(RM) $(TARG_EX_P_fe_01_MT_R)
	@-$(RM) $(TARG_EX_P_in_01_MT_R)
	@-$(RM) $(TARG_EX_P_in_02_MT_R)
	@-$(RM) $(TARG_EX_P_in_03_MT_R)
	@-$(RM) $(TARG_EX_P_in_04_MT_R)
	@-$(RM) $(TARG_EX_P_in_05_MT_R)
	@-$(RM) $(TARG_EX_P_in_06_MT_R)
	@-$(RM) $(TARG_EX_P_in_07_MT_R)
	@-$(RM) $(TARG_EX_P_in_08_MT_R)
	@-$(RM) $(TARG_EX_P_in_09_MT_R)
	@-$(RM) $(TARG_EX_P_in_10_MT_R)
	@-$(RM) $(TARG_EX_P_in_11_MT_R)
	@-$(RM) $(TARG_EX_P_in_12_MT_R)
	@-$(RM) $(TARG_EX_P_in_13_MT_R)
	@-$(RM) $(TARG_EX_P_in_14_MT_R)
	@-$(RM) $(TARG_EX_P_in_15_MT_R)
	@-$(RM) $(TARG_EX_P_in_16_MT_R)
	@-$(RM) $(TARG_EX_P_in_17_MT_R)
	@-$(RM) $(TARG_EX_P_ms_01_MT_R)
	@-$(RM) $(TARG_EX_P_ms_02_MT_R)
	@-$(RM) $(TARG_EX_P_ms_03_MT_R)
	@-$(RM) $(TARG_EX_P_ms_04_MT_R)
	@-$(RM) $(TARG_EX_P_ms_05_MT_R)
	@-$(RM) $(TARG_EX_P_ms_06_MT_R)
	@-$(RM) $(TARG_EX_P_tr_01_MT_R)
	@-$(RM) $(TARG_EX_P_tr_02_MT_R)
	@-$(RM) $(TARG_EX_P_ut_01_MT_R)
	@-$(RM) $(TARG_EX_C_0_MT_R)
	@-$(RM) $(TARG_EX_C_1_MT_R)
	@-$(RM) $(TARG_EX_C_2_MT_R)
	@-$(RM) $(TARG_EX_C_3_MT_R)
	@-$(RM) $(TARG_EX_C_4_MT_R)
	@-$(RM) $(TARG_EX_C_5_MT_R)
	@-$(RM) $(TARG_EX_C_6_MT_R)
	@-$(RM) $(TARG_EX_C_7_MT_R)
	@-$(RM) $(TARG_EX_P_be_01_ST_D)
	@-$(RM) $(TARG_EX_P_be_02_ST_D)
	@-$(RM) $(TARG_EX_P_be_03_ST_D)
	@-$(RM) $(TARG_EX_P_be_04_ST_D)
	@-$(RM) $(TARG_EX_P_be_05_ST_D)
	@-$(RM) $(TARG_EX_P_be_06_ST_D)
	@-$(RM) $(TARG_EX_P_cp_01_ST_D)
	@-$(RM) $(TARG_EX_P_cp_02_ST_D)
	@-$(RM) $(TARG_EX_P_fm_01_ST_D)
	@-$(RM) $(TARG_EX_P_fm_02_ST_D)
	@-$(RM) $(TARG_EX_P_fm_03_ST_D)
	@-$(RM) $(TARG_EX_P_fe_01_ST_D)
	@-$(RM) $(TARG_EX_P_in_01_ST_D)
	@-$(RM) $(TARG_EX_P_in_02_ST_D)
	@-$(RM) $(TARG_EX_P_in_03_ST_D)
	@-$(RM) $(TARG_EX_P_in_04_ST_D)
	@-$(RM) $(TARG_EX_P_in_05_ST_D)
	@-$(RM) $(TARG_EX_P_in_06_ST_D)
	@-$(RM) $(TARG_EX_P_in_07_ST_D)
	@-$(RM) $(TARG_EX_P_in_08_ST_D)
	@-$(RM) $(TARG_EX_P_in_09_ST_D)
	@-$(RM) $(TARG_EX_P_in_10_ST_D)
	@-$(RM) $(TARG_EX_P_in_11_ST_D)
	@-$(RM) $(TARG_EX_P_in_12_ST_D)
	@-$(RM) $(TARG_EX_P_in_13_ST_D)
	@-$(RM) $(TARG_EX_P_in_14_ST_D)
	@-$(RM) $(TARG_EX_P_in_15_ST_D)
	@-$(RM) $(TARG_EX_P_in_16_ST_D)
	@-$(RM) $(TARG_EX_P_in_17_ST_D)
	@-$(RM) $(TARG_EX_P_ms_01_ST_D)
	@-$(RM) $(TARG_EX_P_ms_02_ST_D)
	@-$(RM) $(TARG_EX_P_ms_03_ST_D)
	@-$(RM) $(TARG_EX_P_ms_04_ST_D)
	@-$(RM) $(TARG_EX_P_ms_05_ST_D)
	@-$(RM) $(TARG_EX_P_ms_06_ST_D)
	@-$(RM) $(TARG_EX_P_tr_01_ST_D)
	@-$(RM) $(TARG_EX_P_tr_02_ST_D)
	@-$(RM) $(TARG_EX_P_ut_01_ST_D)
	@-$(RM) $(TARG_EX_C_0_ST_D)
	@-$(RM) $(TARG_EX_C_1_ST_D)
	@-$(RM) $(TARG_EX_C_2_ST_D)
	@-$(RM) $(TARG_EX_C_3_ST_D)
	@-$(RM) $(TARG_EX_C_4_ST_D)
	@-$(RM) $(TARG_EX_C_5_ST_D)
	@-$(RM) $(TARG_EX_C_6_ST_D)
	@-$(RM) $(TARG_EX_C_7_ST_D)
	@-$(RM) $(TARG_EX_P_be_01_ST_R)
	@-$(RM) $(TARG_EX_P_be_02_ST_R)
	@-$(RM) $(TARG_EX_P_be_03_ST_R)
	@-$(RM) $(TARG_EX_P_be_04_ST_R)
	@-$(RM) $(TARG_EX_P_be_05_ST_R)
	@-$(RM) $(TARG_EX_P_be_06_ST_R)
	@-$(RM) $(TARG_EX_P_cp_01_ST_R)
	@-$(RM) $(TARG_EX_P_cp_02_ST_R)
	@-$(RM) $(TARG_EX_P_fm_01_ST_R)
	@-$(RM) $(TARG_EX_P_fm_02_ST_R)
	@-$(RM) $(TARG_EX_P_fm_03_ST_R)
	@-$(RM) $(TARG_EX_P_fe_01_ST_R)
	@-$(RM) $(TARG_EX_P_in_01_ST_R)
	@-$(RM) $(TARG_EX_P_in_02_ST_R)
	@-$(RM) $(TARG_EX_P_in_03_ST_R)
	@-$(RM) $(TARG_EX_P_in_04_ST_R)
	@-$(RM) $(TARG_EX_P_in_05_ST_R)
	@-$(RM) $(TARG_EX_P_in_06_ST_R)
	@-$(RM) $(TARG_EX_P_in_07_ST_R)
	@-$(RM) $(TARG_EX_P_in_08_ST_R)
	@-$(RM) $(TARG_EX_P_in_09_ST_R)
	@-$(RM) $(TARG_EX_P_in_10_ST_R)
	@-$(RM) $(TARG_EX_P_in_11_ST_R)
	@-$(RM) $(TARG_EX_P_in_12_ST_R)
	@-$(RM) $(TARG_EX_P_in_13_ST_R)
	@-$(RM) $(TARG_EX_P_in_14_ST_R)
	@-$(RM) $(TARG_EX_P_in_15_ST_R)
	@-$(RM) $(TARG_EX_P_in_16_ST_R)
	@-$(RM) $(TARG_EX_P_in_17_ST_R)
	@-$(RM) $(TARG_EX_P_ms_01_ST_R)
	@-$(RM) $(TARG_EX_P_ms_02_ST_R)
	@-$(RM) $(TARG_EX_P_ms_03_ST_R)
	@-$(RM) $(TARG_EX_P_ms_04_ST_R)
	@-$(RM) $(TARG_EX_P_ms_05_ST_R)
	@-$(RM) $(TARG_EX_P_ms_06_ST_R)
	@-$(RM) $(TARG_EX_P_tr_01_ST_R)
	@-$(RM) $(TARG_EX_P_tr_02_ST_R)
	@-$(RM) $(TARG_EX_P_ut_01_ST_R)
	@-$(RM) $(TARG_EX_C_0_ST_R)
	@-$(RM) $(TARG_EX_C_1_ST_R)
	@-$(RM) $(TARG_EX_C_2_ST_R)
	@-$(RM) $(TARG_EX_C_3_ST_R)
	@-$(RM) $(TARG_EX_C_4_ST_R)
	@-$(RM) $(TARG_EX_C_5_ST_R)
	@-$(RM) $(TARG_EX_C_6_ST_R)
	@-$(RM) $(TARG_EX_C_7_ST_R)
	@-$(RM) $(OBJ_EX_P_be_01_MT_D)
	@-$(RM) $(OBJ_EX_P_be_02_MT_D)
	@-$(RM) $(OBJ_EX_P_be_03_MT_D)
	@-$(RM) $(OBJ_EX_P_be_04_MT_D)
	@-$(RM) $(OBJ_EX_P_be_05_MT_D)
	@-$(RM) $(OBJ_EX_P_be_06_MT_D)
	@-$(RM) $(OBJ_EX_P_cp_01_MT_D)
	@-$(RM) $(OBJ_EX_P_cp_02_MT_D)
	@-$(RM) $(OBJ_EX_P_fm_01_MT_D)
	@-$(RM) $(OBJ_EX_P_fm_02_MT_D)
	@-$(RM) $(OBJ_EX_P_fm_03_MT_D)
	@-$(RM) $(OBJ_EX_P_fe_01_MT_D)
	@-$(RM) $(OBJ_EX_P_in_01_MT_D)
	@-$(RM) $(OBJ_EX_P_in_02_MT_D)
	@-$(RM) $(OBJ_EX_P_in_03_MT_D)
	@-$(RM) $(OBJ_EX_P_in_04_MT_D)
	@-$(RM) $(OBJ_EX_P_in_05_MT_D)
	@-$(RM) $(OBJ_EX_P_in_06_MT_D)
	@-$(RM) $(OBJ_EX_P_in_07_MT_D)
	@-$(RM) $(OBJ_EX_P_in_08_MT_D)
	@-$(RM) $(OBJ_EX_P_in_09_MT_D)
	@-$(RM) $(OBJ_EX_P_in_10_MT_D)
	@-$(RM) $(OBJ_EX_P_in_11_MT_D)
	@-$(RM) $(OBJ_EX_P_in_12_MT_D)
	@-$(RM) $(OBJ_EX_P_in_13_MT_D)
	@-$(RM) $(OBJ_EX_P_in_14_MT_D)
	@-$(RM) $(OBJ_EX_P_in_15_MT_D)
	@-$(RM) $(OBJ_EX_P_in_16_MT_D)
	@-$(RM) $(OBJ_EX_P_in_17_MT_D)
	@-$(RM) $(OBJ_EX_P_ms_01_MT_D)
	@-$(RM) $(OBJ_EX_P_ms_02_MT_D)
	@-$(RM) $(OBJ_EX_P_ms_03_MT_D)
	@-$(RM) $(OBJ_EX_P_ms_04_MT_D)
	@-$(RM) $(OBJ_EX_P_ms_05_MT_D)
	@-$(RM) $(OBJ_EX_P_ms_06_MT_D)
	@-$(RM) $(OBJ_EX_P_tr_01_MT_D)
	@-$(RM) $(OBJ_EX_P_tr_02_MT_D)
	@-$(RM) $(OBJ_EX_P_ut_01_MT_D)
	@-$(RM) $(OBJ_EX_C_0_MT_D)
	@-$(RM) $(OBJ_EX_C_1_MT_D)
	@-$(RM) $(OBJ_EX_C_2_MT_D)
	@-$(RM) $(OBJ_EX_C_3_MT_D)
	@-$(RM) $(OBJ_EX_C_4_MT_D)
	@-$(RM) $(OBJ_EX_C_5_MT_D)
	@-$(RM) $(OBJ_EX_C_6_MT_D)
	@-$(RM) $(OBJ_EX_C_7_MT_D)
	@-$(RM) $(OBJ_EX_P_be_01_MT_R)
	@-$(RM) $(OBJ_EX_P_be_02_MT_R)
	@-$(RM) $(OBJ_EX_P_be_03_MT_R)
	@-$(RM) $(OBJ_EX_P_be_04_MT_R)
	@-$(RM) $(OBJ_EX_P_be_05_MT_R)
	@-$(RM) $(OBJ_EX_P_be_06_MT_R)
	@-$(RM) $(OBJ_EX_P_cp_01_MT_R)
	@-$(RM) $(OBJ_EX_P_cp_02_MT_R)
	@-$(RM) $(OBJ_EX_P_fm_01_MT_R)
	@-$(RM) $(OBJ_EX_P_fm_02_MT_R)
	@-$(RM) $(OBJ_EX_P_fm_03_MT_R)
	@-$(RM) $(OBJ_EX_P_fe_01_MT_R)
	@-$(RM) $(OBJ_EX_P_in_01_MT_R)
	@-$(RM) $(OBJ_EX_P_in_02_MT_R)
	@-$(RM) $(OBJ_EX_P_in_03_MT_R)
	@-$(RM) $(OBJ_EX_P_in_04_MT_R)
	@-$(RM) $(OBJ_EX_P_in_05_MT_R)
	@-$(RM) $(OBJ_EX_P_in_06_MT_R)
	@-$(RM) $(OBJ_EX_P_in_07_MT_R)
	@-$(RM) $(OBJ_EX_P_in_08_MT_R)
	@-$(RM) $(OBJ_EX_P_in_09_MT_R)
	@-$(RM) $(OBJ_EX_P_in_10_MT_R)
	@-$(RM) $(OBJ_EX_P_in_11_MT_R)
	@-$(RM) $(OBJ_EX_P_in_12_MT_R)
	@-$(RM) $(OBJ_EX_P_in_13_MT_R)
	@-$(RM) $(OBJ_EX_P_in_14_MT_R)
	@-$(RM) $(OBJ_EX_P_in_15_MT_R)
	@-$(RM) $(OBJ_EX_P_in_16_MT_R)
	@-$(RM) $(OBJ_EX_P_in_17_MT_R)
	@-$(RM) $(OBJ_EX_P_ms_01_MT_R)
	@-$(RM) $(OBJ_EX_P_ms_02_MT_R)
	@-$(RM) $(OBJ_EX_P_ms_03_MT_R)
	@-$(RM) $(OBJ_EX_P_ms_04_MT_R)
	@-$(RM) $(OBJ_EX_P_ms_05_MT_R)
	@-$(RM) $(OBJ_EX_P_ms_06_MT_R)
	@-$(RM) $(OBJ_EX_P_tr_01_MT_R)
	@-$(RM) $(OBJ_EX_P_tr_02_MT_R)
	@-$(RM) $(OBJ_EX_P_ut_01_MT_R)
	@-$(RM) $(OBJ_EX_C_0_MT_R)
	@-$(RM) $(OBJ_EX_C_1_MT_R)
	@-$(RM) $(OBJ_EX_C_2_MT_R)
	@-$(RM) $(OBJ_EX_C_3_MT_R)
	@-$(RM) $(OBJ_EX_C_4_MT_R)
	@-$(RM) $(OBJ_EX_C_5_MT_R)
	@-$(RM) $(OBJ_EX_C_6_MT_R)
	@-$(RM) $(OBJ_EX_C_7_MT_R)
	@-$(RM) $(OBJ_EX_P_be_01_ST_D)
	@-$(RM) $(OBJ_EX_P_be_02_ST_D)
	@-$(RM) $(OBJ_EX_P_be_03_ST_D)
	@-$(RM) $(OBJ_EX_P_be_04_ST_D)
	@-$(RM) $(OBJ_EX_P_be_05_ST_D)
	@-$(RM) $(OBJ_EX_P_be_06_ST_D)
	@-$(RM) $(OBJ_EX_P_cp_01_ST_D)
	@-$(RM) $(OBJ_EX_P_cp_02_ST_D)
	@-$(RM) $(OBJ_EX_P_fm_01_ST_D)
	@-$(RM) $(OBJ_EX_P_fm_02_ST_D)
	@-$(RM) $(OBJ_EX_P_fm_03_ST_D)
	@-$(RM) $(OBJ_EX_P_fe_01_ST_D)
	@-$(RM) $(OBJ_EX_P_in_01_ST_D)
	@-$(RM) $(OBJ_EX_P_in_02_ST_D)
	@-$(RM) $(OBJ_EX_P_in_03_ST_D)
	@-$(RM) $(OBJ_EX_P_in_04_ST_D)
	@-$(RM) $(OBJ_EX_P_in_05_ST_D)
	@-$(RM) $(OBJ_EX_P_in_06_ST_D)
	@-$(RM) $(OBJ_EX_P_in_07_ST_D)
	@-$(RM) $(OBJ_EX_P_in_08_ST_D)
	@-$(RM) $(OBJ_EX_P_in_09_ST_D)
	@-$(RM) $(OBJ_EX_P_in_10_ST_D)
	@-$(RM) $(OBJ_EX_P_in_11_ST_D)
	@-$(RM) $(OBJ_EX_P_in_12_ST_D)
	@-$(RM) $(OBJ_EX_P_in_13_ST_D)
	@-$(RM) $(OBJ_EX_P_in_14_ST_D)
	@-$(RM) $(OBJ_EX_P_in_15_ST_D)
	@-$(RM) $(OBJ_EX_P_in_16_ST_D)
	@-$(RM) $(OBJ_EX_P_in_17_ST_D)
	@-$(RM) $(OBJ_EX_P_ms_01_ST_D)
	@-$(RM) $(OBJ_EX_P_ms_02_ST_D)
	@-$(RM) $(OBJ_EX_P_ms_03_ST_D)
	@-$(RM) $(OBJ_EX_P_ms_04_ST_D)
	@-$(RM) $(OBJ_EX_P_ms_05_ST_D)
	@-$(RM) $(OBJ_EX_P_ms_06_ST_D)
	@-$(RM) $(OBJ_EX_P_tr_01_ST_D)
	@-$(RM) $(OBJ_EX_P_tr_02_ST_D)
	@-$(RM) $(OBJ_EX_P_ut_01_ST_D)
	@-$(RM) $(OBJ_EX_C_0_ST_D)
	@-$(RM) $(OBJ_EX_C_1_ST_D)
	@-$(RM) $(OBJ_EX_C_2_ST_D)
	@-$(RM) $(OBJ_EX_C_3_ST_D)
	@-$(RM) $(OBJ_EX_C_4_ST_D)
	@-$(RM) $(OBJ_EX_C_5_ST_D)
	@-$(RM) $(OBJ_EX_C_6_ST_D)
	@-$(RM) $(OBJ_EX_C_7_ST_D)
	@-$(RM) $(OBJ_EX_P_be_01_ST_R)
	@-$(RM) $(OBJ_EX_P_be_02_ST_R)
	@-$(RM) $(OBJ_EX_P_be_03_ST_R)
	@-$(RM) $(OBJ_EX_P_be_04_ST_R)
	@-$(RM) $(OBJ_EX_P_be_05_ST_R)
	@-$(RM) $(OBJ_EX_P_be_06_ST_R)
	@-$(RM) $(OBJ_EX_P_cp_01_ST_R)
	@-$(RM) $(OBJ_EX_P_cp_02_ST_R)
	@-$(RM) $(OBJ_EX_P_fm_01_ST_R)
	@-$(RM) $(OBJ_EX_P_fm_02_ST_R)
	@-$(RM) $(OBJ_EX_P_fm_03_ST_R)
	@-$(RM) $(OBJ_EX_P_fe_01_ST_R)
	@-$(RM) $(OBJ_EX_P_in_01_ST_R)
	@-$(RM) $(OBJ_EX_P_in_02_ST_R)
	@-$(RM) $(OBJ_EX_P_in_03_ST_R)
	@-$(RM) $(OBJ_EX_P_in_04_ST_R)
	@-$(RM) $(OBJ_EX_P_in_05_ST_R)
	@-$(RM) $(OBJ_EX_P_in_06_ST_R)
	@-$(RM) $(OBJ_EX_P_in_07_ST_R)
	@-$(RM) $(OBJ_EX_P_in_08_ST_R)
	@-$(RM) $(OBJ_EX_P_in_09_ST_R)
	@-$(RM) $(OBJ_EX_P_in_10_ST_R)
	@-$(RM) $(OBJ_EX_P_in_11_ST_R)
	@-$(RM) $(OBJ_EX_P_in_12_ST_R)
	@-$(RM) $(OBJ_EX_P_in_13_ST_R)
	@-$(RM) $(OBJ_EX_P_in_14_ST_R)
	@-$(RM) $(OBJ_EX_P_in_15_ST_R)
	@-$(RM) $(OBJ_EX_P_in_16_ST_R)
	@-$(RM) $(OBJ_EX_P_in_17_ST_R)
	@-$(RM) $(OBJ_EX_P_ms_01_ST_R)
	@-$(RM) $(OBJ_EX_P_ms_02_ST_R)
	@-$(RM) $(OBJ_EX_P_ms_03_ST_R)
	@-$(RM) $(OBJ_EX_P_ms_04_ST_R)
	@-$(RM) $(OBJ_EX_P_ms_05_ST_R)
	@-$(RM) $(OBJ_EX_P_ms_06_ST_R)
	@-$(RM) $(OBJ_EX_P_tr_01_ST_R)
	@-$(RM) $(OBJ_EX_P_tr_02_ST_R)
	@-$(RM) $(OBJ_EX_P_ut_01_ST_R)
	@-$(RM) $(OBJ_EX_C_0_ST_R)
	@-$(RM) $(OBJ_EX_C_1_ST_R)
	@-$(RM) $(OBJ_EX_C_2_ST_R)
	@-$(RM) $(OBJ_EX_C_3_ST_R)
	@-$(RM) $(OBJ_EX_C_4_ST_R)
	@-$(RM) $(OBJ_EX_C_5_ST_R)
	@-$(RM) $(OBJ_EX_C_6_ST_R)
	@-$(RM) $(OBJ_EX_C_7_ST_R)

clean.samples:
	@echo Cleaning samples ...

clean.samples.all:	clean.samples

#	@-$(RM) $(TARGETS_SAMPLE)

clean.test.unit:
	@echo Cleaning unit tests ...
	@-$(RM) $(TARG_TEST_UNIT_lvls_dyninit_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_lvls_vals_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_getcurrtm_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_getversion_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_gethostname_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_onbailout_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_strnlen_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_bec_file_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_bec_fprintf_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_bec_fail_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_be_fail_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_be_lrsplit_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_be_N_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_all_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_fail_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_N_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_null_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_simple_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_ins_int_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_ins_w2m_MT_D)
	@-$(RM) $(TARG_TEST_UNIT_lvls_dyninit_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_lvls_vals_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_getcurrtm_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_getversion_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_gethostname_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_onbailout_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_strnlen_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_bec_file_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_bec_fprintf_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_bec_fail_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_be_fail_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_be_lrsplit_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_be_N_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_all_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_fail_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_N_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_null_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_simple_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_ins_int_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_ins_w2m_MT_R)
	@-$(RM) $(TARG_TEST_UNIT_lvls_dyninit_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_lvls_vals_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_getcurrtm_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_getversion_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_gethostname_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_onbailout_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_strnlen_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_bec_file_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_bec_fprintf_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_bec_fail_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_be_fail_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_be_lrsplit_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_be_N_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_all_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_fail_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_N_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_null_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_fe_simple_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_ins_int_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_ins_w2m_ST_D)
	@-$(RM) $(TARG_TEST_UNIT_lvls_dyninit_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_lvls_vals_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_getcurrtm_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_getversion_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_gethostname_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_onbailout_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_strnlen_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_bec_file_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_bec_fprintf_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_bec_fail_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_be_fail_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_be_lrsplit_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_be_N_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_all_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_fail_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_N_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_null_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_fe_simple_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_ins_int_ST_R)
	@-$(RM) $(TARG_TEST_UNIT_ins_w2m_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_dyninit_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_vals_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_getcurrtm_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_getversion_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_gethostname_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_onbailout_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_strnlen_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_bec_file_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fprintf_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fail_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_be_fail_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_be_lrsplit_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_be_N_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_all_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_fail_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_N_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_null_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_simple_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_ins_int_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_ins_w2m_MT_D)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_dyninit_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_vals_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_getcurrtm_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_getversion_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_gethostname_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_onbailout_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_strnlen_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_bec_file_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fprintf_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fail_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_be_fail_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_be_lrsplit_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_be_N_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_all_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_fail_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_N_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_null_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_simple_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_ins_int_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_ins_w2m_MT_R)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_dyninit_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_vals_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_getcurrtm_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_getversion_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_gethostname_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_onbailout_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_strnlen_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_bec_file_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fprintf_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fail_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_be_fail_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_be_lrsplit_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_be_N_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_all_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_fail_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_N_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_null_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_fe_simple_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_ins_int_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_ins_w2m_ST_D)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_dyninit_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_lvls_vals_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_getcurrtm_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_getversion_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_gethostname_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_onbailout_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_strnlen_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_bec_file_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fprintf_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_bec_fail_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_be_fail_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_be_lrsplit_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_be_N_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_all_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_fail_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_N_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_null_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_fe_simple_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_ins_int_ST_R)
	@-$(RM) $(OBJ_TEST_UNIT_ins_w2m_ST_R)

clean.test.component:
	@echo Cleaning component tests ...
	@-$(RM) $(TARG_TEST_COMP_core_initseq_MT_D)
	@-$(RM) $(TARG_TEST_COMP_custsev_MT_D)
	@-$(RM) $(TARG_TEST_COMP_filemt_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_args_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_b64_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_bool_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_hostid_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_int_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_pad_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_ptr_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_prid_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_real_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_slice_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_thrid_MT_D)
	@-$(RM) $(TARG_TEST_COMP_ins_w2m_MT_D)
	@-$(RM) $(TARG_TEST_COMP_log_1_MT_D)
	@-$(RM) $(TARG_TEST_COMP_pan_puts_MT_D)
	@-$(RM) $(TARG_TEST_COMP_pan_prtf_MT_D)
	@-$(RM) $(TARG_TEST_COMP_trace_1_MT_D)
	@-$(RM) $(TARG_TEST_COMP_core_initseq_MT_R)
	@-$(RM) $(TARG_TEST_COMP_custsev_MT_R)
	@-$(RM) $(TARG_TEST_COMP_filemt_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_args_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_b64_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_bool_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_hostid_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_int_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_pad_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_ptr_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_prid_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_real_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_slice_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_thrid_MT_R)
	@-$(RM) $(TARG_TEST_COMP_ins_w2m_MT_R)
	@-$(RM) $(TARG_TEST_COMP_log_1_MT_R)
	@-$(RM) $(TARG_TEST_COMP_pan_puts_MT_R)
	@-$(RM) $(TARG_TEST_COMP_pan_prtf_MT_R)
	@-$(RM) $(TARG_TEST_COMP_trace_1_MT_R)
	@-$(RM) $(TARG_TEST_COMP_core_initseq_ST_D)
	@-$(RM) $(TARG_TEST_COMP_custsev_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_args_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_b64_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_bool_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_hostid_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_int_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_pad_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_ptr_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_prid_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_real_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_slice_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_thrid_ST_D)
	@-$(RM) $(TARG_TEST_COMP_ins_w2m_ST_D)
	@-$(RM) $(TARG_TEST_COMP_log_1_ST_D)
	@-$(RM) $(TARG_TEST_COMP_pan_puts_ST_D)
	@-$(RM) $(TARG_TEST_COMP_pan_prtf_ST_D)
	@-$(RM) $(TARG_TEST_COMP_trace_1_ST_D)
	@-$(RM) $(TARG_TEST_COMP_core_initseq_ST_R)
	@-$(RM) $(TARG_TEST_COMP_custsev_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_args_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_b64_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_bool_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_hostid_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_int_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_pad_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_ptr_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_prid_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_real_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_slice_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_thrid_ST_R)
	@-$(RM) $(TARG_TEST_COMP_ins_w2m_ST_R)
	@-$(RM) $(TARG_TEST_COMP_log_1_ST_R)
	@-$(RM) $(TARG_TEST_COMP_pan_puts_ST_R)
	@-$(RM) $(TARG_TEST_COMP_pan_prtf_ST_R)
	@-$(RM) $(TARG_TEST_COMP_trace_1_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_core_initseq_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_custsev_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_filemt_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_args_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_b64_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_bool_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_hostid_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_int_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_pad_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_ptr_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_prid_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_real_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_slice_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_w2m_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_thrid_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_log_1_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_pan_puts_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_pan_prtf_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_trace_1_MT_D)
	@-$(RM) $(OBJ_TEST_COMP_core_initseq_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_custsev_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_filemt_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_args_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_b64_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_bool_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_hostid_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_int_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_pad_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_ptr_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_prid_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_real_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_slice_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_w2m_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_thrid_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_log_1_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_pan_puts_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_pan_prtf_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_trace_1_MT_R)
	@-$(RM) $(OBJ_TEST_COMP_core_initseq_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_custsev_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_args_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_b64_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_bool_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_hostid_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_int_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_pad_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_ptr_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_prid_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_real_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_slice_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_w2m_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_ins_thrid_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_log_1_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_pan_puts_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_pan_prtf_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_trace_1_ST_D)
	@-$(RM) $(OBJ_TEST_COMP_core_initseq_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_custsev_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_args_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_b64_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_bool_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_hostid_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_int_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_pad_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_ptr_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_prid_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_real_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_slice_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_w2m_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_ins_thrid_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_log_1_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_pan_puts_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_pan_prtf_ST_R)
	@-$(RM) $(OBJ_TEST_COMP_trace_1_ST_R)

clean.test.performance:
	@echo Cleaning performance tests ...
	@-$(RM) $(TARG_TEST_PERF_ins_w2m_MT_D)
	@-$(RM) $(TARG_TEST_PERF_ins_w2m_MT_R)
	@-$(RM) $(TARG_TEST_PERF_ins_w2m_ST_D)
	@-$(RM) $(TARG_TEST_PERF_ins_w2m_ST_R)
	@-$(RM) $(OBJ_TEST_PERF_ins_w2m_MT_D)
	@-$(RM) $(OBJ_TEST_PERF_ins_w2m_MT_R)
	@-$(RM) $(OBJ_TEST_PERF_ins_w2m_ST_D)
	@-$(RM) $(OBJ_TEST_PERF_ins_w2m_ST_R)

clean.test.scratch:
	@echo Cleaning scratch tests ...
	@-$(RM) $(TARG_TEST_MT_D_APIwBE_FPRINTF)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOG_MT_D)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOGWC_MT_D)
	@-$(RM) $(TARG_TEST_MT_D_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(TARG_TEST_FE_MT_D)
	@-$(RM) $(TARG_TEST_BE_N_FAIL_MT_D)
	@-$(RM) $(TARG_TEST_FE_SIMPLE_WC_MT_D)
	@-$(RM) $(TARG_TEST_MT_R_APIwBE_FPRINTF)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOG_MT_R)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOGWC_MT_R)
	@-$(RM) $(TARG_TEST_MT_R_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(TARG_TEST_FE_MT_R)
	@-$(RM) $(TARG_TEST_BE_N_FAIL_MT_R)
	@-$(RM) $(TARG_TEST_FE_SIMPLE_WC_MT_R)
	@-$(RM) $(TARG_TEST_ST_D_APIwBE_FPRINTF)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOG_ST_D)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOGWC_ST_D)
	@-$(RM) $(TARG_TEST_ST_D_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(TARG_TEST_FE_ST_D)
	@-$(RM) $(TARG_TEST_BE_N_FAIL_ST_D)
	@-$(RM) $(TARG_TEST_FE_SIMPLE_WC_ST_D)
	@-$(RM) $(TARG_TEST_ST_R_APIwBE_FPRINTF)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOG_ST_R)
	@-$(RM) $(TARG_TEST_SCRT_BE_SYSLOGWC_ST_R)
	@-$(RM) $(TARG_TEST_ST_R_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(TARG_TEST_FE_ST_R)
	@-$(RM) $(TARG_TEST_BE_N_FAIL_ST_R)
	@-$(RM) $(TARG_TEST_FE_SIMPLE_WC_ST_R)
	@-$(RM) $(OBJ_TEST_MT_D_APIwBE_FPRINTF)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOG_MT_D)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_D)
	@-$(RM) $(OBJ_TEST_MT_D_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(OBJ_TEST_FE_MT_D)
	@-$(RM) $(OBJ_TEST_BE_N_FAIL_MT_D)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_MT_D)
	@-$(RM) $(OBJ_TEST_MT_R_APIwBE_FPRINTF)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOG_MT_R)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_R)
	@-$(RM) $(OBJ_TEST_MT_R_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(OBJ_TEST_FE_MT_R)
	@-$(RM) $(OBJ_TEST_BE_N_FAIL_MT_R)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_MT_R)
	@-$(RM) $(OBJ_TEST_ST_D_APIwBE_FPRINTF)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOG_ST_D)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_D)
	@-$(RM) $(OBJ_TEST_ST_D_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(OBJ_TEST_FE_ST_D)
	@-$(RM) $(OBJ_TEST_BE_N_FAIL_ST_D)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_ST_D)
	@-$(RM) $(OBJ_TEST_ST_R_APIwBE_FPRINTF)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOG_ST_R)
	@-$(RM) $(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_R)
	@-$(RM) $(OBJ_TEST_ST_R_APIwBELR_1_FPRINTF_SYSLOG)
	@-$(RM) $(OBJ_TEST_FE_ST_R)
	@-$(RM) $(OBJ_TEST_BE_N_FAIL_ST_R)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_ST_R)
	@-$(RM) test.scratch.api.mt.debug.o
	@-$(RM) test.scratch.fe.mt.debug.o
	@-$(RM) test.scratch.api.mt.o
	@-$(RM) test.scratch.fe.mt.o
	@-$(RM) test.scratch.api.debug.o
	@-$(RM) test.scratch.fe.debug.o
	@-$(RM) test.scratch.api.o
	@-$(RM) test.scratch.fe.o
	@-$(RM) $(OBJ_TEST_API_MT_D)
	@-$(RM) $(OBJ_TEST_FE_MT_D)
	@-$(RM) $(OBJ_TEST_BE_N_MT_D)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_MT_D)
	@-$(RM) $(OBJ_TEST_API_MT_R)
	@-$(RM) $(OBJ_TEST_FE_MT_R)
	@-$(RM) $(OBJ_TEST_BE_N_MT_R)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_MT_R)
	@-$(RM) $(OBJ_TEST_API_ST_D)
	@-$(RM) $(OBJ_TEST_FE_ST_D)
	@-$(RM) $(OBJ_TEST_BE_N_ST_D)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_ST_D)
	@-$(RM) $(OBJ_TEST_API_ST_R)
	@-$(RM) $(OBJ_TEST_FE_ST_R)
	@-$(RM) $(OBJ_TEST_BE_N_ST_R)
	@-$(RM) $(OBJ_TEST_FE_SIMPLE_WC_ST_R)

ifdef ACE
clean.test.ace:
	@echo Cleaning ACE tests ...
	@-$(RM) $(TEST_SCRT_ACE_MT_D)
	@-$(RM) $(TEST_SCRT_ACE_MT_R)
endif # ACE

clean.unittest.std:
	@echo Cleaning standard unittests ...

ifdef ACE
clean.unittest.ace:
	@echo Cleaning ACE unittests ...
endif # ACE


clean.libs: 		clean.libs.core clean.libs.be clean.libs.fe

clean.libs.3pty:	clean.libs.b64 clean.libs.shwild clean.libs.xtests

clean.libs.all: 	clean.libs clean.libs.3pty

clean.test:			clean.test.unit clean.test.component clean.test.scratch clean.test.performance

ifdef ACE
clean.test.all:		clean.test clean.libs.b64 clean.libs.shwild clean.libs.xtests clean.test.ace
else # ACE
clean.test.all:		clean.test clean.libs.b64 clean.libs.shwild clean.libs.xtests
endif # ACE

clean.unittest:		clean.unittest.std

ifdef ACE
clean.unittest.all:	clean.unittest.std clean.unittest.ace
else # ACE
clean.unittest.all:	clean.unittest.std
endif # ACE

clean.std:			clean.libs clean.test

ifdef ACE
clean.ace:			clean.libs.ace clean.test.ace

endif # ACE
clean.logs:
	@echo Cleaning any log files created in testing ...
	@-$(RM) *.log

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

$(LIB_UTIL_MT_D): $(OBJ_UTIL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_UTIL_MT_D)

$(OBJ_utl_bailout_MT_D): $(PROJ_SRC_DIR)/util/bailout.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/bailout.c
$(OBJ_utl_init_code_strs_MT_D): $(PROJ_SRC_DIR)/util/init_code_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/init_code_strings.c
$(OBJ_utl_sev_strs_MT_D): $(PROJ_SRC_DIR)/util/severity_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/severity_strings.c
$(OBJ_utl_time_MT_D): $(PROJ_SRC_DIR)/util/time.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/time.cpp
$(OBJ_utl_host_name_MT_D): $(PROJ_SRC_DIR)/util/hostname.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/hostname.c
$(OBJ_utl_snprintf_MT_D): $(PROJ_SRC_DIR)/util/snprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/snprintf.c
$(OBJ_utl_strdup_MT_D): $(PROJ_SRC_DIR)/util/strdup.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strdup.c
$(OBJ_utl_procid_MT_D): $(PROJ_SRC_DIR)/util/processid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/processid.c
$(OBJ_utl_thread_MT_D): $(PROJ_SRC_DIR)/util/threadid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/threadid.c
$(OBJ_utl_be_context_MT_D): $(PROJ_SRC_DIR)/util/be.context.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.context.cpp
$(OBJ_utl_core_apidefs_MT_D): $(PROJ_SRC_DIR)/util/core.apidefs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/core.apidefs.cpp
$(OBJ_utl_be_parse_MT_D): $(PROJ_SRC_DIR)/util/be.parse.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.parse.cpp
$(OBJ_utl_strnlenp_MT_D): $(PROJ_SRC_DIR)/util/strnlen.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strnlen.cpp
$(OBJ_utl_strnlen_MT_D): $(PROJ_SRC_DIR)/util/util.strnlen.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/util.strnlen.c

$(LIB_UTIL_MT_R): $(OBJ_UTIL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_UTIL_MT_R)

$(OBJ_utl_bailout_MT_R): $(PROJ_SRC_DIR)/util/bailout.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/bailout.c
$(OBJ_utl_init_code_strs_MT_R): $(PROJ_SRC_DIR)/util/init_code_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/init_code_strings.c
$(OBJ_utl_sev_strs_MT_R): $(PROJ_SRC_DIR)/util/severity_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/severity_strings.c
$(OBJ_utl_time_MT_R): $(PROJ_SRC_DIR)/util/time.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/time.cpp
$(OBJ_utl_host_name_MT_R): $(PROJ_SRC_DIR)/util/hostname.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/hostname.c
$(OBJ_utl_snprintf_MT_R): $(PROJ_SRC_DIR)/util/snprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/snprintf.c
$(OBJ_utl_strdup_MT_R): $(PROJ_SRC_DIR)/util/strdup.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strdup.c
$(OBJ_utl_procid_MT_R): $(PROJ_SRC_DIR)/util/processid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/processid.c
$(OBJ_utl_thread_MT_R): $(PROJ_SRC_DIR)/util/threadid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/threadid.c
$(OBJ_utl_be_context_MT_R): $(PROJ_SRC_DIR)/util/be.context.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.context.cpp
$(OBJ_utl_core_apidefs_MT_R): $(PROJ_SRC_DIR)/util/core.apidefs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/core.apidefs.cpp
$(OBJ_utl_be_parse_MT_R): $(PROJ_SRC_DIR)/util/be.parse.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.parse.cpp
$(OBJ_utl_strnlenp_MT_R): $(PROJ_SRC_DIR)/util/strnlen.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strnlen.cpp
$(OBJ_utl_strnlen_MT_R): $(PROJ_SRC_DIR)/util/util.strnlen.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/util.strnlen.c

$(LIB_UTIL_ST_D): $(OBJ_UTIL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_UTIL_ST_D)

$(OBJ_utl_bailout_ST_D): $(PROJ_SRC_DIR)/util/bailout.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/bailout.c
$(OBJ_utl_init_code_strs_ST_D): $(PROJ_SRC_DIR)/util/init_code_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/init_code_strings.c
$(OBJ_utl_sev_strs_ST_D): $(PROJ_SRC_DIR)/util/severity_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/severity_strings.c
$(OBJ_utl_time_ST_D): $(PROJ_SRC_DIR)/util/time.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/time.cpp
$(OBJ_utl_host_name_ST_D): $(PROJ_SRC_DIR)/util/hostname.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/hostname.c
$(OBJ_utl_snprintf_ST_D): $(PROJ_SRC_DIR)/util/snprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/snprintf.c
$(OBJ_utl_strdup_ST_D): $(PROJ_SRC_DIR)/util/strdup.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strdup.c
$(OBJ_utl_procid_ST_D): $(PROJ_SRC_DIR)/util/processid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/processid.c
$(OBJ_utl_thread_ST_D): $(PROJ_SRC_DIR)/util/threadid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/threadid.c
$(OBJ_utl_be_context_ST_D): $(PROJ_SRC_DIR)/util/be.context.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.context.cpp
$(OBJ_utl_core_apidefs_ST_D): $(PROJ_SRC_DIR)/util/core.apidefs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/core.apidefs.cpp
$(OBJ_utl_be_parse_ST_D): $(PROJ_SRC_DIR)/util/be.parse.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.parse.cpp
$(OBJ_utl_strnlenp_ST_D): $(PROJ_SRC_DIR)/util/strnlen.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strnlen.cpp
$(OBJ_utl_strnlen_ST_D): $(PROJ_SRC_DIR)/util/util.strnlen.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/util.strnlen.c

$(LIB_UTIL_ST_R): $(OBJ_UTIL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_UTIL_ST_R)

$(OBJ_utl_bailout_ST_R): $(PROJ_SRC_DIR)/util/bailout.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/bailout.c
$(OBJ_utl_init_code_strs_ST_R): $(PROJ_SRC_DIR)/util/init_code_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/init_code_strings.c
$(OBJ_utl_sev_strs_ST_R): $(PROJ_SRC_DIR)/util/severity_strings.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/severity_strings.c
$(OBJ_utl_time_ST_R): $(PROJ_SRC_DIR)/util/time.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/time.cpp
$(OBJ_utl_host_name_ST_R): $(PROJ_SRC_DIR)/util/hostname.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/hostname.c
$(OBJ_utl_snprintf_ST_R): $(PROJ_SRC_DIR)/util/snprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/snprintf.c
$(OBJ_utl_strdup_ST_R): $(PROJ_SRC_DIR)/util/strdup.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strdup.c
$(OBJ_utl_procid_ST_R): $(PROJ_SRC_DIR)/util/processid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/processid.c
$(OBJ_utl_thread_ST_R): $(PROJ_SRC_DIR)/util/threadid.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/threadid.c
$(OBJ_utl_be_context_ST_R): $(PROJ_SRC_DIR)/util/be.context.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.context.cpp
$(OBJ_utl_core_apidefs_ST_R): $(PROJ_SRC_DIR)/util/core.apidefs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/core.apidefs.cpp
$(OBJ_utl_be_parse_ST_R): $(PROJ_SRC_DIR)/util/be.parse.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/be.parse.cpp
$(OBJ_utl_strnlenp_ST_R): $(PROJ_SRC_DIR)/util/strnlen.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/strnlen.cpp
$(OBJ_utl_strnlen_ST_R): $(PROJ_SRC_DIR)/util/util.strnlen.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/util/util.strnlen.c


########################################
# Main library - 'pantheios.X.core.a'

$(LIB_CORE_MT_D): $(OBJ_CORE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_CORE_MT_D)

$(OBJ_cr_api_MT_D): $(PROJ_SRC_DIR)/core/api.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.cpp
$(OBJ_cr_api_dep_MT_D): $(PROJ_SRC_DIR)/core/api.deprecated.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.deprecated.c
$(OBJ_cr_api_exit_MT_D): $(PROJ_SRC_DIR)/core/api.exitprocess.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.exitprocess.cpp
$(OBJ_cr_api_logprintf_MT_D): $(PROJ_SRC_DIR)/core/logprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/logprintf.c
$(OBJ_cr_api_logputs_MT_D): $(PROJ_SRC_DIR)/core/api.logputs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.logputs.cpp
$(OBJ_cr_auto_MT_D): $(PROJ_SRC_DIR)/core/auto.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/auto.cpp

$(OBJ_ins_args_MT_D): $(PROJ_SRC_DIR)/inserters/args.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/args.cpp
$(OBJ_ins_b64_MT_D): $(PROJ_SRC_DIR)/inserters/b64.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/b64.cpp
$(OBJ_ins_blob_MT_D): $(PROJ_SRC_DIR)/inserters/blob.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/blob.cpp
$(OBJ_ins_bool_MT_D): $(PROJ_SRC_DIR)/inserters/boolean.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/boolean.cpp
$(OBJ_ins_excp_MT_D): $(PROJ_SRC_DIR)/inserters/exception.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/exception.cpp
$(OBJ_ins_host_MT_D): $(PROJ_SRC_DIR)/inserters/hostid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/hostid.cpp
$(OBJ_ins_int_MT_D): $(PROJ_SRC_DIR)/inserters/integer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/integer.cpp
$(OBJ_ins_ptr_MT_D): $(PROJ_SRC_DIR)/inserters/pointer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/pointer.cpp
$(OBJ_ins_prid_MT_D): $(PROJ_SRC_DIR)/inserters/processid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/processid.cpp
$(OBJ_ins_real_MT_D): $(PROJ_SRC_DIR)/inserters/real.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/real.cpp
$(OBJ_ins_slice_MT_D): $(PROJ_SRC_DIR)/inserters/slice.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/slice.cpp
$(OBJ_ins_thrid_MT_D): $(PROJ_SRC_DIR)/inserters/threadid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/threadid.cpp
$(OBJ_ins_w2m_MT_D): $(PROJ_SRC_DIR)/inserters/w2m.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/w2m.cpp

$(LIB_CORE_MT_R): $(OBJ_CORE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_CORE_MT_R)

$(OBJ_cr_api_MT_R): $(PROJ_SRC_DIR)/core/api.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.cpp
$(OBJ_cr_api_dep_MT_R): $(PROJ_SRC_DIR)/core/api.deprecated.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.deprecated.c
$(OBJ_cr_api_exit_MT_R): $(PROJ_SRC_DIR)/core/api.exitprocess.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.exitprocess.cpp
$(OBJ_cr_api_logprintf_MT_R): $(PROJ_SRC_DIR)/core/logprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/logprintf.c
$(OBJ_cr_api_logputs_MT_R): $(PROJ_SRC_DIR)/core/api.logputs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.logputs.cpp
$(OBJ_cr_auto_MT_R): $(PROJ_SRC_DIR)/core/auto.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/auto.cpp

$(OBJ_ins_args_MT_R): $(PROJ_SRC_DIR)/inserters/args.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/args.cpp
$(OBJ_ins_b64_MT_R): $(PROJ_SRC_DIR)/inserters/b64.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/b64.cpp
$(OBJ_ins_blob_MT_R): $(PROJ_SRC_DIR)/inserters/blob.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/blob.cpp
$(OBJ_ins_bool_MT_R): $(PROJ_SRC_DIR)/inserters/boolean.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/boolean.cpp
$(OBJ_ins_excp_MT_R): $(PROJ_SRC_DIR)/inserters/exception.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/exception.cpp
$(OBJ_ins_host_MT_R): $(PROJ_SRC_DIR)/inserters/hostid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/hostid.cpp
$(OBJ_ins_int_MT_R): $(PROJ_SRC_DIR)/inserters/integer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/integer.cpp
$(OBJ_ins_ptr_MT_R): $(PROJ_SRC_DIR)/inserters/pointer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/pointer.cpp
$(OBJ_ins_prid_MT_R): $(PROJ_SRC_DIR)/inserters/processid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/processid.cpp
$(OBJ_ins_real_MT_R): $(PROJ_SRC_DIR)/inserters/real.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/real.cpp
$(OBJ_ins_slice_MT_R): $(PROJ_SRC_DIR)/inserters/slice.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/slice.cpp
$(OBJ_ins_thrid_MT_R): $(PROJ_SRC_DIR)/inserters/threadid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/threadid.cpp
$(OBJ_ins_w2m_MT_R): $(PROJ_SRC_DIR)/inserters/w2m.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/w2m.cpp

$(LIB_CORE_ST_D): $(OBJ_CORE_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_CORE_ST_D)

$(OBJ_cr_api_ST_D): $(PROJ_SRC_DIR)/core/api.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.cpp
$(OBJ_cr_api_dep_ST_D): $(PROJ_SRC_DIR)/core/api.deprecated.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.deprecated.c
$(OBJ_cr_api_exit_ST_D): $(PROJ_SRC_DIR)/core/api.exitprocess.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.exitprocess.cpp
$(OBJ_cr_api_logprintf_ST_D): $(PROJ_SRC_DIR)/core/logprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/logprintf.c
$(OBJ_cr_api_logputs_ST_D): $(PROJ_SRC_DIR)/core/api.logputs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.logputs.cpp
$(OBJ_cr_auto_ST_D): $(PROJ_SRC_DIR)/core/auto.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/auto.cpp

$(OBJ_ins_args_ST_D): $(PROJ_SRC_DIR)/inserters/args.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/args.cpp
$(OBJ_ins_b64_ST_D): $(PROJ_SRC_DIR)/inserters/b64.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/b64.cpp
$(OBJ_ins_blob_ST_D): $(PROJ_SRC_DIR)/inserters/blob.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/blob.cpp
$(OBJ_ins_bool_ST_D): $(PROJ_SRC_DIR)/inserters/boolean.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/boolean.cpp
$(OBJ_ins_excp_ST_D): $(PROJ_SRC_DIR)/inserters/exception.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/exception.cpp
$(OBJ_ins_host_ST_D): $(PROJ_SRC_DIR)/inserters/hostid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/hostid.cpp
$(OBJ_ins_int_ST_D): $(PROJ_SRC_DIR)/inserters/integer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/integer.cpp
$(OBJ_ins_ptr_ST_D): $(PROJ_SRC_DIR)/inserters/pointer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/pointer.cpp
$(OBJ_ins_prid_ST_D): $(PROJ_SRC_DIR)/inserters/processid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/processid.cpp
$(OBJ_ins_real_ST_D): $(PROJ_SRC_DIR)/inserters/real.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/real.cpp
$(OBJ_ins_slice_ST_D): $(PROJ_SRC_DIR)/inserters/slice.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/slice.cpp
$(OBJ_ins_thrid_ST_D): $(PROJ_SRC_DIR)/inserters/threadid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/threadid.cpp
$(OBJ_ins_w2m_ST_D): $(PROJ_SRC_DIR)/inserters/w2m.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/w2m.cpp

$(LIB_CORE_ST_R): $(OBJ_CORE_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_CORE_ST_R)

$(OBJ_cr_api_ST_R): $(PROJ_SRC_DIR)/core/api.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.cpp
$(OBJ_cr_api_dep_ST_R): $(PROJ_SRC_DIR)/core/api.deprecated.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.deprecated.c
$(OBJ_cr_api_exit_ST_R): $(PROJ_SRC_DIR)/core/api.exitprocess.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.exitprocess.cpp
$(OBJ_cr_api_logprintf_ST_R): $(PROJ_SRC_DIR)/core/logprintf.c $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/logprintf.c
$(OBJ_cr_api_logputs_ST_R): $(PROJ_SRC_DIR)/core/api.logputs.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/api.logputs.cpp
$(OBJ_cr_auto_ST_R): $(PROJ_SRC_DIR)/core/auto.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/core/auto.cpp

$(OBJ_ins_args_ST_R): $(PROJ_SRC_DIR)/inserters/args.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/args.cpp
$(OBJ_ins_b64_ST_R): $(PROJ_SRC_DIR)/inserters/b64.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/b64.cpp
$(OBJ_ins_blob_ST_R): $(PROJ_SRC_DIR)/inserters/blob.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/blob.cpp
$(OBJ_ins_bool_ST_R): $(PROJ_SRC_DIR)/inserters/boolean.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/boolean.cpp
$(OBJ_ins_excp_ST_R): $(PROJ_SRC_DIR)/inserters/exception.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/exception.cpp
$(OBJ_ins_host_ST_R): $(PROJ_SRC_DIR)/inserters/hostid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/hostid.cpp
$(OBJ_ins_int_ST_R): $(PROJ_SRC_DIR)/inserters/integer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/integer.cpp
$(OBJ_ins_ptr_ST_R): $(PROJ_SRC_DIR)/inserters/pointer.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/pointer.cpp
$(OBJ_ins_prid_ST_R): $(PROJ_SRC_DIR)/inserters/processid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/processid.cpp
$(OBJ_ins_real_ST_R): $(PROJ_SRC_DIR)/inserters/real.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/real.cpp
$(OBJ_ins_slice_ST_R): $(PROJ_SRC_DIR)/inserters/slice.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/slice.cpp
$(OBJ_ins_thrid_ST_R): $(PROJ_SRC_DIR)/inserters/threadid.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/threadid.cpp
$(OBJ_ins_w2m_ST_R): $(PROJ_SRC_DIR)/inserters/w2m.cpp $(CORE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/inserters/w2m.cpp

########################################
# Main library - 'pantheios.X.appl.a'

$(LIB_APPL_MT_D): $(OBJ_APPL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_APPL_MT_D)

$(LIB_APPL_MT_R): $(OBJ_APPL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_APPL_MT_R)

$(LIB_APPL_ST_D): $(OBJ_APPL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_APPL_ST_D)

$(LIB_APPL_ST_R): $(OBJ_APPL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_APPL_ST_R)

########################################
# Backend util components

########################################
# Backend common stock library - 'pantheios.bec.stock.a'

########################################
# Backend library - 'pantheios.be.lrsplit.a'

$(LIB_BE_LRSPLIT_MT_D): $(OBJ_BE_LRSPLIT_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_LRSPLIT_MT_D)

$(OBJ_be_lrsplit_MT_D): $(PROJ_SRC_DIR)/backends/be.lrsplit.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.lrsplit.c

$(LIB_BE_LRSPLIT_MT_R): $(OBJ_BE_LRSPLIT_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_LRSPLIT_MT_R)

$(OBJ_be_lrsplit_MT_R): $(PROJ_SRC_DIR)/backends/be.lrsplit.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.lrsplit.c

$(LIB_BE_LRSPLIT_ST_D): $(OBJ_BE_LRSPLIT_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_LRSPLIT_ST_D)

$(OBJ_be_lrsplit_ST_D): $(PROJ_SRC_DIR)/backends/be.lrsplit.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.lrsplit.c

$(LIB_BE_LRSPLIT_ST_R): $(OBJ_BE_LRSPLIT_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_LRSPLIT_ST_R)

$(OBJ_be_lrsplit_ST_R): $(PROJ_SRC_DIR)/backends/be.lrsplit.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.lrsplit.c

########################################
# Backend library - 'pantheios.be.N.a'

$(LIB_BE_N_MT_D): $(OBJ_BE_N_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_N_MT_D)

$(OBJ_be_N_MT_D): $(PROJ_SRC_DIR)/backends/be.N.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.N.c

$(LIB_BE_N_MT_R): $(OBJ_BE_N_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_N_MT_R)

$(OBJ_be_N_MT_R): $(PROJ_SRC_DIR)/backends/be.N.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.N.c

$(LIB_BE_N_ST_D): $(OBJ_BE_N_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_N_ST_D)

$(OBJ_be_N_ST_D): $(PROJ_SRC_DIR)/backends/be.N.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.N.c

$(LIB_BE_N_ST_R): $(OBJ_BE_N_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_N_ST_R)

$(OBJ_be_N_ST_R): $(PROJ_SRC_DIR)/backends/be.N.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be.N.c

########################################
# Back-end library - 'pantheios.bec.test.a'

$(LIB_BEC_TEST_MT_D): $(OBJ_BEC_TEST_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_TEST_MT_D)

$(OBJ_bec_test_MT_D): $(PROJ_SRC_DIR)/backends/bec.test.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.test.cpp

$(LIB_BEC_TEST_MT_R): $(OBJ_BEC_TEST_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_TEST_MT_R)

$(OBJ_bec_test_MT_R): $(PROJ_SRC_DIR)/backends/bec.test.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.test.cpp

$(LIB_BEC_TEST_ST_D): $(OBJ_BEC_TEST_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_TEST_ST_D)

$(OBJ_bec_test_ST_D): $(PROJ_SRC_DIR)/backends/bec.test.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.test.cpp

$(LIB_BEC_TEST_ST_R): $(OBJ_BEC_TEST_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_TEST_ST_R)

$(OBJ_bec_test_ST_R): $(PROJ_SRC_DIR)/backends/bec.test.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.test.cpp

########################################
# Back-end library - 'pantheios.be.test.a'

$(LIB_BE_TEST_MT_D): $(OBJ_BE_TEST_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_TEST_MT_D)

$(OBJ_be_test_MT_D): $(PROJ_SRC_DIR)/backends/be/be.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.test.c

$(LIB_BE_TEST_MT_R): $(OBJ_BE_TEST_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_TEST_MT_R)

$(OBJ_be_test_MT_R): $(PROJ_SRC_DIR)/backends/be/be.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.test.c

$(LIB_BE_TEST_ST_D): $(OBJ_BE_TEST_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_TEST_ST_D)

$(OBJ_be_test_ST_D): $(PROJ_SRC_DIR)/backends/be/be.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.test.c

$(LIB_BE_TEST_ST_R): $(OBJ_BE_TEST_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_TEST_ST_R)

$(OBJ_be_test_ST_R): $(PROJ_SRC_DIR)/backends/be/be.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.test.c

########################################
# Back-end library - 'pantheios.bel.test.a'

$(LIB_BEL_TEST_MT_D): $(OBJ_BEL_TEST_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_TEST_MT_D)

$(OBJ_bel_test_MT_D): $(PROJ_SRC_DIR)/backends/bel/bel.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.test.c

$(LIB_BEL_TEST_MT_R): $(OBJ_BEL_TEST_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_TEST_MT_R)

$(OBJ_bel_test_MT_R): $(PROJ_SRC_DIR)/backends/bel/bel.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.test.c

$(LIB_BEL_TEST_ST_D): $(OBJ_BEL_TEST_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_TEST_ST_D)

$(OBJ_bel_test_ST_D): $(PROJ_SRC_DIR)/backends/bel/bel.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.test.c

$(LIB_BEL_TEST_ST_R): $(OBJ_BEL_TEST_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_TEST_ST_R)

$(OBJ_bel_test_ST_R): $(PROJ_SRC_DIR)/backends/bel/bel.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.test.c

########################################
# Back-end library - 'pantheios.ber.test.a'

$(LIB_BER_TEST_MT_D): $(OBJ_BER_TEST_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_TEST_MT_D)

$(OBJ_ber_test_MT_D): $(PROJ_SRC_DIR)/backends/ber/ber.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.test.c

$(LIB_BER_TEST_MT_R): $(OBJ_BER_TEST_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_TEST_MT_R)

$(OBJ_ber_test_MT_R): $(PROJ_SRC_DIR)/backends/ber/ber.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.test.c

$(LIB_BER_TEST_ST_D): $(OBJ_BER_TEST_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_TEST_ST_D)

$(OBJ_ber_test_ST_D): $(PROJ_SRC_DIR)/backends/ber/ber.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.test.c

$(LIB_BER_TEST_ST_R): $(OBJ_BER_TEST_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_TEST_ST_R)

$(OBJ_ber_test_ST_R): $(PROJ_SRC_DIR)/backends/ber/ber.test.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.test.c

########################################
# Backend library - 'pantheios.bec.fail.a'

$(LIB_BEC_FAIL_MT_D): $(OBJ_BEC_FAIL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FAIL_MT_D)

$(OBJ_bec_fail_MT_D): $(PROJ_SRC_DIR)/backends/bec.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fail.c

$(LIB_BEC_FAIL_MT_R): $(OBJ_BEC_FAIL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FAIL_MT_R)

$(OBJ_bec_fail_MT_R): $(PROJ_SRC_DIR)/backends/bec.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fail.c

$(LIB_BEC_FAIL_ST_D): $(OBJ_BEC_FAIL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FAIL_ST_D)

$(OBJ_bec_fail_ST_D): $(PROJ_SRC_DIR)/backends/bec.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fail.c

$(LIB_BEC_FAIL_ST_R): $(OBJ_BEC_FAIL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FAIL_ST_R)

$(OBJ_bec_fail_ST_R): $(PROJ_SRC_DIR)/backends/bec.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fail.c


########################################
# Backend library - 'pantheios.be.fail.a'

$(LIB_BE_FAIL_MT_D): $(OBJ_BE_FAIL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FAIL_MT_D)

$(OBJ_be_fail_MT_D): $(PROJ_SRC_DIR)/backends/be/be.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fail.c

$(LIB_BE_FAIL_MT_R): $(OBJ_BE_FAIL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FAIL_MT_R)

$(OBJ_be_fail_MT_R): $(PROJ_SRC_DIR)/backends/be/be.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fail.c

$(LIB_BE_FAIL_ST_D): $(OBJ_BE_FAIL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FAIL_ST_D)

$(OBJ_be_fail_ST_D): $(PROJ_SRC_DIR)/backends/be/be.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fail.c

$(LIB_BE_FAIL_ST_R): $(OBJ_BE_FAIL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FAIL_ST_R)

$(OBJ_be_fail_ST_R): $(PROJ_SRC_DIR)/backends/be/be.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fail.c


########################################
# Backend library - 'pantheios.bel.fail.a'

$(LIB_BEL_FAIL_MT_D): $(OBJ_BEL_FAIL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FAIL_MT_D)

$(OBJ_bel_fail_MT_D): $(PROJ_SRC_DIR)/backends/bel/bel.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fail.c

$(LIB_BEL_FAIL_MT_R): $(OBJ_BEL_FAIL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FAIL_MT_R)

$(OBJ_bel_fail_MT_R): $(PROJ_SRC_DIR)/backends/bel/bel.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fail.c

$(LIB_BEL_FAIL_ST_D): $(OBJ_BEL_FAIL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FAIL_ST_D)

$(OBJ_bel_fail_ST_D): $(PROJ_SRC_DIR)/backends/bel/bel.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fail.c

$(LIB_BEL_FAIL_ST_R): $(OBJ_BEL_FAIL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FAIL_ST_R)

$(OBJ_bel_fail_ST_R): $(PROJ_SRC_DIR)/backends/bel/bel.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fail.c


########################################
# Backend library - 'pantheios.ber.fail.a'

$(LIB_BER_FAIL_MT_D): $(OBJ_BER_FAIL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FAIL_MT_D)

$(OBJ_ber_fail_MT_D): $(PROJ_SRC_DIR)/backends/ber/ber.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fail.c

$(LIB_BER_FAIL_MT_R): $(OBJ_BER_FAIL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FAIL_MT_R)

$(OBJ_ber_fail_MT_R): $(PROJ_SRC_DIR)/backends/ber/ber.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fail.c

$(LIB_BER_FAIL_ST_D): $(OBJ_BER_FAIL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FAIL_ST_D)

$(OBJ_ber_fail_ST_D): $(PROJ_SRC_DIR)/backends/ber/ber.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fail.c

$(LIB_BER_FAIL_ST_R): $(OBJ_BER_FAIL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FAIL_ST_R)

$(OBJ_ber_fail_ST_R): $(PROJ_SRC_DIR)/backends/ber/ber.fail.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fail.c


########################################
# Backend library - 'pantheios.bec.file.a'

$(LIB_BEC_FILE_MT_D): $(OBJ_BEC_FILE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_MT_D)

$(OBJ_bec_file_MT_D): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp

$(LIB_BEC_FILE_MT_R): $(OBJ_BEC_FILE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_MT_R)

$(OBJ_bec_file_MT_R): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp

$(LIB_BEC_FILE_ST_D): $(OBJ_BEC_FILE_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_ST_D)

$(OBJ_bec_file_ST_D): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp

$(LIB_BEC_FILE_ST_R): $(OBJ_BEC_FILE_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_ST_R)

$(OBJ_bec_file_ST_R): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp


########################################
# Backend library - 'pantheios.bec.file.WithCallback.a'

$(LIB_BEC_FILE_WC_MT_D): $(OBJ_BEC_FILE_WC_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_WC_MT_D)

$(OBJ_bec_file_WC_MT_D): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp

$(LIB_BEC_FILE_WC_MT_R): $(OBJ_BEC_FILE_WC_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_WC_MT_R)

$(OBJ_bec_file_WC_MT_R): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp

$(LIB_BEC_FILE_WC_ST_D): $(OBJ_BEC_FILE_WC_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_WC_ST_D)

$(OBJ_bec_file_WC_ST_D): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp

$(LIB_BEC_FILE_WC_ST_R): $(OBJ_BEC_FILE_WC_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FILE_WC_ST_R)

$(OBJ_bec_file_WC_ST_R): $(PROJ_SRC_DIR)/backends/bec.file.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.file.cpp


########################################
# Backend library - 'pantheios.be.file.a'

$(LIB_BE_FILE_MT_D): $(OBJ_BE_FILE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FILE_MT_D)

$(OBJ_be_file_MT_D): $(PROJ_SRC_DIR)/backends/be/be.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.file.c

$(LIB_BE_FILE_MT_R): $(OBJ_BE_FILE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FILE_MT_R)

$(OBJ_be_file_MT_R): $(PROJ_SRC_DIR)/backends/be/be.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.file.c

$(LIB_BE_FILE_ST_D): $(OBJ_BE_FILE_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FILE_ST_D)

$(OBJ_be_file_ST_D): $(PROJ_SRC_DIR)/backends/be/be.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.file.c

$(LIB_BE_FILE_ST_R): $(OBJ_BE_FILE_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FILE_ST_R)

$(OBJ_be_file_ST_R): $(PROJ_SRC_DIR)/backends/be/be.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.file.c


########################################
# Backend library - 'pantheios.bel.file.a'

$(LIB_BEL_FILE_MT_D): $(OBJ_BEL_FILE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FILE_MT_D)

$(OBJ_bel_file_MT_D): $(PROJ_SRC_DIR)/backends/bel/bel.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.file.c

$(LIB_BEL_FILE_MT_R): $(OBJ_BEL_FILE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FILE_MT_R)

$(OBJ_bel_file_MT_R): $(PROJ_SRC_DIR)/backends/bel/bel.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.file.c

$(LIB_BEL_FILE_ST_D): $(OBJ_BEL_FILE_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FILE_ST_D)

$(OBJ_bel_file_ST_D): $(PROJ_SRC_DIR)/backends/bel/bel.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.file.c

$(LIB_BEL_FILE_ST_R): $(OBJ_BEL_FILE_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FILE_ST_R)

$(OBJ_bel_file_ST_R): $(PROJ_SRC_DIR)/backends/bel/bel.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.file.c


########################################
# Backend library - 'pantheios.ber.file.a'

$(LIB_BER_FILE_MT_D): $(OBJ_BER_FILE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FILE_MT_D)

$(OBJ_ber_file_MT_D): $(PROJ_SRC_DIR)/backends/ber/ber.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.file.c

$(LIB_BER_FILE_MT_R): $(OBJ_BER_FILE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FILE_MT_R)

$(OBJ_ber_file_MT_R): $(PROJ_SRC_DIR)/backends/ber/ber.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.file.c

$(LIB_BER_FILE_ST_D): $(OBJ_BER_FILE_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FILE_ST_D)

$(OBJ_ber_file_ST_D): $(PROJ_SRC_DIR)/backends/ber/ber.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.file.c

$(LIB_BER_FILE_ST_R): $(OBJ_BER_FILE_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FILE_ST_R)

$(OBJ_ber_file_ST_R): $(PROJ_SRC_DIR)/backends/ber/ber.file.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.file.c


########################################
# Backend library - 'pantheios.bec.fprintf.a'

$(LIB_BEC_FPRINTF_MT_D): $(OBJ_BEC_FPRINTF_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_MT_D)

$(OBJ_bec_fprintf_MT_D): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp

$(LIB_BEC_FPRINTF_MT_R): $(OBJ_BEC_FPRINTF_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_MT_R)

$(OBJ_bec_fprintf_MT_R): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp

$(LIB_BEC_FPRINTF_ST_D): $(OBJ_BEC_FPRINTF_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_ST_D)

$(OBJ_bec_fprintf_ST_D): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp

$(LIB_BEC_FPRINTF_ST_R): $(OBJ_BEC_FPRINTF_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_ST_R)

$(OBJ_bec_fprintf_ST_R): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp


########################################
# Backend library - 'pantheios.bec.fprintf.WithCallback.a'

$(LIB_BEC_FPRINTF_WC_MT_D): $(OBJ_BEC_FPRINTF_WC_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_WC_MT_D)

$(OBJ_bec_fprintf_WC_MT_D): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp

$(LIB_BEC_FPRINTF_WC_MT_R): $(OBJ_BEC_FPRINTF_WC_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_WC_MT_R)

$(OBJ_bec_fprintf_WC_MT_R): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp

$(LIB_BEC_FPRINTF_WC_ST_D): $(OBJ_BEC_FPRINTF_WC_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_WC_ST_D)

$(OBJ_bec_fprintf_WC_ST_D): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_D) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp

$(LIB_BEC_FPRINTF_WC_ST_R): $(OBJ_BEC_FPRINTF_WC_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_FPRINTF_WC_ST_R)

$(OBJ_bec_fprintf_WC_ST_R): $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_ST_R) -DPANTHEIOS_BE_USE_CALLBACK $(CPPC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.fprintf.cpp


########################################
# Backend library - 'pantheios.be.fprintf.a'

$(LIB_BE_FPRINTF_MT_D): $(OBJ_BE_FPRINTF_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FPRINTF_MT_D)

$(OBJ_be_fprintf_MT_D): $(PROJ_SRC_DIR)/backends/be/be.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fprintf.c

$(LIB_BE_FPRINTF_MT_R): $(OBJ_BE_FPRINTF_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FPRINTF_MT_R)

$(OBJ_be_fprintf_MT_R): $(PROJ_SRC_DIR)/backends/be/be.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fprintf.c

$(LIB_BE_FPRINTF_ST_D): $(OBJ_BE_FPRINTF_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FPRINTF_ST_D)

$(OBJ_be_fprintf_ST_D): $(PROJ_SRC_DIR)/backends/be/be.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fprintf.c

$(LIB_BE_FPRINTF_ST_R): $(OBJ_BE_FPRINTF_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_FPRINTF_ST_R)

$(OBJ_be_fprintf_ST_R): $(PROJ_SRC_DIR)/backends/be/be.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.fprintf.c


########################################
# Backend library - 'pantheios.bel.fprintf.a'

$(LIB_BEL_FPRINTF_MT_D): $(OBJ_BEL_FPRINTF_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FPRINTF_MT_D)

$(OBJ_bel_fprintf_MT_D): $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c

$(LIB_BEL_FPRINTF_MT_R): $(OBJ_BEL_FPRINTF_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FPRINTF_MT_R)

$(OBJ_bel_fprintf_MT_R): $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c

$(LIB_BEL_FPRINTF_ST_D): $(OBJ_BEL_FPRINTF_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FPRINTF_ST_D)

$(OBJ_bel_fprintf_ST_D): $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c

$(LIB_BEL_FPRINTF_ST_R): $(OBJ_BEL_FPRINTF_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_FPRINTF_ST_R)

$(OBJ_bel_fprintf_ST_R): $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.fprintf.c


########################################
# Backend library - 'pantheios.ber.fprintf.a'

$(LIB_BER_FPRINTF_MT_D): $(OBJ_BER_FPRINTF_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FPRINTF_MT_D)

$(OBJ_ber_fprintf_MT_D): $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c

$(LIB_BER_FPRINTF_MT_R): $(OBJ_BER_FPRINTF_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FPRINTF_MT_R)

$(OBJ_ber_fprintf_MT_R): $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c

$(LIB_BER_FPRINTF_ST_D): $(OBJ_BER_FPRINTF_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FPRINTF_ST_D)

$(OBJ_ber_fprintf_ST_D): $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c

$(LIB_BER_FPRINTF_ST_R): $(OBJ_BER_FPRINTF_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_FPRINTF_ST_R)

$(OBJ_ber_fprintf_ST_R): $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.fprintf.c


########################################
# Backend library - 'pantheios.bec.null.a'

$(LIB_BEC_NULL_MT_D): $(OBJ_BEC_NULL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_NULL_MT_D)

$(OBJ_bec_null_MT_D): $(PROJ_SRC_DIR)/backends/bec.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.null.c

$(LIB_BEC_NULL_MT_R): $(OBJ_BEC_NULL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_NULL_MT_R)

$(OBJ_bec_null_MT_R): $(PROJ_SRC_DIR)/backends/bec.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.null.c

$(LIB_BEC_NULL_ST_D): $(OBJ_BEC_NULL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_NULL_ST_D)

$(OBJ_bec_null_ST_D): $(PROJ_SRC_DIR)/backends/bec.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.null.c

$(LIB_BEC_NULL_ST_R): $(OBJ_BEC_NULL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_NULL_ST_R)

$(OBJ_bec_null_ST_R): $(PROJ_SRC_DIR)/backends/bec.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.null.c


########################################
# Backend library - 'pantheios.be.null.a'

$(LIB_BE_NULL_MT_D): $(OBJ_BE_NULL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_NULL_MT_D)

$(OBJ_be_null_MT_D): $(PROJ_SRC_DIR)/backends/be/be.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.null.c

$(LIB_BE_NULL_MT_R): $(OBJ_BE_NULL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_NULL_MT_R)

$(OBJ_be_null_MT_R): $(PROJ_SRC_DIR)/backends/be/be.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.null.c

$(LIB_BE_NULL_ST_D): $(OBJ_BE_NULL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_NULL_ST_D)

$(OBJ_be_null_ST_D): $(PROJ_SRC_DIR)/backends/be/be.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.null.c

$(LIB_BE_NULL_ST_R): $(OBJ_BE_NULL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_NULL_ST_R)

$(OBJ_be_null_ST_R): $(PROJ_SRC_DIR)/backends/be/be.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.null.c


########################################
# Backend library - 'pantheios.bel.null.a'

$(LIB_BEL_NULL_MT_D): $(OBJ_BEL_NULL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_NULL_MT_D)

$(OBJ_bel_null_MT_D): $(PROJ_SRC_DIR)/backends/bel/bel.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.null.c

$(LIB_BEL_NULL_MT_R): $(OBJ_BEL_NULL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_NULL_MT_R)

$(OBJ_bel_null_MT_R): $(PROJ_SRC_DIR)/backends/bel/bel.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.null.c

$(LIB_BEL_NULL_ST_D): $(OBJ_BEL_NULL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_NULL_ST_D)

$(OBJ_bel_null_ST_D): $(PROJ_SRC_DIR)/backends/bel/bel.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.null.c

$(LIB_BEL_NULL_ST_R): $(OBJ_BEL_NULL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_NULL_ST_R)

$(OBJ_bel_null_ST_R): $(PROJ_SRC_DIR)/backends/bel/bel.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.null.c


########################################
# Backend library - 'pantheios.ber.null.a'

$(LIB_BER_NULL_MT_D): $(OBJ_BER_NULL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_NULL_MT_D)

$(OBJ_ber_null_MT_D): $(PROJ_SRC_DIR)/backends/ber/ber.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.null.c

$(LIB_BER_NULL_MT_R): $(OBJ_BER_NULL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_NULL_MT_R)

$(OBJ_ber_null_MT_R): $(PROJ_SRC_DIR)/backends/ber/ber.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.null.c

$(LIB_BER_NULL_ST_D): $(OBJ_BER_NULL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_NULL_ST_D)

$(OBJ_ber_null_ST_D): $(PROJ_SRC_DIR)/backends/ber/ber.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.null.c

$(LIB_BER_NULL_ST_R): $(OBJ_BER_NULL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_NULL_ST_R)

$(OBJ_ber_null_ST_R): $(PROJ_SRC_DIR)/backends/ber/ber.null.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.null.c


ifdef ACE
########################################
# Backend library - 'pantheios.bec.ACE.a'

$(LIB_BEC_ACE_MT_D): $(OBJ_BEC_ACE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_ACE_MT_D)

$(OBJ_bec_ACE_MT_D): $(PROJ_SRC_DIR)/backends/bec.ACE.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_D) $(CPPC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/bec.ACE.cpp

$(LIB_BEC_ACE_MT_R): $(OBJ_BEC_ACE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_ACE_MT_R)

$(OBJ_bec_ACE_MT_R): $(PROJ_SRC_DIR)/backends/bec.ACE.cpp $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_LIB_ARGS_MT_R) $(CPPC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/bec.ACE.cpp


########################################
# Backend library - 'pantheios.be.ACE.a'

$(LIB_BE_ACE_MT_D): $(OBJ_BE_ACE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_ACE_MT_D)

$(OBJ_be_ACE_MT_D): $(PROJ_SRC_DIR)/backends/be/be.ACE.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/be/be.ACE.c

$(LIB_BE_ACE_MT_R): $(OBJ_BE_ACE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_ACE_MT_R)

$(OBJ_be_ACE_MT_R): $(PROJ_SRC_DIR)/backends/be/be.ACE.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/be/be.ACE.c


########################################
# Backend library - 'pantheios.bel.ACE.a'

$(LIB_BEL_ACE_MT_D): $(OBJ_BEL_ACE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_ACE_MT_D)

$(OBJ_bel_ACE_MT_D): $(PROJ_SRC_DIR)/backends/bel/bel.ACE.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.ACE.c

$(LIB_BEL_ACE_MT_R): $(OBJ_BEL_ACE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEL_ACE_MT_R)

$(OBJ_bel_ACE_MT_R): $(PROJ_SRC_DIR)/backends/bel/bel.ACE.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/bel/bel.ACE.c


########################################
# Backend library - 'pantheios.ber.ACE.a'

$(LIB_BER_ACE_MT_D): $(OBJ_BER_ACE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_ACE_MT_D)

$(OBJ_ber_ACE_MT_D): $(PROJ_SRC_DIR)/backends/ber/ber.ACE.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.ACE.c

$(LIB_BER_ACE_MT_R): $(OBJ_BER_ACE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_ACE_MT_R)

$(OBJ_ber_ACE_MT_R): $(PROJ_SRC_DIR)/backends/ber/ber.ACE.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES_ACE) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.ACE.c


endif # ACE
########################################
# Backend library - 'pantheios.bec.syslog.a'

$(LIB_BEC_SYSLOG_MT_D): $(OBJ_BEC_SYSLOG_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOG_MT_D)

$(OBJ_bec_SYSLOG_MT_D): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c

$(LIB_BEC_SYSLOG_MT_R): $(OBJ_BEC_SYSLOG_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOG_MT_R)

$(OBJ_bec_SYSLOG_MT_R): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c

$(LIB_BEC_SYSLOG_ST_D): $(OBJ_BEC_SYSLOG_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOG_ST_D)

$(OBJ_bec_SYSLOG_ST_D): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c

$(LIB_BEC_SYSLOG_ST_R): $(OBJ_BEC_SYSLOG_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOG_ST_R)

$(OBJ_bec_SYSLOG_ST_R): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c


########################################
# Backend library - 'pantheios.bec.syslog.WithCallback.a'

$(LIB_BEC_SYSLOGWC_MT_D): $(OBJ_BEC_SYSLOGWC_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOGWC_MT_D)

$(OBJ_bec_SYSLOG_WC_MT_D): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -DPANTHEIOS_BE_USE_CALLBACK -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c

$(LIB_BEC_SYSLOGWC_MT_R): $(OBJ_BEC_SYSLOGWC_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOGWC_MT_R)

$(OBJ_bec_SYSLOG_WC_MT_R): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -DPANTHEIOS_BE_USE_CALLBACK -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c

$(LIB_BEC_SYSLOGWC_ST_D): $(OBJ_BEC_SYSLOGWC_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOGWC_ST_D)

$(OBJ_bec_SYSLOG_WC_ST_D): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -DPANTHEIOS_BE_USE_CALLBACK -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c

$(LIB_BEC_SYSLOGWC_ST_R): $(OBJ_BEC_SYSLOGWC_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BEC_SYSLOGWC_ST_R)

$(OBJ_bec_SYSLOG_WC_ST_R): $(PROJ_SRC_DIR)/backends/bec.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -DPANTHEIOS_BE_USE_CALLBACK -o $@ $(PROJ_SRC_DIR)/backends/bec.syslog.c


########################################
# Backend library - 'pantheios.be.syslog.a'

$(LIB_BE_SYSLOG_MT_D): $(OBJ_BE_SYSLOG_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_SYSLOG_MT_D)

$(OBJ_be_SYSLOG_MT_D): $(PROJ_SRC_DIR)/backends/be/be.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.syslog.c

$(LIB_BE_SYSLOG_MT_R): $(OBJ_BE_SYSLOG_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_SYSLOG_MT_R)

$(OBJ_be_SYSLOG_MT_R): $(PROJ_SRC_DIR)/backends/be/be.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.syslog.c

$(LIB_BE_SYSLOG_ST_D): $(OBJ_BE_SYSLOG_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_SYSLOG_ST_D)

$(OBJ_be_SYSLOG_ST_D): $(PROJ_SRC_DIR)/backends/be/be.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.syslog.c

$(LIB_BE_SYSLOG_ST_R): $(OBJ_BE_SYSLOG_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BE_SYSLOG_ST_R)

$(OBJ_be_SYSLOG_ST_R): $(PROJ_SRC_DIR)/backends/be/be.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/be/be.syslog.c


########################################
# Backend library - 'pantheios.ber.syslog.a'

$(LIB_BER_SYSLOG_MT_D): $(OBJ_BER_SYSLOG_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_SYSLOG_MT_D)

$(OBJ_ber_SYSLOG_MT_D): $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c

$(LIB_BER_SYSLOG_MT_R): $(OBJ_BER_SYSLOG_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_SYSLOG_MT_R)

$(OBJ_ber_SYSLOG_MT_R): $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c

$(LIB_BER_SYSLOG_ST_D): $(OBJ_BER_SYSLOG_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_SYSLOG_ST_D)

$(OBJ_ber_SYSLOG_ST_D): $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c

$(LIB_BER_SYSLOG_ST_R): $(OBJ_BER_SYSLOG_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_BER_SYSLOG_ST_R)

$(OBJ_ber_SYSLOG_ST_R): $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c $(BE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/backends/ber/ber.syslog.c


########################################
# Front-end library - 'pantheios.fe.all.a'

$(LIB_FE_ALL_MT_D): $(OBJ_FE_ALL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_MT_D)

$(OBJ_fe_all_MT_D): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

$(LIB_FE_ALL_MT_R): $(OBJ_FE_ALL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_MT_R)

$(OBJ_fe_all_MT_R): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

$(LIB_FE_ALL_ST_D): $(OBJ_FE_ALL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_ST_D)

$(OBJ_fe_all_ST_D): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

$(LIB_FE_ALL_ST_R): $(OBJ_FE_ALL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_ST_R)

$(OBJ_fe_all_ST_R): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

########################################
# Front-end library - 'pantheios.fe.all.WithCallback.a'

$(LIB_FE_ALL_WC_MT_D): $(OBJ_FE_ALL_WC_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_WC_MT_D)

$(OBJ_fe_all_WC_MT_D): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

$(LIB_FE_ALL_WC_MT_R): $(OBJ_FE_ALL_WC_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_WC_MT_R)

$(OBJ_fe_all_WC_MT_R): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

$(LIB_FE_ALL_WC_ST_D): $(OBJ_FE_ALL_WC_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_WC_ST_D)

$(OBJ_fe_all_WC_ST_D): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

$(LIB_FE_ALL_WC_ST_R): $(OBJ_FE_ALL_WC_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_ALL_WC_ST_R)

$(OBJ_fe_all_WC_ST_R): $(PROJ_SRC_DIR)/frontends/fe.all.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.all.c

########################################
# Front-end library - 'pantheios.fe.fail.a'

$(LIB_FE_FAIL_MT_D): $(OBJ_FE_FAIL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_FAIL_MT_D)

$(OBJ_fe_fail_MT_D): $(PROJ_SRC_DIR)/frontends/fe.fail.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.fail.c

$(LIB_FE_FAIL_MT_R): $(OBJ_FE_FAIL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_FAIL_MT_R)

$(OBJ_fe_fail_MT_R): $(PROJ_SRC_DIR)/frontends/fe.fail.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.fail.c

$(LIB_FE_FAIL_ST_D): $(OBJ_FE_FAIL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_FAIL_ST_D)

$(OBJ_fe_fail_ST_D): $(PROJ_SRC_DIR)/frontends/fe.fail.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.fail.c

$(LIB_FE_FAIL_ST_R): $(OBJ_FE_FAIL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_FAIL_ST_R)

$(OBJ_fe_fail_ST_R): $(PROJ_SRC_DIR)/frontends/fe.fail.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.fail.c

########################################
# Front-end library - 'pantheios.fe.N.a'

$(LIB_FE_N_MT_D): $(OBJ_FE_N_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_MT_D)

$(OBJ_fe_N_MT_D): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

$(LIB_FE_N_MT_R): $(OBJ_FE_N_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_MT_R)

$(OBJ_fe_N_MT_R): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

$(LIB_FE_N_ST_D): $(OBJ_FE_N_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_ST_D)

$(OBJ_fe_N_ST_D): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

$(LIB_FE_N_ST_R): $(OBJ_FE_N_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_ST_R)

$(OBJ_fe_N_ST_R): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

########################################
# Front-end library - 'pantheios.fe.N.WithCallback.a'

$(LIB_FE_N_WC_MT_D): $(OBJ_FE_N_WC_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_WC_MT_D)

$(OBJ_fe_N_WC_MT_D): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

$(LIB_FE_N_WC_MT_R): $(OBJ_FE_N_WC_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_WC_MT_R)

$(OBJ_fe_N_WC_MT_R): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

$(LIB_FE_N_WC_ST_D): $(OBJ_FE_N_WC_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_WC_ST_D)

$(OBJ_fe_N_WC_ST_D): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

$(LIB_FE_N_WC_ST_R): $(OBJ_FE_N_WC_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_N_WC_ST_R)

$(OBJ_fe_N_WC_ST_R): $(PROJ_SRC_DIR)/frontends/fe.N.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.N.c

########################################
# Front-end library - 'pantheios.fe.null.a'

$(LIB_FE_NULL_MT_D): $(OBJ_FE_NULL_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_MT_D)

$(OBJ_fe_null_MT_D): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

$(LIB_FE_NULL_MT_R): $(OBJ_FE_NULL_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_MT_R)

$(OBJ_fe_null_MT_R): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

$(LIB_FE_NULL_ST_D): $(OBJ_FE_NULL_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_ST_D)

$(OBJ_fe_null_ST_D): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

$(LIB_FE_NULL_ST_R): $(OBJ_FE_NULL_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_ST_R)

$(OBJ_fe_null_ST_R): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

########################################
# Front-end library - 'pantheios.fe.null.WithCallback.a'

$(LIB_FE_NULL_WC_MT_D): $(OBJ_FE_NULL_WC_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_WC_MT_D)

$(OBJ_fe_null_WC_MT_D): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

$(LIB_FE_NULL_WC_MT_R): $(OBJ_FE_NULL_WC_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_WC_MT_R)

$(OBJ_fe_null_WC_MT_R): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

$(LIB_FE_NULL_WC_ST_D): $(OBJ_FE_NULL_WC_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_WC_ST_D)

$(OBJ_fe_null_WC_ST_D): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

$(LIB_FE_NULL_WC_ST_R): $(OBJ_FE_NULL_WC_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_NULL_WC_ST_R)

$(OBJ_fe_null_WC_ST_R): $(PROJ_SRC_DIR)/frontends/fe.null.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.null.c

########################################
# Front-end library - 'pantheios.fe.simple.a'

$(LIB_FE_SIMPLE_MT_D): $(OBJ_FE_SIMPLE_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_MT_D)

$(OBJ_fe_simple_MT_D): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

$(LIB_FE_SIMPLE_MT_R): $(OBJ_FE_SIMPLE_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_MT_R)

$(OBJ_fe_simple_MT_R): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

$(LIB_FE_SIMPLE_ST_D): $(OBJ_FE_SIMPLE_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_ST_D)

$(OBJ_fe_simple_ST_D): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

$(LIB_FE_SIMPLE_ST_R): $(OBJ_FE_SIMPLE_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_ST_R)

$(OBJ_fe_simple_ST_R): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

########################################
# Front-end library - 'pantheios.fe.simple.WithCallback.a'

$(LIB_FE_SIMPLE_WC_MT_D): $(OBJ_FE_SIMPLE_WC_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_WC_MT_D)

$(OBJ_fe_simple_WC_MT_D): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

$(LIB_FE_SIMPLE_WC_MT_R): $(OBJ_FE_SIMPLE_WC_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_WC_MT_R)

$(OBJ_fe_simple_WC_MT_R): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_MT_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

$(LIB_FE_SIMPLE_WC_ST_D): $(OBJ_FE_SIMPLE_WC_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_WC_ST_D)

$(OBJ_fe_simple_WC_ST_D): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_D) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

$(LIB_FE_SIMPLE_WC_ST_R): $(OBJ_FE_SIMPLE_WC_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_FE_SIMPLE_WC_ST_R)

$(OBJ_fe_simple_WC_ST_R): $(PROJ_SRC_DIR)/frontends/fe.simple.c $(FE_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_LIB_ARGS_ST_R) -DPANTHEIOS_BE_USE_CALLBACK $(CC_INCLUDES) -o $@ $(PROJ_SRC_DIR)/frontends/fe.simple.c

########################################
# b64 library - 'b64.X.pan.core.a'

$(LIB_B64_MT_D): $(OBJ_B64_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_B64_MT_D)

$(OBJ_b64_b64_MT_D): $(B64_SRC_DIR)/b64.c $(B64_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_B64_ARGS_MT_D) $(CC_B64_INCLUDES_MT_D) -o $@ $(B64_SRC_DIR)/b64.c

$(LIB_B64_MT_R): $(OBJ_B64_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_B64_MT_R)

$(OBJ_b64_b64_MT_R): $(B64_SRC_DIR)/b64.c $(B64_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_B64_ARGS_MT_R) $(CC_B64_INCLUDES_MT_R) -o $@ $(B64_SRC_DIR)/b64.c

$(LIB_B64_ST_D): $(OBJ_B64_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_B64_ST_D)

$(OBJ_b64_b64_ST_D): $(B64_SRC_DIR)/b64.c $(B64_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_B64_ARGS_ST_D) $(CC_B64_INCLUDES_ST_D) -o $@ $(B64_SRC_DIR)/b64.c

$(LIB_B64_ST_R): $(OBJ_B64_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_B64_ST_R)

$(OBJ_b64_b64_ST_R): $(B64_SRC_DIR)/b64.c $(B64_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_B64_ARGS_ST_R) $(CC_B64_INCLUDES_ST_R) -o $@ $(B64_SRC_DIR)/b64.c

########################################
# shwild library - 'shwild.X.pan.core.a'

$(LIB_SHWILD_MT_D): $(OBJ_SHWILD_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_SHWILD_MT_D)

$(OBJ_shw_api_MT_D): $(SHWILD_SRC_DIR)/api.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_MT_D) $(CPPC_SHWILD_INCLUDES_MT_D) -o $@ $(SHWILD_SRC_DIR)/api.cpp
$(OBJ_shw_matches_MT_D): $(SHWILD_SRC_DIR)/matches.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_MT_D) $(CPPC_SHWILD_INCLUDES_MT_D) -o $@ $(SHWILD_SRC_DIR)/matches.cpp
$(OBJ_shw_pattern_MT_D): $(SHWILD_SRC_DIR)/pattern.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_MT_D) $(CPPC_SHWILD_INCLUDES_MT_D) -o $@ $(SHWILD_SRC_DIR)/pattern.cpp

$(LIB_SHWILD_MT_R): $(OBJ_SHWILD_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_SHWILD_MT_R)

$(OBJ_shw_api_MT_R): $(SHWILD_SRC_DIR)/api.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_MT_R) $(CPPC_SHWILD_INCLUDES_MT_R) -o $@ $(SHWILD_SRC_DIR)/api.cpp
$(OBJ_shw_matches_MT_R): $(SHWILD_SRC_DIR)/matches.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_MT_R) $(CPPC_SHWILD_INCLUDES_MT_R) -o $@ $(SHWILD_SRC_DIR)/matches.cpp
$(OBJ_shw_pattern_MT_R): $(SHWILD_SRC_DIR)/pattern.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_MT_R) $(CPPC_SHWILD_INCLUDES_MT_R) -o $@ $(SHWILD_SRC_DIR)/pattern.cpp

$(LIB_SHWILD_ST_D): $(OBJ_SHWILD_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_SHWILD_ST_D)

$(OBJ_shw_api_ST_D): $(SHWILD_SRC_DIR)/api.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_ST_D) $(CPPC_SHWILD_INCLUDES_ST_D) -o $@ $(SHWILD_SRC_DIR)/api.cpp
$(OBJ_shw_matches_ST_D): $(SHWILD_SRC_DIR)/matches.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_ST_D) $(CPPC_SHWILD_INCLUDES_ST_D) -o $@ $(SHWILD_SRC_DIR)/matches.cpp
$(OBJ_shw_pattern_ST_D): $(SHWILD_SRC_DIR)/pattern.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_ST_D) $(CPPC_SHWILD_INCLUDES_ST_D) -o $@ $(SHWILD_SRC_DIR)/pattern.cpp

$(LIB_SHWILD_ST_R): $(OBJ_SHWILD_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_SHWILD_ST_R)

$(OBJ_shw_api_ST_R): $(SHWILD_SRC_DIR)/api.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_ST_R) $(CPPC_SHWILD_INCLUDES_ST_R) -o $@ $(SHWILD_SRC_DIR)/api.cpp
$(OBJ_shw_matches_ST_R): $(SHWILD_SRC_DIR)/matches.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_ST_R) $(CPPC_SHWILD_INCLUDES_ST_R) -o $@ $(SHWILD_SRC_DIR)/matches.cpp
$(OBJ_shw_pattern_ST_R): $(SHWILD_SRC_DIR)/pattern.cpp $(SHWILD_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_SHWILD_ARGS_ST_R) $(CPPC_SHWILD_INCLUDES_ST_R) -o $@ $(SHWILD_SRC_DIR)/pattern.cpp

########################################
# xTests library - 'xtests.X.pan.core.a'

$(LIB_XTESTS_MT_D): $(OBJ_XTESTS_MT_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_XTESTS_MT_D)

$(OBJ_xts_core_MT_D): $(XTESTS_SRC_DIR)/xtests.core.cpp $(XTESTS_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_XTESTS_ARGS_MT_D) $(CPPC_XTESTS_INCLUDES_MT_D) -o $@ $(XTESTS_SRC_DIR)/xtests.core.cpp

$(LIB_XTESTS_MT_R): $(OBJ_XTESTS_MT_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_XTESTS_MT_R)

$(OBJ_xts_core_MT_R): $(XTESTS_SRC_DIR)/xtests.core.cpp $(XTESTS_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_XTESTS_ARGS_MT_R) $(CPPC_XTESTS_INCLUDES_MT_R) -o $@ $(XTESTS_SRC_DIR)/xtests.core.cpp

$(LIB_XTESTS_ST_D): $(OBJ_XTESTS_ST_D) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_XTESTS_ST_D)

$(OBJ_xts_core_ST_D): $(XTESTS_SRC_DIR)/xtests.core.cpp $(XTESTS_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_XTESTS_ARGS_ST_D) $(CPPC_XTESTS_INCLUDES_ST_D) -o $@ $(XTESTS_SRC_DIR)/xtests.core.cpp

$(LIB_XTESTS_ST_R): $(OBJ_XTESTS_ST_R) $(PROJ_MAKEFILE_NAME)
	@-mkdir -p $(PROJ_LIB_DIR)
	$(AR) $(AR_ARGS) rcs $@ $(OBJ_XTESTS_ST_R)

$(OBJ_xts_core_ST_R): $(XTESTS_SRC_DIR)/xtests.core.cpp $(XTESTS_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_XTESTS_ARGS_ST_R) $(CPPC_XTESTS_INCLUDES_ST_R) -o $@ $(XTESTS_SRC_DIR)/xtests.core.cpp

##################################################
# Examples

#  object files

$(OBJ_EX_P_be_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp
$(OBJ_EX_P_be_02_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp
$(OBJ_EX_P_be_03_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp
$(OBJ_EX_P_be_04_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp
$(OBJ_EX_P_be_05_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp
$(OBJ_EX_P_be_06_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp
$(OBJ_EX_P_cp_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp
$(OBJ_EX_P_cp_02_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp
$(OBJ_EX_P_fm_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp
$(OBJ_EX_P_fm_02_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp
$(OBJ_EX_P_fm_03_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp
$(OBJ_EX_P_fe_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp
$(OBJ_EX_P_in_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp
$(OBJ_EX_P_in_02_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp
$(OBJ_EX_P_in_03_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp
$(OBJ_EX_P_in_04_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp
$(OBJ_EX_P_in_05_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp
$(OBJ_EX_P_in_06_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp
$(OBJ_EX_P_in_07_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp
$(OBJ_EX_P_in_08_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp
$(OBJ_EX_P_in_09_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp
$(OBJ_EX_P_in_10_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp
$(OBJ_EX_P_in_11_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp
$(OBJ_EX_P_in_12_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp
$(OBJ_EX_P_in_13_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp
$(OBJ_EX_P_in_14_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp
$(OBJ_EX_P_in_15_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp
$(OBJ_EX_P_in_16_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp
$(OBJ_EX_P_in_17_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp
$(OBJ_EX_P_ms_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp
$(OBJ_EX_P_ms_02_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp
$(OBJ_EX_P_ms_03_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp
$(OBJ_EX_P_ms_04_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp
$(OBJ_EX_P_ms_05_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp
$(OBJ_EX_P_ms_06_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp
$(OBJ_EX_P_tr_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp
$(OBJ_EX_P_tr_02_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp
$(OBJ_EX_P_ut_01_MT_D):	$(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp
$(OBJ_EX_C_0_MT_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c
$(OBJ_EX_C_1_MT_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c
$(OBJ_EX_C_2_MT_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c
$(OBJ_EX_C_3_MT_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c
$(OBJ_EX_C_4_MT_D):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c
$(OBJ_EX_C_5_MT_D):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c
$(OBJ_EX_C_6_MT_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c
$(OBJ_EX_C_7_MT_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c

$(OBJ_EX_P_be_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp
$(OBJ_EX_P_be_02_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp
$(OBJ_EX_P_be_03_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp
$(OBJ_EX_P_be_04_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp
$(OBJ_EX_P_be_05_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp
$(OBJ_EX_P_be_06_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp
$(OBJ_EX_P_cp_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp
$(OBJ_EX_P_cp_02_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp
$(OBJ_EX_P_fm_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp
$(OBJ_EX_P_fm_02_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp
$(OBJ_EX_P_fm_03_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp
$(OBJ_EX_P_fe_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp
$(OBJ_EX_P_in_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp
$(OBJ_EX_P_in_02_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp
$(OBJ_EX_P_in_03_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp
$(OBJ_EX_P_in_04_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp
$(OBJ_EX_P_in_05_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp
$(OBJ_EX_P_in_06_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp
$(OBJ_EX_P_in_07_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp
$(OBJ_EX_P_in_08_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp
$(OBJ_EX_P_in_09_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp
$(OBJ_EX_P_in_10_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp
$(OBJ_EX_P_in_11_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp
$(OBJ_EX_P_in_12_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp
$(OBJ_EX_P_in_13_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp
$(OBJ_EX_P_in_14_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp
$(OBJ_EX_P_in_15_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp
$(OBJ_EX_P_in_16_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp
$(OBJ_EX_P_in_17_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp
$(OBJ_EX_P_ms_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp
$(OBJ_EX_P_ms_02_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp
$(OBJ_EX_P_ms_03_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp
$(OBJ_EX_P_ms_04_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp
$(OBJ_EX_P_ms_05_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp
$(OBJ_EX_P_ms_06_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp
$(OBJ_EX_P_tr_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp
$(OBJ_EX_P_tr_02_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp
$(OBJ_EX_P_ut_01_MT_R):	$(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp
$(OBJ_EX_C_0_MT_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c
$(OBJ_EX_C_1_MT_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c
$(OBJ_EX_C_2_MT_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c
$(OBJ_EX_C_3_MT_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c
$(OBJ_EX_C_4_MT_R):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c
$(OBJ_EX_C_5_MT_R):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c
$(OBJ_EX_C_6_MT_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c
$(OBJ_EX_C_7_MT_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c

$(OBJ_EX_P_be_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp
$(OBJ_EX_P_be_02_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp
$(OBJ_EX_P_be_03_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp
$(OBJ_EX_P_be_04_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp
$(OBJ_EX_P_be_05_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp
$(OBJ_EX_P_be_06_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp
$(OBJ_EX_P_cp_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp
$(OBJ_EX_P_cp_02_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp
$(OBJ_EX_P_fm_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp
$(OBJ_EX_P_fm_02_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp
$(OBJ_EX_P_fm_03_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp
$(OBJ_EX_P_fe_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp
$(OBJ_EX_P_in_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp
$(OBJ_EX_P_in_02_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp
$(OBJ_EX_P_in_03_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp
$(OBJ_EX_P_in_04_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp
$(OBJ_EX_P_in_05_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp
$(OBJ_EX_P_in_06_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp
$(OBJ_EX_P_in_07_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp
$(OBJ_EX_P_in_08_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp
$(OBJ_EX_P_in_09_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp
$(OBJ_EX_P_in_10_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp
$(OBJ_EX_P_in_11_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp
$(OBJ_EX_P_in_12_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp
$(OBJ_EX_P_in_13_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp
$(OBJ_EX_P_in_14_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp
$(OBJ_EX_P_in_15_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp
$(OBJ_EX_P_in_16_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp
$(OBJ_EX_P_in_17_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp
$(OBJ_EX_P_ms_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp
$(OBJ_EX_P_ms_02_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp
$(OBJ_EX_P_ms_03_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp
$(OBJ_EX_P_ms_04_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp
$(OBJ_EX_P_ms_05_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp
$(OBJ_EX_P_ms_06_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp
$(OBJ_EX_P_tr_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp
$(OBJ_EX_P_tr_02_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp
$(OBJ_EX_P_ut_01_ST_D):	$(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp
$(OBJ_EX_C_0_ST_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c
$(OBJ_EX_C_1_ST_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c
$(OBJ_EX_C_2_ST_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c
$(OBJ_EX_C_3_ST_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c
$(OBJ_EX_C_4_ST_D):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c
$(OBJ_EX_C_5_ST_D):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c
$(OBJ_EX_C_6_ST_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c
$(OBJ_EX_C_7_ST_D):	$(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c

$(OBJ_EX_P_be_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.callback/example.cpp.backends.callback.cpp
$(OBJ_EX_P_be_02_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file/example.cpp.backends.file.cpp
$(OBJ_EX_P_be_03_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.callback/example.cpp.backends.file.callback.cpp
$(OBJ_EX_P_be_04_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.file.lrsplit/example.cpp.backends.file.lrsplit.cpp
$(OBJ_EX_P_be_05_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.1/example.cpp.backends.mx.1.cpp
$(OBJ_EX_P_be_06_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/backends/example.cpp.backends.mx.2/example.cpp.backends.mx.2.cpp
$(OBJ_EX_P_cp_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/example.cpp.contract.PANTHEIOS_ASSERT.cpp
$(OBJ_EX_P_cp_02_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/contract/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT/example.cpp.contract.PANTHEIOS_MESSAGE_ASSERT.cpp
$(OBJ_EX_P_fm_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.highres/example.cpp.format.highres.cpp
$(OBJ_EX_P_fm_02_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.lowres/example.cpp.format.lowres.cpp
$(OBJ_EX_P_fm_03_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/format/example.cpp.format.pad/example.cpp.format.pad.cpp
$(OBJ_EX_P_fe_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/frontends/example.cpp.frontends.custom/example.cpp.frontends.custom.cpp
$(OBJ_EX_P_in_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.args/example.cpp.inserter.args.cpp
$(OBJ_EX_P_in_02_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b/example.cpp.inserter.b.cpp
$(OBJ_EX_P_in_03_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.b64/example.cpp.inserter.b64.cpp
$(OBJ_EX_P_in_04_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.blob/example.cpp.inserter.blob.cpp
$(OBJ_EX_P_in_05_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.character/example.cpp.inserter.character.cpp
$(OBJ_EX_P_in_06_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hex_ptr/example.cpp.inserter.hex_ptr.cpp
$(OBJ_EX_P_in_07_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.hostid/example.cpp.inserter.hostid.cpp
$(OBJ_EX_P_in_08_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.i/example.cpp.inserter.i.cpp
$(OBJ_EX_P_in_09_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.integer/example.cpp.inserter.integer.cpp
$(OBJ_EX_P_in_10_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.p/example.cpp.inserter.p.cpp
$(OBJ_EX_P_in_11_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.pointer/example.cpp.inserter.pointer.cpp
$(OBJ_EX_P_in_12_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.processid/example.cpp.inserter.processid.cpp
$(OBJ_EX_P_in_13_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.real/example.cpp.inserter.real.cpp
$(OBJ_EX_P_in_14_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.threadid/example.cpp.inserter.threadid.cpp
$(OBJ_EX_P_in_15_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xi/example.cpp.inserter.xi.cpp
$(OBJ_EX_P_in_16_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.xp/example.cpp.inserter.xp.cpp
$(OBJ_EX_P_in_17_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/inserters/example.cpp.inserter.w2m/example.cpp.inserter.w2m.cpp
$(OBJ_EX_P_ms_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.101/example.cpp.misc.101.cpp
$(OBJ_EX_P_ms_02_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp
$(OBJ_EX_P_ms_03_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.hetero1/example.cpp.misc.hetero1.cpp
$(OBJ_EX_P_ms_04_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.no_namespace/example.cpp.misc.no_namespace.cpp
$(OBJ_EX_P_ms_05_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.strings/example.cpp.misc.strings.cpp
$(OBJ_EX_P_ms_06_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/misc/example.cpp.misc.extended_severity_information/example.cpp.misc.extended_severity_information.cpp
$(OBJ_EX_P_tr_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.standard/example.cpp.tracing.standard.cpp
$(OBJ_EX_P_tr_02_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/tracing/example.cpp.tracing.with_function/example.cpp.tracing.with_function.cpp
$(OBJ_EX_P_ut_01_ST_R):	$(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_EXAMPLE_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/cpp/util/example.cpp.util.strdup/example.cpp.util.strdup.cpp
$(OBJ_EX_C_0_ST_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.101/example.c.101.c
$(OBJ_EX_C_1_ST_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.log_n/example.c.log_n.c
$(OBJ_EX_C_2_ST_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.core.pantheios_logprintf/example.c.core.pantheios_logprintf.c
$(OBJ_EX_C_3_ST_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.N/example.c.N.c
$(OBJ_EX_C_4_ST_R):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.gethostname/example.c.util.gethostname.c
$(OBJ_EX_C_5_ST_R):	$(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/util/example.c.util.getcurrenttime/example.c.util.getcurrenttime.c
$(OBJ_EX_C_6_ST_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.assert/example.c.assert.c
$(OBJ_EX_C_7_ST_R):	$(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c $(TEST_C_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_EXAMPLE_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_EXAMPLES_DIR)/c/example.c.extended_severity/example.c.extended_severity.c


# executables

$(TARG_EX_P_be_01_MT_D): \
			$(OBJ_EX_P_be_01_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_be_01_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_SYSLOG_MT_D)\
		$(LIB_BEC_SYSLOG_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_be_02_MT_D): \
			$(OBJ_EX_P_be_02_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FILE_MT_D)\
			$(LIB_BEC_FILE_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_be_02_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FILE_MT_D)\
		$(LIB_BEC_FILE_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_be_03_MT_D): \
			$(OBJ_EX_P_be_03_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FILE_MT_D)\
			$(LIB_BEC_FILE_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_be_03_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FILE_MT_D)\
		$(LIB_BEC_FILE_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_be_04_MT_D): \
			$(OBJ_EX_P_be_04_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FILE_MT_D)\
			$(LIB_BEC_FILE_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_be_04_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FILE_MT_D)\
		$(LIB_BEC_FILE_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_be_05_MT_D): \
			$(OBJ_EX_P_be_05_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_N_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_BEC_SYSLOG_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_be_05_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_N_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_BEC_SYSLOG_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_be_06_MT_D): \
			$(OBJ_EX_P_be_06_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_N_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_BEC_SYSLOG_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_be_06_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_N_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_BEC_SYSLOG_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_cp_01_MT_D): \
			$(OBJ_EX_P_cp_01_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_cp_01_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_cp_02_MT_D): \
			$(OBJ_EX_P_cp_02_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_cp_02_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_ms_06_MT_D): \
			$(OBJ_EX_P_ms_06_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_ms_06_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_fm_01_MT_D): \
			$(OBJ_EX_P_fm_01_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_fm_01_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_fm_02_MT_D): \
			$(OBJ_EX_P_fm_02_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_fm_02_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_fm_03_MT_D): \
			$(OBJ_EX_P_fm_03_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_fm_03_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_fe_01_MT_D): \
			$(OBJ_EX_P_fe_01_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_fe_01_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_01_MT_D): \
			$(OBJ_EX_P_in_01_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_01_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_02_MT_D): \
			$(OBJ_EX_P_in_02_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_02_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_03_MT_D): \
			$(OBJ_EX_P_in_03_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_03_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_04_MT_D): \
			$(OBJ_EX_P_in_04_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_04_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_05_MT_D): \
			$(OBJ_EX_P_in_05_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_05_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_06_MT_D): \
			$(OBJ_EX_P_in_06_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_06_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_07_MT_D): \
			$(OBJ_EX_P_in_07_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_07_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_08_MT_D): \
			$(OBJ_EX_P_in_08_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_08_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_09_MT_D): \
			$(OBJ_EX_P_in_09_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_09_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_10_MT_D): \
			$(OBJ_EX_P_in_10_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_10_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_11_MT_D): \
			$(OBJ_EX_P_in_11_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_11_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_12_MT_D): \
			$(OBJ_EX_P_in_12_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_12_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_13_MT_D): \
			$(OBJ_EX_P_in_13_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_13_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_14_MT_D): \
			$(OBJ_EX_P_in_14_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_14_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_15_MT_D): \
			$(OBJ_EX_P_in_15_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_15_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_16_MT_D): \
			$(OBJ_EX_P_in_16_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_16_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_in_17_MT_D): \
			$(OBJ_EX_P_in_17_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_in_17_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_ms_01_MT_D): \
			$(OBJ_EX_P_ms_01_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_ms_01_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_ms_02_MT_D): \
			$(OBJ_EX_P_ms_02_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_ms_02_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_ms_03_MT_D): \
			$(OBJ_EX_P_ms_03_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_ms_03_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_ms_04_MT_D): \
			$(OBJ_EX_P_ms_04_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_ms_04_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_ms_05_MT_D): \
			$(OBJ_EX_P_ms_05_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_ms_05_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_tr_01_MT_D): \
			$(OBJ_EX_P_tr_01_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_tr_01_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_tr_02_MT_D): \
			$(OBJ_EX_P_tr_02_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_tr_02_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_ut_01_MT_D): \
			$(OBJ_EX_P_ut_01_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_P_ut_01_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_0_MT_D): \
			$(OBJ_EX_C_0_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_0_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_1_MT_D): \
			$(OBJ_EX_C_1_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_1_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_2_MT_D): \
			$(OBJ_EX_C_2_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_2_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_3_MT_D): \
			$(OBJ_EX_C_3_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_N_MT_D)\
			$(LIB_BE_N_MT_D)\
			$(LIB_BEC_FILE_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_BEC_NULL_MT_D)\
			$(LIB_BEC_SYSLOG_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_3_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_N_MT_D)\
		$(LIB_BE_N_MT_D)\
		$(LIB_BEC_FILE_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_BEC_NULL_MT_D)\
		$(LIB_BEC_SYSLOG_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_4_MT_D): \
			$(OBJ_EX_C_4_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_4_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_5_MT_D): \
			$(OBJ_EX_C_5_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_5_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_6_MT_D): \
			$(OBJ_EX_C_6_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_6_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_C_7_MT_D): \
			$(OBJ_EX_C_7_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_EX_C_7_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_EX_P_be_01_MT_R): \
			$(OBJ_EX_P_be_01_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_be_01_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_SYSLOG_MT_R)\
		$(LIB_BEC_SYSLOG_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_be_02_MT_R): \
			$(OBJ_EX_P_be_02_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FILE_MT_R)\
			$(LIB_BEC_FILE_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_be_02_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FILE_MT_R)\
		$(LIB_BEC_FILE_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_be_03_MT_R): \
			$(OBJ_EX_P_be_03_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FILE_MT_R)\
			$(LIB_BEC_FILE_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_be_03_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FILE_MT_R)\
		$(LIB_BEC_FILE_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_be_04_MT_R): \
			$(OBJ_EX_P_be_04_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FILE_MT_R)\
			$(LIB_BEC_FILE_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_be_04_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FILE_MT_R)\
		$(LIB_BEC_FILE_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_be_05_MT_R): \
			$(OBJ_EX_P_be_05_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_N_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_BEC_SYSLOG_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_be_05_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_N_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_BEC_SYSLOG_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_be_06_MT_R): \
			$(OBJ_EX_P_be_06_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_N_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_BEC_SYSLOG_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_be_06_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_N_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_BEC_SYSLOG_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_cp_01_MT_R): \
			$(OBJ_EX_P_cp_01_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_cp_01_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_cp_02_MT_R): \
			$(OBJ_EX_P_cp_02_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_cp_02_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_ms_06_MT_R): \
			$(OBJ_EX_P_ms_06_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_ms_06_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_fm_01_MT_R): \
			$(OBJ_EX_P_fm_01_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_fm_01_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_fm_02_MT_R): \
			$(OBJ_EX_P_fm_02_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_fm_02_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_fm_03_MT_R): \
			$(OBJ_EX_P_fm_03_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_fm_03_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_fe_01_MT_R): \
			$(OBJ_EX_P_fe_01_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_fe_01_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_01_MT_R): \
			$(OBJ_EX_P_in_01_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_01_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_02_MT_R): \
			$(OBJ_EX_P_in_02_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_02_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_03_MT_R): \
			$(OBJ_EX_P_in_03_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_03_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_04_MT_R): \
			$(OBJ_EX_P_in_04_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_04_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_05_MT_R): \
			$(OBJ_EX_P_in_05_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_05_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_06_MT_R): \
			$(OBJ_EX_P_in_06_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_06_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_07_MT_R): \
			$(OBJ_EX_P_in_07_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_07_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_08_MT_R): \
			$(OBJ_EX_P_in_08_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_08_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_09_MT_R): \
			$(OBJ_EX_P_in_09_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_09_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_10_MT_R): \
			$(OBJ_EX_P_in_10_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_10_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_11_MT_R): \
			$(OBJ_EX_P_in_11_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_11_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_12_MT_R): \
			$(OBJ_EX_P_in_12_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_12_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_13_MT_R): \
			$(OBJ_EX_P_in_13_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_13_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_14_MT_R): \
			$(OBJ_EX_P_in_14_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_14_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_15_MT_R): \
			$(OBJ_EX_P_in_15_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_15_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_16_MT_R): \
			$(OBJ_EX_P_in_16_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_16_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_in_17_MT_R): \
			$(OBJ_EX_P_in_17_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_in_17_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_ms_01_MT_R): \
			$(OBJ_EX_P_ms_01_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_ms_01_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_ms_02_MT_R): \
			$(OBJ_EX_P_ms_02_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_ms_02_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_ms_03_MT_R): \
			$(OBJ_EX_P_ms_03_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_ms_03_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_ms_04_MT_R): \
			$(OBJ_EX_P_ms_04_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_ms_04_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_ms_05_MT_R): \
			$(OBJ_EX_P_ms_05_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_ms_05_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_tr_01_MT_R): \
			$(OBJ_EX_P_tr_01_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_tr_01_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_tr_02_MT_R): \
			$(OBJ_EX_P_tr_02_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_tr_02_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_ut_01_MT_R): \
			$(OBJ_EX_P_ut_01_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_P_ut_01_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_0_MT_R): \
			$(OBJ_EX_C_0_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_0_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_1_MT_R): \
			$(OBJ_EX_C_1_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_1_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_2_MT_R): \
			$(OBJ_EX_C_2_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_2_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_3_MT_R): \
			$(OBJ_EX_C_3_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_N_MT_R)\
			$(LIB_BE_N_MT_R)\
			$(LIB_BEC_FILE_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_BEC_NULL_MT_R)\
			$(LIB_BEC_SYSLOG_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_3_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_N_MT_R)\
		$(LIB_BE_N_MT_R)\
		$(LIB_BEC_FILE_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_BEC_NULL_MT_R)\
		$(LIB_BEC_SYSLOG_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_4_MT_R): \
			$(OBJ_EX_C_4_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_4_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_5_MT_R): \
			$(OBJ_EX_C_5_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_5_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_6_MT_R): \
			$(OBJ_EX_C_6_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_6_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_C_7_MT_R): \
			$(OBJ_EX_C_7_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_EX_C_7_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_EX_P_be_01_ST_D): \
			$(OBJ_EX_P_be_01_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_be_01_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_SYSLOG_ST_D)\
		$(LIB_BEC_SYSLOG_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_be_02_ST_D): \
			$(OBJ_EX_P_be_02_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FILE_ST_D)\
			$(LIB_BEC_FILE_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_be_02_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FILE_ST_D)\
		$(LIB_BEC_FILE_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_be_03_ST_D): \
			$(OBJ_EX_P_be_03_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FILE_ST_D)\
			$(LIB_BEC_FILE_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_be_03_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FILE_ST_D)\
		$(LIB_BEC_FILE_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_be_04_ST_D): \
			$(OBJ_EX_P_be_04_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FILE_ST_D)\
			$(LIB_BEC_FILE_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_be_04_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FILE_ST_D)\
		$(LIB_BEC_FILE_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_be_05_ST_D): \
			$(OBJ_EX_P_be_05_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_N_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_BEC_SYSLOG_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_be_05_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_N_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_BEC_SYSLOG_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_be_06_ST_D): \
			$(OBJ_EX_P_be_06_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_N_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_BEC_SYSLOG_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_be_06_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_N_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_BEC_SYSLOG_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_cp_01_ST_D): \
			$(OBJ_EX_P_cp_01_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_cp_01_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_cp_02_ST_D): \
			$(OBJ_EX_P_cp_02_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_cp_02_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_ms_06_ST_D): \
			$(OBJ_EX_P_ms_06_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_ms_06_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_fm_01_ST_D): \
			$(OBJ_EX_P_fm_01_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_fm_01_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_fm_02_ST_D): \
			$(OBJ_EX_P_fm_02_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_fm_02_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_fm_03_ST_D): \
			$(OBJ_EX_P_fm_03_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_fm_03_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_fe_01_ST_D): \
			$(OBJ_EX_P_fe_01_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_fe_01_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_01_ST_D): \
			$(OBJ_EX_P_in_01_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_01_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_02_ST_D): \
			$(OBJ_EX_P_in_02_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_02_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_03_ST_D): \
			$(OBJ_EX_P_in_03_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_03_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_04_ST_D): \
			$(OBJ_EX_P_in_04_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_04_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_05_ST_D): \
			$(OBJ_EX_P_in_05_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_05_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_06_ST_D): \
			$(OBJ_EX_P_in_06_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_06_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_07_ST_D): \
			$(OBJ_EX_P_in_07_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_07_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_08_ST_D): \
			$(OBJ_EX_P_in_08_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_08_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_09_ST_D): \
			$(OBJ_EX_P_in_09_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_09_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_10_ST_D): \
			$(OBJ_EX_P_in_10_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_10_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_11_ST_D): \
			$(OBJ_EX_P_in_11_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_11_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_12_ST_D): \
			$(OBJ_EX_P_in_12_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_12_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_13_ST_D): \
			$(OBJ_EX_P_in_13_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_13_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_14_ST_D): \
			$(OBJ_EX_P_in_14_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_14_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_15_ST_D): \
			$(OBJ_EX_P_in_15_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_15_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_16_ST_D): \
			$(OBJ_EX_P_in_16_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_16_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_in_17_ST_D): \
			$(OBJ_EX_P_in_17_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_in_17_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_ms_01_ST_D): \
			$(OBJ_EX_P_ms_01_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_ms_01_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_ms_02_ST_D): \
			$(OBJ_EX_P_ms_02_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_ms_02_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_ms_03_ST_D): \
			$(OBJ_EX_P_ms_03_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_ms_03_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_ms_04_ST_D): \
			$(OBJ_EX_P_ms_04_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_ms_04_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_ms_05_ST_D): \
			$(OBJ_EX_P_ms_05_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_ms_05_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_tr_01_ST_D): \
			$(OBJ_EX_P_tr_01_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_tr_01_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_tr_02_ST_D): \
			$(OBJ_EX_P_tr_02_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_tr_02_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_ut_01_ST_D): \
			$(OBJ_EX_P_ut_01_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_P_ut_01_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_0_ST_D): \
			$(OBJ_EX_C_0_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_0_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_1_ST_D): \
			$(OBJ_EX_C_1_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_1_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_2_ST_D): \
			$(OBJ_EX_C_2_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_2_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_3_ST_D): \
			$(OBJ_EX_C_3_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_N_ST_D)\
			$(LIB_BE_N_ST_D)\
			$(LIB_BEC_FILE_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_BEC_NULL_ST_D)\
			$(LIB_BEC_SYSLOG_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_3_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_N_ST_D)\
		$(LIB_BE_N_ST_D)\
		$(LIB_BEC_FILE_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_BEC_NULL_ST_D)\
		$(LIB_BEC_SYSLOG_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_4_ST_D): \
			$(OBJ_EX_C_4_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_4_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_5_ST_D): \
			$(OBJ_EX_C_5_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_5_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_6_ST_D): \
			$(OBJ_EX_C_6_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_6_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_C_7_ST_D): \
			$(OBJ_EX_C_7_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_EX_C_7_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_EX_P_be_01_ST_R): \
			$(OBJ_EX_P_be_01_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_be_01_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_SYSLOG_ST_R)\
		$(LIB_BEC_SYSLOG_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_be_02_ST_R): \
			$(OBJ_EX_P_be_02_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FILE_ST_R)\
			$(LIB_BEC_FILE_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_be_02_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FILE_ST_R)\
		$(LIB_BEC_FILE_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_be_03_ST_R): \
			$(OBJ_EX_P_be_03_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FILE_ST_R)\
			$(LIB_BEC_FILE_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_be_03_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FILE_ST_R)\
		$(LIB_BEC_FILE_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_be_04_ST_R): \
			$(OBJ_EX_P_be_04_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FILE_ST_R)\
			$(LIB_BEC_FILE_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_be_04_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FILE_ST_R)\
		$(LIB_BEC_FILE_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_be_05_ST_R): \
			$(OBJ_EX_P_be_05_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_N_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_BEC_SYSLOG_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_be_05_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_N_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_BEC_SYSLOG_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_be_06_ST_R): \
			$(OBJ_EX_P_be_06_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_N_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_BEC_SYSLOG_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_be_06_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_N_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_BEC_SYSLOG_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_cp_01_ST_R): \
			$(OBJ_EX_P_cp_01_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_cp_01_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_cp_02_ST_R): \
			$(OBJ_EX_P_cp_02_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_cp_02_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_ms_06_ST_R): \
			$(OBJ_EX_P_ms_06_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_ms_06_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_fm_01_ST_R): \
			$(OBJ_EX_P_fm_01_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_fm_01_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_fm_02_ST_R): \
			$(OBJ_EX_P_fm_02_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_fm_02_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_fm_03_ST_R): \
			$(OBJ_EX_P_fm_03_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_fm_03_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_fe_01_ST_R): \
			$(OBJ_EX_P_fe_01_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_fe_01_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_01_ST_R): \
			$(OBJ_EX_P_in_01_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_01_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_02_ST_R): \
			$(OBJ_EX_P_in_02_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_02_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_03_ST_R): \
			$(OBJ_EX_P_in_03_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_03_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_04_ST_R): \
			$(OBJ_EX_P_in_04_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_04_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_05_ST_R): \
			$(OBJ_EX_P_in_05_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_05_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_06_ST_R): \
			$(OBJ_EX_P_in_06_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_06_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_07_ST_R): \
			$(OBJ_EX_P_in_07_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_07_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_08_ST_R): \
			$(OBJ_EX_P_in_08_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_08_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_09_ST_R): \
			$(OBJ_EX_P_in_09_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_09_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_10_ST_R): \
			$(OBJ_EX_P_in_10_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_10_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_11_ST_R): \
			$(OBJ_EX_P_in_11_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_11_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_12_ST_R): \
			$(OBJ_EX_P_in_12_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_12_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_13_ST_R): \
			$(OBJ_EX_P_in_13_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_13_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_14_ST_R): \
			$(OBJ_EX_P_in_14_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_14_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_15_ST_R): \
			$(OBJ_EX_P_in_15_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_15_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_16_ST_R): \
			$(OBJ_EX_P_in_16_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_16_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_in_17_ST_R): \
			$(OBJ_EX_P_in_17_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_in_17_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_ms_01_ST_R): \
			$(OBJ_EX_P_ms_01_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_ms_01_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_ms_02_ST_R): \
			$(OBJ_EX_P_ms_02_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_ms_02_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_ms_03_ST_R): \
			$(OBJ_EX_P_ms_03_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_ms_03_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_ms_04_ST_R): \
			$(OBJ_EX_P_ms_04_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_ms_04_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_ms_05_ST_R): \
			$(OBJ_EX_P_ms_05_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_ms_05_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_tr_01_ST_R): \
			$(OBJ_EX_P_tr_01_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_tr_01_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_tr_02_ST_R): \
			$(OBJ_EX_P_tr_02_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_tr_02_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_P_ut_01_ST_R): \
			$(OBJ_EX_P_ut_01_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_P_ut_01_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_0_ST_R): \
			$(OBJ_EX_C_0_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_0_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_1_ST_R): \
			$(OBJ_EX_C_1_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_1_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_2_ST_R): \
			$(OBJ_EX_C_2_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_2_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_3_ST_R): \
			$(OBJ_EX_C_3_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_N_ST_R)\
			$(LIB_BE_N_ST_R)\
			$(LIB_BEC_FILE_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_BEC_NULL_ST_R)\
			$(LIB_BEC_SYSLOG_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_3_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_N_ST_R)\
		$(LIB_BE_N_ST_R)\
		$(LIB_BEC_FILE_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_BEC_NULL_ST_R)\
		$(LIB_BEC_SYSLOG_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_4_ST_R): \
			$(OBJ_EX_C_4_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_4_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_5_ST_R): \
			$(OBJ_EX_C_5_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_5_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_6_ST_R): \
			$(OBJ_EX_C_6_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_6_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_EX_C_7_ST_R): \
			$(OBJ_EX_C_7_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_EX_C_7_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)


##################################################
# Tests

##############################
# Component Tests

$(TARG_TEST_COMP_core_initseq_MT_D):	\
			$(OBJ_TEST_COMP_core_initseq_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_core_initseq_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_core_initseq_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c

$(TARG_TEST_COMP_core_initseq_MT_R):	\
			$(OBJ_TEST_COMP_core_initseq_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_core_initseq_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_core_initseq_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c

$(TARG_TEST_COMP_core_initseq_ST_D):	\
			$(OBJ_TEST_COMP_core_initseq_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_core_initseq_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_core_initseq_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c

$(TARG_TEST_COMP_core_initseq_ST_R):	\
			$(OBJ_TEST_COMP_core_initseq_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_core_initseq_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_core_initseq_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.initialisation_sequence/test.component.core.initialisation_sequence.c

$(TARG_TEST_COMP_custsev_MT_D):	\
			$(OBJ_TEST_COMP_custsev_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_custsev_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_custsev_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp

$(TARG_TEST_COMP_custsev_MT_R):	\
			$(OBJ_TEST_COMP_custsev_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_custsev_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_custsev_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp

$(TARG_TEST_COMP_custsev_ST_D):	\
			$(OBJ_TEST_COMP_custsev_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_custsev_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_custsev_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp

$(TARG_TEST_COMP_custsev_ST_R):	\
			$(OBJ_TEST_COMP_custsev_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_custsev_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_custsev_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.custom_severity/test.component.custom_severity.cpp

$(TARG_TEST_COMP_filemt_MT_D):	\
			$(OBJ_TEST_COMP_filemt_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_N_MT_D)\
			$(LIB_BE_N_MT_D)\
			$(LIB_BEC_FILE_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_filemt_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_N_MT_D)\
		$(LIB_BE_N_MT_D)\
		$(LIB_BEC_FILE_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_filemt_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.be.file.threading/test.component.be.file.threading.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.be.file.threading/test.component.be.file.threading.cpp

$(TARG_TEST_COMP_filemt_MT_R):	\
			$(OBJ_TEST_COMP_filemt_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_N_MT_R)\
			$(LIB_BE_N_MT_R)\
			$(LIB_BEC_FILE_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_filemt_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_N_MT_R)\
		$(LIB_BE_N_MT_R)\
		$(LIB_BEC_FILE_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_filemt_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.be.file.threading/test.component.be.file.threading.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.be.file.threading/test.component.be.file.threading.cpp

$(TARG_TEST_COMP_ins_args_MT_D):	\
			$(OBJ_TEST_COMP_ins_args_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_args_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_args_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp

$(TARG_TEST_COMP_ins_args_MT_R):	\
			$(OBJ_TEST_COMP_ins_args_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_args_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_args_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp

$(TARG_TEST_COMP_ins_args_ST_D):	\
			$(OBJ_TEST_COMP_ins_args_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_args_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_args_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp

$(TARG_TEST_COMP_ins_args_ST_R):	\
			$(OBJ_TEST_COMP_ins_args_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_args_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_args_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.args/test.component.inserters.args.cpp

$(TARG_TEST_COMP_ins_b64_MT_D):	\
			$(OBJ_TEST_COMP_ins_b64_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_B64_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_b64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_B64_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_b64_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp

$(TARG_TEST_COMP_ins_b64_MT_R):	\
			$(OBJ_TEST_COMP_ins_b64_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_B64_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_b64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_B64_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_b64_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp

$(TARG_TEST_COMP_ins_b64_ST_D):	\
			$(OBJ_TEST_COMP_ins_b64_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_B64_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_b64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_B64_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_b64_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp

$(TARG_TEST_COMP_ins_b64_ST_R):	\
			$(OBJ_TEST_COMP_ins_b64_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_B64_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_b64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_B64_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_b64_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.b64/test.component.inserters.b64.cpp

$(TARG_TEST_COMP_ins_bool_MT_D):	\
			$(OBJ_TEST_COMP_ins_bool_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_bool_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_bool_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp

$(TARG_TEST_COMP_ins_bool_MT_R):	\
			$(OBJ_TEST_COMP_ins_bool_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_bool_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_bool_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp

$(TARG_TEST_COMP_ins_bool_ST_D):	\
			$(OBJ_TEST_COMP_ins_bool_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_bool_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_bool_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp

$(TARG_TEST_COMP_ins_bool_ST_R):	\
			$(OBJ_TEST_COMP_ins_bool_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_bool_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_bool_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.boolean/test.component.inserters.boolean.cpp

$(TARG_TEST_COMP_ins_hostid_MT_D):	\
			$(OBJ_TEST_COMP_ins_hostid_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_hostid_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_hostid_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp

$(TARG_TEST_COMP_ins_hostid_MT_R):	\
			$(OBJ_TEST_COMP_ins_hostid_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_hostid_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_hostid_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp

$(TARG_TEST_COMP_ins_hostid_ST_D):	\
			$(OBJ_TEST_COMP_ins_hostid_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_hostid_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_hostid_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp

$(TARG_TEST_COMP_ins_hostid_ST_R):	\
			$(OBJ_TEST_COMP_ins_hostid_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_hostid_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_hostid_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.hostId/test.component.inserters.hostId.cpp

$(TARG_TEST_COMP_ins_int_MT_D):	\
			$(OBJ_TEST_COMP_ins_int_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_int_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_int_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp

$(TARG_TEST_COMP_ins_int_MT_R):	\
			$(OBJ_TEST_COMP_ins_int_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_int_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_int_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp

$(TARG_TEST_COMP_ins_int_ST_D):	\
			$(OBJ_TEST_COMP_ins_int_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_int_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_int_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp

$(TARG_TEST_COMP_ins_int_ST_R):	\
			$(OBJ_TEST_COMP_ins_int_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_int_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_int_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.integer/test.component.inserters.integer.cpp

$(TARG_TEST_COMP_ins_pad_MT_D):	\
			$(OBJ_TEST_COMP_ins_pad_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_pad_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_pad_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp

$(TARG_TEST_COMP_ins_pad_MT_R):	\
			$(OBJ_TEST_COMP_ins_pad_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_pad_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_pad_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp

$(TARG_TEST_COMP_ins_pad_ST_D):	\
			$(OBJ_TEST_COMP_ins_pad_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_pad_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_pad_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp

$(TARG_TEST_COMP_ins_pad_ST_R):	\
			$(OBJ_TEST_COMP_ins_pad_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_pad_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_pad_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pad/test.component.inserters.pad.cpp

$(TARG_TEST_COMP_ins_ptr_MT_D):	\
			$(OBJ_TEST_COMP_ins_ptr_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_ptr_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_ptr_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp

$(TARG_TEST_COMP_ins_ptr_MT_R):	\
			$(OBJ_TEST_COMP_ins_ptr_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_ptr_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_ptr_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp

$(TARG_TEST_COMP_ins_ptr_ST_D):	\
			$(OBJ_TEST_COMP_ins_ptr_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_ptr_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_ptr_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp

$(TARG_TEST_COMP_ins_ptr_ST_R):	\
			$(OBJ_TEST_COMP_ins_ptr_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_ptr_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_ptr_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.pointer/test.component.inserters.pointer.cpp

$(TARG_TEST_COMP_ins_prid_MT_D):	\
			$(OBJ_TEST_COMP_ins_prid_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_prid_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_prid_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp

$(TARG_TEST_COMP_ins_prid_MT_R):	\
			$(OBJ_TEST_COMP_ins_prid_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_prid_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_prid_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp

$(TARG_TEST_COMP_ins_prid_ST_D):	\
			$(OBJ_TEST_COMP_ins_prid_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_prid_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_prid_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp

$(TARG_TEST_COMP_ins_prid_ST_R):	\
			$(OBJ_TEST_COMP_ins_prid_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_prid_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_prid_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.processId/test.component.inserters.processId.cpp

$(TARG_TEST_COMP_ins_real_MT_D):	\
			$(OBJ_TEST_COMP_ins_real_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_real_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_real_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp

$(TARG_TEST_COMP_ins_real_MT_R):	\
			$(OBJ_TEST_COMP_ins_real_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_real_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_real_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp

$(TARG_TEST_COMP_ins_real_ST_D):	\
			$(OBJ_TEST_COMP_ins_real_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_real_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_real_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp

$(TARG_TEST_COMP_ins_real_ST_R):	\
			$(OBJ_TEST_COMP_ins_real_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_real_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_real_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.real/test.component.inserters.real.cpp

$(TARG_TEST_COMP_ins_slice_MT_D):	\
			$(OBJ_TEST_COMP_ins_slice_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_slice_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_slice_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp

$(TARG_TEST_COMP_ins_slice_MT_R):	\
			$(OBJ_TEST_COMP_ins_slice_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_slice_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_slice_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp

$(TARG_TEST_COMP_ins_slice_ST_D):	\
			$(OBJ_TEST_COMP_ins_slice_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_slice_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_slice_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp

$(TARG_TEST_COMP_ins_slice_ST_R):	\
			$(OBJ_TEST_COMP_ins_slice_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_slice_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_slice_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.slice/test.component.inserters.slice.cpp

$(TARG_TEST_COMP_ins_thrid_MT_D):	\
			$(OBJ_TEST_COMP_ins_thrid_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_thrid_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_thrid_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp

$(TARG_TEST_COMP_ins_thrid_MT_R):	\
			$(OBJ_TEST_COMP_ins_thrid_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_thrid_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_thrid_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp

$(TARG_TEST_COMP_ins_thrid_ST_D):	\
			$(OBJ_TEST_COMP_ins_thrid_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_thrid_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_thrid_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp

$(TARG_TEST_COMP_ins_thrid_ST_R):	\
			$(OBJ_TEST_COMP_ins_thrid_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_thrid_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_thrid_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.threadId/test.component.inserters.threadId.cpp

$(TARG_TEST_COMP_ins_w2m_MT_D):	\
			$(OBJ_TEST_COMP_ins_w2m_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_ins_w2m_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_ins_w2m_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp

$(TARG_TEST_COMP_ins_w2m_MT_R):	\
			$(OBJ_TEST_COMP_ins_w2m_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_ins_w2m_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_ins_w2m_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp

$(TARG_TEST_COMP_ins_w2m_ST_D):	\
			$(OBJ_TEST_COMP_ins_w2m_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_ins_w2m_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_ins_w2m_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp

$(TARG_TEST_COMP_ins_w2m_ST_R):	\
			$(OBJ_TEST_COMP_ins_w2m_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_ins_w2m_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_ins_w2m_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.inserters.w2m/test.component.inserters.w2m.cpp

$(TARG_TEST_COMP_log_1_MT_D):	\
			$(OBJ_TEST_COMP_log_1_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_log_1_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_log_1_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp

$(TARG_TEST_COMP_log_1_MT_R):	\
			$(OBJ_TEST_COMP_log_1_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_log_1_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_log_1_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp

$(TARG_TEST_COMP_log_1_ST_D):	\
			$(OBJ_TEST_COMP_log_1_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_log_1_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_log_1_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp

$(TARG_TEST_COMP_log_1_ST_R):	\
			$(OBJ_TEST_COMP_log_1_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_log_1_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_log_1_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.log.1/test.component.log.1.cpp

$(TARG_TEST_COMP_trace_1_MT_D):	\
			$(OBJ_TEST_COMP_trace_1_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_trace_1_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_trace_1_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp

$(TARG_TEST_COMP_trace_1_MT_R):	\
			$(OBJ_TEST_COMP_trace_1_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_trace_1_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_trace_1_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp

$(TARG_TEST_COMP_trace_1_ST_D):	\
			$(OBJ_TEST_COMP_trace_1_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_trace_1_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_trace_1_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp

$(TARG_TEST_COMP_trace_1_ST_R):	\
			$(OBJ_TEST_COMP_trace_1_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_trace_1_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_trace_1_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.trace.1/test.component.trace.1.cpp

$(TARG_TEST_COMP_pan_puts_MT_D):	\
			$(OBJ_TEST_COMP_pan_puts_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_pan_puts_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_pan_puts_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp

$(TARG_TEST_COMP_pan_puts_MT_R):	\
			$(OBJ_TEST_COMP_pan_puts_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_pan_puts_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_pan_puts_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp

$(TARG_TEST_COMP_pan_puts_ST_D):	\
			$(OBJ_TEST_COMP_pan_puts_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_pan_puts_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_pan_puts_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp

$(TARG_TEST_COMP_pan_puts_ST_R):	\
			$(OBJ_TEST_COMP_pan_puts_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_pan_puts_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_pan_puts_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logputs/test.component.core.pantheios_logputs.cpp

$(TARG_TEST_COMP_pan_prtf_MT_D):	\
			$(OBJ_TEST_COMP_pan_prtf_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_BE_TEST_MT_D)\
			$(LIB_BEC_TEST_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_COMP_pan_prtf_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_BE_TEST_MT_D)\
		$(LIB_BEC_TEST_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_COMP_pan_prtf_MT_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp

$(TARG_TEST_COMP_pan_prtf_MT_R):	\
			$(OBJ_TEST_COMP_pan_prtf_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_BE_TEST_MT_R)\
			$(LIB_BEC_TEST_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_COMP_pan_prtf_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_BE_TEST_MT_R)\
		$(LIB_BEC_TEST_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_COMP_pan_prtf_MT_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp

$(TARG_TEST_COMP_pan_prtf_ST_D):	\
			$(OBJ_TEST_COMP_pan_prtf_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_BE_TEST_ST_D)\
			$(LIB_BEC_TEST_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_COMP_pan_prtf_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_BE_TEST_ST_D)\
		$(LIB_BEC_TEST_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_COMP_pan_prtf_ST_D):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp

$(TARG_TEST_COMP_pan_prtf_ST_R):	\
			$(OBJ_TEST_COMP_pan_prtf_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_BE_TEST_ST_R)\
			$(LIB_BEC_TEST_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_COMP_pan_prtf_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_BE_TEST_ST_R)\
		$(LIB_BEC_TEST_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_COMP_pan_prtf_ST_R):	$(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_COMPONENT_DIR)/test.component.core.pantheios_logprintf/test.component.core.pantheios_logprintf.cpp

##############################
# Performance Tests

$(TARG_TEST_PERF_ins_w2m_MT_D):	\
			$(OBJ_TEST_PERF_ins_w2m_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_NULL_MT_D)\
			$(LIB_BEC_NULL_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_PERF_ins_w2m_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_NULL_MT_D)\
		$(LIB_BEC_NULL_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_PERF_ins_w2m_MT_D):	$(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp

$(TARG_TEST_PERF_ins_w2m_MT_R):	\
			$(OBJ_TEST_PERF_ins_w2m_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_NULL_MT_R)\
			$(LIB_BEC_NULL_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_PERF_ins_w2m_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_NULL_MT_R)\
		$(LIB_BEC_NULL_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_PERF_ins_w2m_MT_R):	$(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp

$(TARG_TEST_PERF_ins_w2m_ST_D):	\
			$(OBJ_TEST_PERF_ins_w2m_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_NULL_ST_D)\
			$(LIB_BEC_NULL_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_PERF_ins_w2m_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_NULL_ST_D)\
		$(LIB_BEC_NULL_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_PERF_ins_w2m_ST_D):	$(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp

$(TARG_TEST_PERF_ins_w2m_ST_R):	\
			$(OBJ_TEST_PERF_ins_w2m_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_NULL_ST_R)\
			$(LIB_BEC_NULL_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_PERF_ins_w2m_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_NULL_ST_R)\
		$(LIB_BEC_NULL_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_PERF_ins_w2m_ST_R):	$(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_PERFORMANCE_DIR)/test.performance.inserters.w2m/test.performance.inserters.w2m.cpp

##############################
# Scratch Tests

#  object files

$(OBJ_TEST_API_MT_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp
$(OBJ_TEST_API_MT_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp
$(OBJ_TEST_API_ST_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp
$(OBJ_TEST_API_ST_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.api/test.scratch.api.cpp

# executables

ifdef ACE

$(TEST_SCRT_ACE_MT_D):	\
			$(OBJ_SCRT_ACE_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_ACE_MT_D)\
			$(LIB_BEC_ACE_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ -L"$(ACE)/lib" -L"$(ACE)/ace"\
		$(OBJ_SCRT_ACE_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_ACE_MT_D)\
		$(LIB_BEC_ACE_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_ACE)\
		$(TARGET_LIB_MT_D)

$(OBJ_SCRT_ACE_MT_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.ACE/test.scratch.ACE.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_D) $(CPPC_INCLUDES_ACE) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.ACE/test.scratch.ACE.cpp
endif # ACE

$(TARG_TEST_MT_D_APIwBE_FPRINTF):	\
			$(OBJ_TEST_API_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_API_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_FPRINTF)\
		$(TARGET_LIB_MT_D)

$(TARG_TEST_SCRT_BE_SYSLOG_MT_D):	\
			$(OBJ_TEST_SCRT_BE_SYSLOG_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_SYSLOG_MT_D)\
			$(LIB_BEC_SYSLOG_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOG_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_SYSLOG_MT_D)\
		$(LIB_BEC_SYSLOG_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_SCRT_BE_SYSLOG_MT_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp

$(TARG_TEST_SCRT_BE_SYSLOGWC_MT_D):	\
			$(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_SYSLOG_MT_D)\
			$(LIB_BEC_SYSLOGWC_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_SYSLOG_MT_D)\
		$(LIB_BEC_SYSLOGWC_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp

$(TARG_TEST_MT_D_APIwBELR_1_FPRINTF_SYSLOG):	\
			$(OBJ_TEST_API_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_LRSPLIT_MT_D)\
			$(LIB_BEL_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_BER_SYSLOG_MT_D)\
			$(LIB_BEC_SYSLOG_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_API_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_LRSPLIT_MT_D)\
		$(LIB_BEL_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_BER_SYSLOG_MT_D)\
		$(LIB_BEC_SYSLOG_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(TARG_TEST_FE_MT_D): \
			$(OBJ_TEST_FE_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_FE_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_FE_MT_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp

$(TARG_TEST_BE_N_FAIL_MT_D):	\
			$(OBJ_TEST_BE_N_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_N_MT_D)\
			$(LIB_BE_N_MT_D)\
			$(LIB_BEC_FAIL_MT_D)\
			$(LIB_BEC_FILE_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_BE_N_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_N_MT_D)\
		$(LIB_BE_N_MT_D)\
		$(LIB_BEC_FAIL_MT_D)\
		$(LIB_BEC_FILE_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_BE_N_MT_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp


$(TARG_TEST_FE_SIMPLE_WC_MT_D):	\
			$(OBJ_TEST_FE_SIMPLE_WC_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_SIMPLE_WC_MT_D)\
			$(LIB_BE_FPRINTF_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_FE_SIMPLE_WC_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_FE_SIMPLE_WC_MT_D)\
		$(LIB_BE_FPRINTF_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_FE_SIMPLE_WC_MT_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp


ifdef ACE

$(TEST_SCRT_ACE_MT_R):	\
			$(OBJ_SCRT_ACE_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_ACE_MT_R)\
			$(LIB_BEC_ACE_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ -L"$(ACE)/lib" -L"$(ACE)/ace"\
		$(OBJ_SCRT_ACE_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_ACE_MT_R)\
		$(LIB_BEC_ACE_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_ACE)\
		$(TARGET_LIB_MT_R)

$(OBJ_SCRT_ACE_MT_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.ACE/test.scratch.ACE.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_R) $(CPPC_INCLUDES_ACE) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.ACE/test.scratch.ACE.cpp
endif # ACE

$(TARG_TEST_MT_R_APIwBE_FPRINTF):	\
			$(OBJ_TEST_API_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_API_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_FPRINTF)\
		$(TARGET_LIB_MT_R)

$(TARG_TEST_SCRT_BE_SYSLOG_MT_R):	\
			$(OBJ_TEST_SCRT_BE_SYSLOG_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_SYSLOG_MT_R)\
			$(LIB_BEC_SYSLOG_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOG_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_SYSLOG_MT_R)\
		$(LIB_BEC_SYSLOG_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_SCRT_BE_SYSLOG_MT_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp

$(TARG_TEST_SCRT_BE_SYSLOGWC_MT_R):	\
			$(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_SYSLOG_MT_R)\
			$(LIB_BEC_SYSLOGWC_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_SYSLOG_MT_R)\
		$(LIB_BEC_SYSLOGWC_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_SCRT_BE_SYSLOGWC_MT_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp

$(TARG_TEST_MT_R_APIwBELR_1_FPRINTF_SYSLOG):	\
			$(OBJ_TEST_API_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_LRSPLIT_MT_R)\
			$(LIB_BEL_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_BER_SYSLOG_MT_R)\
			$(LIB_BEC_SYSLOG_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_API_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_LRSPLIT_MT_R)\
		$(LIB_BEL_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_BER_SYSLOG_MT_R)\
		$(LIB_BEC_SYSLOG_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(TARG_TEST_FE_MT_R): \
			$(OBJ_TEST_FE_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_FE_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_FE_MT_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp

$(TARG_TEST_BE_N_FAIL_MT_R):	\
			$(OBJ_TEST_BE_N_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_N_MT_R)\
			$(LIB_BE_N_MT_R)\
			$(LIB_BEC_FAIL_MT_R)\
			$(LIB_BEC_FILE_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_BE_N_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_N_MT_R)\
		$(LIB_BE_N_MT_R)\
		$(LIB_BEC_FAIL_MT_R)\
		$(LIB_BEC_FILE_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_BE_N_MT_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp


$(TARG_TEST_FE_SIMPLE_WC_MT_R):	\
			$(OBJ_TEST_FE_SIMPLE_WC_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_SIMPLE_WC_MT_R)\
			$(LIB_BE_FPRINTF_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_FE_SIMPLE_WC_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_FE_SIMPLE_WC_MT_R)\
		$(LIB_BE_FPRINTF_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_FE_SIMPLE_WC_MT_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp



$(TARG_TEST_ST_D_APIwBE_FPRINTF):	\
			$(OBJ_TEST_API_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_API_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_FPRINTF)\
		$(TARGET_LIB_ST_D)

$(TARG_TEST_SCRT_BE_SYSLOG_ST_D):	\
			$(OBJ_TEST_SCRT_BE_SYSLOG_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_SYSLOG_ST_D)\
			$(LIB_BEC_SYSLOG_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOG_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_SYSLOG_ST_D)\
		$(LIB_BEC_SYSLOG_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_SCRT_BE_SYSLOG_ST_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp

$(TARG_TEST_SCRT_BE_SYSLOGWC_ST_D):	\
			$(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_SYSLOG_ST_D)\
			$(LIB_BEC_SYSLOGWC_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_SYSLOG_ST_D)\
		$(LIB_BEC_SYSLOGWC_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp

$(TARG_TEST_ST_D_APIwBELR_1_FPRINTF_SYSLOG):	\
			$(OBJ_TEST_API_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_LRSPLIT_ST_D)\
			$(LIB_BEL_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_BER_SYSLOG_ST_D)\
			$(LIB_BEC_SYSLOG_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_API_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_LRSPLIT_ST_D)\
		$(LIB_BEL_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_BER_SYSLOG_ST_D)\
		$(LIB_BEC_SYSLOG_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(TARG_TEST_FE_ST_D): \
			$(OBJ_TEST_FE_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_FE_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_FE_ST_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp

$(TARG_TEST_BE_N_FAIL_ST_D):	\
			$(OBJ_TEST_BE_N_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_N_ST_D)\
			$(LIB_BE_N_ST_D)\
			$(LIB_BEC_FAIL_ST_D)\
			$(LIB_BEC_FILE_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_BE_N_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_N_ST_D)\
		$(LIB_BE_N_ST_D)\
		$(LIB_BEC_FAIL_ST_D)\
		$(LIB_BEC_FILE_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_BE_N_ST_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp


$(TARG_TEST_FE_SIMPLE_WC_ST_D):	\
			$(OBJ_TEST_FE_SIMPLE_WC_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_SIMPLE_WC_ST_D)\
			$(LIB_BE_FPRINTF_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_FE_SIMPLE_WC_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_FE_SIMPLE_WC_ST_D)\
		$(LIB_BE_FPRINTF_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_FE_SIMPLE_WC_ST_D):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp



$(TARG_TEST_ST_R_APIwBE_FPRINTF):	\
			$(OBJ_TEST_API_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_API_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_FPRINTF)\
		$(TARGET_LIB_ST_R)

$(TARG_TEST_SCRT_BE_SYSLOG_ST_R):	\
			$(OBJ_TEST_SCRT_BE_SYSLOG_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_SYSLOG_ST_R)\
			$(LIB_BEC_SYSLOG_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOG_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_SYSLOG_ST_R)\
		$(LIB_BEC_SYSLOG_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_SCRT_BE_SYSLOG_ST_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog/test.scratch.be.syslog.cpp

$(TARG_TEST_SCRT_BE_SYSLOGWC_ST_R):	\
			$(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_SYSLOG_ST_R)\
			$(LIB_BEC_SYSLOGWC_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_SYSLOG_ST_R)\
		$(LIB_BEC_SYSLOGWC_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_SYSLOG)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_SCRT_BE_SYSLOGWC_ST_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.syslog.WithCallback/test.scratch.be.syslog.WithCallback.cpp

$(TARG_TEST_ST_R_APIwBELR_1_FPRINTF_SYSLOG):	\
			$(OBJ_TEST_API_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_LRSPLIT_ST_R)\
			$(LIB_BEL_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_BER_SYSLOG_ST_R)\
			$(LIB_BEC_SYSLOG_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_API_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_LRSPLIT_ST_R)\
		$(LIB_BEL_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_BER_SYSLOG_ST_R)\
		$(LIB_BEC_SYSLOG_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(TARG_TEST_FE_ST_R): \
			$(OBJ_TEST_FE_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_FE_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_FE_ST_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe/test.scratch.fe.cpp

$(TARG_TEST_BE_N_FAIL_ST_R):	\
			$(OBJ_TEST_BE_N_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_N_ST_R)\
			$(LIB_BE_N_ST_R)\
			$(LIB_BEC_FAIL_ST_R)\
			$(LIB_BEC_FILE_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_BE_N_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_N_ST_R)\
		$(LIB_BE_N_ST_R)\
		$(LIB_BEC_FAIL_ST_R)\
		$(LIB_BEC_FILE_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_BE_N_ST_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.be.N.fail/test.scratch.be.N.fail.cpp


$(TARG_TEST_FE_SIMPLE_WC_ST_R):	\
			$(OBJ_TEST_FE_SIMPLE_WC_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_SIMPLE_WC_ST_R)\
			$(LIB_BE_FPRINTF_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_FE_SIMPLE_WC_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_FE_SIMPLE_WC_ST_R)\
		$(LIB_BE_FPRINTF_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIBS_3PTY_BE_N_FAIL)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_FE_SIMPLE_WC_ST_R):	$(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_SCRATCH_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_SCRATCH_DIR)/test.scratch.fe.simple.WithCallback/test.scratch.fe.simple.WithCallback.cpp


##############################
# Unit Tests

$(TARG_TEST_UNIT_lvls_dyninit_MT_D):	\
			$(OBJ_TEST_UNIT_lvls_dyninit_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_lvls_dyninit_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_lvls_dyninit_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp

$(TARG_TEST_UNIT_lvls_dyninit_MT_R):	\
			$(OBJ_TEST_UNIT_lvls_dyninit_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_lvls_dyninit_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_lvls_dyninit_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp

$(TARG_TEST_UNIT_lvls_dyninit_ST_D):	\
			$(OBJ_TEST_UNIT_lvls_dyninit_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_lvls_dyninit_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_lvls_dyninit_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp

$(TARG_TEST_UNIT_lvls_dyninit_ST_R):	\
			$(OBJ_TEST_UNIT_lvls_dyninit_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_lvls_dyninit_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_lvls_dyninit_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.dynamic_initialisation/test.unit.levels.dynamic_initialisation.cpp

$(TARG_TEST_UNIT_lvls_vals_MT_D):	\
			$(OBJ_TEST_UNIT_lvls_vals_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_lvls_vals_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_lvls_vals_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c

$(TARG_TEST_UNIT_lvls_vals_MT_R):	\
			$(OBJ_TEST_UNIT_lvls_vals_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_lvls_vals_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_lvls_vals_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c

$(TARG_TEST_UNIT_lvls_vals_ST_D):	\
			$(OBJ_TEST_UNIT_lvls_vals_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_lvls_vals_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_lvls_vals_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c

$(TARG_TEST_UNIT_lvls_vals_ST_R):	\
			$(OBJ_TEST_UNIT_lvls_vals_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_lvls_vals_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_lvls_vals_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.levels.values/test.unit.levels.values.c

$(TARG_TEST_UNIT_getcurrtm_MT_D):	\
			$(OBJ_TEST_UNIT_getcurrtm_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_getcurrtm_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_getcurrtm_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp

$(TARG_TEST_UNIT_getcurrtm_MT_R):	\
			$(OBJ_TEST_UNIT_getcurrtm_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_getcurrtm_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_getcurrtm_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp

$(TARG_TEST_UNIT_getcurrtm_ST_D):	\
			$(OBJ_TEST_UNIT_getcurrtm_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_getcurrtm_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_getcurrtm_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp

$(TARG_TEST_UNIT_getcurrtm_ST_R):	\
			$(OBJ_TEST_UNIT_getcurrtm_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_getcurrtm_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_getcurrtm_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.getcurrenttime/test.unit.util.getcurrenttime.cpp

$(TARG_TEST_UNIT_getversion_MT_D):	\
			$(OBJ_TEST_UNIT_getversion_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_FAIL_MT_D)\
			$(LIB_BE_FAIL_MT_D)\
			$(LIB_BEC_FAIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_getversion_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_FE_FAIL_MT_D)\
		$(LIB_BE_FAIL_MT_D)\
		$(LIB_BEC_FAIL_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_getversion_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c

$(TARG_TEST_UNIT_getversion_MT_R):	\
			$(OBJ_TEST_UNIT_getversion_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_FAIL_MT_R)\
			$(LIB_BE_FAIL_MT_R)\
			$(LIB_BEC_FAIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_getversion_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_FE_FAIL_MT_R)\
		$(LIB_BE_FAIL_MT_R)\
		$(LIB_BEC_FAIL_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_getversion_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c

$(TARG_TEST_UNIT_getversion_ST_D):	\
			$(OBJ_TEST_UNIT_getversion_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_FAIL_ST_D)\
			$(LIB_BE_FAIL_ST_D)\
			$(LIB_BEC_FAIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_getversion_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_FE_FAIL_ST_D)\
		$(LIB_BE_FAIL_ST_D)\
		$(LIB_BEC_FAIL_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_getversion_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c

$(TARG_TEST_UNIT_getversion_ST_R):	\
			$(OBJ_TEST_UNIT_getversion_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_FAIL_ST_R)\
			$(LIB_BE_FAIL_ST_R)\
			$(LIB_BEC_FAIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_getversion_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_FE_FAIL_ST_R)\
		$(LIB_BE_FAIL_ST_R)\
		$(LIB_BEC_FAIL_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_getversion_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.getversion/test.unit.getversion.c

$(TARG_TEST_UNIT_gethostname_MT_D):	\
			$(OBJ_TEST_UNIT_gethostname_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_gethostname_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_gethostname_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp

$(TARG_TEST_UNIT_gethostname_MT_R):	\
			$(OBJ_TEST_UNIT_gethostname_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_gethostname_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_gethostname_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp

$(TARG_TEST_UNIT_gethostname_ST_D):	\
			$(OBJ_TEST_UNIT_gethostname_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_gethostname_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_gethostname_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp

$(TARG_TEST_UNIT_gethostname_ST_R):	\
			$(OBJ_TEST_UNIT_gethostname_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_gethostname_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_gethostname_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.gethostname/test.unit.util.gethostname.cpp

$(TARG_TEST_UNIT_onbailout_MT_D):	\
			$(OBJ_TEST_UNIT_onbailout_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_onbailout_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_onbailout_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp

$(TARG_TEST_UNIT_onbailout_MT_R):	\
			$(OBJ_TEST_UNIT_onbailout_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_onbailout_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_onbailout_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp

$(TARG_TEST_UNIT_onbailout_ST_D):	\
			$(OBJ_TEST_UNIT_onbailout_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_onbailout_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_onbailout_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp

$(TARG_TEST_UNIT_onbailout_ST_R):	\
			$(OBJ_TEST_UNIT_onbailout_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_onbailout_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_onbailout_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.onbailout/test.unit.util.onbailout.cpp

$(TARG_TEST_UNIT_strnlen_MT_D):	\
			$(OBJ_TEST_UNIT_strnlen_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_strnlen_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_strnlen_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp

$(TARG_TEST_UNIT_strnlen_MT_R):	\
			$(OBJ_TEST_UNIT_strnlen_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_strnlen_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_strnlen_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp

$(TARG_TEST_UNIT_strnlen_ST_D):	\
			$(OBJ_TEST_UNIT_strnlen_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_strnlen_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_strnlen_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp

$(TARG_TEST_UNIT_strnlen_ST_R):	\
			$(OBJ_TEST_UNIT_strnlen_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_strnlen_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_strnlen_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.util.strnlen/test.unit.util.strnlen.cpp

$(TARG_TEST_UNIT_bec_file_MT_D):	\
			$(OBJ_TEST_UNIT_bec_file_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_BEC_FILE_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_NULL_MT_D)\
			$(LIB_BEC_NULL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_bec_file_MT_D)\
		$(LIB_BEC_FILE_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_NULL_MT_D)\
		$(LIB_BEC_NULL_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_bec_file_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp

$(TARG_TEST_UNIT_bec_file_MT_R):	\
			$(OBJ_TEST_UNIT_bec_file_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_BEC_FILE_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_NULL_MT_R)\
			$(LIB_BEC_NULL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_bec_file_MT_R)\
		$(LIB_BEC_FILE_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_NULL_MT_R)\
		$(LIB_BEC_NULL_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_bec_file_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp

$(TARG_TEST_UNIT_bec_file_ST_D):	\
			$(OBJ_TEST_UNIT_bec_file_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_BEC_FILE_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_NULL_ST_D)\
			$(LIB_BEC_NULL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_bec_file_ST_D)\
		$(LIB_BEC_FILE_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_NULL_ST_D)\
		$(LIB_BEC_NULL_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_bec_file_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp

$(TARG_TEST_UNIT_bec_file_ST_R):	\
			$(OBJ_TEST_UNIT_bec_file_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_BEC_FILE_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_NULL_ST_R)\
			$(LIB_BEC_NULL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_bec_file_ST_R)\
		$(LIB_BEC_FILE_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_NULL_ST_R)\
		$(LIB_BEC_NULL_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_bec_file_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.file/test.unit.bec.file.cpp

$(TARG_TEST_UNIT_bec_fprintf_MT_D):	\
			$(OBJ_TEST_UNIT_bec_fprintf_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_BEC_FPRINTF_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_BE_NULL_MT_D)\
			$(LIB_BEC_NULL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_bec_fprintf_MT_D)\
		$(LIB_BEC_FPRINTF_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_BE_NULL_MT_D)\
		$(LIB_BEC_NULL_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_bec_fprintf_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp

$(TARG_TEST_UNIT_bec_fprintf_MT_R):	\
			$(OBJ_TEST_UNIT_bec_fprintf_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_BEC_FPRINTF_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_BE_NULL_MT_R)\
			$(LIB_BEC_NULL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_bec_fprintf_MT_R)\
		$(LIB_BEC_FPRINTF_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_BE_NULL_MT_R)\
		$(LIB_BEC_NULL_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_bec_fprintf_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp

$(TARG_TEST_UNIT_bec_fprintf_ST_D):	\
			$(OBJ_TEST_UNIT_bec_fprintf_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_BEC_FPRINTF_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_BE_NULL_ST_D)\
			$(LIB_BEC_NULL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_bec_fprintf_ST_D)\
		$(LIB_BEC_FPRINTF_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_BE_NULL_ST_D)\
		$(LIB_BEC_NULL_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_bec_fprintf_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp

$(TARG_TEST_UNIT_bec_fprintf_ST_R):	\
			$(OBJ_TEST_UNIT_bec_fprintf_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_BEC_FPRINTF_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_BE_NULL_ST_R)\
			$(LIB_BEC_NULL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_bec_fprintf_ST_R)\
		$(LIB_BEC_FPRINTF_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_BE_NULL_ST_R)\
		$(LIB_BEC_NULL_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_bec_fprintf_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fprintf/test.unit.bec.fprintf.cpp

$(TARG_TEST_UNIT_bec_fail_MT_D):	\
			$(OBJ_TEST_UNIT_bec_fail_MT_D)\
			$(LIB_BEC_FAIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_bec_fail_MT_D)\
		$(LIB_BEC_FAIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_bec_fail_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c

$(TARG_TEST_UNIT_bec_fail_MT_R):	\
			$(OBJ_TEST_UNIT_bec_fail_MT_R)\
			$(LIB_BEC_FAIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_bec_fail_MT_R)\
		$(LIB_BEC_FAIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_bec_fail_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c

$(TARG_TEST_UNIT_bec_fail_ST_D):	\
			$(OBJ_TEST_UNIT_bec_fail_ST_D)\
			$(LIB_BEC_FAIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_bec_fail_ST_D)\
		$(LIB_BEC_FAIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_bec_fail_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c

$(TARG_TEST_UNIT_bec_fail_ST_R):	\
			$(OBJ_TEST_UNIT_bec_fail_ST_R)\
			$(LIB_BEC_FAIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_bec_fail_ST_R)\
		$(LIB_BEC_FAIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_bec_fail_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.bec.fail/test.unit.bec.fail.c

$(TARG_TEST_UNIT_be_fail_MT_D):	\
			$(OBJ_TEST_UNIT_be_fail_MT_D)\
			$(LIB_BE_FAIL_MT_D)\
			$(LIB_BEC_FAIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_be_fail_MT_D)\
		$(LIB_BE_FAIL_MT_D)\
		$(LIB_BEC_FAIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_be_fail_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c

$(TARG_TEST_UNIT_be_fail_MT_R):	\
			$(OBJ_TEST_UNIT_be_fail_MT_R)\
			$(LIB_BE_FAIL_MT_R)\
			$(LIB_BEC_FAIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_be_fail_MT_R)\
		$(LIB_BE_FAIL_MT_R)\
		$(LIB_BEC_FAIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_be_fail_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c

$(TARG_TEST_UNIT_be_fail_ST_D):	\
			$(OBJ_TEST_UNIT_be_fail_ST_D)\
			$(LIB_BE_FAIL_ST_D)\
			$(LIB_BEC_FAIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_be_fail_ST_D)\
		$(LIB_BE_FAIL_ST_D)\
		$(LIB_BEC_FAIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_be_fail_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c

$(TARG_TEST_UNIT_be_fail_ST_R):	\
			$(OBJ_TEST_UNIT_be_fail_ST_R)\
			$(LIB_BE_FAIL_ST_R)\
			$(LIB_BEC_FAIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_be_fail_ST_R)\
		$(LIB_BE_FAIL_ST_R)\
		$(LIB_BEC_FAIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_be_fail_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.fail/test.unit.be.fail.c

#
$(TARG_TEST_UNIT_be_lrsplit_MT_D):	\
			$(OBJ_TEST_UNIT_be_lrsplit_MT_D)\
			$(LIB_BE_LRSPLIT_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_be_lrsplit_MT_D)\
		$(LIB_BE_LRSPLIT_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_be_lrsplit_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c

$(TARG_TEST_UNIT_be_lrsplit_MT_R):	\
			$(OBJ_TEST_UNIT_be_lrsplit_MT_R)\
			$(LIB_BE_LRSPLIT_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_be_lrsplit_MT_R)\
		$(LIB_BE_LRSPLIT_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_be_lrsplit_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c

$(TARG_TEST_UNIT_be_lrsplit_ST_D):	\
			$(OBJ_TEST_UNIT_be_lrsplit_ST_D)\
			$(LIB_BE_LRSPLIT_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_be_lrsplit_ST_D)\
		$(LIB_BE_LRSPLIT_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_be_lrsplit_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c

$(TARG_TEST_UNIT_be_lrsplit_ST_R):	\
			$(OBJ_TEST_UNIT_be_lrsplit_ST_R)\
			$(LIB_BE_LRSPLIT_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_be_lrsplit_ST_R)\
		$(LIB_BE_LRSPLIT_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_be_lrsplit_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.lrsplit/test.unit.be.lrsplit.c

$(TARG_TEST_UNIT_be_N_MT_D):	\
			$(OBJ_TEST_UNIT_be_N_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_BE_N_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_be_N_MT_D)\
		$(LIB_BE_N_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_be_N_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c

$(TARG_TEST_UNIT_be_N_MT_R):	\
			$(OBJ_TEST_UNIT_be_N_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_BE_N_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_be_N_MT_R)\
		$(LIB_BE_N_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_be_N_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c

$(TARG_TEST_UNIT_be_N_ST_D):	\
			$(OBJ_TEST_UNIT_be_N_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_BE_N_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_be_N_ST_D)\
		$(LIB_BE_N_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_be_N_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c

$(TARG_TEST_UNIT_be_N_ST_R):	\
			$(OBJ_TEST_UNIT_be_N_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_BE_N_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_be_N_ST_R)\
		$(LIB_BE_N_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_be_N_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.be.N/test.unit.be.N.c

$(TARG_TEST_UNIT_fe_all_MT_D):	\
			$(OBJ_TEST_UNIT_fe_all_MT_D)\
			$(LIB_FE_ALL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_all_MT_D)\
		$(LIB_FE_ALL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_fe_all_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c

$(TARG_TEST_UNIT_fe_all_MT_R):	\
			$(OBJ_TEST_UNIT_fe_all_MT_R)\
			$(LIB_FE_ALL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_all_MT_R)\
		$(LIB_FE_ALL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_fe_all_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c

$(TARG_TEST_UNIT_fe_all_ST_D):	\
			$(OBJ_TEST_UNIT_fe_all_ST_D)\
			$(LIB_FE_ALL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_all_ST_D)\
		$(LIB_FE_ALL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_fe_all_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c

$(TARG_TEST_UNIT_fe_all_ST_R):	\
			$(OBJ_TEST_UNIT_fe_all_ST_R)\
			$(LIB_FE_ALL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_all_ST_R)\
		$(LIB_FE_ALL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_fe_all_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.all/test.unit.fe.all.c

$(TARG_TEST_UNIT_fe_fail_MT_D):	\
			$(OBJ_TEST_UNIT_fe_fail_MT_D)\
			$(LIB_FE_FAIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_fail_MT_D)\
		$(LIB_FE_FAIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_fe_fail_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c

$(TARG_TEST_UNIT_fe_fail_MT_R):	\
			$(OBJ_TEST_UNIT_fe_fail_MT_R)\
			$(LIB_FE_FAIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_fail_MT_R)\
		$(LIB_FE_FAIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_fe_fail_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c

$(TARG_TEST_UNIT_fe_fail_ST_D):	\
			$(OBJ_TEST_UNIT_fe_fail_ST_D)\
			$(LIB_FE_FAIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_fail_ST_D)\
		$(LIB_FE_FAIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_fe_fail_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c

$(TARG_TEST_UNIT_fe_fail_ST_R):	\
			$(OBJ_TEST_UNIT_fe_fail_ST_R)\
			$(LIB_FE_FAIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_fail_ST_R)\
		$(LIB_FE_FAIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_fe_fail_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.fail/test.unit.fe.fail.c

$(TARG_TEST_UNIT_fe_N_MT_D):	\
			$(OBJ_TEST_UNIT_fe_N_MT_D)\
			$(LIB_FE_N_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_N_MT_D)\
		$(LIB_FE_N_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_fe_N_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c

$(TARG_TEST_UNIT_fe_N_MT_R):	\
			$(OBJ_TEST_UNIT_fe_N_MT_R)\
			$(LIB_FE_N_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_N_MT_R)\
		$(LIB_FE_N_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_fe_N_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c

$(TARG_TEST_UNIT_fe_N_ST_D):	\
			$(OBJ_TEST_UNIT_fe_N_ST_D)\
			$(LIB_FE_N_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_N_ST_D)\
		$(LIB_FE_N_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_fe_N_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c

$(TARG_TEST_UNIT_fe_N_ST_R):	\
			$(OBJ_TEST_UNIT_fe_N_ST_R)\
			$(LIB_FE_N_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_N_ST_R)\
		$(LIB_FE_N_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_fe_N_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.N/test.unit.fe.N.c

$(TARG_TEST_UNIT_fe_null_MT_D):	\
			$(OBJ_TEST_UNIT_fe_null_MT_D)\
			$(LIB_FE_NULL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_null_MT_D)\
		$(LIB_FE_NULL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_fe_null_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c

$(TARG_TEST_UNIT_fe_null_MT_R):	\
			$(OBJ_TEST_UNIT_fe_null_MT_R)\
			$(LIB_FE_NULL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_null_MT_R)\
		$(LIB_FE_NULL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_fe_null_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c

$(TARG_TEST_UNIT_fe_null_ST_D):	\
			$(OBJ_TEST_UNIT_fe_null_ST_D)\
			$(LIB_FE_NULL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_null_ST_D)\
		$(LIB_FE_NULL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_fe_null_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c

$(TARG_TEST_UNIT_fe_null_ST_R):	\
			$(OBJ_TEST_UNIT_fe_null_ST_R)\
			$(LIB_FE_NULL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_null_ST_R)\
		$(LIB_FE_NULL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_fe_null_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.null/test.unit.fe.null.c

$(TARG_TEST_UNIT_fe_simple_MT_D):	\
			$(OBJ_TEST_UNIT_fe_simple_MT_D)\
			$(LIB_FE_SIMPLE_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(LIB_SHWILD_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_simple_MT_D)\
		$(LIB_FE_SIMPLE_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIB_SHWILD_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_fe_simple_MT_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c

$(TARG_TEST_UNIT_fe_simple_MT_R):	\
			$(OBJ_TEST_UNIT_fe_simple_MT_R)\
			$(LIB_FE_SIMPLE_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(LIB_SHWILD_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_simple_MT_R)\
		$(LIB_FE_SIMPLE_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIB_SHWILD_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_fe_simple_MT_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_MT_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c

$(TARG_TEST_UNIT_fe_simple_ST_D):	\
			$(OBJ_TEST_UNIT_fe_simple_ST_D)\
			$(LIB_FE_SIMPLE_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(LIB_SHWILD_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_fe_simple_ST_D)\
		$(LIB_FE_SIMPLE_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIB_SHWILD_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_fe_simple_ST_D):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_D) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c

$(TARG_TEST_UNIT_fe_simple_ST_R):	\
			$(OBJ_TEST_UNIT_fe_simple_ST_R)\
			$(LIB_FE_SIMPLE_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(LIB_SHWILD_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_fe_simple_ST_R)\
		$(LIB_FE_SIMPLE_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIB_SHWILD_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_fe_simple_ST_R):	$(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CC) $(CC_TEST_UNIT_ARGS_ST_R) $(CC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/test.unit.fe.simple/test.unit.fe.simple.c

$(TARG_TEST_UNIT_ins_int_MT_D):	\
			$(OBJ_TEST_UNIT_ins_int_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_ins_int_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_ins_int_MT_D):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp

$(TARG_TEST_UNIT_ins_int_MT_R):	\
			$(OBJ_TEST_UNIT_ins_int_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_ins_int_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_ins_int_MT_R):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp

$(TARG_TEST_UNIT_ins_int_ST_D):	\
			$(OBJ_TEST_UNIT_ins_int_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_ins_int_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_ins_int_ST_D):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp

$(TARG_TEST_UNIT_ins_int_ST_R):	\
			$(OBJ_TEST_UNIT_ins_int_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_ins_int_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_ins_int_ST_R):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.integer/test.unit.inserter.integer.cpp

$(TARG_TEST_UNIT_ins_w2m_MT_D):	\
			$(OBJ_TEST_UNIT_ins_w2m_MT_D)\
			$(LIB_CORE_MT_D)\
			$(LIB_UTIL_MT_D)\
			$(LIB_FE_NULL_MT_D)\
			$(LIB_BE_NULL_MT_D)\
			$(LIB_BEC_NULL_MT_D)\
			$(LIB_XTESTS_MT_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_D) -o $@ \
		$(OBJ_TEST_UNIT_ins_w2m_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_UTIL_MT_D)\
		$(LIB_FE_NULL_MT_D)\
		$(LIB_BE_NULL_MT_D)\
		$(LIB_BEC_NULL_MT_D)\
		$(LIB_CORE_MT_D)\
		$(LIB_XTESTS_MT_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_D)

$(OBJ_TEST_UNIT_ins_w2m_MT_D):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp

$(TARG_TEST_UNIT_ins_w2m_MT_R):	\
			$(OBJ_TEST_UNIT_ins_w2m_MT_R)\
			$(LIB_CORE_MT_R)\
			$(LIB_UTIL_MT_R)\
			$(LIB_FE_NULL_MT_R)\
			$(LIB_BE_NULL_MT_R)\
			$(LIB_BEC_NULL_MT_R)\
			$(LIB_XTESTS_MT_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_MT_R) -o $@ \
		$(OBJ_TEST_UNIT_ins_w2m_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_UTIL_MT_R)\
		$(LIB_FE_NULL_MT_R)\
		$(LIB_BE_NULL_MT_R)\
		$(LIB_BEC_NULL_MT_R)\
		$(LIB_CORE_MT_R)\
		$(LIB_XTESTS_MT_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_MT_R)

$(OBJ_TEST_UNIT_ins_w2m_MT_R):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_MT_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp

$(TARG_TEST_UNIT_ins_w2m_ST_D):	\
			$(OBJ_TEST_UNIT_ins_w2m_ST_D)\
			$(LIB_CORE_ST_D)\
			$(LIB_UTIL_ST_D)\
			$(LIB_FE_NULL_ST_D)\
			$(LIB_BE_NULL_ST_D)\
			$(LIB_BEC_NULL_ST_D)\
			$(LIB_XTESTS_ST_D)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_D) -o $@ \
		$(OBJ_TEST_UNIT_ins_w2m_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_UTIL_ST_D)\
		$(LIB_FE_NULL_ST_D)\
		$(LIB_BE_NULL_ST_D)\
		$(LIB_BEC_NULL_ST_D)\
		$(LIB_CORE_ST_D)\
		$(LIB_XTESTS_ST_D)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_D)

$(OBJ_TEST_UNIT_ins_w2m_ST_D):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_D) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp

$(TARG_TEST_UNIT_ins_w2m_ST_R):	\
			$(OBJ_TEST_UNIT_ins_w2m_ST_R)\
			$(LIB_CORE_ST_R)\
			$(LIB_UTIL_ST_R)\
			$(LIB_FE_NULL_ST_R)\
			$(LIB_BE_NULL_ST_R)\
			$(LIB_BEC_NULL_ST_R)\
			$(LIB_XTESTS_ST_R)\
			$(PROJ_MAKEFILE_NAME)
	$(LD) $(LD_ARGS_ST_R) -o $@ \
		$(OBJ_TEST_UNIT_ins_w2m_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_UTIL_ST_R)\
		$(LIB_FE_NULL_ST_R)\
		$(LIB_BE_NULL_ST_R)\
		$(LIB_BEC_NULL_ST_R)\
		$(LIB_CORE_ST_R)\
		$(LIB_XTESTS_ST_R)\
		$(LIBS_3PTY_TEST_UNIT)\
		$(TARGET_LIB_ST_R)

$(OBJ_TEST_UNIT_ins_w2m_ST_R):	$(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp $(TEST_CPP_HEADERS) $(PROJ_MAKEFILE_NAME)
	$(CPPC) $(CPPC_TEST_UNIT_ARGS_ST_R) $(CPPC_INCLUDES) -o $@ $(PROJ_TEST_UNIT_DIR)/inserters/test.unit.inserter.w2m/test.unit.inserter.w2m.cpp

############################### end of file ################################
