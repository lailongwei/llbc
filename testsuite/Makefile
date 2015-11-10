# Longwei Lai
# lailongwei@126.com
#****************************************************************************
# This is a GNU make (gmake) Makefile

#****************************************************************************
# platform type
#****************************************************************************
PLATFORMNAME ?= $(shell echo $(shell uname) | tr "[:upper:]" "[:lower:]")

#****************************************************************************
# GCC can be set to YES to use gcc command to compile, or NO use g++
#****************************************************************************
#GCC := YES
GCC := NO

#****************************************************************************
# CCPP can be set to YES to make c and cpp files, or only make cpp files
#****************************************************************************
#CCPP := YES
CCPP := YES

#****************************************************************************
# CROSS can be set to YES to use cross, or NO use common
#****************************************************************************
#CROSS  := YES
CROSS   := NO
ARMPATH := $(HOME)/arm-linux-gcc-3.4.6/bin

#****************************************************************************
# STATIC can be set to YES to link static library first, or NO link dynamic library first
#****************************************************************************
#STATIC := YES
STATIC := NO

#****************************************************************************
# CCACHE can be set to YES to use ccache, or NO use not use ccache
#****************************************************************************
#CCACHE := YES
CCACHE := NO

#****************************************************************************
# No builtin malloc attribute.
#****************************************************************************
#NOBUILTIN_MALLOC := YES
NOBUILTIN_MALLOC := NO

#****************************************************************************
# Optimization
#****************************************************************************
OPTIMIZATION=-O2

#****************************************************************************
# Project specific cxx flags define.
# PROJECT_COMMON_CXXFLAGS  : debug/release compile command will use these
# 					         flags to compile your source file(s).
# PROJECT_DEBUG_CXX_FLAGS  : debug compile command will use these flags to
# 						     compile your source file(s).
# PROJECT_RELEASE_CXX_FLAGS: release compile command will use these flags to
# 							 compile your source file(s).
#****************************************************************************
ifeq ($(PLATFORMNAME),darwin)
  PROJECT_COMMON_CXXFLAGS  := -fvisibility=hidden -fno-strit-alising -Wno-invalid-source-encoding -DHAVE_PTHREADS -DTARGET_OS_MAC
  PROJECT_DEBUG_CXXFLAGS   :=
  PROJECT_RELEASE_CXXFLAGS :=
else
  PROJECT_COMMON_CXXFLAGS  := -fvisibility=hidden -fno-strict-aliasing
  PROJECT_DEBUG_CXXFLAGS   :=
  PROJECT_RELEASE_CXXFLAGS :=
endif

#****************************************************************************
# Project specific ld flags define.
# PROJECT_COMMON_LDFLAGS  : debug/release link command will use these flags
# 							to link your all object file(s).
# PROJECT_DEBUG_LDFLAGS   : debug link command will use these flags to link
# 							your all object file(s).
# PROJECT_RELEASE_LDFLAGS : release link command will use these flags to link
# 						    your all object file(s).
#****************************************************************************
PROJECT_COMMON_LDFLAGS  :=
PROJECT_DEBUG_LDFLAGS   :=
PROJECT_RELEASE_LDFLAGS :=

#****************************************************************************
# Preprocessor directives
# #****************************************************************************
DEFS =

#****************************************************************************
# This project home path
#****************************************************************************
PROJECTDIR ?= $(MAKEDIR)

#****************************************************************************
# Include paths
#****************************************************************************
# INCLS  = -I$(PROJECTDIR)/path/to/header/files
INCLS  = -I$(PROJECTDIR)
INCLS += -I$(PROJECTDIR)/../llbc/include

#****************************************************************************
# Not source directories and not source files
#****************************************************************************
NOTSRCDIRS=
NOTSRCFILES=

#****************************************************************************
# Library files for link
#****************************************************************************
# LIBS += -Lyour/libs/path -lyour/libs/name
RTLIB   := -lrt
DLLIB	:= -ldl
UUIDLIB := -luuid
LLBCLIB := -L$(PROJECTDIR)/../llbc/lib -lllbc

LIBS += $(RTLIB)
LIBS += $(DLLIB)
LIBS += $(UUIDLIB)
LIBS += $(LLBCLIB)

#****************************************************************************
# Specify some special variables in this file
#****************************************************************************
# ... ...

#****************************************************************************
# Specify some special variables in rules.mk
#****************************************************************************
PROJECTNAME ?= testsuite

#****************************************************************************
# Targets of the build
#****************************************************************************
EXECNAME=
BASICNAME = $(if $(EXECNAME),$(EXECNAME),$(PROJECTNAME))$(TARGETSUFFIX)
OUTPUTLIB = $(if $(PREFIX),$(PREFIX),$(PROJECTDIR))/lib/$(LIBPREFIX)$(BASICNAME).$(LIBSUFFIX)
OUTPUTDLL = $(if $(PREFIX),$(PREFIX),$(PROJECTDIR))/lib/$(DLLPREFIX)$(BASICNAME).$(DLLSUFFIX)
OUTPUTEXE = $(if $(PREFIX),$(PREFIX),$(PROJECTDIR))/bin/$(BASICNAME)

OUTPUT    = $(OUTPUTEXE)

#****************************************************************************
# Include common rules
#****************************************************************************
MAKEDIR ?= $(CURDIR)
include $(PROJECTDIR)/../rules.mk

#****************************************************************************
# Specify rule for output
#****************************************************************************
$(OUTPUTLIB) : $(OBJS)
	$(CREATEDIR)
	$(MAKELIB) -o $@ $^

$(OUTPUTDLL) : $(OBJS)
	$(CREATEDIR)
	$(MAKEDLL) -o $@ $^ $(LIBS)

$(OUTPUTEXE) : $(OBJS)
	$(CREATEDIR)
	$(MAKEEXE) -o $@ $^ $(LIBS)

tar :
	@(cd $(PROJECTDIR))
	$(MAKE) clean
	$(MAKE) cleandebug
	@(cd .. && tar --exclude "*.svn" -zcvf $(PROJECTNAME)_$(shell date +%Y%m%d_%H%M%S).tgz $(notdir $(shell pwd)))
