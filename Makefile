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
	CC  = g++
	CFLAGS=  -I/usr/include/X11R6 -I/cit/rjpeters/include \
		-I/cit/rjpeters/gcc/include/g++-3 -DIRIX6
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
	$(ARCH)/main.o \
	$(ARCH)/params.o \
	$(ARCH)/timing.o \
	$(ARCH)/trace.o \
	$(ARCH)/xstuff.o \
	$(GFXOBJ)

TARGET = $(HOME)/bin/$(ARCH)/balls3

ALL   = $(TARGET)

all:	$(ALL)
	$(TARGET) xxx

clean:
	rm -f core *.o *.a $(ALL)

$(TARGET): $(OBJ)
	$(CC) -o $@ ${OBJ} ${LFLAGS} $(LIB) 

$(ARCH)/%.o : %.cc
	$(CC) $(CFLAGS) -c $< -o $@ 

$(ARCH)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

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

MAIN_CC = $(BALLSEXPT_H) $(XHINTS_H) main.c

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

$(ARCH)/applic.o: $(APPLIC_CC)
$(ARCH)/application.o: $(APPLICATION_CC)
$(ARCH)/balls.o: $(BALLS_CC)
$(ARCH)/ballsexpt.o: $(BALLSEXPT_CC)
$(ARCH)/glfont.o: $(GLFONT_CC)
$(ARCH)/graphics.o: $(GRAPHICS_CC)
$(ARCH)/main.o: $(MAIN_CC)
$(ARCH)/openglgfx.o: $(OPENGLGFX_CC)
$(ARCH)/params.o: $(PARAMS_CC)
$(ARCH)/starbasegfx.o: $(STARBASEGFX_CC)
$(ARCH)/timing.o: $(TIMING_CC)
$(ARCH)/trace.o: $(TRACE_CC)
$(ARCH)/xstuff.o: $(XSTUFF_CC)

TAGS: *.[ch]
	etags *.[ch]*