///////////////////////////////////////////////////////////////////////
//
// ballsexpt.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:51 2000
// written: Wed Sep  3 14:10:43 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLSEXPT_CC_DEFINED
#define BALLSEXPT_CC_DEFINED

#include "ballsexpt.h"

#include "balls.h"
#include "defs.h"
#include "graphics.h"
#include "params.h"
#include "timing.h"

#include <cstdio>
#include <cstring>
#include <time.h>

#include "trace.h"
#include "debug.h"

namespace
{
  const int NUM_TRIALS = 8;

  const int TRACK_NUMBERS[9][NUM_TRIALS] =
    {
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

struct BallsExpt::Impl
{
  Impl(Params& p) :
    timepoints(),
    timepointIdx(0),
    ballset(p),
    params(p)
  {}

  /// XXX this needs to be at least as big as (CYCLE_NUMBER+1)*NUM_CONDITIONS
  timeval timepoints[128];
  int timepointIdx;
  Balls ballset;
  Params& params;

  void logTimePoints(FILE* fp)
  {
    for (int i = 0; i < timepointIdx; ++i)
      {
        printf( " %d %lf\n", i,
                Timing::elapsedMsec( &timepoints[0],
                                     &timepoints[i] ) );
        fprintf( fp, " %d %lf\n", i,
                 Timing::elapsedMsec( &timepoints[0],
                                      &timepoints[i] ) );
      }
  }
};

BallsExpt::BallsExpt(Graphics& gfx, Params& p) :
  Application(gfx),
  rep(new Impl(p))
{
DOTRACE("BallsExpt::BallsExpt");

  rep->params.readFromFile(gfx, "sta");

  Balls::generateColors();
}

BallsExpt::~BallsExpt()
{
DOTRACE("BallsExpt::~BallsExpt");

  rep->params.writeToFile("sta");

  delete rep;
}

void BallsExpt::onExpose()
{
DOTRACE("BallsExpt::onExpose");

  makeMenu();
}

bool BallsExpt::onKey(char c)
{
DOTRACE("BallsExpt::onKey");
  switch( c )
    {
    case 'q':
      return true;
      break;

    case 'r':
      runExperiment();
      makeMenu();
      break;

    case 'x':
      rep->params.setGroup1(this->graphics());
      break;

    case 'y':
      rep->params.setGroup2(this->graphics());
      break;

    case 'z':
      rep->params.setGroup3(this->graphics());
      break;

    case 'p':
      rep->params.showSettings(this->graphics());
      break;

    default:
      makeMenu();
      break;
    }

  return false;
}

void BallsExpt::makeMenu()
{
DOTRACE("BallsExpt::makeMenu");

  const int nitems = 8;
  char menu[nitems][STRINGSIZE];

  strncpy( menu[0], "r     run experiment", STRINGSIZE);
  strncpy( menu[1], "x     set parameters 1", STRINGSIZE);
  strncpy( menu[2], "y     set parameters 2", STRINGSIZE);
  strncpy( menu[3], "z     set parameters 3", STRINGSIZE);
  strncpy( menu[4], "p     show parameters", STRINGSIZE);
  strncpy( menu[5], "q     quit program", STRINGSIZE);
  strncpy( menu[6], "", STRINGSIZE);
  snprintf( menu[7], STRINGSIZE, "recent percent correct: %d",
           int(Timing::recentPercentCorrect()) );

  graphics().showMenu(menu, nitems);

  graphics().swapBuffers();
}

void BallsExpt::runFixationCalibration()
{
DOTRACE("BallsExpt::runFixationCalibration");

  int w = graphics().width();
  int h = graphics().height();

  int x[] = { w/2, 50, w/2, w-50,  50, w/2, w-50,   50,  w/2, w-50, w/2 };
  int y[] = { h/2, 50,  50,   50, h/2, h/2,  h/2, h-50, h-50, h-50, h/2 };

  int seq[] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9, 10 };

  for (int i = 0; i < 11; ++i)
    {
      graphics().clearFrontBuffer();
      graphics().drawCross(x[seq[i]], y[seq[i]]);
      graphics().swapBuffers();
      Timing::mainTimer.set();
      graphics().gfxWait( 1.0 );
    }
}

void BallsExpt::runExperiment()
{
DOTRACE("BallsExpt::runExperiment");

  ParamFile tmefile(rep->params.filestem, 'a', "tme");

  rep->params.writeToFile("cur");

  time_t t = time(0);
  char* p = ctime(&t);
  fprintf( tmefile.fp(), "\n\n%s\n\n", p);

  rep->params.appendToFile(tmefile);

  fprintf( tmefile.fp(), "\n\n");

  graphics().clearFrontBuffer();

  for (int k = 0; k < 2; ++k)
    {
      graphics().clearBackBuffer();
      graphics().drawCross();
      graphics().swapBuffers();
    }

  Timing::mainTimer.set();
  Timing::getTime( &rep->timepoints[0] );
  graphics().gfxWait( rep->params.WAIT_DURATION );

  rep->timepointIdx = 1;

  if (Params::FMRI_SESSION == rep->params.appMode)
    runFmriExpt();
  else if (Params::EYE_TRACKING == rep->params.appMode)
    runEyeTrackingExpt();
  else if (Params::TRAINING == rep->params.appMode)
    runTrainingExpt();

  Timing::getTime( &rep->timepoints[rep->timepointIdx++] );

  rep->logTimePoints(tmefile.fp());

  buttonPressLoop();

  Timing::tallyReactionTime(tmefile.fp(),
                            rep->params.REMIND_DURATION);
}

void BallsExpt::runFmriExpt()
{
DOTRACE("BallsExpt::runFmriExpt");

  if (rep->params.doMovie)
    graphics().startRecording(rep->params.DISPLAY_X,
                              rep->params.DISPLAY_Y);

  for (int trial = 0; trial < NUM_TRIALS; ++trial)
    {
      int track_number =
        TRACK_NUMBERS[ rep->params.FMRI_SESSION_NUMBER-1 ][ trial ];

      if (track_number < 0)
        {
          // do nothing; skip this trial
          continue;
        }
      else if (track_number == 0)
        {
          // Run passive trial
          rep->ballset.runTrial
            (graphics(), &rep->timepoints[rep->timepointIdx++],
             Balls::PASSIVE);
        }
      else
        {
          rep->params.BALL_TRACK_NUMBER = track_number;

          // Run active tracking trial with objective check
          rep->ballset.runTrial
            (graphics(), &rep->timepoints[rep->timepointIdx++],
             Balls::CHECK_ONE);
        }

      // If there will be more trials, then do a fixation cross interval
      if ( trial < (NUM_TRIALS-1) )
        {
          graphics().clearFrontBuffer();

          for (int k = 0; k < 2; ++k)
            {
              graphics().clearBackBuffer();
              graphics().drawCross();
              graphics().swapBuffers();
            }

          Timing::mainTimer.set();
          Timing::getTime( &rep->timepoints[rep->timepointIdx++] );
          graphics().gfxWait( rep->params.WAIT_DURATION );
        }
    }

  if (rep->params.doMovie)
    graphics().stopRecording();
}

void BallsExpt::runEyeTrackingExpt()
{
DOTRACE("BallsExpt::runEyeTrackingExpt");

  for (int cycle=0; cycle < rep->params.CYCLE_NUMBER; ++cycle)
    {
      // Run active tracking trial
      runFixationCalibration();
      rep->ballset.runTrial
        (graphics(), &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ALL);

      // Run active tracking trial with objective check
      runFixationCalibration();
      rep->ballset.runTrial
        (graphics(), &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ONE);

      // Run passive trial
      runFixationCalibration();
      rep->ballset.runTrial
        (graphics(), &rep->timepoints[rep->timepointIdx++],
         Balls::PASSIVE);
    }
}

void BallsExpt::runTrainingExpt()
{
DOTRACE("BallsExpt::runTrainingExpt");

  for (int cycle=0; cycle < rep->params.CYCLE_NUMBER; ++cycle)
    {
      // Run active tracking trial
      rep->ballset.runTrial
        (graphics(), &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ALL);
      // Run active tracking trial with objective check
      rep->ballset.runTrial
        (graphics(), &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ONE);
    }
}

static const char vcid_ballsexpt_cc[] = "$Header$";
#endif // !BALLSEXPT_CC_DEFINED
