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

CXX    := g++
CFLAGS := -O2 -Wall -Werror
LFLAGS := -L/usr/X11R6/lib
LIB    := -lGLU -lGL -lX11 -lm

OBJ   := \
$(objdir)/application.o \
$(objdir)/balls.o \
$(objdir)/ballsexpt.o \
$(objdir)/glfont.o \
$(objdir)/graphics.o \
$(objdir)/menuapp.o \
$(objdir)/openglgfx.o \
$(objdir)/params.o \
$(objdir)/simplemovie.o \
$(objdir)/timing.o \
$(objdir)/trace.o \
$(objdir)/xstuff.o \

ALL_SRCS := $(shell find $(SRC) -follow -name \*.h -or -name \*.cc -or -name \*.C)

#
# Targets
#

default: all

all: \
$(prefix)/bin/balls3_train \
$(prefix)/bin/balls3_itrk \
$(prefix)/bin/balls3_fmri \
$(prefix)/bin/balls3_movie \

cppdeps: cppdeps.cc
	$(CXX) -O2 -Wall $^ -o $@

alldepends: cppdeps $(ALL_SRCS)
	time ./cppdeps \
		--objdir $(objdir)/ --objext .o \
		--output-compile-deps \
		--output-link-deps \
		--srcdir ./ | sort > $@


#		--linkformat ":$(objdir)/*.o" \
#		--exeformat "main.cc:$(prefix)/bin/balls3_train" \

include alldepends

mvtest: mvtest.cc
	$(CXX) $(CFLAGS) -o mvtest simplemovie.cc mvtest.cc -lmoviefile -ldmedia

$(prefix)/bin/balls3_itrk: $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_EYE_TRACKING main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(prefix)/bin/balls3_fmri: $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_FMRI_SESSION main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(prefix)/bin/balls3_train: $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_TRAINING main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(prefix)/bin/balls3_movie: $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_FMRI_SESSION -DMAKE_MOVIE main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(objdir)/%.o : %.cc
	@mkdir -p $(objdir)
	time $(CXX) $(CFLAGS) \
		-c $< \
		-o $@

$(objdir)/application.o: $(APPLICATION_CC)
$(objdir)/balls.o: $(BALLS_CC)
$(objdir)/ballsexpt.o: $(BALLSEXPT_CC)
$(objdir)/glfont.o: $(GLFONT_CC)
$(objdir)/graph ics.o: $(GRAPHICS_CC)
$(objdir)/menuapp.o: $(MENUAPP_CC)
$(objdir)/openglgfx.o: $(OPENGLGFX_CC)
$(objdir)/params.o: $(PARAMS_CC)
$(objdir)/simplemovie.o: $(SIMPLEMOVIE_CC)
$(objdir)/timing.o: $(TIMING_CC)
$(objdir)/trace.o: $(TRACE_CC)
$(objdir)/xstuff.o: $(XSTUFF_CC)

TAGS: *.h *.cc
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
