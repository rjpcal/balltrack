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
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

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

struct Response
{
  Response(double t, int v) : time(t), val(v) {}
  double time;
  int val;
};


struct BallsExpt::Impl
{
  Impl(Params& p, Graphics& g) :
    timepoints(),
    ballset(p),
    params(p),
    gfx(g),
    stimuli(),
    stimTime0(),
    respTime0(),
    percentCorrect(0.0)
  {}

  void tallyReactionTime(ParamFileOut& f)
  {
    double xtime;
    int nbutton;

    std::vector<Response> responses;

    while (this->gfx.xstuff().getButtonPress(xtime, nbutton))
      {
        double delta = xtime - this->respTime0;

        if (delta < 0.0)
          delta = delta + 4294967295.0;

        std::cout << " button " << nbutton
                  << " at delta " << delta << " msec" << std::endl;

        switch (nbutton)
          {
          case 1:  responses.push_back(Response(delta, BUTTON1)); break;
          case 2:  responses.push_back(Response(delta, BUTTON2)); break;
          case 3:  responses.push_back(Response(delta, BUTTON3)); break;
          default: responses.push_back(Response(delta, 0)); break;
          }
      }

    // write reactions to the log file

    char buf[512];

    f.putLine("");
    snprintf(buf, 512, " %-4s %15s   %20s",
             "#", "reaction time", "reaction correct?");
    f.putLine(buf);

    int number_correct = 0;

    // Compute the response time for each stimulus (or indicate a
    // non-response with -1.0)
    for (unsigned int i = 1;  i < this->stimuli.size(); ++i)
      {
        bool reaction_correct = false;
        double reaction_time = -1.0;

        unsigned int j;

        // FIXME switch to do the getButtonPress() loop here rather
        // than above

        // Find the first response (j'th) that came after the i'th
        // stimulus
        for (j = 0; j < responses.size(); ++j)
          {
            if (responses[j].time > this->stimuli[i].msecFrom(this->stimTime0))
              break;
          }

        // If we found a corresponding response, compute the response
        // time...
        if (j < responses.size())
          {
            double diff =
              responses[j].time - this->stimuli[i].msecFrom(this->stimTime0);

            // Make sure the reaction time wasn't too large
            if (diff <= params.remindSeconds*1000)
              {
                reaction_time = diff;
                reaction_correct =
                  (responses[j].val == this->stimuli[i].correct_val);
              }
          }

        if (reaction_correct) ++number_correct;

        snprintf(buf, 512, " %-4d %15.2f   %20d",
                 i,
                 reaction_time,
                 int(reaction_correct));
        f.putLine(buf);
      }

    this->percentCorrect = (100.0 * number_correct) / double(this->stimuli.size() - 1);

    f.putLine("");
    f.putLine("");

    snprintf(buf, 512, " percent correct: %.2f", this->percentCorrect);
    f.putLine(buf);
    f.putLine("");
  }

  std::vector<timeval> timepoints;
  Balls ballset;
  Params& params;
  Graphics& gfx;

  std::vector<Stimulus> stimuli;

  struct timeval stimTime0;
  double respTime0;

  double percentCorrect;
};

BallsExpt::BallsExpt(Graphics& gfx, Params& p) :
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

void BallsExpt::run()
{
DOTRACE("BallsExpt::run");
  rep->gfx.xstuff().eventLoop(static_cast<void*>(this),
                              &onExpose,
                              &onKey);
}

void BallsExpt::onExpose(void* cdata)
{
DOTRACE("BallsExpt::onExpose");

  BallsExpt* p = static_cast<BallsExpt*>(cdata);
  p->makeMenu();
}

bool BallsExpt::onKey(void* cdata, double xtime, char c)
{
DOTRACE("BallsExpt::onKey");
  BallsExpt* p = static_cast<BallsExpt*>(cdata);
  switch (c)
    {
    case 'q':
      return true;
      break;

    case 'r':
      p->runExperiment(xtime);
      p->makeMenu();
      break;

    case 'x':
      p->rep->params.setGroup1(p->rep->gfx);
      break;

    case 'y':
      p->rep->params.setGroup2(p->rep->gfx);
      break;

    case 'z':
      p->rep->params.setGroup3(p->rep->gfx);
      break;

    case 'p':
      p->rep->params.showSettings(p->rep->gfx);
      break;

    default:
      p->makeMenu();
      break;
    }

  return false;
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

  char buf[64];
  snprintf(buf, 64, "%.2f", rep->percentCorrect);

  menu[7] = std::string("recent percent correct: ") + buf;

  rep->gfx.clearBackBuffer();
  rep->gfx.drawStrings(menu, nitems, 100, -200, 20);

  rep->gfx.swapBuffers();
}

void BallsExpt::runFixationCalibration()
{
DOTRACE("BallsExpt::runFixationCalibration");

  const int w = rep->gfx.width();
  const int h = rep->gfx.height();

  const int x[] = { w/2, 50, w/2, w-50,  50, w/2, w-50,   50,  w/2, w-50, w/2 };
  const int y[] = { h/2, 50,  50,   50, h/2, h/2,  h/2, h-50, h-50, h-50, h/2 };

  const int seq[] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9, 10 };

  for (int i = 0; i < 11; ++i)
    {
      rep->gfx.clearBackBuffer();
      rep->gfx.drawCross(x[seq[i]], y[seq[i]]);
      rep->gfx.swapBuffers();
      Timer t;
      t.reset();
      rep->gfx.gfxWait(t, 1.0);
    }
}

void BallsExpt::runExperiment(double xtime)
{
DOTRACE("BallsExpt::runExperiment");

  rep->respTime0 = xtime;

  rep->stimTime0 = Timing::now();

  rep->stimuli.clear();
  rep->stimuli.push_back(Stimulus(rep->stimTime0, 0));

  ParamFileOut tmefile(rep->params.filestem, 'a', "tme");

  time_t t = time(0);
  char* p = ctime(&t);

  tmefile.stream() << "\n\n";
  tmefile.putLine(p);
  tmefile.stream() << "\n\n";

  rep->params.appendToFile(tmefile);

  tmefile.putLine("");
  tmefile.putLine("");

  for (int k = 0; k < 2; ++k)
    {
      rep->gfx.clearBackBuffer();
      rep->gfx.drawCross();
      rep->gfx.swapBuffers();
    }

  Timer timer;
  timer.reset();

  rep->timepoints.clear();

  rep->timepoints.push_back(Timing::now());
  rep->gfx.gfxWait(timer, rep->params.waitSeconds);

  switch (rep->params.appMode)
    {
    case Params::FMRI_SESSION: runFmriExpt(); break;
    case Params::EYE_TRACKING: runEyeTrackingExpt(); break;
    case Params::TRAINING:     runTrainingExpt(); break;
    }

  rep->timepoints.push_back(Timing::now());

  for (unsigned int i = 0; i < rep->timepoints.size(); ++i)
    {
      char buf[512];
      snprintf(buf, 512, " %d %lf", i,
               Timing::elapsedMsec(rep->timepoints[0],
                                   rep->timepoints[i]));

      std::cout << buf << '\n';
      tmefile.putLine(buf);
    }

  rep->tallyReactionTime(tmefile);
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
          rep->timepoints.push_back(Timing::now());
          rep->ballset.runTrial
            (rep->gfx, rep->stimuli, Balls::PASSIVE);
        }
      else
        {
          rep->params.ballTrackNumber = track_number;

          // Run active tracking trial with objective check
          rep->timepoints.push_back(Timing::now());
          rep->ballset.runTrial
            (rep->gfx, rep->stimuli, Balls::CHECK_ONE);
        }

      // If there will be more trials, then do a fixation cross interval
      if (trial < (NUM_TRIALS-1))
        {
          for (int k = 0; k < 2; ++k)
            {
              rep->gfx.clearBackBuffer();
              rep->gfx.drawCross();
              rep->gfx.swapBuffers();
            }

          Timer t;
          t.reset();
          rep->timepoints.push_back(Timing::now());
          rep->gfx.gfxWait(t, rep->params.waitSeconds);
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
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->stimuli, Balls::CHECK_ALL);

      // Run active tracking trial with objective check
      runFixationCalibration();
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->stimuli, Balls::CHECK_ONE);

      // Run passive trial
      runFixationCalibration();
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->stimuli, Balls::PASSIVE);
    }
}

void BallsExpt::runTrainingExpt()
{
DOTRACE("BallsExpt::runTrainingExpt");

  for (int cycle=0; cycle < rep->params.cycleNumber; ++cycle)
    {
      // Run active tracking trial
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->stimuli, Balls::CHECK_ALL);
      // Run active tracking trial with objective check
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->stimuli, Balls::CHECK_ONE);
    }
}

static const char vcid_ballsexpt_cc[] = "$Header$";
#endif // !BALLSEXPT_CC_DEFINED
