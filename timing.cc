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

static const char vcid_timing_cc[] = "$Header$";
#endif // !TIMING_CC_DEFINED
