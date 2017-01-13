# File:  Makefile
# Brief: easy to build llbc library and testsuite.
#############################################################################

#****************************************************************************
# Determine platform name
#****************************************************************************
SUPPORTED_PLATFORMS	:= linux darwin
PLATFORMNAME		?= $(shell echo $(shell uname) | tr "[:upper:]" "[:lower:]")
$(if $(findstring $(PLATFORMNAME),$(SUPPORTED_PLATFORMS)),,$(error "Unsupported os, must be one of '$(SUPPORTED_PLATFORMS)'"))

#****************************************************************************
# some useful variables
#****************************************************************************
# Parse debug option
CONFIG_OPT		?= $(shell echo $(config) | tr "[:upper:]" "[:lower:]")
ifeq ($(CONFIG_OPT),debug)
  DEBUG_OPT=TRUE
else
  DEBUG_OPT=FALSE
endif

# All make targets define
PREMAKE_TARGET	:= build_makefiles

CORELIB_TARGET  := core_lib
TEST_TARGET     := test
WRAPS_TARGET    := wraps

PYWRAP_TARGET   := py_wrap
CSWRAP_TARGET	:= cs_wrap

ALL_WRAP_TARGETS:= $(PYWRAP_TARGET) $(CSWRAP_TARGET)

# All targets output directory
ALL_TARGETS_OUTPUT := output/gmake
# Some prefixs/suffixes define
ifeq ($(PLATFORMNAME),darwin)
  DYNLIB_SUFFIX := .dylib
else
  DYNLIB_SUFFIX := .so
endif
DEBUG_SUFFIX    := _debug
EXE_SUFFIX      :=

# Target names/paths define
ifeq ($(DEBUG_OPT),FALSE)
  CORELIB_TARGET_NAME   := libllbc$(DYNLIB_SUFFIX)
  TESTSUITE_TARGET_NAME := testsuite$(EXE_SUFFIX)
  PYWRAP_TARGET_NAME    := llbc$(DYNLIB_SUFFIX)
else
  CORELIB_TARGET_NAME   := libllbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
  TESTSUITE_TARGET_NAME := testsuite$(DEBUG_SUFFIX)$(EXE_SUFFIX)
  PYWRAP_TARGET_NAME    := llbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
endif

CORELIB_TARGET_PATH   := $(ALL_TARGETS_OUTPUT)/$(CORELIB_TARGET_NAME)
TESTSUITE_TARGET_PATH := $(ALL_TARGETS_OUTPUT)/$(TESTSUITE_TARGET_NAME)
PYWRAP_TARGET_PATH    := $(ALL_TARGETS_OUTPUT)/$(PYWRAP_TARGET_NAME)

# Some variables define
PREMAKE_PATH	:= "tools/premake"
PREMAKE_NAME	:= "premake5_$(PLATFORMNAME)"

#****************************************************************************
# all real make commands
#****************************************************************************
# Set phonies
.PHONY: help all install clean tar $(PREMAKE_TARGET)
help:
	@echo "make commands:"
	@echo "========================================================================="
	@echo "  make [help]   - display this help information"
	@echo "========================================================================="
	@echo "  make all      - make core library, testsuite and all wrapped libraries"
	@echo "  make $(CORELIB_TARGET) - make c++ core library"
	@echo "  make $(TEST_TARGET)     - make c++ core library testsuite"
	@echo "  make $(WRAPS_TARGET)    - make all language specificed warpped libraries"
	@echo "                  now supported languages: python"
	@echo "  make $(PYWRAP_TARGET)  - make python wrapped library"
	@echo "  make $(CSWRAP_TARGET)  - make csharp wrapped library"
	@echo "========================================================================="
	@echo "  make clean          - remove all object directories and target files"
	@echo "  make clean_$(CORELIB_TARGET) - remove all '$(CORELIB_TARGET)' object directories and target files"
	@echo "  make clean_$(TEST_TARGET)     - remove all '$(TEST_TARGET)' object directories and target files"
	@echo "  make clean_$(WRAPS_TARGET)    - remove all '$(WRAPS_TARGET)' object directories and target files"
	@echo "  make clean_$(PYWRAP_TARGET)  - remove all '$(PYWRAP_TARGET)' object directories and target files"
	@echo "  make clean_$(CSWRAP_TARGET)  - remove all '$(CSWRAP_TARGET)' object directories and target files"
	@echo "========================================================================="
	@echo "  make install          - install c++ core library and all wrapped libraries to system directory"
	@echo "  make install_$(CORELIB_TARGET) - install c++ core library to system directory"
	@echo "  make install_$(WRAPS_TARGET)    - install all wrapped libraries to system directory"
	@echo "  make install_$(PYWRAP_TARGET)  - install python language specified library to system directory"
	@echo "  make install_$(CSWRAP_TARGET)  - install csharp language specified library to system directory"
	@echo "========================================================================="
	@echo "  make tar      - tarball llbc framework(included core library, testsuite"
	@echo "                  codes and all language specificed wrapped libraries"

all: $(PREMAKE_TARGET) $(CORELIB_TARGET) $(TEST_TARGET) $(ALL_WRAP_TARGETS)

$(PREMAKE_TARGET):
	@(cd $(PREMAKE_PATH) && ./$(PREMAKE_NAME) gmake)

$(CORELIB_TARGET): $(PREMAKE_TARGET)
	@(cd build/gmake && $(MAKE) -f llbc.make)

$(TEST_TARGET): $(CORELIB_TARGET)
	@(cd build/gmake && $(MAKE) -f testsuite.make)

$(WRAPS_TARGET): $(ALL_WRAP_TARGETS)
$(PYWRAP_TARGET): $(CORELIB_TARGET)
	@(cd build/gmake && $(MAKE) -f pyllbc.make)
$(CSWRAP_TARGET): $(CORELIB_TARGET)
	@(cd build/gmake && $(MAKE) -f csllbc_native.make)
	@(cd build/gmake && $(MAKE) -f csllbc.make)
	@(cd build/gmake && $(MAKE) -f csllbc_testsuite.make)

clean: $(addprefix clean_,$(CORELIB_TARGET) $(TEST_TARGET) $(WRAPS_TARGET))
	@$(shell find ./ -name "._*" -exec rm {} \;)
	@$(shell find ./ -name ".DS_Store" -exec rm {} \;)
	@$(shell find ./ -type f -name "*.buildlog" -exec rm {} \;)

clean_$(CORELIB_TARGET):
	@(cd build/gmake && $(MAKE) clean -f llbc.make)

clean_$(TEST_TARGET):
	@(cd build/gmake && $(MAKE) clean -f testsuite.make)

clean_$(WRAPS_TARGET): $(addprefix clean_,$(ALL_WRAP_TARGETS))
clean_$(PYWRAP_TARGET):
	@(cd build/gmake && $(MAKE) clean -f pyllbc.make)
	@$(shell find ./wrap/pyllbc -type f -name "*.pyc" -exec rm {} \;)
clean_$(CSWRAP_TARGET):
	@(cd build/gmake && $(MAKE) clean -f csllbc_native.make)
	@(cd build/gmake && $(MAKE) clean -f csllbc.make)
	@(cd build/gmake && $(MAKE) clean -f csllbc_testsuite.make)

install: install_$(CORELIB_TARGET) install_$(WRAPS_TARGET)

install_$(CORELIB_TARGET):
	@(if [ ! -e $(CORELIB_TARGET_PATH) ]; then echo "not found '$(CORELIB_TARGET)' targets, install failed!"; exit -1; fi)
	@rm -rf /usr/include/llbc
	@(cd $(ALL_TARGETS_OUTPUT) && \cp -rfv $(CORELIB_TARGET_NAME) /usr/lib)
	@(cd llbc/include && \cp -rfv llbc.h /usr/include)
	@(cd llbc/include && \rsync -av --exclude='*.svn' llbc /usr/include/)

install_$(WRAPS_TARGET): install_$(CORELIB_TARGET) $(addprefix install_,$(ALL_WRAP_TARGETS))
install_$(PYWRAP_TARGET):
	@(if [ ! -e $(PYWRAP_TARGET_PATH) ]; then echo "not found '$(PYWRAP_TARGET)' targets, install failed!"; exit -1; fi)
	@(cd $(ALL_TARGETS_OUTPUT) && \cp -rfv $(PYWRAP_TARGET_NAME) /usr/local/lib/python2.7/site-packages/)
install_$(CSWRAP_TARGET):
	@echo "!!!csrarp library could not be install, please copy the libraries file to your project directory"
	
tar:
	@(cd tools && python tar.py)

