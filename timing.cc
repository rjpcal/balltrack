///////////////////////////////////////////////////////////////////////
//
// timing.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb  1 16:42:55 2000
// written: Wed Sep  3 14:16:50 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_CC_DEFINED
#define TIMING_CC_DEFINED

#include "timing.h"

#include "params.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "debug.h"
#include "trace.h"

///////////////////////////////////////////////////////////////////////
//
// Timer member definitions
//
///////////////////////////////////////////////////////////////////////

void Timer::set()
{
DOTRACE("Timer::set");
  struct timeval  tp;
  struct timezone tzp;

  gettimeofday(&tp, &tzp);

  itsSec  = tp.tv_sec;
  itsUsec = tp.tv_usec;
}

void Timer::wait(double requested_delay)
{
DOTRACE("Timer::wait");

  struct timeval  tp;
  struct timezone tzp;

  long aim_for_secs  = (long)             floor(requested_delay);
  long aim_for_usecs = (long)(1000000. *  fmod(requested_delay, 1.0));

  long sec_stop  = aim_for_secs  + long(itsSec);
  long usec_stop = aim_for_usecs + long(itsUsec);

  while (usec_stop > 1000000L)
    {
      sec_stop  += 1L;
      usec_stop -= 1000000L;
    }

  while (usec_stop < 0L)
    {
      sec_stop  -= 1L;
      usec_stop += 1000000L;
    }

  do
    {
      gettimeofday(&tp, &tzp);
    } while (tp.tv_sec < sec_stop);

  do
    {
      gettimeofday(&tp, &tzp);
    } while (tp.tv_usec < usec_stop);

  itsSec  = tp.tv_sec;
  itsUsec = tp.tv_usec;
}

///////////////////////////////////////////////////////////////////////
//
// Timing member definitions
//
///////////////////////////////////////////////////////////////////////

Timing::Timing() :
  mainTimer(),
  logTimer()
{}

void Timing::getTime(timeval* tp)
{
DOTRACE("Timing::getTime");

  struct timezone tzp;
  gettimeofday(tp, &tzp);
}

double Timing::elapsedMsec(timeval* tp0, timeval* tp1)
{
DOTRACE("Timing::elapsedMsec");

  double sec_lapsed  = double(tp1->tv_sec  - tp0->tv_sec);
  double msec_lapsed = double(tp1->tv_usec - tp0->tv_usec) / 1000.0;

  double delta       = sec_lapsed * 1000. + msec_lapsed;

  return delta;
}


struct Stimulus
{
  Stimulus(double t, int v) : time(t), correct_val(v) {}
  double time;
  int correct_val;

  double reaction_time;
  bool reaction_correct;
};

struct Response
{
  Response(double t, int v) : time(t), val(v) {}
  double time;
  int val;
};

std::vector<Stimulus> stimulus_hist;
std::vector<Response> response_hist;

struct timeval ss_0;
double response_time_stack_0;
struct timeval response_timeval_0;

double percent_correct = 0.0;

void Timing::initTimeStack(double xtime, timeval* tp)
{
DOTRACE("Timing::initTimeStack");

  response_time_stack_0 = xtime;

  response_timeval_0 = *tp;

  response_hist.clear();
  response_hist.push_back(Response(0.0, 0));
  stimulus_hist.clear();
  stimulus_hist.push_back(Stimulus(0.0, 0));
  ss_0 = *tp;
}

void Timing::addToStimulusStack(int correct_nbutton)
{
DOTRACE("Timing::addToStimulusStack");

  struct timeval tp;

  Timing::getTime(&tp);

  // (1) Compute the trial onset time relative to the first time
  // (2) Note the correct response value
  stimulus_hist.push_back(Stimulus(elapsedMsec(&ss_0, &tp),
                                   correct_nbutton));
}

void Timing::addToResponseStack(double xtime, int nbutton)
{
DOTRACE("Timing::addToResponseStack");

  double delta = xtime - response_time_stack_0;

  if (delta < 0.0)
    delta = delta + 4294967295.0;

  response_hist.push_back(Response(delta, nbutton));
}

void Timing::tallyReactionTime(ParamFile& f, float remind_duration)
{
DOTRACE("Timing::tallyReactionTime");

  int total_stims = 0;
  int number_correct = 0;

  // Compute the response time for each stimulus (or indicate a
  // non-response with -1.0)
  for (unsigned int i = 1;  i < stimulus_hist.size(); ++i)
    {
      unsigned int j;

      // Find the first response (j'th) that came after the i'th stimulus
      for (j = 0; j < response_hist.size(); ++j)
        {
          if (response_hist[j].time > stimulus_hist[i].time)
            break;
        }

      // If we found a corresponding response, compute the response time...
      if (j < response_hist.size())
        {
          stimulus_hist[i].reaction_time =
            response_hist[j].time - stimulus_hist[i].time;
          stimulus_hist[i].reaction_correct =
            (response_hist[j].val == stimulus_hist[i].correct_val);
        }

      // But if there was no corresponding response, indicate a
      // non-response with -1.0
      else
        {
          stimulus_hist[i].reaction_time = -1.0;
          stimulus_hist[i].reaction_correct = false;
        }

      // If the reaction time was too large, it doesn't count, so
      // indicate a non-response with -1.0
      if (stimulus_hist[i].reaction_time > remind_duration*1000)
        {
          stimulus_hist[i].reaction_time = -1.0;
          stimulus_hist[i].reaction_correct = false;
        }

      ++total_stims;
      if (stimulus_hist[i].reaction_correct) ++number_correct;
  }

  percent_correct = (100.0 * number_correct) / total_stims;

  // write reactions to the log file

  char buf[512];

  f.putLine(" reaction times:");
  for (unsigned int i = 1; i < stimulus_hist.size(); ++i)
    {
      snprintf(buf, 512, " %d %.0lf",
               i, stimulus_hist[i].reaction_time);
      f.putLine(buf);
    }
  f.putLine("");
  f.putLine("");

  f.putLine(" reaction correct?:");
  for (unsigned int j = 1; j < stimulus_hist.size(); ++j)
    {
      snprintf(buf, 512, " %d %d",
               j, int(stimulus_hist[j].reaction_correct));
      f.putLine(buf);
    }
  f.putLine("");

  snprintf(buf, 512, " percent correct: %d", int(percent_correct));
  f.putLine(buf);
  f.putLine("");
}

double Timing::recentPercentCorrect()
{
DOTRACE("Timing::recentPercentCorrect");
  return percent_correct;
}

static const char vcid_timing_cc[] = "$Header$";
#endif // !TIMING_CC_DEFINED
