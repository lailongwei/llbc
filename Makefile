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
  export config=Release
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
SUPPORTED_CONFIGS   :=Debug,Release
$(if $(findstring $(config),$(SUPPORTED_CONFIGS)),,$(error "Unsupported config, must be one of'$(SUPPORTED_CONFIGS)'))

# Define build directory & build type.
ifeq ($(config),Release)
  BUILD_DIR=build
  CMAKE_BUILD_TYPE=Release
else
  BUILD_DIR=build_debug
  CMAKE_BUILD_TYPE=Debug
endif

# All make targets define.
PREMAKE_TARGET  			:= build_makefiles

CORELIB_TARGET    			:= core_lib
CORELIB_TESTS_TARGET 		:= tests
CORELIB_EXAMPLE_TARGET 		:= example
CORELIB_FUNC_TEST_TARGET  	:= func_test
CORELIB_UNIT_TEST_TARGET  	:= unit_test
ALL_CORELIB_TEST_TARGETS    := $(CORELIB_EXAMPLE_TARGET) $(CORELIB_FUNC_TEST_TARGET) $(CORELIB_UNIT_TEST_TARGET)

WRAPS_TARGET      			:= wraps
PYWRAP_TARGET       		:= py_wrap
CSWRAP_TARGET       		:= cs_wrap
LUWRAP_TARGET       		:= lu_wrap
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

# Some variables define.
PREMAKE_PATH := "tools/premake"
PREMAKE_NAME := "premake5_$(SYSTEM_NAME)_$(ARCHITECTURE_NAME)"

#****************************************************************************
# Useful functions.
#****************************************************************************
# log functions.
define output_r
	@if [ "$(2)" = "true" ]; then \
		printf "\033[31m%s\033[0m\n" "$(1)"; \
	else \
		printf "\033[31m%s\033[0m" "$(1)"; \
	fi
endef
define output_rg
	@if [ "$(2)" = "true" ]; then \
		printf "\033[1;31m%s\033[0m\n" "$(1)"; \
	else \
		printf "\033[1;31m%s\033[0m" "$(1)"; \
	fi

endef

define output_g
	@if [ "$(2)" = "true" ]; then \
		printf "\033[32m%s\033[0m\n" "$(1)"; \
	else \
		printf "\033[32m%s\033[0m" "$(1)"; \
	fi
endef
define output_hg
	@if [ "$(2)" = "true" ]; then \
		printf "\033[1;32m%s\033[0m\n" "$(1)"; \
	else \
		printf "\033[1;32m%s\033[0m" "$(1)"; \
	fi
endef

define output_y
	@if [ "$(2)" = "true" ]; then \
		printf "\033[33m%s\033[0m\n" "$(1)"; \
	else \
		printf "\033[33m%s\033[0m" "$(1)"; \
	fi
endef
define output_yg
	@if [ "$(2)" = "true" ]; then \
		printf "\033[1;33m%s\033[0m"\n "$(1)"; \
	else \
		printf "\033[1;33m%s\033[0m" "$(1)"; \
	fi
endef

define output
	@echo "$(1)"
endef

#****************************************************************************
# all real make commands.
#****************************************************************************
.PHONY: help all install clean tar $(PREMAKE_TARGET)
help:
	$(call output_hg,"Makefile\ commands:","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make\ [help]")
	$(call output," - display this help information","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make\ all")
	$(call output," - make [core library/core library tests/wrapped libraries]","true")
	$(call output,"","true")
	$(call output_g,"make\ $(CORELIB_TARGET)")
	$(call output,"\ \ \ \ - make c++ core library","true")
	$(call output,"","true")
	$(call output_g,"make\ $(CORELIB_TESTS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - make c++ core library all tests[$(ALL_CORELIB_TEST_TARGETS)]","true")
	$(call output_g,"make\ $(CORELIB_EXAMPLE_TARGET)")
	$(call output,"\ \ \ \ \ - make c++ core library test target: $(CORELIB_EXAMPLE_TARGET)","true")
	$(call output_g,"make\ $(CORELIB_FUNC_TEST_TARGET)")
	$(call output,"\ \ \ - make c++ core library test target: $(CORELIB_FUNC_TEST_TARGET)","true")
	$(call output_g,"make\ $(CORELIB_UNIT_TEST_TARGET)")
	$(call output,"\ \ \ - make c++ core library test target: $(CORELIB_UNIT_TEST_TARGET)","true")
	$(call output,"","true")
	$(call output_g,"make\ $(WRAPS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - make all language specificed warpped libraries[$(ALL_WRAP_TARGETS)]","true")
	$(call output_g,"make\ $(PYWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - make python wrapped library","true")
	$(call output_g,"make\ $(CSWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - make csharp wrapped library","true")
	$(call output_g,"make\ $(LUWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - make lua wrapped library","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make\ clean")
	$(call output,"\ \ \ \ \ \ \ \ \ \ \ \ \ - clean all target files","true")
	$(call output,"","true")
	$(call output_g,"make\ clean_$(CORELIB_TARGET)")
	$(call output,"\ \ \ \ - remove c++ core library target output files","true")
	$(call output,"","true")
	$(call output_g,"make\ clean_$(CORELIB_TESTS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - remove c++ core library all test targets[$(ALL_CORELIB_TEST_TARGETS)] output files","true")
	$(call output_g,"make\ clean_$(CORELIB_EXAMPLE_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(CORELIB_EXAMPLE_TARGET)' target output files","true")
	$(call output_g,"make\ clean_$(CORELIB_FUNC_TEST_TARGET)")
	$(call output,"\ \ \ - remove '$(CORELIB_FUNC_TEST_TARGET)' target output files","true")
	$(call output_g,"make\ clean_$(CORELIB_UNIT_TEST_TARGET)")
	$(call output,"\ \ \ - remove '$(CORELIB_UNIT_TEST_TARGET)' target output files","true")
	$(call output,"","true")
	$(call output_g,"make\ clean_$(WRAPS_TARGET)")
	$(call output,"\ \ \ \ \ \ \ - remove all wrap targets[$(WRAPS_TARGET)] output files","true")
	$(call output_g,"make\ clean_$(PYWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(PYWRAP_TARGET)' target output files","true")
	$(call output_g,"make\ clean_$(CSWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(CSWRAP_TARGET)' target output files","true")
	$(call output_g,"make\ clean_$(LUWRAP_TARGET)")
	$(call output,"\ \ \ \ \ - remove '$(LUWRAP_TARGET)' target output files","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make\ install")
	$(call output,"\ \ \ \ \ \ \ \ \ \ - install c++ core library and all wrapped libraries to system directory","true")
	$(call output_g,"make\ install_$(CORELIB_TARGET)")
	$(call output," - install c++ core library to system directory","true")
	$(call output_g,"make\ install_$(WRAPS_TARGET)")
	$(call output,"\ \ \ \ - install all wrapped libraries to system directory","true")
	$(call output_g,"make\ install_$(PYWRAP_TARGET)")
	$(call output,"\ \ - install python language specified library to system directory","true")
	$(call output_g,"make\ install_$(CSWRAP_TARGET)")
	$(call output,"\ \ - install csharp language specified library to system directory","true")
	$(call output_g,"make\ install_$(LUWRAP_TARGET)")
	$(call output,"\ \ - install csharp language specified library to system directory","true")
	$(call output,"=========================================================================","true")
	$(call output_g,"make\ tar")
	$(call output," - tarball llbc framework","true")

all: $(PREMAKE_TARGET) $(CORELIB_TARGET) $(ALL_CORELIB_TEST_TARGETS) $(ALL_WRAP_TARGETS)

$(PREMAKE_TARGET):
	@echo "CMAKE_BUILD_TYPE IS: ${CMAKE_BUILD_TYPE}"
	@mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) ..

$(CORELIB_TARGET): $(PREMAKE_TARGET)
	@cd $(BUILD_DIR) && $(MAKE) llbc_shared
	@cd $(BUILD_DIR) && $(MAKE) llbc_static

$(CORELIB_TESTS_TARGET): $(ALL_CORELIB_TEST_TARGETS)
$(CORELIB_EXAMPLE_TARGET): $(CORELIB_TARGET)
	@cd $(BUILD_DIR) && $(MAKE) example_shared
	@cd $(BUILD_DIR) && $(MAKE) example_static
$(CORELIB_FUNC_TEST_TARGET): $(CORELIB_TARGET)
	@cd $(BUILD_DIR) && $(MAKE) func_test_shared
	@cd $(BUILD_DIR) && $(MAKE) func_test_static
$(CORELIB_UNIT_TEST_TARGET): $(CORELIB_TARGET)
	@cd $(BUILD_DIR) && $(MAKE) unit_test_shared
	@cd $(BUILD_DIR) && $(MAKE) unit_test_static

$(WRAPS_TARGET): $(ALL_WRAP_TARGETS)
$(PYWRAP_TARGET): $(CORELIB_TARGET)
	@cd $(BUILD_DIR) && $(MAKE) pyllbc
$(CSWRAP_TARGET): $(CORELIB_TARGET)
	@cd $(BUILD_DIR) && $(MAKE) csllbc
	@cd $(BUILD_DIR) && $(MAKE) csllbc_testsuite
	$(MAKE) -C build/$(PREMAKE_ACTION) -f lullbc_luaexec.make
$(LUWRAP_TARGET): $(CORELIB_TARGET)
	@cd $(BUILD_DIR) && $(MAKE) lullbc

clean: $(addprefix clean_,$(CORELIB_TARGET) $(CORELIB_TESTS_TARGET) $(WRAPS_TARGET))
	@echo "Cleaning up temporary files ..."
	@find ./ \( -name "._*" -o -name ".DS_Store" -o -name ".*buildlog" \) -delete

clean_$(CORELIB_TARGET):
	@if [ -e $(BUILD_DIR)/llbc/Makefile ]; then cd $(BUILD_DIR)/llbc && $(MAKE) clean; fi

clean_$(CORELIB_TESTS_TARGET): $(addprefix clean_,$(ALL_CORELIB_TEST_TARGETS))
clean_$(CORELIB_EXAMPLE_TARGET):
	@if [ -e $(BUILD_DIR)/tests/example/Makefile ]; then cd $(BUILD_DIR)/tests/example && $(MAKE) clean; fi
clean_$(CORELIB_FUNC_TEST_TARGET):
	@if [ -e $(BUILD_DIR)/tests/func_test/Makefile ]; then cd $(BUILD_DIR)/tests/func_test && $(MAKE) clean; fi
clean_$(CORELIB_UNIT_TEST_TARGET):
	@if [ -e $(BUILD_DIR)/tests/unit_test/Makefile ]; then cd $(BUILD_DIR)/tests/unit_test && $(MAKE) clean; fi

clean_$(WRAPS_TARGET): $(addprefix clean_,$(ALL_WRAP_TARGETS))
clean_$(PYWRAP_TARGET):
	@if [ -e $(BUILD_DIR)/wrap/pyllbc/Makefile ]; then cd $(BUILD_DIR)/wrap/pyllbc && $(MAKE) clean; fi
	@echo "Cleaning up cpython build targets ..."
	@if [ -f ./wrap/pyllbc/cpython/Makefile ]; then cd ./wrap/pyllbc/cpython && make clean; fi
	@echo "Cleaning up python cache files(.pyc/__pycache__) ..."
	@find ./wrap/pyllbc \( -type f -name "*.pyc" -o -type d -name "__pycache__" \) -delete
clean_$(CSWRAP_TARGET):
	@if [ -e $(BUILD_DIR)/wrap/csllbc/Makefile ]; then cd $(BUILD_DIR)/wrap/csllbc && $(MAKE) clean; fi
clean_$(LUWRAP_TARGET):
	@if [ -e $(BUILD_DIR)/wrap/lullbc/Makefile ]; then cd $(BUILD_DIR)/wrap/lullbc && $(MAKE) clean; fi

install:
	@echo "Not supported for now"

tar:
	@echo "Not supported for now"

