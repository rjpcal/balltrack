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

namespace taux
{
  timeval now();
  double elapsedSec(const timeval& tp0, const timeval& tp1);
}

timeval taux::now()
{
DOTRACE("taux::now");

  struct timeval tp;
  struct timezone tzp;
  gettimeofday(&tp, &tzp);
  return tp;
}

double taux::elapsedSec(const timeval& tp0, const timeval& tp1)
{
DOTRACE("taux::elapsedSec");

  const double sec_lapsed  = double(tp1.tv_sec  - tp0.tv_sec);
  const double usec_lapsed = double(tp1.tv_usec - tp0.tv_usec);

  return sec_lapsed + usec_lapsed/1000000.0;
}

///////////////////////////////////////////////////////////////////////
//
// Timer member definitions
//
///////////////////////////////////////////////////////////////////////

void Timepoint::reset()
{
DOTRACE("Timer::set");

  itsStartTime = taux::now();
}

double Timepoint::elapsedSecSince(const Timepoint& start) const
{
DOTRACE("Timepoint::elapsedSecSince");
  return taux::elapsedSec(start.itsStartTime, this->itsStartTime);
}

double Timepoint::elapsedSec() const
{
DOTRACE("Timepoint::elapsedSec");

  const timeval tv = taux::now();

  return taux::elapsedSec(itsStartTime, tv);
}

double Timepoint::elapsedSecAndReset()
{
DOTRACE("Timepoint::elapsedSecAndReset");

  const timeval tv = taux::now();

  const double sec = taux::elapsedSec(itsStartTime, tv);

  itsStartTime = tv;

  return sec;
}

static const char vcid_timing_cc[] = "$Header$";
#endif // !TIMING_CC_DEFINED
