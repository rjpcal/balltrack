///////////////////////////////////////////////////////////////////////
//
// ballsexpt.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:51 2000
// written: Wed Feb 28 15:30:29 2001
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

namespace {
  const int NUM_TRIALS = 8;

  const int TRACK_NUMBERS[9][NUM_TRIALS] = {
	 {2, 0, 4, 5, 3, 0, 5, 2},
	 {3, 4, 2, 0, 5, 3, 4, 0},
	 {4, 0, 5, 3, 2, 0, 3, 4},
	 {5, 2, 3, 0, 4, 5, 2, 0},
	 {2, -1, 4, 5, 3, -1, 5, 2},
	 {3, 4, 2, -1, 5, 3, 4, -1},
	 {4, -1, 5, 3, 2, -1, 3, 4},
	 {5, 2, 3, -1, 4, 5, 2, -1},
	 {-1, -1, -1, 0, 2, 3, 4, 5}
  };
}

struct BallsExpt::Impl {
  /// XXX this needs to be at least as big as (CYCLE_NUMBER+1)*NUM_CONDITIONS
  timeval itsTimePoints[128];
  int itsTpIndex;
  Balls itsBalls;

  void logTimePoints(FILE* fp)
	 {
		for( int i = 0; i < itsTpIndex; ++i )
		  {
			 printf( " %d %lf\n", i,
						Timing::elapsedMsec( &itsTimePoints[0], 
													&itsTimePoints[i] ) );
			 fprintf( fp, " %d %lf\n", i,
						 Timing::elapsedMsec( &itsTimePoints[0],
													 &itsTimePoints[i] ) );
		  }
	 }
};


BallsExpt::BallsExpt(const XHints& hints) :
  MenuApplication(hints),
  itsImpl(new Impl)
{
DOTRACE("BallsExpt::BallsExpt");
  Balls::generateColors();

  if ( !hints.rgba() ) {
	 graphics()->loadColormap(Balls::theColors, Balls::COLOR_NUMBER);
  }
}

BallsExpt::~BallsExpt(){
DOTRACE("BallsExpt::~BallsExpt");
  delete itsImpl;
}


void BallsExpt::fillMenu(char menu[][STRINGSIZE], int nitems) {
DOTRACE("BallsExpt::fillMenu");

  if (nitems < 8) {
	 strcpy( menu[0], "insufficient space to create menu" );
  } 
  else {
	 strcpy( menu[0], "r     run experiment");
	 strcpy( menu[1], "x     set parameters 1");
	 strcpy( menu[2], "y     set parameters 2");
	 strcpy( menu[3], "z     set parameters 3");
	 strcpy( menu[4], "p     show parameters");
	 strcpy( menu[5], "q     quit program");
	 strcpy( menu[6], "");
	 sprintf( menu[7], "recent percent correct: %d",
				 int(Timing::recentPercentCorrect()) );
  }
}

void BallsExpt::onMenuChoice(char c) {
DOTRACE("BallsExpt::onMenuChoice");
  switch( c ) {
  case 'q':
	 this->quit(0);
	 break;

  case 'r':
	 runExperiment();
	 makeMenu();
	 break;

  case 'x':
	 SetParameters1(this);
	 break;

  case 'y':
	 SetParameters2(this);
	 break;

  case 'z':
	 SetParameters3(this);
	 break;

  case 'p':
	 ListParams(this);
	 break;

  default:
	 makeMenu();
	 break;
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
		graphics()->swapBuffers();
		Timing::mainTimer.set();
		graphics()->gfxWait( 1.0 );
	 }
}

void BallsExpt::runExperiment() {
DOTRACE("BallsExpt::runExperiment");

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
  Timing::getTime( &itsImpl->itsTimePoints[0] );
  graphics()->gfxWait( WAIT_DURATION );

  itsImpl->itsTpIndex = 1;

  if (FMRI_SESSION == APPLICATION_MODE)
	 runFmriExpt();
  else if (EYE_TRACKING == APPLICATION_MODE)
	 runEyeTrackingExpt();
  else if (TRAINING == APPLICATION_MODE)
	 runTrainingExpt();


  Timing::getTime( &itsImpl->itsTimePoints[itsImpl->itsTpIndex++] );

  itsImpl->logTimePoints(fl);

  graphics()->writeAllPlanes();

  buttonPressLoop();

  Timing::tallyReactionTime( fl );

  Closefile( fl );
}

void BallsExpt::runFmriExpt() {
DOTRACE("BallsExpt::runFmriExpt");

  if (MAKING_MOVIE)
	 graphics()->startRecording();

  for (int trial = 0; trial < NUM_TRIALS; ++trial)
	 {
		int track_number = 
		  TRACK_NUMBERS[ FMRI_SESSION_NUMBER-1 ][ trial ];

		if (track_number < 0)
		  {
			 // do nothing; skip this trial
			 continue;
		  }
		else if (track_number == 0)
		  {
			 // Run passive trial
			 itsImpl->itsBalls.runTrial(
                  graphics(), &itsImpl->itsTimePoints[itsImpl->itsTpIndex++],
						Balls::PASSIVE);
		  }
		else
		  {
			 BALL_TRACK_NUMBER = track_number;

			 // Run active tracking trial with objective check
			 itsImpl->itsBalls.runTrial(
               graphics(), &itsImpl->itsTimePoints[itsImpl->itsTpIndex++],
					Balls::CHECK_ONE);
		  }

		// If there will be more trials, then do a fixation cross interval
		if ( trial < (NUM_TRIALS-1) )
		  {
			 graphics()->writeAllPlanes();
			 graphics()->clearFrontBuffer();

			 for (int k = 0; k < 2; ++k) {
				graphics()->clearBackBuffer();
				graphics()->drawCross();
				graphics()->swapBuffers();
			 }

			 Timing::mainTimer.set();
			 Timing::getTime( &itsImpl->itsTimePoints[itsImpl->itsTpIndex++] );
			 graphics()->gfxWait( WAIT_DURATION );
		  }
	 }


  if (MAKING_MOVIE)
	 graphics()->stopRecording();
}

void BallsExpt::runEyeTrackingExpt() {
DOTRACE("BallsExpt::runEyeTrackingExpt");

  for( int cycle=0; cycle<CYCLE_NUMBER; ++cycle ) {
	 // Run active tracking trial
	 runFixationCalibration();
	 itsImpl->itsBalls.runTrial(
         graphics(), &itsImpl->itsTimePoints[itsImpl->itsTpIndex++],
			Balls::CHECK_ALL);

	 // Run active tracking trial with objective check
	 runFixationCalibration();
	 itsImpl->itsBalls.runTrial(
         graphics(), &itsImpl->itsTimePoints[itsImpl->itsTpIndex++],
			Balls::CHECK_ONE);

	 // Run passive trial
	 runFixationCalibration();
	 itsImpl->itsBalls.runTrial(
         graphics(), &itsImpl->itsTimePoints[itsImpl->itsTpIndex++],
			Balls::PASSIVE);
  }
}

void BallsExpt::runTrainingExpt() {
DOTRACE("BallsExpt::runTrainingExpt");

  for( int cycle=0; cycle<CYCLE_NUMBER; ++cycle ) {
	 // Run active tracking trial
	 itsImpl->itsBalls.runTrial(
         graphics(), &itsImpl->itsTimePoints[itsImpl->itsTpIndex++],
			Balls::CHECK_ALL);
	 // Run active tracking trial with objective check
	 itsImpl->itsBalls.runTrial(
         graphics(), &itsImpl->itsTimePoints[itsImpl->itsTpIndex++],
			Balls::CHECK_ONE);
  }
}


static const char vcid_ballsexpt_cc[] = "$Header$";
#endif // !BALLSEXPT_CC_DEFINED
