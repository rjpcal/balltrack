///////////////////////////////////////////////////////////////////////
//
// main.c
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 12:27:11 2000
// written: Wed Feb 23 15:42:10 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MAIN_C_DEFINED
#define MAIN_C_DEFINED

#include "ballsexpt.h"

int main( int argc, char** argv ) {

  BallsExpt theApp(argc, argv); 

  theApp.run();

  theApp.quit(0);
}

static const char vcid_main_c[] = "$Header$";
#endif // !MAIN_C_DEFINED
