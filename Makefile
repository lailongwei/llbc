# File:  Makefile
# Brief: easy to build llbc libraries and testsuites.
#############################################################################

#****************************************************************************
# Determine platform name
#****************************************************************************
SUPPORTED_SYSTEMS := linux,darwin
SYSTEM_NAME       ?= $(shell echo $(shell uname) | tr "[:upper:]" "[:lower:]")
ARCHITECTURE_NAME ?= $(shell echo $(shell uname -m) | tr "[:upper:]" "[:lower:]")
$(if $(findstring $(SYSTEM_NAME),$(SUPPORTED_SYSTEMS)),,$(error "Unsupported system, must be one of '$(SUPPORTED_SYSTEMS)'"))

#****************************************************************************
# Default config define
ifndef config
  export config=release64
endif
#****************************************************************************

#****************************************************************************
# Enable verbose log(for premake tool)
ifndef verbose
  export verbose=1
endif
#****************************************************************************

#****************************************************************************
# some useful variables
#****************************************************************************
# Parse debug option
CONFIG_OPT          ?= $(shell echo $(config) | tr "[:upper:]" "[:lower:]")
SUPPORTED_CONFIGS   :=debug32,release32,debug64,release64
$(if $(findstring $(CONFIG_OPT),$(SUPPORTED_CONFIGS)),,$(error "Unsupported config, must be one of'$(SUPPORTED_CONFIGS)'))
DEBUG_OPT           := $(shell echo $(CONFIG_OPT) | tr -d "[:digit:]" | tr "[:lower:]" "[:upper:]")

# All make targets define
PREMAKE_TARGET  := build_makefiles

CORELIB_TARGET    := core_lib
TEST_TARGET       := test
QUICK_START_TARGET:= quick_start
WRAPS_TARGET      := wraps

PYWRAP_TARGET       := py_wrap
CSWRAP_TARGET       := cs_wrap
LUWRAP_TARGET       := lu_wrap
LUWRAP_LUALIB_TARGET:= lu_wrap_lualib
LUWRAP_LUAEXE_TARGET:= lu_wrap_luaexe

ALL_WRAP_TARGETS:= $(PYWRAP_TARGET) $(CSWRAP_TARGET) $(LUWRAP_TARGET)

# Premake action
PREMAKE_ACTION  := gmake2
# All targets output directory
ALL_TARGETS_OUTPUT := output/$(PREMAKE_ACTION)/$(config)
# Some prefixs/suffixes define
ifeq ($(SYSTEM_NAME),darwin)
  DYNLIB_SUFFIX := .dylib
else
  DYNLIB_SUFFIX := .so
endif
DEBUG_SUFFIX    := _debug
EXE_SUFFIX      :=

# Target names/paths define
ifeq ($(DEBUG_OPT),RELEASE)
  CORELIB_TARGET_NAME    := libllbc$(DYNLIB_SUFFIX)
  TESTSUITE_TARGET_NAME  := testsuite$(EXE_SUFFIX)
  QUICK_START_TARGET_NAME:= quick_start$(EXE_SUFFIX)
  PYWRAP_TARGET_NAME     := llbc$(DYNLIB_SUFFIX)
  LUWRAP_TARGET_NAME     := lullbc$(DYNLIB_SUFFIX)
else
  CORELIB_TARGET_NAME    := libllbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
  TESTSUITE_TARGET_NAME  := testsuite$(DEBUG_SUFFIX)$(EXE_SUFFIX)
  QUICK_START_TARGET_NAME:=quick_start$(DEBUG_SUFFIX)$(EXE_SUFFIX)
  PYWRAP_TARGET_NAME     := llbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
  LUWRAP_TARGET_NAME     := lullbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
endif

CORELIB_TARGET_PATH    := $(ALL_TARGETS_OUTPUT)/$(CORELIB_TARGET_NAME)
TESTSUITE_TARGET_PATH  := $(ALL_TARGETS_OUTPUT)/$(TESTSUITE_TARGET_NAME)
QUICK_START_TARGET_PATH:= $(ALL_TARGETS_OUTPUT)/$(QUICK_START_TARGET_NAME)
PYWRAP_TARGET_PATH     := $(ALL_TARGETS_OUTPUT)/$(PYWRAP_TARGET_NAME)
LUWRAP_TARGET_PATH     := $(ALL_TARGETS_OUTPUT)/$(LUWRAP_TARGET_NAME)

# Some variables define
PREMAKE_PATH := "tools/premake"
PREMAKE_NAME := "premake5_$(SYSTEM_NAME)_$(ARCHITECTURE_NAME)"

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
	@echo "  make $(CORELIB_TARGET)     - make c++ core library"
	@echo "  make $(TEST_TARGET)        - make c++ core library testsuite"
	@echo "  make $(QUICK_START_TARGET) - make c++ core library quick start demos"
	@echo "  make $(WRAPS_TARGET)       - make all language specificed warpped libraries"
	@echo "                               now supported languages: python, csharp, lua"
	@echo "  make $(PYWRAP_TARGET)      - make python wrapped library"
	@echo "  make $(CSWRAP_TARGET)      - make csharp wrapped library"
	@echo "  make $(LUWRAP_TARGET)      - make lua wrapped library"
	@echo "========================================================================="
	@echo "  make clean          - remove all object directories and target files"
	@echo "  make clean_$(CORELIB_TARGET)     - remove all '$(CORELIB_TARGET)' object directories and target files"
	@echo "  make clean_$(TEST_TARGET)        - remove all '$(TEST_TARGET)' object directories and target files"
	@echo "  make clean_$(QUICK_START_TARGET) - remove all '$(QUICK_START_TARGET)' object directories and target files"
	@echo "  make clean_$(WRAPS_TARGET)       - remove all '$(WRAPS_TARGET)' object directories and target files"
	@echo "  make clean_$(PYWRAP_TARGET)      - remove all '$(PYWRAP_TARGET)' object directories and target files"
	@echo "  make clean_$(CSWRAP_TARGET)      - remove all '$(CSWRAP_TARGET)' object directories and target files"
	@echo "  make clean_$(LUWRAP_TARGET)      - remove all '$(LUWRAP_TARGET)' object directories and target files"
	@echo "========================================================================="
	@echo "  make install          - install c++ core library and all wrapped libraries to system directory"
	@echo "  make install_$(CORELIB_TARGET) - install c++ core library to system directory"
	@echo "  make install_$(WRAPS_TARGET)    - install all wrapped libraries to system directory"
	@echo "  make install_$(PYWRAP_TARGET)  - install python language specified library to system directory"
	@echo "  make install_$(CSWRAP_TARGET)  - install csharp language specified library to system directory"
	@echo "  make install_$(LUWRAP_TARGET)  - install csharp language specified library to system directory"
	@echo "========================================================================="
	@echo "  make tar      - tarball llbc framework(included core library, testsuite"
	@echo "                  codes and all language specificed wrapped libraries)"

all: $(PREMAKE_TARGET) $(CORELIB_TARGET) $(TEST_TARGET) $(QUICK_START_TARGET) $(ALL_WRAP_TARGETS)

$(PREMAKE_TARGET):
	@(cd $(PREMAKE_PATH) && ./$(PREMAKE_NAME) $(PREMAKE_ACTION))

$(CORELIB_TARGET): $(PREMAKE_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f llbc.make

$(TEST_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f testsuite.make

$(QUICK_START_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f quick_start.make

$(WRAPS_TARGET): $(ALL_WRAP_TARGETS)
$(PYWRAP_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f pyllbc.make
$(CSWRAP_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_native.make
	$(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc.make
	$(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_testsuite.make
$(LUWRAP_LUALIB_TARGET):
	$(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_lualib.make
$(LUWRAP_LUAEXE_TARGET): $(LUWRAP_LUALIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_luaexec.make
$(LUWRAP_TARGET): $(CORELIB_TARGET) $(LUWRAP_LUALIB_TARGET) $(LUWRAP_LUAEXE_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc.make

clean: $(addprefix clean_,$(CORELIB_TARGET) $(TEST_TARGET) $(QUICK_START_TARGET) $(WRAPS_TARGET))
	@$(shell find ./ -name "._*" -exec rm {} \;)
	@$(shell find ./ -name ".DS_Store" -exec rm {} \;)
	@$(shell find ./ -type f -name "*.buildlog" -exec rm {} \;)

clean_$(CORELIB_TARGET):
	@(if [ -e build/$(PREMAKE_ACTION)/llbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f llbc.make clean; fi)

clean_$(TEST_TARGET):
	@(if [ -e build/$(PREMAKE_ACTION)/testsuite.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f testsuite.make clean; fi)

clean_$(QUICK_START_TARGET):
	@(if [ -e build/$(PREMAKE_ACTION)/quick_start.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f quick_start.make clean; fi)

clean_$(WRAPS_TARGET): $(addprefix clean_,$(ALL_WRAP_TARGETS))
clean_$(PYWRAP_TARGET):
	@(if [ -e build/$(PREMAKE_ACTION)/pyllbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f pyllbc.make clean; fi)
	@(if [ -d ./wrap/pyllbc/cpython ]; then cd ./wrap/pyllbc/cpython && make clean; fi)
	@$(shell find ./wrap/pyllbc -type f -name "*.pyc" -exec rm {} \;)
clean_$(CSWRAP_TARGET):
	@(if [ -e build/$(PREMAKE_ACTION)/csllbc_native.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_native.make clean; fi)
	@(if [ -e build/$(PREMAKE_ACTION)/csllbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc.make clean; fi)
	@(if [ -e build/$(PREMAKE_ACTION)/csllbc_testsuite.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_testsuite.make clean; fi)
clean_$(LUWRAP_TARGET):
	@(if [ -e build/$(PREMAKE_ACTION)/lullbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc.make clean; fi)
	@(if [ -e build/$(PREMAKE_ACTION)/lullbc_lualib.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_lualib.make clean; fi)
	@(if [ -e build/$(PREMAKE_ACTION)/lullbc_luaexec.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_luaexec.make clean; fi)

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
install_$(LUWRAP_TARGET):
	@echo "!!!luwrap library could not be install, please copy the libraries file to your project directory"
	
tar:
	@(cd tools && python tar.py)

