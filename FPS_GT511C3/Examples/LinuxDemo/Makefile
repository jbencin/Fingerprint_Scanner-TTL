############################################################
# fpsdemo makefile
############################################################

all : build

############################################################
# Config
############################################################

NAME     := fpsdemo

BASEDIR  := $(shell pwd)
SRCDIRS  := $(BASEDIR) $(BASEDIR)/../..
OUTDIR   := $(BASEDIR)/output
OBJDIR   := $(OUTDIR)/obj

INCLUDE  := $(SRCDIRS:%=-I%)
CC       := gcc
CCFLAGS  += -O3 -fno-exceptions -fno-rtti

CXX      := g++
CXXFLAGS += $(CCFLAGS) -std=c++11

LIBS      =
LDFLAGS  +=

OBJSRAW  := main.o FPS_GT511C3.o SerialDevice.o
OBJS     := $(OBJSRAW:%=$(OBJDIR)/%)

# Overrides settings above
-include .config

############################################################
#
#  Implicit rules.
#  .d files are dependecies per source file
#       Dependency files set the .o and .d files to depend
#       on the same source files. If any of those are
#       changed, both the .o and .d will be rebuilt.
#
############################################################

define DEPCODE
$$(OBJDIR)/%.o: $(1)/%.c | $$(OBJDIR)
	@echo Compiling $$<...
	@$$(CC) $$(CCFLAGS) $$(CCDEFINES) $$(INCLUDE) -c -o $$@ $$<
	@$$(CC) -M $$(CCFLAGS) $$(CCDEFINES) $$(INCLUDE) $$< \
		| sed -e's!\(.*:\)!$$(OBJDIR)/\1 Makefile !' \
		| tee $$(OBJDIR)/$$*.d \
		| sed -e 's!.*:!!' -e 's!\\$$$$!!' \
		| fmt -1 \
		| sed -e 's!^ *!!' -e 's!$$$$!:!' \
		>> $$(OBJDIR)/$$*.d

$$(OBJDIR)/%.o: $(1)/%.cpp | $$(OBJDIR)
	@echo Compiling $$<...
	@$$(CXX) $$(CXXFLAGS) $$(CXXDEFINES) $$(INCLUDE) -c -o $$@ $$<
	@$$(CXX) -M $$(CXXFLAGS) $$(CXXDEFINES) $$(INCLUDE) $$< \
		| sed -e's!\(.*:\)!$$(OBJDIR)/\1 Makefile !' \
		| tee $$(OBJDIR)/$$*.d \
		| sed -e 's!.*:!!' -e 's!\\$$$$!!' \
		| fmt -1 \
		| sed -e 's!^ *!!' -e 's!$$$$!:!' \
		>> $$(OBJDIR)/$$*.d
endef

# Make some code from stuff above
$(foreach SRCDIR, $(SRCDIRS), $(eval $(call DEPCODE,$(SRCDIR))))

############################################################
#  Build
############################################################

# Include dependency files.
-include $(OBJS:.o=.d)

.PHONY: dirs $(NAME) build

$(OBJDIR):
	@mkdir -p $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

build: dirs $(NAME)
	
############################################################
#  Clean
############################################################
	
.PHONY: clean distclean

clean: 
	-rm -rf $(NAME) $(OUTDIR)

distclean: clean
	rm -rf .config
