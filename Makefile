#########################################################################
#
# Makefile for Balls3 psychophysics experiment
#
# Rob Peters rjpeters@klab.caltech.edu
# created: Feb 2000
# $Id$
#
#########################################################################

prefix := $(HOME)/local/$(ARCH)/bin/
exec_prefix := $(prefix)

ifeq ($(ARCH),hp9000s700)
	CC  := aCC
	CFLAGS := +W302,495,829,749,740 -I/usr/include/X11R6 -I/usr/include \
		-I/cit/rjpeters/include -DHP9000S700
	LFLAGS := -L/usr/lib/X11R6 -L/opt/graphics/common/lib -L/opt/graphics/OpenGL/lib
	LIB   := -lGLU -lGL -lXwindow -lsb -lXhp11 -lX11 -lm -ldld
	GFXOBJ := $(ARCH)/starbasegfx.o
endif

ifeq ($(ARCH),irix6)
	CC  := /opt/MIPSpro/bin/CC
	CFLAGS :=  -I/usr/include/X11R6 -I/cit/rjpeters/include \
		-I/cit/rjpeters/include/cppheaders -DIRIX6
	LFLAGS :=
	LIB   := -lmoviefile -ldmedia -lGLU -lGL -lX11 -lm
	GFXOBJ := $(ARCH)/glfont.o $(ARCH)/openglgfx.o
endif

ifeq ($(ARCH),i686)
	CC  := g++
	CFLAGS :=  -DI686 -Wall -Werror
	LFLAGS := -L/usr/X11R6/lib
	LIB   := -lGLU -lGL -lX11 -lm
	GFXOBJ := $(ARCH)/glfont.o $(ARCH)/openglgfx.o
endif

OBJ   := \
	$(ARCH)/application.o \
	$(ARCH)/balls.o \
	$(ARCH)/ballsexpt.o \
	$(ARCH)/graphics.o \
	$(ARCH)/menuapp.o \
	$(ARCH)/params.o \
	$(ARCH)/simplemovie.o \
	$(ARCH)/timing.o \
	$(ARCH)/trace.o \
	$(ARCH)/xstuff.o \
	$(GFXOBJ)

TRAIN_TARGET := $(prefix)/balls3_train
ITRK_TARGET := $(prefix)/balls3_itrk
FMRI_TARGET := $(prefix)/balls3_fmri
MOVIE_TARGET := $(prefix)/balls3_movie

ifeq ($(ARCH),hp9000s700)
	ALL   := $(TRAIN_TARGET) $(ITRK_TARGET) $(FMRI_TARGET) $(MOVIE_TARGET) serial
endif
ifeq ($(ARCH),irix6)
	ALL   := $(TRAIN_TARGET) $(ITRK_TARGET) $(FMRI_TARGET) $(MOVIE_TARGET)
endif
ifeq ($(ARCH),i686)
	ALL   := $(TRAIN_TARGET) $(ITRK_TARGET) $(FMRI_TARGET)
endif

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
STARBASEGFX_H := $(GRAPHICS_H) starbasegfx.h

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

STARBASEGFX_CC := $(STARBASEGFX_H) $(PARAMS_H) $(TIMING_H) \
	$(XSTUFF_H) $(TRACE_H) $(DEBUG_H) starbasegfx.cc

TIMING_CC := $(TIMING_H) $(APPLICATION_H) $(DEFS_H) $(PARAMS_H) \
	$(DEBUG_H) $(TRACE_H) timing.cc

TRACE_CC := $(TRACE_H) trace.cc

XSTUFF_CC := $(XSTUFF_H) $(DEFS_H) $(XHINTS_H) $(TRACE_H) $(DEBUG_H) xstuff.cc

#
# Targets
#

all:	dirs $(ALL)

dirs:
	mkdir -p $(ARCH)

mvtest: mvtest.cc
	$(CC) $(CFLAGS) -o mvtest simplemovie.cc mvtest.cc -lmoviefile -ldmedia

clean:
	rm -f core *.o *.a $(ALL)

$(ITRK_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_EYE_TRACKING main.cc $(OBJ) -o $@ $(LFLAGS) $(LIB)

$(FMRI_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_FMRI_SESSION main.cc $(OBJ) -o $@ $(LFLAGS) $(LIB)

$(TRAIN_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_TRAINING main.cc $(OBJ) -o $@ $(LFLAGS) $(LIB)

$(MOVIE_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_FMRI_SESSION -DMAKE_MOVIE main.cc \
		$(OBJ) -o $@ $(LFLAGS) $(LIB)

serial: serial.c eventnames.h
	cc -o $@ $< -L/usr/lib/X11R6 -lXwindow -lsb -lXhp11 -lX11 -lm -ldld

$(ARCH)/%.o : %.cc
	time $(CC) $(CFLAGS) -c $< -o $@

$(ARCH)/application.o: $(APPLICATION_CC)
$(ARCH)/balls.o: $(BALLS_CC)
$(ARCH)/ballsexpt.o: $(BALLSEXPT_CC)
$(ARCH)/glfont.o: $(GLFONT_CC)
$(ARCH)/graphics.o: $(GRAPHICS_CC)
$(ARCH)/menuapp.o: $(MENUAPP_CC)
$(ARCH)/openglgfx.o: $(OPENGLGFX_CC)
$(ARCH)/params.o: $(PARAMS_CC)
$(ARCH)/simplemovie.o: $(SIMPLEMOVIE_CC)
$(ARCH)/starbasegfx.o: $(STARBASEGFX_CC)
$(ARCH)/timing.o: $(TIMING_CC)
$(ARCH)/trace.o: $(TRACE_CC)
$(ARCH)/xstuff.o: $(XSTUFF_CC)

TAGS: *.h *.cc *.c
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
