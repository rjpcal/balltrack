
CC  = cc
CFLAGS= -I/usr/include/X11R6 -I./ -I/usr/include
LFLAGS= -L/usr/lib/X11R6 -L/opt/audio/lib -L/opt/graphics/common/lib
LIB   = -lXwindow -lsb -lXhp11 -lX11 -lAlib -lm -ldld

OBJ   = main.o image.o applic.o balls.o timing.o params.o
INC   = incl.h params.h

ALL   = test

all:	$(ALL)

clean:
	rm -f core *.o *.a $(ALL)

test: $(OBJ)
	$(CC) -o $@ ${OBJ} ${LFLAGS} $(LIB) 

main.o: $(INC)
image.o: $(INC)
applic.o: $(INC)
balls.o: $(INC)
timing.o: $(INC)
params.o: $(INC)

