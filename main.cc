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

#include "params.h"
#include "xhints.h"

#include <cstdlib>
#include <iostream>

const char* WINDOW_NAME = "tracking";

const int WINDOW_X = 1280;
const int WINDOW_Y = 1024;

const int DEPTH_HINT = 24;
const char* VISUAL_CLASS = "TrueColor";
const bool PRIVATE_CMAP = false;
const bool DOUBLEBUFFER = true;
const bool USE_RGBA = true;


///////////////////////////////////////////////////////////////////////
//
// main function
//
// Although there's some ugly hackery in using the compile-time macros
// to determine the application mode, the macros are only tested in
// this main file, and are used to set (global) variables
// accordingly. This limits the amount of ifdef'ing that is needed.
//
///////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
  APPLICATION_MODE = TRAINING;

  for (int i = 0; i < argc; ++i)
    {
      if (strcmp(argv[i], "--session") == 0)
        {
          FMRI_SESSION_NUMBER = atoi(argv[++i]);
        }
      else if (strcmp(argv[i], "--fmri") == 0)
        {
          APPLICATION_MODE = FMRI_SESSION;
        }
      else if (strcmp(argv[i], "--train") == 0)
        {
          APPLICATION_MODE = TRAINING;
        }
      else if (strcmp(argv[i], "--itrk") == 0)
        {
          APPLICATION_MODE = EYE_TRACKING;
        }
      else if (strcmp(argv[i], "--makemovie") == 0)
        {
          MAKING_MOVIE = true;
        }
    }

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

  if (FMRI_SESSION == APPLICATION_MODE)
    {
      if (FMRI_SESSION_NUMBER < 0 || FMRI_SESSION_NUMBER > 4)
        {
          std::cout << "session number must be 1, 2, 3, or 4\n";
          return 1;
        }
    }

  BallsExpt theApp(hints);

  theApp.run();

  theApp.quit(0);
}

static const char vcid_main_c[] = "$Header$";
#endif // !MAIN_C_DEFINED
