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
  Params p;

  p.APPLICATION_MODE = Params::TRAINING;

  strncpy(p.PROGRAM, argv[0], STRINGSIZE);

  bool got_filename = false;

  for (int i = 1; i < argc; ++i)
    {
      if (strcmp(argv[i], "--session") == 0)
        {
          p.FMRI_SESSION_NUMBER = atoi(argv[++i]);
        }
      else if (strcmp(argv[i], "--fmri") == 0)
        {
          p.APPLICATION_MODE = Params::FMRI_SESSION;
        }
      else if (strcmp(argv[i], "--train") == 0)
        {
          p.APPLICATION_MODE = Params::TRAINING;
        }
      else if (strcmp(argv[i], "--itrk") == 0)
        {
          p.APPLICATION_MODE = Params::EYE_TRACKING;
        }
      else if (strcmp(argv[i], "--makemovie") == 0)
        {
          p.MAKING_MOVIE = true;
        }
      else if (!got_filename)
        {
          strncpy(p.FILENAME, argv[i], STRINGSIZE);
          strncpy(p.OBSERVER, argv[i], STRINGSIZE);
          fprintf(stdout, " filename '%s'\n", argv[i]);
          got_filename = true;
        }
      else
        {
          fprintf(stderr, "unknown command-line argument '%s'\n",
                  argv[i]);
          exit(1);
        }
    }

  if (!got_filename)
    {
      fprintf(stderr, "need to specify a file basename\n");
      exit(1);
    }

  if (Params::FMRI_SESSION == p.APPLICATION_MODE)
    {
      if (p.FMRI_SESSION_NUMBER < 0 || p.FMRI_SESSION_NUMBER > 4)
        {
          fprintf(stderr, "session number must be 1, 2, 3, or 4\n");
          exit(1);
        }
    }

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
