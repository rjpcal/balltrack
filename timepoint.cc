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

#include "timepoint.h"

#include "trace.h"

void Timepoint::reset()
{
DOTRACE("Timepoint::reset");

  struct timezone tzp;
  gettimeofday(&itsStartTime, &tzp);
}

double Timepoint::elapsedSecSince(const Timepoint& start) const
{
DOTRACE("Timepoint::elapsedSecSince");

  const timeval& tp0 = start.itsStartTime;
  const timeval& tp1 = this->itsStartTime;

  const double sec_lapsed  = double(tp1.tv_sec  - tp0.tv_sec);
  const double usec_lapsed = double(tp1.tv_usec - tp0.tv_usec);

  return sec_lapsed + usec_lapsed/1000000.0;
}

double Timepoint::elapsedSec() const
{
DOTRACE("Timepoint::elapsedSec");

  return Timepoint::now().elapsedSecSince(*this);
}

double Timepoint::elapsedSecAndReset()
{
DOTRACE("Timepoint::elapsedSecAndReset");

  const Timepoint t = Timepoint::now();
  const double sec = t.elapsedSecSince(*this);
  *this = t;
  return sec;
}

static const char vcid_timing_cc[] = "$Header$";
#endif // !TIMING_CC_DEFINED
