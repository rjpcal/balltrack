///////////////////////////////////////////////////////////////////////
//
// main.c
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 12:27:11 2000
// written: Mon Jun 12 10:49:50 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MAIN_C_DEFINED
#define MAIN_C_DEFINED

#include "ballsexpt.h"

#include "params.h"
#include "xhints.h"

#include <iostream.h>
#include <strstream.h>

const char* WINDOW_NAME = "tracking";

const int WINDOW_X = 1280;
const int WINDOW_Y = 1024;

#if defined(HP9000S700)

const int DEPTH_HINT = 8;
const char* VISUAL_CLASS = "PseudoColor";
const bool PRIVATE_CMAP = true;
const bool DOUBLEBUFFER = false;
const bool USE_RGBA = false;

#elif defined(IRIX6)

const int DEPTH_HINT = 24;
const char* VISUAL_CLASS = "TrueColor";
const bool PRIVATE_CMAP = false;
const bool DOUBLEBUFFER = true;
const bool USE_RGBA = true;

#else
#  error No architecture macro.
#endif

int main( int argc, char** argv ) {

#if defined(MODE_TRAINING)
  APPLICATION_MODE = TRAINING;
#elif defined(MODE_EYE_TRACKING)
  APPLICATION_MODE = EYE_TRACKING;
#elif defined(MODE_FMRI_SESSION)
  APPLICATION_MODE = FMRI_SESSION;
#else
#  error No application mode macro.
#endif

  XHints hints;

  hints
	 .argc(argc)
	 .argv(argv)
	 .name(WINDOW_NAME)
	 .width(WINDOW_X)
	 .height(WINDOW_Y)
	 .depth(DEPTH_HINT)
	 .visualClass(VISUAL_CLASS)
	 .privateCmap(PRIVATE_CMAP)
	 .doubleBuffer(DOUBLEBUFFER)
	 .rgba(USE_RGBA);

  if (FMRI_SESSION == APPLICATION_MODE && argc >= 3)
	 {
		istrstream ist(argv[2]);
		ist >> FMRI_SESSION_NUMBER;
		if ( FMRI_SESSION_NUMBER < 0 || FMRI_SESSION_NUMBER > 4 )
		  {
			 cout << "session number must be 1, 2, 3, or 4" << endl;
			 return 0;
		  }
	 }

  BallsExpt theApp(hints); 

  theApp.run();

  theApp.quit(0);
}

static const char vcid_main_c[] = "$Header$";
#endif // !MAIN_C_DEFINED
