///////////////////////////////////////////////////////////////////////
//
// ballsexpt.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:51 2000
// written: Wed Feb 23 15:41:58 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLSEXPT_CC_DEFINED
#define BALLSEXPT_CC_DEFINED

#include "ballsexpt.h"

void BallsExpt::runExperiment() {
DOTRACE("BallsExpt::runExperiment");

  FILE *fl;

  /// XXX this needs to be at least as big as (CYCLE_NUMBER+1)*2
  struct timeval tp[10];

  Balls theBalls;

  Openfile(this, &fl, APPEND, "tme");

  LogParams(this, fl);

  Graphics::writeAllPlanes(this->fildes());
  ClearWindow(this->fildes());
  DrawCross(this);
  SetTransparent(this->fildes());

  Timing::mainTimer.set();

  Timing::getTime( &tp[0] );

  SetMessage(this->fildes());

  Timing::mainTimer.wait( WAIT_DURATION );

  for( int cycle=0; cycle<CYCLE_NUMBER; cycle++ ) {

	 // Pause and display text cue
	 Timing::mainTimer.set();

	 Timing::getTime( &tp[2*cycle+1] );

	 ClearUpperPlanes(this->fildes());
	 DrawMessage( this, "KCART" );

	 theBalls.prepare(this);

	 Timing::mainTimer.wait( PAUSE_DURATION );

	 ClearUpperPlanes(this->fildes());
	 DrawCross(this);

	 // Run active tracking trial
	 theBalls.runTrial(this, true);

	 // Pause and display text cue
	 Timing::mainTimer.set();

	 Timing::getTime( &tp[2*cycle+2] );

	 ClearUpperPlanes(this->fildes());
	 DrawMessage( this, " POTS " );

	 theBalls.prepare(this);

	 Timing::mainTimer.wait( PAUSE_DURATION );

	 ClearUpperPlanes(this->fildes());
	 DrawCross(this);

	 // Run passive trial
	 theBalls.runTrial(this, false);
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

  Graphics::writeAllPlanes(this->fildes());

  this->buttonPressLoop();

  Timing::tallyReactionTime( fl );

  Closefile( fl );
}

static const char vcid_ballsexpt_cc[] = "$Header$";
#endif // !BALLSEXPT_CC_DEFINED
