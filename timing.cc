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
// Timer member definitions
//
///////////////////////////////////////////////////////////////////////

void CountdownTimer::reset()
{
DOTRACE("Timer::set");

  itsStartTime = Timing::now();
}

double CountdownTimer::elapsedMsec() const
{
DOTRACE("Timer::wait");

  const timeval tv = Timing::now();

  return Timing::elapsedMsec(itsStartTime, tv);
}

double CountdownTimer::elapsedMsecAndReset()
{
DOTRACE("CountdownTimer::elapsedMsecAndReset");

  const timeval tv = Timing::now();

  const double result = Timing::elapsedMsec(itsStartTime, tv);

  itsStartTime = tv;

  return result;
}

static const char vcid_timing_cc[] = "$Header$";
#endif // !TIMING_CC_DEFINED
