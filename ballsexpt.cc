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

class ResponseData
{
public:
  ResponseData();

  // Used in BallsExpt::onButton
  void addToResponseStack(double xtime, int nbutton);

  // From BallsExpt::onKey
  void initTimeStack(double xtime, timeval* tp);

  // From BallsExpt::runExperiment
  void tallyReactionTime(ParamFile& f, float remind_duration);

  double recentPercentCorrect();

  std::vector<Stimulus> itsStimuli;
  std::vector<Response> itsResponses;

  struct timeval itsStimulusTime0;
  double itsResponseTime0;

  double itsPercentCorrect;
};

ResponseData::ResponseData() :
  itsStimuli(),
  itsResponses(),
  itsStimulusTime0(),
  itsResponseTime0(),
  itsPercentCorrect(0.0)
{}


void ResponseData::initTimeStack(double xtime, timeval* tp)
{
DOTRACE("ResponseData::initTimeStack");

  itsResponseTime0 = xtime;

  itsResponses.clear();
  itsResponses.push_back(Response(0.0, 0));
  itsStimuli.clear();
  itsStimuli.push_back(Stimulus(*tp, 0));
  itsStimulusTime0 = *tp;
}

void ResponseData::addToResponseStack(double xtime, int nbutton)
{
DOTRACE("ResponseData::addToResponseStack");

  double delta = xtime - itsResponseTime0;

  if (delta < 0.0)
    delta = delta + 4294967295.0;

  itsResponses.push_back(Response(delta, nbutton));
}

void ResponseData::tallyReactionTime(ParamFile& f, float remind_duration)
{
DOTRACE("ResponseData::tallyReactionTime");

  int total_stims = 0;
  int number_correct = 0;

  // Compute the response time for each stimulus (or indicate a
  // non-response with -1.0)
  for (unsigned int i = 1;  i < itsStimuli.size(); ++i)
    {
      unsigned int j;

      // Find the first response (j'th) that came after the i'th stimulus
      for (j = 0; j < itsResponses.size(); ++j)
        {
          if (itsResponses[j].time > itsStimuli[i].msecFrom(itsStimulusTime0))
            break;
        }

      // If we found a corresponding response, compute the response time...
      if (j < itsResponses.size())
        {
          itsStimuli[i].reaction_time =
            itsResponses[j].time - itsStimuli[i].msecFrom(itsStimulusTime0);
          itsStimuli[i].reaction_correct =
            (itsResponses[j].val == itsStimuli[i].correct_val);
        }

      // But if there was no corresponding response, indicate a
      // non-response with -1.0
      else
        {
          itsStimuli[i].reaction_time = -1.0;
          itsStimuli[i].reaction_correct = false;
        }

      // If the reaction time was too large, it doesn't count, so
      // indicate a non-response with -1.0
      if (itsStimuli[i].reaction_time > remind_duration*1000)
        {
          itsStimuli[i].reaction_time = -1.0;
          itsStimuli[i].reaction_correct = false;
        }

      ++total_stims;
      if (itsStimuli[i].reaction_correct) ++number_correct;
  }

  itsPercentCorrect = (100.0 * number_correct) / total_stims;

  // write reactions to the log file

  char buf[512];

  f.putLine(" reaction times:");
  for (unsigned int i = 1; i < itsStimuli.size(); ++i)
    {
      snprintf(buf, 512, " %d %.0lf",
               i, itsStimuli[i].reaction_time);
      f.putLine(buf);
    }
  f.putLine("");
  f.putLine("");

  f.putLine(" reaction correct?:");
  for (unsigned int j = 1; j < itsStimuli.size(); ++j)
    {
      snprintf(buf, 512, " %d %d",
               j, int(itsStimuli[j].reaction_correct));
      f.putLine(buf);
    }
  f.putLine("");

  snprintf(buf, 512, " percent correct: %d", int(itsPercentCorrect));
  f.putLine(buf);
  f.putLine("");
}

double ResponseData::recentPercentCorrect()
{
DOTRACE("ResponseData::recentPercentCorrect");
  return itsPercentCorrect;
}

struct BallsExpt::Impl
{
  Impl(Params& p, Graphics& g) :
    timepoints(),
    ballset(p),
    params(p),
    gfx(g),
    rdata()
  {}

  std::vector<timeval> timepoints;
  Balls ballset;
  Params& params;
  Graphics& gfx;
  ResponseData rdata;
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
      struct timeval tp;
      gettimeofday(&tp, (struct timezone*)0);
      p->rep->rdata.initTimeStack(xtime, &tp);

      p->runExperiment();
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

void BallsExpt::onButton(void* cdata, double xtime, int button_number)
{
DOTRACE("BallsExpt::onButton");
  BallsExpt* p = static_cast<BallsExpt*>(cdata);
  switch (button_number)
    {
    case 1:  p->rep->rdata.addToResponseStack(xtime, BUTTON1); break;
    case 2:  p->rep->rdata.addToResponseStack(xtime, BUTTON2); break;
    case 3:  p->rep->rdata.addToResponseStack(xtime, BUTTON3); break;
    default: p->rep->rdata.addToResponseStack(xtime, 0); break;
    }
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
    + makestring(int(rep->rdata.recentPercentCorrect()));

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
      rep->gfx.clearFrontBuffer();
      rep->gfx.drawCross(x[seq[i]], y[seq[i]]);
      rep->gfx.swapBuffers();
      Timer t;
      t.reset();
      rep->gfx.gfxWait(t, 1.0);
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

  rep->gfx.xstuff().buttonPressLoop(static_cast<void*>(this),
                                    &onButton);

  rep->rdata.tallyReactionTime(tmefile,
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
          rep->timepoints.push_back(Timing::now());
          rep->ballset.runTrial
            (rep->gfx, rep->rdata.itsStimuli, Balls::PASSIVE);
        }
      else
        {
          rep->params.ballTrackNumber = track_number;

          // Run active tracking trial with objective check
          rep->timepoints.push_back(Timing::now());
          rep->ballset.runTrial
            (rep->gfx, rep->rdata.itsStimuli, Balls::CHECK_ONE);
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
        (rep->gfx, rep->rdata.itsStimuli, Balls::CHECK_ALL);

      // Run active tracking trial with objective check
      runFixationCalibration();
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->rdata.itsStimuli, Balls::CHECK_ONE);

      // Run passive trial
      runFixationCalibration();
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->rdata.itsStimuli, Balls::PASSIVE);
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
        (rep->gfx, rep->rdata.itsStimuli, Balls::CHECK_ALL);
      // Run active tracking trial with objective check
      rep->timepoints.push_back(Timing::now());
      rep->ballset.runTrial
        (rep->gfx, rep->rdata.itsStimuli, Balls::CHECK_ONE);
    }
}

static const char vcid_ballsexpt_cc[] = "$Header$";
#endif // !BALLSEXPT_CC_DEFINED
