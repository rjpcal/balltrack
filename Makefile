#########################################################################
#
# Makefile for Balls3 psychophysics experiment
#
# Rob Peters rjpeters@klab.caltech.edu
# created: Feb 2000
# $Id$
#
#########################################################################

prefix := ./

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

TRAIN_TARGET := $(prefix)/bin/balls3_train
ITRK_TARGET := $(prefix)/bin/balls3_itrk
FMRI_TARGET := $(prefix)/bin/balls3_fmri
MOVIE_TARGET := $(prefix)/bin/balls3_movie

ALL   := $(TRAIN_TARGET) $(ITRK_TARGET) $(FMRI_TARGET) $(MOVIE_TARGET)

# Level 0
APPLICATION_H := application.h
BALLS_H := balls.h
DEBUG_H := debug.h
DEFS_H := defs.h
GLFONT_H := glfont.h
PARAMS_H := params.h
SIMPLEMOVIE_H := simplemovie.h
TIMING_H := timing.h
TRACE_H := trace.h
XHINTS_H := xhints.h
XSTUFF_H := xstuff.h

# Level 1
MENUAPP_H := $(APPLICATION_H) menuapp.h
GRAPHICS_H := $(DEFS_H) graphics.h

# Level 2
BALLSEXPT_H := $(MENUAPP_H) ballsexpt.h
OPENGLGFX_H := $(GRAPHICS_H) openglgfx.h

MENUAPP_CC := $(MENUAPP_H) $(DEFS_H) $(GRAPHICS_H) \
	$(PARAMS_H) $(TRACE_H) $(DEBUG_H) menuapp.cc

APPLICATION_CC := $(APPLICATION_H) $(DEFS_H) $(OPENGLGFX_H) \
	$(PARAMS_H) $(STARBASEGFX_H) $(TIMING_H) $(XHINTS_H) \
	$(XSTUFF_H) $(TRACE_H) $(DEBUG_H) application.cc

BALLS_CC := $(BALLS_H) $(DEFS_H) $(GRAPHICS_H) $(PARAMS_H) $(TIMING_H) \
	$(TRACE_H) $(DEBUG_H) balls.cc

BALLSEXPT_CC := $(BALLSEXPT_H) $(BALLS_H) $(GRAPHICS_H) $(PARAMS_H) \
	$(TIMING_H) $(XHINTS_H) $(TRACE_H) $(DEBUG_H) ballsexpt.cc

GLFONT_CC := $(GLFONT_H) $(TRACE_H) $(DEBUG_H) glfont.cc

GRAPHICS_CC := $(GRAPHICS_H) $(TIMING_H) $(TRACE_H) $(TIMING_H) graphics.cc

MAIN_CC := $(BALLSEXPT_H) $(PARAMS_H) $(XHINTS_H) main.cc

OPENGLGFX_CC := $(OPENGLGFX_H) $(GLFONT_H) $(PARAMS_H) \
	$(SIMPLEMOVIE_H) $(TIMING_H) $(XHINTS_H) $(XSTUFF_H) \
	$(TRACE_H) $(DEBUG_H) openglgfx.cc

PARAMS_CC := $(PARAMS_H) $(APPLICATION_H) $(DEFS_H) $(GRAPHICS_H) \
	$(TRACE_H) $(DEBUG_H) params.cc

SIMPLEMOVIE_CC := $(SIMPLEMOVIE_H) simplemovie.cc

TIMING_CC := $(TIMING_H) $(APPLICATION_H) $(DEFS_H) $(PARAMS_H) \
	$(DEBUG_H) $(TRACE_H) timing.cc

TRACE_CC := $(TRACE_H) trace.cc

XSTUFF_CC := $(XSTUFF_H) $(DEFS_H) $(XHINTS_H) $(TRACE_H) $(DEBUG_H) xstuff.cc

#
# Targets
#

all:	dirs $(ALL)

dirs:
	mkdir -p $(objdir)

mvtest: mvtest.cc
	$(CXX) $(CFLAGS) -o mvtest simplemovie.cc mvtest.cc -lmoviefile -ldmedia

clean:
	rm -f core *.o *.a $(ALL)

$(ITRK_TARGET): $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_EYE_TRACKING main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(FMRI_TARGET): $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_FMRI_SESSION main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(TRAIN_TARGET): $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_TRAINING main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(MOVIE_TARGET): $(OBJ) $(MAIN_CC)
	mkdir -p $(dir $@)
	time $(CXX) $(CFLAGS) -DMODE_FMRI_SESSION -DMAKE_MOVIE main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

$(objdir)/%.o : %.cc
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
