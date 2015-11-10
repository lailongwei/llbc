# File:  Makefile
# Brief: easy to build llbc library and testsuite.
#############################################################################

#****************************************************************************
# Determine platform name
#****************************************************************************
OS = $(shell uname)
PLATFORMNAME=linux

#****************************************************************************
# some useful variables
#****************************************************************************
CORELIB_TARGET  := core_lib
TEST_TARGET     := test
WRAPS_TARGET    := wraps

PYWRAP_TARGET   := py_wrap

ALL_WRAP_TARGETS:= $(PYWRAP_TARGET)

#****************************************************************************
# all real make commands
#****************************************************************************
# Set phonies
.PHONY: help all install clean tar
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
	@echo "========================================================================="
	@echo "  make clean          - remove all object directories and target files"
	@echo "  make clean_$(CORELIB_TARGET) - remove all '$(CORELIB_TARGET)' object directories and target files"
	@echo "  make clean_$(TEST_TARGET)     - remove all '$(TEST_TARGET)' object directories and target files"
	@echo "  make clean_$(WRAPS_TARGET)    - remove all '$(WRAPS_TARGET)' object directories and target files"
	@echo "  make clean_$(PYWRAP_TARGET)  - remove all '$(PYWRAP_TARGET)' object directories and target files"
	@echo "========================================================================="
	@echo "  make install          - install c++ core library and all wrapped libraries to system directory"
	@echo "  make install_$(CORELIB_TARGET) - install c++ core library to system directory"
	@echo "  make install_$(WRAPS_TARGET)    - install all wrapped libraries to system directory"
	@echo "  make install_$(PYWRAP_TARGET)  - install python language specified library to system directory"
	@echo "========================================================================="
	@echo "  make tar      - tarball llbc framework(included core library, testsuite"
	@echo "                  codes and all language specificed wrapped libraries"

all: $(CORELIB_TARGET) $(TEST_TARGET) $(ALL_WRAP_TARGETS)

$(CORELIB_TARGET):
	@(cd llbc && $(MAKE))

$(TEST_TARGET): $(CORELIB_TARGET)
	@(cd testsuite && $(MAKE))

$(WRAPS_TARGET): $(ALL_WRAP_TARGETS)
$(PYWRAP_TARGET): $(CORELIB_TARGET)
	@(cd wrap/pyllbc && $(MAKE) -f Makefile)

clean: $(addprefix clean_,$(CORELIB_TARGET) $(TEST_TARGET) $(WRAPS_TARGET))
	@$(shell find ./ -name "._*" -exec rm {} \;)
	@$(shell find ./ -name ".DS_Store" -exec rm {} \;)
	@$(shell find ./ -type f -name "*.buildlog" -exec rm {} \;)

clean_$(CORELIB_TARGET):
	@(cd llbc && $(MAKE) clean)
	@(cd llbc && $(MAKE) cleandebug)
	@rm -rf llbc/$(PLATFORMNAME) llbc/lib

clean_$(TEST_TARGET):
	@(cd testsuite && $(MAKE) clean)
	@(cd testsuite && $(MAKE) cleandebug)
	@rm -rf testsuite/$(PLATFORMNAME) testsuite/bin

clean_$(WRAPS_TARGET): $(addprefix clean_,$(ALL_WRAP_TARGETS))
clean_$(PYWRAP_TARGET):
	@(cd wrap/pyllbc && $(MAKE) clean)
	@(cd wrap/pyllbc && $(MAKE) cleandebug)
	@rm -rf wrap/pyllbc/$(PLATFORMNAME) wrap/pyllbc/lib
	@$(shell find ./wrap/pyllbc -type f -name "*.pyc" -exec rm {} \;)

install: install_$(CORELIB_TARGET) install_$(WRAPS_TARGET)

install_$(CORELIB_TARGET):
	@(rm -rf /usr/include/llbc)
	@(cd llbc/lib && \cp -rfv libllbc* /usr/lib)
	@(cd llbc/include && \cp -rfv llbc.h /usr/include)
	@(cd llbc/include && \rsync -av --exclude='*.svn' llbc /usr/include/)

install_$(WRAPS_TARGET): install_$(CORELIB_TARGET) $(addprefix install_,$(ALL_WRAP_TARGETS))
install_$(PYWRAP_TARGET):
	@(cd wrap/pyllbc/lib && \cp -rfv llbc.so /usr/local/lib/python2.7/site-packages/)
	
tar: clean
	@(cd .. && tar -p --exclude "*.svn" -jcvf llbc_$(shell date +%Y%m%d_%H%M%S).tar.bz2 $(notdir $(shell pwd)))

