#########################################################################
#
# Makefile for Balls3 psychophysics experiment
#
# Rob Peters rjpeters@klab.caltech.edu
# created: Feb 2000
# $Id$
#
#########################################################################

prefix := .
objdir := obj
srcdir := .

CXX      := g++
CXXFLAGS := -O2 -Wall -Werror
LDFLAGS  := -L/usr/X11R6/lib
LIBS     := -lGLU -lGL -lX11 -lm

#
# Targets
#

ALL_SRCS := $(shell find $(srcdir) -follow -name \*.h -or -name \*.cc -or -name \*.C)

default: all

all: $(prefix)/bin/balltrack

cppdeps: cppdeps.cc
	$(CXX) -O2 -Wall $^ -o $@

alldepends: cppdeps $(ALL_SRCS)
	time ./cppdeps \
		--objdir $(objdir)/ --objext .o \
		--output-compile-deps \
		--output-link-deps \
		--linkformat ":$(objdir)/*.o" \
		--exeformat "main.cc:$(prefix)/bin/balltrack" \
		--srcdir $(srcdir) > $@.tmp
	mv $@.tmp $@

include alldepends

$(prefix)/bin/%:
	mkdir -p $(dir $@)
	time $(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS)

$(objdir)/%.o : %.cc
	@mkdir -p $(objdir)
	time $(CXX) $(CXXFLAGS) \
		-c $< \
		-o $@

TAGS: $(ALL_SRCS)
	etags $+

DISTNAME := balltrack_$(shell date +%Y%m%d)
CVS_ROOT := $(shell test -e ./CVS/Root && cat ./CVS/Root)
CVS_REPO := $(shell test -e ./CVS/Root && cat ./CVS/Repository)

export:
	echo $(DISTNAME)
	mkdir -p snapshots
	cd snapshots && \
	  cvs -z3 -d $(CVS_ROOT) export -r HEAD -d $(DISTNAME) $(CVS_REPO) \
	  && tar cvfz $(DISTNAME).tar.gz $(DISTNAME)
