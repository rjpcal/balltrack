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
#include "graphics.h"
#include "params.h"
#include "timing.h"

#include <cstdio>
#include <cstring>
#include <string>
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
  Impl(Params& p, Graphics& g) :
    timepoints(),
    timepointIdx(0),
    ballset(p),
    params(p),
    gfx(g)
  {}

  /// XXX this needs to be at least as big as (cycleNumber+1)*NUM_CONDITIONS
  timeval timepoints[128];
  int timepointIdx;
  Balls ballset;
  Params& params;
  Graphics& gfx;

  void logTimePoints(ParamFile& f)
  {
    for (int i = 0; i < timepointIdx; ++i)
      {
        char buf[512];
        snprintf(buf, 512, " %d %lf", i,
                 Timing::elapsedMsec(&timepoints[0], &timepoints[i]));

        printf("%s\n", buf);
        f.putLine(buf);
      }
  }
};

BallsExpt::BallsExpt(Graphics& gfx, Params& p) :
  Application(gfx),
  rep(new Impl(p, gfx))
{
DOTRACE("BallsExpt::BallsExpt");

  rep->params.readFromFile(gfx, "sta");
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
  switch (c)
    {
    case 'q':
      return true;
      break;

    case 'r':
      runExperiment();
      makeMenu();
      break;

    case 'x':
      rep->params.setGroup1(rep->gfx);
      break;

    case 'y':
      rep->params.setGroup2(rep->gfx);
      break;

    case 'z':
      rep->params.setGroup3(rep->gfx);
      break;

    case 'p':
      rep->params.showSettings(rep->gfx);
      break;

    default:
      makeMenu();
      break;
    }

  return false;
}

namespace
{
  std::string makestring(int i)
  {
    char buf[64];
    snprintf(buf, 64, "%d", i);
    return std::string(&buf[0]);
  }
}

void BallsExpt::makeMenu()
{
DOTRACE("BallsExpt::makeMenu");

  const int nitems = 8;

  std::string menu[nitems];

  menu[0] = "r     run experiment";
  menu[1] = "x     set parameters 1";
  menu[2] = "y     set parameters 2";
  menu[3] = "z     set parameters 3";
  menu[4] = "p     show parameters";
  menu[5] = "q     quit program";
  menu[6] = "";
  menu[7] = "recent percent correct: "
    + makestring(int(Timing::recentPercentCorrect()));

  rep->gfx.drawStrings(menu, nitems, 100, -200, 20);

  rep->gfx.swapBuffers();
}

void BallsExpt::runFixationCalibration()
{
DOTRACE("BallsExpt::runFixationCalibration");

  int w = rep->gfx.width();
  int h = rep->gfx.height();

  int x[] = { w/2, 50, w/2, w-50,  50, w/2, w-50,   50,  w/2, w-50, w/2 };
  int y[] = { h/2, 50,  50,   50, h/2, h/2,  h/2, h-50, h-50, h-50, h/2 };

  int seq[] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9, 10 };

  for (int i = 0; i < 11; ++i)
    {
      rep->gfx.clearFrontBuffer();
      rep->gfx.drawCross(x[seq[i]], y[seq[i]]);
      rep->gfx.swapBuffers();
      Timing::mainTimer.set();
      rep->gfx.gfxWait(1.0);
    }
}

void BallsExpt::runExperiment()
{
DOTRACE("BallsExpt::runExperiment");

  ParamFile tmefile(rep->params.filestem, 'a', "tme");

  rep->params.writeToFile("cur");

  time_t t = time(0);
  char* p = ctime(&t);

  tmefile.putLine("");
  tmefile.putLine("");
  tmefile.putLine(p);
  tmefile.putLine("");
  tmefile.putLine("");

  rep->params.appendToFile(tmefile);

  tmefile.putLine("");
  tmefile.putLine("");

  rep->gfx.clearFrontBuffer();

  for (int k = 0; k < 2; ++k)
    {
      rep->gfx.clearBackBuffer();
      rep->gfx.drawCross();
      rep->gfx.swapBuffers();
    }

  Timing::mainTimer.set();
  Timing::getTime(&rep->timepoints[0]);
  rep->gfx.gfxWait(rep->params.waitSeconds);

  rep->timepointIdx = 1;

  if (Params::FMRI_SESSION == rep->params.appMode)
    runFmriExpt();
  else if (Params::EYE_TRACKING == rep->params.appMode)
    runEyeTrackingExpt();
  else if (Params::TRAINING == rep->params.appMode)
    runTrainingExpt();

  Timing::getTime(&rep->timepoints[rep->timepointIdx++]);

  rep->logTimePoints(tmefile);

  buttonPressLoop();

  Timing::tallyReactionTime(tmefile,
                            rep->params.remindSeconds);
}

void BallsExpt::runFmriExpt()
{
DOTRACE("BallsExpt::runFmriExpt");

  if (rep->params.doMovie)
    rep->gfx.startRecording(rep->params.displayX,
                              rep->params.displayY);

  for (int trial = 0; trial < NUM_TRIALS; ++trial)
    {
      int track_number =
        TRACK_NUMBERS[ rep->params.fmriSessionNumber-1 ][ trial ];

      if (track_number < 0)
        {
          // do nothing; skip this trial
          continue;
        }
      else if (track_number == 0)
        {
          // Run passive trial
          rep->ballset.runTrial
            (rep->gfx, &rep->timepoints[rep->timepointIdx++],
             Balls::PASSIVE);
        }
      else
        {
          rep->params.ballTrackNumber = track_number;

          // Run active tracking trial with objective check
          rep->ballset.runTrial
            (rep->gfx, &rep->timepoints[rep->timepointIdx++],
             Balls::CHECK_ONE);
        }

      // If there will be more trials, then do a fixation cross interval
      if (trial < (NUM_TRIALS-1))
        {
          rep->gfx.clearFrontBuffer();

          for (int k = 0; k < 2; ++k)
            {
              rep->gfx.clearBackBuffer();
              rep->gfx.drawCross();
              rep->gfx.swapBuffers();
            }

          Timing::mainTimer.set();
          Timing::getTime(&rep->timepoints[rep->timepointIdx++]);
          rep->gfx.gfxWait(rep->params.waitSeconds);
        }
    }

  if (rep->params.doMovie)
    rep->gfx.stopRecording();
}

void BallsExpt::runEyeTrackingExpt()
{
DOTRACE("BallsExpt::runEyeTrackingExpt");

  for (int cycle=0; cycle < rep->params.cycleNumber; ++cycle)
    {
      // Run active tracking trial
      runFixationCalibration();
      rep->ballset.runTrial
        (rep->gfx, &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ALL);

      // Run active tracking trial with objective check
      runFixationCalibration();
      rep->ballset.runTrial
        (rep->gfx, &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ONE);

      // Run passive trial
      runFixationCalibration();
      rep->ballset.runTrial
        (rep->gfx, &rep->timepoints[rep->timepointIdx++],
         Balls::PASSIVE);
    }
}

void BallsExpt::runTrainingExpt()
{
DOTRACE("BallsExpt::runTrainingExpt");

  for (int cycle=0; cycle < rep->params.cycleNumber; ++cycle)
    {
      // Run active tracking trial
      rep->ballset.runTrial
        (rep->gfx, &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ALL);
      // Run active tracking trial with objective check
      rep->ballset.runTrial
        (rep->gfx, &rep->timepoints[rep->timepointIdx++],
         Balls::CHECK_ONE);
    }
}

static const char vcid_ballsexpt_cc[] = "$Header$";
#endif // !BALLSEXPT_CC_DEFINED
