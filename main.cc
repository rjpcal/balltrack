///////////////////////////////////////////////////////////////////////
//
// main.c
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 12:27:11 2000
// written: Wed Sep  3 14:19:41 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MAIN_C_DEFINED
#define MAIN_C_DEFINED

#include "ballsexpt.h"
#include "graphics.h"
#include "params.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int main( int argc, char** argv )
{
  Params p(argc, argv);

  Graphics gfx("tracking", // window name
               1280, // window width
               1024, // window height
               24    // window bit depth
               );

  BallsExpt theApp(gfx, p);

  theApp.run();

  fprintf(stderr, "done\n");

  return 0;
}

static const char vcid_main_c[] = "$Header$";
#endif // !MAIN_C_DEFINED
