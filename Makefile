#########################################################################
#
# Makefile for Balls3 psychophysics experiment
#
# Rob Peters rjpeters@klab.caltech.edu
# created: Feb 2000
# $Id$
#
#########################################################################

ifeq ($(ARCH),hp9000s700)
	CC  = aCC
	CFLAGS= +W302,495,829,749,740 -I/usr/include/X11R6 -I/usr/include \
		-I/cit/rjpeters/include -DHP9000S700
	LFLAGS= -L/usr/lib/X11R6 -L/opt/graphics/common/lib -L/opt/graphics/OpenGL/lib
	LIB   = -lGLU -lGL -lXwindow -lsb -lXhp11 -lX11 -lm -ldld
	GFXOBJ = $(ARCH)/starbasegfx.o
endif

ifeq ($(ARCH),irix6)
	CC  = /opt/MIPSpro/bin/CC
	CFLAGS=  -I/usr/include/X11R6 -I/cit/rjpeters/include \
		-I/cit/rjpeters/include/cppheaders -DIRIX6
	LFLAGS= 
	LIB   = -lGLU -lGL -lX11 -lm
	GFXOBJ = $(ARCH)/glfont.o $(ARCH)/openglgfx.o
endif

OBJ   = \
	$(ARCH)/applic.o \
	$(ARCH)/application.o \
	$(ARCH)/balls.o \
	$(ARCH)/ballsexpt.o \
	$(ARCH)/graphics.o \
	$(ARCH)/params.o \
	$(ARCH)/timing.o \
	$(ARCH)/trace.o \
	$(ARCH)/xstuff.o \
	$(GFXOBJ)

TRAIN_TARGET = $(HOME)/local/$(ARCH)/bin/balls3_train
ITRK_TARGET = $(HOME)/local/$(ARCH)/bin/balls3_itrk
FMRI_TARGET = $(HOME)/local/$(ARCH)/bin/balls3_fmri
MOVIE_TARGET = $(HOME)/local/$(ARCH)/bin/balls3_movie

ALL   = $(TRAIN_TARGET) $(ITRK_TARGET) $(FMRI_TARGET) $(MOVIE_TARGET) serial

# Level 0
APPLICATION_H = application.h
BALLS_H = balls.h
DEBUG_H = debug.h
DEFS_H = defs.h
GLFONT_H = glfont.h
PARAMS_H = params.h
TIMING_H = timing.h
TRACE_H = trace.h
XHINTS_H = xhints.h
XSTUFF_H = xstuff.h

# Level 1
APPLIC_H = $(APPLICATION_H) applic.h
GRAPHICS_H = $(DEFS_H) graphics.h

# Level 2
BALLSEXPT_H = $(APPLIC_H) ballsexpt.h
OPENGLGFX_H = $(GRAPHICS_H) openglgfx.h
STARBASEGFX_H = $(GRAPHICS_H) starbasegfx.h

APPLIC_CC = $(APPLIC_H) $(DEFS_H) $(GRAPHICS_H) \
	$(PARAMS_H) $(TRACE_H) $(DEBUG_H) applic.c

APPLICATION_CC = $(APPLICATION_H) $(DEFS_H) $(OPENGLGFX_H) \
	$(PARAMS_H) $(STARBASEGFX_H) $(TIMING_H) $(XHINTS_H) \
	$(XSTUFF_H) $(TRACE_H) $(DEBUG_H) application.cc

BALLS_CC = $(BALLS_H) $(DEFS_H) $(GRAPHICS_H) $(PARAMS_H) $(TIMING_H) \
	$(TRACE_H) $(DEBUG_H) balls.c

BALLSEXPT_CC = $(BALLSEXPT_H) $(BALLS_H) $(GRAPHICS_H) $(PARAMS_H) \
	$(TIMING_H) $(XHINTS_H) $(TRACE_H) $(DEBUG_H) ballsexpt.cc

GLFONT_CC = $(GLFONT_H) $(TRACE_H) $(DEBUG_H) glfont.cc

GRAPHICS_CC = $(GRAPHICS_H) $(TRACE_H) graphics.cc

MAIN_CC = $(BALLSEXPT_H) $(PARAMS_H) $(XHINTS_H) main.c

OPENGLGFX_CC = $(OPENGLGFX_H) $(GLFONT_H) $(PARAMS_H) \
	$(TIMING_H) $(XHINTS_H) $(XSTUFF_H) $(TRACE_H) $(DEBUG_H) openglgfx.cc

PARAMS_CC = $(PARAMS_H) $(APPLICATION_H) $(DEFS_H) $(GRAPHICS_H) \
	$(TRACE_H) $(DEBUG_H) params.c

STARBASEGFX_CC = $(STARBASEGFX_H) $(PARAMS_H) $(TIMING_H) \
	$(XSTUFF_H) $(TRACE_H) $(DEBUG_H) starbasegfx.cc

TIMING_CC = $(TIMING_H) $(APPLICATION_H) $(DEFS_H) $(PARAMS_H) \
	$(DEBUG_H) $(TRACE_H) timing.c

TRACE_CC = $(TRACE_H) trace.cc

XSTUFF_CC = $(XSTUFF_H) $(DEFS_H) $(XHINTS_H) $(TRACE_H) $(DEBUG_H) xstuff.cc

#
# Targets
#

all:	$(ALL)

mvtest: mvtest.cc
	$(CC) $(CFLAGS) -o mvtest simplemovie.cc mvtest.cc -lmoviefile -ldmedia

clean:
	rm -f core *.o *.a $(ALL)

$(TRAIN_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_TRAINING main.c $(OBJ) -o $@ $(LFLAGS) $(LIB) 

$(ITRK_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_EYE_TRACKING main.c $(OBJ) -o $@ $(LFLAGS) $(LIB) 

$(FMRI_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_FMRI_SESSION main.c $(OBJ) -o $@ $(LFLAGS) $(LIB) 

$(MOVIE_TARGET): $(OBJ) $(MAIN_CC)
	time $(CC) $(CFLAGS) -DMODE_FMRI_SESSION -DMAKE_MOVIE main.c \
		$(OBJ) -o $@ $(LFLAGS) $(LIB) 

serial: serial.c eventnames.h
	cc -o $@ $< -L/usr/lib/X11R6 -lXwindow -lsb -lXhp11 -lX11 -lm -ldld

$(ARCH)/%.o : %.cc
	time $(CC) $(CFLAGS) -c $< -o $@ 

$(ARCH)/%.o : %.c
	time $(CC) $(CFLAGS) -c $< -o $@ 

$(ARCH)/applic.o: $(APPLIC_CC)
$(ARCH)/application.o: $(APPLICATION_CC)
$(ARCH)/balls.o: $(BALLS_CC)
$(ARCH)/ballsexpt.o: $(BALLSEXPT_CC)
$(ARCH)/glfont.o: $(GLFONT_CC)
$(ARCH)/graphics.o: $(GRAPHICS_CC)
$(ARCH)/openglgfx.o: $(OPENGLGFX_CC)
$(ARCH)/params.o: $(PARAMS_CC)
$(ARCH)/starbasegfx.o: $(STARBASEGFX_CC)
$(ARCH)/timing.o: $(TIMING_CC)
$(ARCH)/trace.o: $(TRACE_CC)
$(ARCH)/xstuff.o: $(XSTUFF_CC)

TAGS: *.[ch]
	etags *.[ch]*