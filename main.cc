///////////////////////////////////////////////////////////////////////
//
// main.c
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 12:27:11 2000
// written: Tue Feb 22 20:40:40 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MAIN_C_DEFINED
#define MAIN_C_DEFINED

#include "application.h"
#include "applic.h"
#include "image.h"

Application theApp;

int main( int argc, char** argv ) {

  theApp.initialize(argc, argv); 

  theApp.install();

  WhoAreYou( &theApp );

  InitWindow( &theApp );

  InitApplication( &theApp );

  theApp.run();

  theApp.quit(0);
}

static const char vcid_main_c[] = "$Header$";
#endif // !MAIN_C_DEFINED
