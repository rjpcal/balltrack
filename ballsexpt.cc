///////////////////////////////////////////////////////////////////////
//
// ballsexpt.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:51 2000
// written: Thu Mar 30 17:27:57 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLSEXPT_CC_DEFINED
#define BALLSEXPT_CC_DEFINED

#include "ballsexpt.h"

#include <cstdio>
#include <cstring>

#include "balls.h"
#include "graphics.h"
#include "params.h"
#include "timing.h"
#include "xhints.h"

#include "trace.h"
#include "debug.h"

BallsExpt::BallsExpt(const XHints& hints) :
  MenuApplication(hints)
{
DOTRACE("BallsExpt::BallsExpt");
  Balls::generateColors();

  if ( !hints.rgba() ) {
	 graphics()->loadColormap(Balls::theColors, Balls::COLOR_NUMBER);
  }
}

void BallsExpt::fillMenu(char menu[][STRINGSIZE], int nitems) {
DOTRACE("BallsExpt::fillMenu");

  if (nitems < 7) {
	 strcpy( menu[0], "insufficient space to create menu" );
  } 
  else {
	 strcpy( menu[0], "r     run experiment");
	 strcpy( menu[1], "x     set parameters 1");
	 strcpy( menu[2], "y     set parameters 2");
	 strcpy( menu[3], "p     show parameters");
	 strcpy( menu[4], "q     quit program");
	 strcpy( menu[5], "");
	 sprintf( menu[6], "recent percent correct: %d",
				 int(Timing::recentPercentCorrect()) );
  }
}

void BallsExpt::runFixationCalibration() {
DOTRACE("BallsExpt::runFixationCalibration");

  int w = graphics()->width();
  int h = graphics()->height();

  int x[] = { w/2, 50, w/2, w-50,  50, w/2, w-50,   50,  w/2, w-50, w/2 };
  int y[] = { h/2, 50,  50,   50, h/2, h/2,  h/2, h-50, h-50, h-50, h/2 };

  int seq[] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9, 10 };

  for (int i = 0; i < 11; ++i)
	 {
		graphics()->writeAllPlanes();
		graphics()->waitVerticalRetrace();
		graphics()->clearFrontBuffer();
		graphics()->drawCross(x[seq[i]], y[seq[i]]);
		Timing::mainTimer.set();
		Timing::mainTimer.wait( 1.0 );
	 }
}

void BallsExpt::runExperiment() {
DOTRACE("BallsExpt::runExperiment");

  const int NUM_CONDITIONS = 3; 

  /// XXX this needs to be at least as big as (CYCLE_NUMBER+1)*NUM_CONDITIONS
  struct timeval tp[32];

  Balls theBalls;

  FILE *fl;
  Openfile(this, &fl, APPEND, "tme");
  LogParams(this, fl);

  graphics()->writeAllPlanes();

  graphics()->clearFrontBuffer();

  for (int k = 0; k < 2; ++k) {
	 graphics()->clearBackBuffer();
	 graphics()->drawCross();
	 graphics()->swapBuffers();
  }

  Timing::mainTimer.set();

  Timing::getTime( &tp[0] );

  Timing::mainTimer.wait( WAIT_DURATION );

  int timepoint = 1;

  int cycle;
  for( cycle=0; cycle<CYCLE_NUMBER; ++cycle ) {

	 if (EYE_TRACKING == APPLICATION_MODE)
		{
		  // Run active tracking trial
		  runFixationCalibration();
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::CHECK_ALL);

		  // Run active tracking trial with objective check
		  runFixationCalibration();
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::CHECK_ONE);

		  // Run passive trial
		  runFixationCalibration();
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::PASSIVE);
		}

	 if (TRAINING == APPLICATION_MODE)
		{
		  // Run active tracking trial
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::CHECK_ALL);
		  // Run active tracking trial with objective check
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::CHECK_ONE);
		  // Run passive trial
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::PASSIVE);
		}

	 if (FMRI_SESSION == APPLICATION_MODE)
		{
		  // Run active tracking trial with objective check
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::CHECK_ONE);
		  // Run passive trial
		  theBalls.runTrial(graphics(), &tp[timepoint++], Balls::PASSIVE);
		}
  }

  Timing::mainTimer.set();

  Timing::getTime( &tp[timepoint++] );

  Timing::mainTimer.wait( WAIT_DURATION );

  Timing::getTime( &tp[timepoint++] );

  for( int i=0; i<timepoint; i++ )
    {
		printf( " %d %lf\n", i, Timing::elapsedMsec( &tp[0], &tp[i] ) );
		fprintf( fl, " %d %lf\n", i, Timing::elapsedMsec( &tp[0], &tp[i] ) );
    }

  graphics()->writeAllPlanes();

  buttonPressLoop();

  Timing::tallyReactionTime( fl );

  Closefile( fl );
}

static const char vcid_ballsexpt_cc[] = "$Header$";
#endif // !BALLSEXPT_CC_DEFINED
