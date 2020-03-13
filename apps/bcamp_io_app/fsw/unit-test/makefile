##############################################################################
## GNU Makefile for building UT unit tests

#
# Supported MAKEFILE targets:
#   clean - deletes object files, executables, output files, and gcov files
#   all   - makes utf_test_runner.exe
#   run   - runs utf_test_runner.exe
#   gcov  - prints a GCOV coverage report (make all, make run, make gcov)
#

APP=bcamp_io_app
$(info $$APP is [${APP}])

mkfile_path := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CFS_MISSION := $(realpath $(mkfile_path)/../../../../)

CFS_APP_SRC = $(CFS_MISSION)/apps/$(APP)

UT_ASSERT_SRC := $(CFS_MISSION)/osal/ut_assert

CFE_DIR = $(CFS_MISSION)/cfe
OSAL_DIR = $(CFS_MISSION)/osal
BSP_TYPE ?= pc-linux
BSP_DIR := $(OSAL_DIR)/src/bsp/$(BSP_TYPE)/ut-src
PSP_DIR = $(CFS_MISSION)/psp
APP_DIR = $(CFS_MISSION)/apps

#
# VPATH specifies the search paths for source files outside of the current directory.  Note that
# all object files will be created in the current directory even if the source file is not in the 
# current directory.
#
VPATH := $(CFS_APP_SRC)/fsw/src
VPATH += $(UT_ASSERT_SRC)/src
VPATH += $(BSP_DIR)
VPATH += $(PSP_DIR)/fsw/ut-stubs
VPATH += $(CFE_DIR)/fsw/cfe-core/ut-stubs
VPATH += $(CFE_DIR)/fsw/cfe-core/unit-test
VPATH += $(OSAL_DIR)/src/ut-stubs

#
# INCLUDES specifies the search paths for include files outside of the current directory.  
# Note that the -I is required. 
#
INCLUDES += -I../src
INCLUDES += -I../platform_inc
INCLUDES += -I../mission_inc
INCLUDES += -I$(UT_ASSERT_SRC)/inc
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/inc
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/time
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/es
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/src/evs
INCLUDES += -I$(CFE_DIR)/fsw/cfe-core/unit-test
INCLUDES += -I$(CFE_DIR)/fsw/mission_inc
INCLUDES += -I$(CFE_DIR)/fsw/platform_inc/cpu1
INCLUDES += -I$(CFE_DIR)/cmake/target/inc
INCLUDES += -I$(OSAL_DIR)/src/os/inc
INCLUDES += -I$(OSAL_DIR)/src/os/shared
INCLUDES += -I$(OSAL_DIR)/src/bsp/pc-linux/config
INCLUDES += -I$(PSP_DIR)/fsw/inc

#
# LIB_OBJS specifies library object files.
#
LIB_OBJS := $(APP).o

#
# GCOV_OBJS specifies the object files targeted for code coverage
# They're not necessarily the same set as the LIB_OBJS.
#
GCOV_OBJS := $(APP).o

#
# UT_OBJS specifies unit test object files.
#
UT_SRCS := $(wildcard $(UT_ASSERT_SRC)/src/*.c)
UT_SRCS := $(foreach d,$(UT_SRCS),$(notdir $(d)))
UT_OBJS := $(patsubst %.c,%.o,$(UT_SRCS))

BSP_SRCS := $(wildcard $(BSP_DIR)/*.c)
BSP_SRCS := $(foreach d,$(BSP_SRCS),$(notdir $(d)))
BSP_OBJS := $(patsubst %.c,%.o,$(BSP_SRCS))
UT_OBJS += $(BSP_OBJS)

PSP_SRCS := $(wildcard $(PSP_DIR)/fsw/ut-stubs/*.c)
PSP_SRCS := $(foreach d,$(PSP_SRCS),$(notdir $(d)))
PSP_OBJS := $(patsubst %.c,%.o,$(PSP_SRCS))
UT_OBJS += $(PSP_OBJS)

CFE_SRCS := $(wildcard $(CFE_DIR)/fsw/cfe-core/ut-stubs/*.c)
CFE_SRCS := $(foreach d,$(CFE_SRCS),$(notdir $(d)))
CFE_OBJS := $(patsubst %.c,%.o,$(CFE_SRCS))
UT_OBJS += $(CFE_OBJS)

OSAL_SRCS := $(wildcard $(OSAL_DIR)/src/ut-stubs/*.c)
OSAL_SRCS := $(foreach d,$(OSAL_SRCS),$(notdir $(d)))
OSAL_OBJS := $(patsubst %.c,%.o,$(OSAL_SRCS))
UT_OBJS += $(OSAL_OBJS)

UT_OBJS += ut_support.o

UT_OBJS += $(APP)_testcase.o
#UT_OBJS += $(APP)_stubs.o

###############################################################################

COMPILER=gcc
LINKER=gcc

#
# Compiler and Linker Options
#
ENABLE_GCOV = TRUE
ifeq ($(ENABLE_GCOV), TRUE)
GCOV_COPT = -fprofile-arcs -ftest-coverage -pg -p
GCOV_LOPT = -pg -p -fprofile-arcs -ftest-coverage -lgcov
endif

#WARNINGS = -Wall -W -ansi -Werror -Wstrict-prototypes -Wundef
WARNINGS = -Wall -Wstrict-prototypes
DEBUGGER = -g

#COPT = $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D__x86_64__ -D_LINUX_OS_
COPT = $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D_ix86_ -D_LINUX_OS_ -DUT_VERBOSE -DOSAPI_NO_SPECIAL_ATTRIBS

LOPT = $(GCOV_LOPT)

###############################################################################
## Rule to make the specified TARGET
##
%.exe: %.o
	$(LINKER) $(LOPT) $^ -o $*.exe

###############################################################################
##  "C" COMPILER RULE
##
%.o: %.c
	$(COMPILER) -c $(COPT) $(INCLUDES) $<

###############################################################################
##
##
all:$(APP)_testrunner.exe

$(APP)_testrunner.exe: $(APP)_testrunner.o $(UT_OBJS) $(LIB_OBJS)

clean ::
	rm -f *.o *.exe *.gcda *.gcno *.gcov gmon.out
	rm -rf ram0 ram1 ram2 ram3 ram4 ram5 eeprom1

run ::
	./$(APP)_testrunner.exe

gcov ::
	@gcov -f -b $(GCOV_OBJS:.o=.gcda)

# end of file

