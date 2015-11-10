# rules.mk
# author: Longwei Lai<lailongwei@126.com>
#****************************************************************************
# This is a GNU make(gmake) makefile rules

#****************************************************************************
# platform type determine
# Now support operation system: Linux/Darwin
#****************************************************************************
SUPPORTED_PLATFORMS := linux darwin
PLATFORMNAME        ?= $(shell echo $(shell uname) | tr "[:upper:]" "[:lower:]")
$(if $(findstring $(PLATFORMNAME),$(SUPPORTED_PLATFORMS)),,$(error "Unsupported os, must be one of '$(SUPPORTED_PLATFORMS)'"))

#****************************************************************************
# compile commands
#****************************************************************************
ifeq ($(CCACHE),YES)
  CXX := ccache
endif

ifeq ($(GCC),YES)
  CXX := gcc
  OBJS = $(COBJS)
  DEPS = $(CDEPENDS)
else
  CXX := g++
  OBJS = $(CPPOBJS)
  DEPS = $(CPPDEPENDS)
  ifeq ($(CCPP),YES)
  OBJS += $(COBJS)
  DEPS += $(CDEPENDS)
  endif
endif

ifeq ($(CROSS),YES)
  CXX := $(ARMPATH)/arm-linux-$(CXX)
endif

LD := $(CXX)
AR := ar -rc

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************
COMMON_CXXFLAGS   := -DLINUX -Wall -rdynamic -fPIC
ifneq ($(CROSS),YES)
  COMMON_CXXFLAGS += -msse3
endif
ifeq ($(NOBUILTIN_MALLOC),YES)
  COMMON_CXXFLAGS += -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free
endif
ifneq ($(PROJECT_COMMON_CXXFLAGS),)
  COMMON_CXXFLAGS += $(PROJECT_COMMON_CXXFLAGS)
endif
DEBUG_CXXFLAGS    := $(COMMON_CXXFLAGS) -g -ggdb -DDEBUG
ifneq ($(PROJECT_DEBUG_CXXFLAGS),)
  DEBUG_CXXFLAGS  += $(PROJECT_DEBUG_CXXFLAGS)
endif
RELEASE_CXXFLAGS  := $(COMMON_CXXFLAGS) $(OPTIMIZATION)
ifneq ($(PROJECT_RELEASE_CXXFLAGS),)
  RELEASE_CXXFLAGS+= $(PROJECT_RELEASE_CXXFLAGS)
endif

COMMON_LDFLAGS    := -fPIC -rdynamic
ifneq ($(PROJECT_COMMON_LDFLAGS),)
  COMMON_LDFLAGS  += $(PROJECT_COMMON_LDFLAGS)
endif
DEBUG_LDFLAGS     := $(COMMON_LDFLAGS) -g -ggdb
ifneq ($(PROJECT_DEBUG_LDFLAGS),)
  DEBUG_LDFLAGS   += $(PROJECT_DEBUG_LDFLAGS)
endif
RELEASE_LDFLAGS   := $(COMMON_LDFLAGS)
ifneq ($(PROJECT_RELEASE_LDFLAGS),)
  RELEASE_LDFLAGS += $(PROJECT_RELEASE_LDFLAGS)
endif

CXXFLAGS = $(if $(DEBUG),$(DEBUG_CXXFLAGS),$(RELEASE_CXXFLAGS))
LDFLAGS  = $(if $(DEBUG),$(DEBUG_LDFLAGS),$(RELEASE_LDFLAGS))

#****************************************************************************
# real compile commands
#****************************************************************************
MAKEDEP    = $(CXX) -MM $(CXXFLAGS) $(DEFS) $(INCLS) $< > $@; \
             $(ECHO) -e '\t$(MAKEOBJ)' >> $@
MAKEOBJ    = $(CXX) $(CXXFLAGS) $(DEFS) $(INCLS) -c -o $(patsubst %.$(DEPENDSUFFIX),%.$(OBJSUFFIX),$@) $<
MAKELIB    = $(AR)
MAKEDLL    = $(LD) $(LDFLAGS) -shared
MAKEEXE    = $(LD) $(LDFLAGS)
ifeq ($(STATIC),YES)
  MAKEEXE += -static
endif
CREATEDIR = +@[ -d $(dir $@) ] || mkdir -p $(dir $@)

#****************************************************************************
# some useful commands
#****************************************************************************
CP   = cp -f
RM   = rm -rf
MV   = mv
ECHO = echo

#****************************************************************************
# some useful variables
#****************************************************************************
LIBPREFIX     = lib
DLLPREFIX     = lib
CSRCSUFFIX    = c
CPPSRCSUFFIX  = cpp
OBJSUFFIX     = o
LIBSUFFIX     = a
DEPENDSUFFIX  = d
EXESUFFIX     = exe
DLLSUFFIX     = $(if $(findstring $(PLATFORMNAME),linux),so,dylib)

DEBUGTARGET   = debug
RELEASETARGET = release
CONFIGURENAME = $(if $(DEBUG),$(DEBUGTARGET),$(RELEASETARGET))

DEBUGDEPENDTARGET   = debug_deps
RELEASEDEPENDTARGET = release_deps
DEPENDTARGET        = $(if $(MAKECMDGOALS),$(MAKECMDGOALS)_deps,$(RELEASEDEPENDTARGET))
CONFIGUREDEPENDNAME = $(if $(DEBUG),$(DEBUGDEPENDTARGET),$(RELEASEDEPENDTARGET))

PROJECTNAME  ?= $(shell basename $(subst ../,,$(subst src,,$(PROJECTDIR))))
OBJDIR        = $(PROJECTDIR)/$(PLATFORMNAME)/$(PROJECTNAME)
TARGET        = $(if $(MAKECMDGOALS),$(MAKECMDGOALS),)
TARGETSUFFIX  = $(patsubst %$(DEBUGTARGET),_$(DEBUGTARGET),$(filter %$(DEBUGTARGET),$(MAKECMDGOALS)))

ALLSRCDIRS    = $(filter-out $(OBJDIR)%,$(shell find $(PROJECTDIR) -type d ! -path *.svn* ! -path *.idea* ! -path *.xcodeproj* ! -path 3rd_party))
FILTEROUTDIRS = $(foreach DIR,$(NOTSRCDIRS),$(if $(findstring $(PROJECTDIR),$(DIR)),$(DIR),$(PROJECTDIR)/$(DIR)))
SRCDIRS       = $(filter-out $(NOTSRCDIRS),$(ALLSRCDIRS))

FILTOUTSRCS   = $(foreach FILE,$(NOTSRCFILES),$(if $(findstring $(PROJECTDIR),$(FILE)),$(FILE),$(PROJECTDIR)/$(FILE)))
CSRCS         = $(filter-out $(FILTOUTSRCS),$(foreach DIR,$(SRCDIRS),$(wildcard $(DIR)/*.$(CSRCSUFFIX))))
CPPSRCS       = $(filter-out $(FILTOUTSRCS),$(foreach DIR,$(SRCDIRS),$(wildcard $(DIR)/*.$(CPPSRCSUFFIX))))

CDEPENDS      = $(patsubst %.$(CSRCSUFFIX),%.$(DEPENDSUFFIX),$(notdir $(CSRCS)))
CPPDEPENDS    = $(patsubst %.$(CPPSRCSUFFIX),%.$(DEPENDSUFFIX),$(notdir $(CPPSRCS)))

COBJS         = $(patsubst %.$(CSRCSUFFIX),%.$(OBJSUFFIX),$(notdir $(CSRCS)))
CPPOBJS       = $(patsubst %.$(CPPSRCSUFFIX),%.$(OBJSUFFIX),$(notdir $(CPPSRCS)))

ifeq (DO_MAKE_DEPEND,$(MAKING_STATUS))
# Already enter object directory, do make depends
# Set some phonies
.PHONY: $(CONFIGUREDEPENDNAME)
# Set source directoy
VPATH=$(SRCDIRS)

# Implicity rules
# make the file from $(SRCDIRS)
# If the obj's directory is not created, create it
# -d means check exists
# || means when not exists (got false of the -d) do ...
# $(dir $@) means fetch the path of the output target
.SUFFIXES:
.SUFFIXES: .$(DEPENDSUFFIX) .$(CSRCSUFFIX) .$(CPPSRCSUFFIX)
.$(CSRCSUFFIX).$(DEPENDSUFFIX):
	$(MAKEDEP)

.$(CPPSRCSUFFIX).$(DEPENDSUFFIX):
	$(MAKEDEP)

# All configurations
$(RELEASEDEPENDTARGET) : $(DEPS)
$(DEBUGDEPENDTARGET)   : DEBUG:=YES
$(DEBUGDEPENDTARGET)   : $(DEPS)

endif

ifeq (DO_MAKE_TARGET,$(MAKING_STATUS))
# Already enter object directory, do make
# Set some phonies
.PHONY: all $(CONFIGURENAME)

# All configurations
all: $(CONFIGURENAME)

$(RELEASETARGET) : $(OUTPUT)
$(DEBUGTARGET)   : DEBUG:=YES
$(DEBUGTARGET)   : $(OUTPUT)

# Cleanup all implicity rules
.SUFFIXES:

#! For now, don't need %.o->%.c/.cpp implicity rules to build targets
#  Just simply include all depends files
include $(DEPS)

# Implicity rules
# make the file from $(SRCDIRS)
# If the obj's directory is not created, create it
# -d means check exists
# || means when not exists (got false of the -d) do ...
# $(dir $@) means fetch the path of the output target
# .SUFFIXES:
# .SUFFIXES: .$(OBJSUFFIX) .$(CSRCSUFFIX) .$(CPPSRCSUFFIX)
# .$(CSRCSUFFIX).$(OBJSUFFIX):
#   $(CREATEDIR)
#   $(MAKEOBJ)
# 
# .$(CPPSRCSUFFIX).$(OBJSUFFIX):
#   $(CREATEDIR)
#   $(MAKEOBJ)
endif

ifeq (undefined,$(origin MAKING_STATUS))
# In source directory now
# 1. cleanup all implicy rules
# 2. enter target directory & make by the first part depend files of makefile
# 3. enter target directory & make by the first part of makefile


# Cleanup all implicy rules
.SUFFIXES:

# Specify intermeidate directory
INTDIR = $(OBJDIR)/$(CONFIGURENAME)
# Fetch -f Makefile name
SPEC_MAKE_FILE := $(word 1,$(MAKEFILE_LIST))

# Make depends command
# 1. Select target directory
# 2. Using this makefile again (select the part by input MACRO: MAKING_STATUS)
# 3. Specify the MAKEDIR
# 4. Specify the making status (already in target dir)
# 5. Send the makeing goals to following operation
MAKEDEPEND = $(MAKE) --no-print-directory \
                     -C $(INTDIR) \
                     -f $(MAKEDIR)/$(SPEC_MAKE_FILE) \
                     INTDIR=$(INTDIR) \
                     MAKEDIR=$(MAKEDIR) \
                     MAKING_STATUS=DO_MAKE_DEPEND \
                     -j4 \
                     $(DEPENDTARGET)

# Make target command
# 1. Select target directory
# 2. Using this makefile again (select the part by input MACRO: MAKING_STATUS)
# 3. Specify the MAKEDIR
# 4. Specify the making status (already in target dir)
# 5. Send the makeing goals to following operation
MAKETARGET = $(MAKE) --no-print-directory \
                     -C $(INTDIR) \
                     -f $(MAKEDIR)/$(SPEC_MAKE_FILE) \
                     INTDIR=$(INTDIR) \
                     MAKEDIR=$(MAKEDIR) \
                     MAKING_STATUS=DO_MAKE_TARGET \
					 -j4 \
					 $(TARGET)

# Configrations
$(RELEASETARGET) : all
$(DEBUGTARGET)   : DEBUG:=YES
$(DEBUGTARGET)   : all


# Common target
all: $(DEPENDTARGET)
	@mkdir -p $(INTDIR)
	@$(MAKETARGET)

$(DEPENDTARGET):
	@mkdir -p $(INTDIR)
	@$(MAKEDEPEND)

clean:clean$(RELEASETARGET)
cleanall:clean$(RELEASETARGET) clean$(DEBUGTARGET)
clean$(RELEASETARGET):
	$(RM) $(OBJDIR)/$(RELEASETARGET) $(OUTPUT)
clean$(DEBUGTARGET):
	$(RM) $(OBJDIR)/$(DEBUGTARGET) $(OUTPUT)
dclean:dclean$(RELEASETARGET)
dclean$(DEBUGTARGET):
	$(RM) $(addprefix $(OBJDIR)/$(DEBUGTARGET)/,$(DEPS))
dclean$(RELEASETARGET):
	$(RM) $(addprefix $(OBJDIR)/$(RELEASETARGET)/,$(DEPS))

oclean:oclean$(RELEASETARGET)
oclean$(DEBUGTARGET):
	$(RM) $(addprefix $(OBJDIR)/$(DEBUGTARGET)/,$(OBJS))
oclean$(RELEASETARGET):
	$(RM) $(addprefix $(OBJDIR)/$(RELEASETARGET)/,$(OBJS))

echo:
	@$(ECHO) "object directory:"
	@$(ECHO) $(OBJDIR)
	@$(ECHO) "csrcs:"
	@$(ECHO) $(CSRCS)
	@$(ECHO) "cppsrcs:"
	@$(ECHO) $(CPPSRCS)
	@$(ECHO) "depends:"
	@$(ECHO) $(DEPS)
	@$(ECHO) "objects:"
	@$(ECHO) $(OBJS)
	@$(ECHO) "targets:"
	@$(ECHO) $(OUTPUT)
	@$(ECHO) "file out dirs:"
	@$(ECHO) $(FILTOUTDIRS)
	@$(ECHO) "CONFIGURENAME:$(CONFIGURENAME)"

info:
	@$(ECHO) "make commands:"
	@$(ECHO) "  make               - same with 'make $(RELEASETARGET)'"
	@$(ECHO) "  make all           - same with 'make'"
	@$(ECHO) "  make $(RELEASETARGET)       - compile $(RELEASETARGET) program"
	@$(ECHO) "  make $(DEBUGTARGET)         - compile $(DEBUGTARGET) program"
	@$(ECHO) "  make clean         - same with 'make clean$(RELEASETARGET)'"
	@$(ECHO) "  make cleanall      - remove all object directories and target files"
	@$(ECHO) "  make clean$(RELEASETARGET)  - remove $(RELEASETARGET) object directory"
	@$(ECHO) "  make clean$(DEBUGTARGET)    - remove $(DEBUGTARGET) object directory"
	@$(ECHO) "  make dclean        - same with 'make oclean$(RELEASETARGET)'"
	@$(ECHO) "  make dclean$(RELEASETARGET) - remove $(RELEASETARGET) dependent files"
	@$(ECHO) "  make dclean$(DEBUGTARGET)   - remove $(DEBUGTARGET) dependent files"
	@$(ECHO) "  make oclean        - same with 'make oclean$(RELEASETARGET)'"
	@$(ECHO) "  make oclean$(RELEASETARGET) - remove $(RELEASETARGET) object files"
	@$(ECHO) "  make oclean$(DEBUGTARGET)   - remove $(DEBUGTARGET) object files"
	@$(ECHO) "  make echo          - print object files and target files"
	@$(ECHO) "  make info          - display this infomation"
endif

