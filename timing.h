///////////////////////////////////////////////////////////////////////
//
// timing.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:52:28 2000
// written: Wed Sep  3 14:19:41 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_H_DEFINED
#define TIMING_H_DEFINED

#include <cstdio>
#include <sys/time.h>
#include <vector>

class ParamFile;

enum Buttons
  {
    BUTTON1,
    BUTTON2,
    BUTTON3
  };

class Timer
{
public:
  void reset();
  void wait(double delay_seconds);

private:
  double itsSec;
  double itsUsec;
};

namespace Timing
{
  timeval now();
  double elapsedMsec(const timeval& tp0, const timeval& tp1);
}

struct Stimulus
{
  Stimulus(const timeval& now, int v) :
    time(now),
    correct_val(v)
  {}
  timeval time;
  int correct_val;

  double msecFrom(const timeval& time0) const
  { return Timing::elapsedMsec(time0, this->time); }

  double reaction_time;
  bool reaction_correct;
};

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


static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
