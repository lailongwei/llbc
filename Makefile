# File:  Makefile
# Brief: easy to build llbc core library/core library tests/wrap libraries.
#############################################################################

#****************************************************************************
# Determine platform name.
#****************************************************************************
SUPPORTED_SYSTEMS := linux,darwin
SYSTEM_NAME       ?= $(shell echo $(shell uname) | tr "[:upper:]" "[:lower:]")
ARCHITECTURE_NAME ?= $(shell echo $(shell uname -m) | tr "[:upper:]" "[:lower:]")
$(if $(findstring $(SYSTEM_NAME),$(SUPPORTED_SYSTEMS)),,$(error "Unsupported system, must be one of '$(SUPPORTED_SYSTEMS)'"))

#****************************************************************************
# Default config define.
ifndef config
  export config=release64
endif
#****************************************************************************

#****************************************************************************
# Enable verbose log(for premake tool).
ifndef verbose
  export verbose=1
endif
#****************************************************************************

#****************************************************************************
# Some useful variables.
#****************************************************************************
# Parse debug option.
CONFIG_OPT          ?= $(shell echo $(config) | tr "[:upper:]" "[:lower:]")
SUPPORTED_CONFIGS   :=debug32,release32,debug64,release64
$(if $(findstring $(CONFIG_OPT),$(SUPPORTED_CONFIGS)),,$(error "Unsupported config, must be one of'$(SUPPORTED_CONFIGS)'))
DEBUG_OPT           := $(shell echo $(CONFIG_OPT) | tr -d "[:digit:]" | tr "[:lower:]" "[:upper:]")

# All make targets define.
PREMAKE_TARGET  			:= build_makefiles

CORELIB_TARGET    			:= core_lib
CORELIB_TESTS_TARGET 		:= tests
CORELIB_EXAMPLE_TARGET 		:= example
CORELIB_FUNC_TEST_TARGET  	:= func_test
CORELIB_UNIT_TEST_TARGET  	:= unit_test
CORELIB_QUICK_START_TARGET 	:= quick_start
ALL_CORELIB_TEST_TARGETS    := $(CORELIB_EXAMPLE_TARGET) $(CORELIB_FUNC_TEST_TARGET) $(CORELIB_UNIT_TEST_TARGET) $(CORELIB_QUICK_START_TARGET)

WRAPS_TARGET      			:= wraps
PYWRAP_TARGET       		:= py_wrap
CSWRAP_TARGET       		:= cs_wrap
LUWRAP_TARGET       		:= lu_wrap
LUWRAP_LUALIB_TARGET		:= lu_wrap_lualib
LUWRAP_LUAEXE_TARGET		:= lu_wrap_luaexe
ALL_WRAP_TARGETS			:= $(PYWRAP_TARGET) $(CSWRAP_TARGET) $(LUWRAP_TARGET)

# Premake action.
PREMAKE_ACTION  := gmake2
# All targets output directory.
ALL_TARGETS_OUTPUT := output/$(PREMAKE_ACTION)/$(config)
# Some prefixs/suffixes define.
ifeq ($(SYSTEM_NAME),darwin)
  DYNLIB_SUFFIX := .dylib
else
  DYNLIB_SUFFIX := .so
endif
DEBUG_SUFFIX    := _debug
EXE_SUFFIX      :=

# Target names/paths define.
ifeq ($(DEBUG_OPT),RELEASE)
  CORELIB_TARGET_NAME    			:= libllbc$(DYNLIB_SUFFIX)
  CORELIB_EXAMPLE_TARGET_NAME  		:= example$(EXE_SUFFIX)
  CORELIB_FUNC_TEST_TARGET_NAME  	:= func_test$(EXE_SUFFIX)
  CORELIB_UNIT_TEST_TARGET_NAME  	:= unit_test$(EXE_SUFFIX)
  CORELIB_QUICK_START_TARGET_NAME  	:= quick_start$(EXE_SUFFIX)
  PYWRAP_TARGET_NAME     			:= llbc$(DYNLIB_SUFFIX)
  LUWRAP_TARGET_NAME     			:= lullbc$(DYNLIB_SUFFIX)
else
  CORELIB_TARGET_NAME    			:= libllbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
  CORELIB_EXAMPLE_TARGET_NAME  		:= example$(DEBUG_SUFFIX)$(EXE_SUFFIX)
  CORELIB_FUNC_TEST_TARGET_NAME  	:= func_test$(DEBUG_SUFFIX)$(EXE_SUFFIX)
  CORELIB_UNIT_TEST_TARGET_NAME  	:= unit_test$(DEBUG_SUFFIX)$(EXE_SUFFIX)
  CORELIB_QUICK_START_TARGET_NAME  	:= quick_start$(DEBUG_SUFFIX)$(EXE_SUFFIX)
  PYWRAP_TARGET_NAME     			:= llbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
  LUWRAP_TARGET_NAME     			:= lullbc$(DEBUG_SUFFIX)$(DYNLIB_SUFFIX)
endif

CORELIB_TARGET_PATH    				:= $(ALL_TARGETS_OUTPUT)/$(CORELIB_TARGET_NAME)
CORELIB_EXAMPLE_TARGET_PATH   		:= $(ALL_TARGETS_OUTPUT)/$(CORELIB_EXAMPLE_TARGET_NAME)
CORELIB_FUNC_TEST_TARGET_PATH   	:= $(ALL_TARGETS_OUTPUT)/$(CORELIB_FUNC_TEST_TARGET_NAME)
CORELIB_UNIT_TEST_TARGET_PATH   	:= $(ALL_TARGETS_OUTPUT)/$(CORELIB_UNIT_TEST_TARGET_NAME)
CORELIB_QUICK_START_TARGET_PATH   	:= $(ALL_TARGETS_OUTPUT)/$(CORELIB_QUICK_START_TARGET_NAME)
PYWRAP_TARGET_PATH     				:= $(ALL_TARGETS_OUTPUT)/$(PYWRAP_TARGET_NAME)
LUWRAP_TARGET_PATH     				:= $(ALL_TARGETS_OUTPUT)/$(LUWRAP_TARGET_NAME)

# Some variables define.
PREMAKE_PATH := "tools/premake"
PREMAKE_NAME := "premake5_$(SYSTEM_NAME)_$(ARCHITECTURE_NAME)"

#****************************************************************************
# Useful functions.
#****************************************************************************
# log functions.
define output_r
	@if [ "$(2)" = "true" ]; then \
		@echo -n -e "\e[31m$(1)\e[0m\n"; \
	else \
		@echo -n -e "\e[31m$(1)\e[0m"; \
	fi
endef
define output_rg
	@if [ "$(2)" = "true" ]; then \
		@echo -n -e "\e[1;31m$(1)\e[0m\n"; \
	else \
		@echo -n -e "\e[1;31m$(1)\e[0m"; \
	fi

endef

define output_g
	@if [ "$(2)" = "true" ]; then \
		echo -n -e "\e[32m$(1)\e[0m\n"; \
	else \
		echo -n -e "\e[32m$(1)\e[0m"; \
	fi
endef
define output_hg
	@if [ "$(2)" = "true" ]; then \
		echo -n -e "\e[1;32m$(1)\e[0m\n"; \
	else \
		echo -n -e "\e[1;32m$(1)\e[0m"; \
	fi
endef

define output_y
	@if [ "$(2)" = "true" ]; then \
		@echo -n -e "\e[33m$(1)\e[0m\n"; \
	else \
		@echo -n -e "\e[33m$(1)\e[0m"; \
	fi
endef
define output_yg
	@if [ "$(2)" = "true" ]; then \
		@echo -n -e "\e[1;33m$(1)\e[0m"\n; \
	else \
		@echo -n -e "\e[1;33m$(1)\e[0m"; \
	fi
endef

define output
	@if [ "$(2)" = "true" ]; then \
		echo -n -e "$(1)\n"; \
	else \
		echo -n -e "$(1)"; \
	fi
endef

#****************************************************************************
# all real make commands.
#****************************************************************************
.PHONY: help all install clean tar $(PREMAKE_TARGET)
help:
	$(call output_hg,"Makefile commands:","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make [help]")
	$(call output," - display this help information","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make all")
	$(call output," - make [core library/core library tests/wrapped libraries]","true")
	$(call output,"","true")
	$(call output_g,"make $(CORELIB_TARGET)")
	$(call output,"\ \ \ \ - make c++ core library","true")
	$(call output,"","true")
	$(call output_g,"make $(CORELIB_TESTS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - make c++ core library all tests[$(ALL_CORELIB_TEST_TARGETS)]","true")
	$(call output_g,"make $(CORELIB_EXAMPLE_TARGET)")
	$(call output,"\ \ \ \ \ - make c++ core library test target: $(CORELIB_EXAMPLE_TARGET)","true")
	$(call output_g,"make $(CORELIB_FUNC_TEST_TARGET)")
	$(call output,"\ \ \ - make c++ core library test target: $(CORELIB_FUNC_TEST_TARGET)","true")
	$(call output_g,"make $(CORELIB_UNIT_TEST_TARGET)")
	$(call output,"\ \ \ - make c++ core library test target: $(CORELIB_UNIT_TEST_TARGET)","true")
	$(call output_g,"make $(CORELIB_QUICK_START_TARGET)")
	$(call output," - make c++ core library test target: quick start target","true")
	$(call output,"","true")
	$(call output_g,"make $(WRAPS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - make all language specificed warpped libraries[$(ALL_WRAP_TARGETS)]","true")
	$(call output_g,"make $(PYWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - make python wrapped library","true")
	$(call output_g,"make $(CSWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - make csharp wrapped library","true")
	$(call output_g,"make $(LUWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - make lua wrapped library","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make clean")
	$(call output,"\ \ \ \ \ \ \ \ \ \ \ \ \ - clean all target files","true")
	$(call output,"","true")
	$(call output_g,"make clean_$(CORELIB_TARGET)")
	$(call output,"\ \ \ \ - remove c++ core library target output files","true")
	$(call output,"","true")
	$(call output_g,"make clean_$(CORELIB_TESTS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - remove c++ core library all test targets[$(ALL_CORELIB_TEST_TARGETS)] output files","true")
	$(call output_g,"make clean_$(CORELIB_EXAMPLE_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(CORELIB_EXAMPLE_TARGET)' target output files","true")
	$(call output_g,"make clean_$(CORELIB_FUNC_TEST_TARGET)")
	$(call output,"\ \ \ - remove '$(CORELIB_FUNC_TEST_TARGET)' target output files","true")
	$(call output_g,"make clean_$(CORELIB_UNIT_TEST_TARGET)")
	$(call output,"\ \ \ - remove '$(CORELIB_UNIT_TEST_TARGET)' target output files","true")
	$(call output_g,"make clean_$(CORELIB_QUICK_START_TARGET)")
	$(call output," - remove '$(CORELIB_QUICK_START_TARGET)' target output files","true")
	$(call output,"","true")
	$(call output_g,"make clean_$(WRAPS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - remove all wrap targets[$(WRAPS_TARGET)] output files","true")
	$(call output_g,"make clean_$(PYWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(PYWRAP_TARGET)' target output files","true")
	$(call output_g,"make clean_$(CSWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(CSWRAP_TARGET)' target output files","true")
	$(call output_g,"make clean_$(LUWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(LUWRAP_TARGET)' target output files","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make install")
	$(call output,"\ \ \ \ \ \ \ \ \ \ - install c++ core library and all wrapped libraries to system directory","true")
	$(call output_g,"make install_$(CORELIB_TARGET)")
	$(call output," - install c++ core library to system directory","true")
	$(call output_g,"make install_$(WRAPS_TARGET)")
	$(call output,"\ \ \ \ - install all wrapped libraries to system directory","true")
	$(call output_g,"make install_$(PYWRAP_TARGET)")
	$(call output,"\ \ - install python language specified library to system directory","true")
	$(call output_g,"make install_$(CSWRAP_TARGET)")
	$(call output,"\ \ - install csharp language specified library to system directory","true")
	$(call output_g,"make install_$(LUWRAP_TARGET)")
	$(call output,"\ \ - install csharp language specified library to system directory","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make tar")
	$(call output," - tarball llbc framework","true")

all: $(PREMAKE_TARGET) $(CORELIB_TARGET) $(ALL_CORELIB_TEST_TARGETS) $(ALL_WRAP_TARGETS)

$(PREMAKE_TARGET):
	@cd $(PREMAKE_PATH) && ./$(PREMAKE_NAME) $(PREMAKE_ACTION)

$(CORELIB_TARGET): $(PREMAKE_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f llbc.make

$(CORELIB_TESTS_TARGET): $(ALL_CORELIB_TEST_TARGETS)
$(CORELIB_EXAMPLE_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f example.make
$(CORELIB_FUNC_TEST_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f func_test.make
$(CORELIB_UNIT_TEST_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f unit_test.make
$(CORELIB_QUICK_START_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f quick_start.make

$(WRAPS_TARGET): $(ALL_WRAP_TARGETS)
$(PYWRAP_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f pyllbc.make
$(CSWRAP_TARGET): $(CORELIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_native.make
	$(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc.make
	$(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_testsuite.make
$(LUWRAP_LUALIB_TARGET): $(PREMAKE_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_lualib.make
$(LUWRAP_LUAEXE_TARGET): $(LUWRAP_LUALIB_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_luaexec.make
$(LUWRAP_TARGET): $(CORELIB_TARGET) $(LUWRAP_LUALIB_TARGET) $(LUWRAP_LUAEXE_TARGET)
	$(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc.make

clean: $(addprefix clean_,$(CORELIB_TARGET) $(CORELIB_TESTS_TARGET) $(WRAPS_TARGET))
	@echo "Cleaning up temporary files ..."
	@find ./ \( -name "._*" -o -name ".DS_Store" -o -name ".*buildlog" \) -delete

clean_$(CORELIB_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/llbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f llbc.make clean; fi

clean_$(CORELIB_TESTS_TARGET): $(addprefix clean_,$(ALL_CORELIB_TEST_TARGETS))
clean_$(CORELIB_EXAMPLE_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/example.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f example.make clean; fi
clean_$(CORELIB_FUNC_TEST_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/func_test.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f func_test.make clean; fi
clean_$(CORELIB_UNIT_TEST_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/unit_test.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f unit_test.make clean; fi
	@echo "Cleaning up googletest build directory ..."
	@if [ -e tests/3rdparty/googletest/build ]; then \rm -rf tests/3rdparty/googletest/build ; fi
clean_$(CORELIB_QUICK_START_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/quick_start.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f quick_start.make clean; fi

clean_$(WRAPS_TARGET): $(addprefix clean_,$(ALL_WRAP_TARGETS))
clean_$(PYWRAP_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/pyllbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f pyllbc.make clean; fi
	@echo "Cleaning up cpython build targets ..."
	@if [ -f ./wrap/pyllbc/cpython/Makefile ]; then cd ./wrap/pyllbc/cpython && make clean; fi
	@echo "Cleaning up python cache files(.pyc/__pycache__) ..."
	@find ./wrap/pyllbc \( -type f -name "*.pyc" -o -type d -name "__pycache__" \) -delete
clean_$(CSWRAP_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/csllbc_native.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_native.make clean; fi
	@if [ -e build/$(PREMAKE_ACTION)/csllbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc.make clean; fi
	@if [ -e build/$(PREMAKE_ACTION)/csllbc_testsuite.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f csllbc_testsuite.make clean; fi
clean_$(LUWRAP_TARGET):
	@if [ -e build/$(PREMAKE_ACTION)/lullbc.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc.make clean; fi
	@if [ -e build/$(PREMAKE_ACTION)/lullbc_lualib.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_lualib.make clean; fi
	@if [ -e build/$(PREMAKE_ACTION)/lullbc_luaexec.make ]; then $(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_luaexec.make clean; fi

install: install_$(CORELIB_TARGET) install_$(WRAPS_TARGET)

install_$(CORELIB_TARGET):
	@if [ ! -e $(CORELIB_TARGET_PATH) ]; then echo "not found '$(CORELIB_TARGET)' targets, install failed!"; exit -1; fi
	@rm -rf /usr/include/llbc
	@cd $(ALL_TARGETS_OUTPUT) && \cp -rfv $(CORELIB_TARGET_NAME) /usr/lib
	@cd llbc/include && \cp -rfv llbc.h /usr/include
	@cd llbc/include && \rsync -av --exclude='*.svn' llbc /usr/include/

install_$(WRAPS_TARGET): install_$(CORELIB_TARGET) $(addprefix install_,$(ALL_WRAP_TARGETS))
install_$(PYWRAP_TARGET):
	@if [ ! -e $(PYWRAP_TARGET_PATH) ]; then echo "not found '$(PYWRAP_TARGET)' targets, install failed!"; exit -1; fi
	@cd $(ALL_TARGETS_OUTPUT) && \cp -rfv $(PYWRAP_TARGET_NAME) /usr/local/lib/python2.7/site-packages/
install_$(CSWRAP_TARGET):
	@echo "!!!csrarp library could not be install, please copy the libraries file to your project directory"
install_$(LUWRAP_TARGET):
	@echo "!!!luwrap library could not be install, please copy the libraries file to your project directory"
	
tar:
	@cd tools && python tar.py

