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

void Timer::reset()
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

timeval Timing::now()
{
DOTRACE("Timing::now");

  struct timeval tp;
  struct timezone tzp;
  gettimeofday(&tp, &tzp);
  return tp;
}

double Timing::elapsedMsec(const timeval& tp0, const timeval& tp1)
{
DOTRACE("Timing::elapsedMsec");

  const double sec_lapsed  = double(tp1.tv_sec  - tp0.tv_sec);
  const double msec_lapsed = double(tp1.tv_usec - tp0.tv_usec) / 1000.0;

  return sec_lapsed * 1000. + msec_lapsed;
}

///////////////////////////////////////////////////////////////////////
//
// ResponseData member definitions
//
///////////////////////////////////////////////////////////////////////

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
  itsStimuli.push_back(Stimulus(0.0, 0));
  itsStimulusTime0 = *tp;
}

void ResponseData::addToStimulusStack(int correct_nbutton)
{
DOTRACE("ResponseData::addToStimulusStack");

  const timeval tp = Timing::now();

  // (1) Compute the trial onset time relative to the first time
  // (2) Note the correct response value
  itsStimuli.push_back(Stimulus(Timing::elapsedMsec(itsStimulusTime0, tp),
                                correct_nbutton));
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
          if (itsResponses[j].time > itsStimuli[i].msec_from_time0)
            break;
        }

      // If we found a corresponding response, compute the response time...
      if (j < itsResponses.size())
        {
          itsStimuli[i].reaction_time =
            itsResponses[j].time - itsStimuli[i].msec_from_time0;
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

static const char vcid_timing_cc[] = "$Header$";
#endif // !TIMING_CC_DEFINED
