///////////////////////////////////////////////////////////////////////
//
// ballsexpt.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:51 2000
// written: Tue Feb 29 16:45:00 2000
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

void BallsExpt::runExperiment() {
DOTRACE("BallsExpt::runExperiment");

  FILE *fl;

  /// XXX this needs to be at least as big as (CYCLE_NUMBER+1)*2
  struct timeval tp[10];

  Balls theBalls;

  Openfile(this, &fl, APPEND, "tme");

  LogParams(this, fl);

  graphics()->writeAllPlanes();

  for (int k = 0; k < 2; ++k) {
	 graphics()->clearFrontBuffer();
	 graphics()->drawCross();
	 graphics()->swapBuffers();
  }

  Timing::mainTimer.set();

  Timing::getTime( &tp[0] );

  Timing::mainTimer.wait( WAIT_DURATION );

  int cycle;
  for( cycle=0; cycle<CYCLE_NUMBER; cycle++ ) {

	 // Run active tracking trial
	 theBalls.runTrial(graphics(), &tp[2*cycle+1], Balls::CHECK_ALL);

	 // Run active tracking trial with objective check
	 theBalls.runTrial(graphics(), &tp[2*cycle+1], Balls::CHECK_ONE);

// 	 // Run passive trial
// 	 theBalls.runTrial(graphics(), &tp[2*cycle+2], Balls::PASSIVE);
  }

  Timing::mainTimer.set();

  Timing::getTime( &tp[2*cycle+1] );

  Timing::mainTimer.wait( WAIT_DURATION );

  Timing::getTime( &tp[2*cycle+2] );

  for( int i=0; i<2*cycle+2; i++ )
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
