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

#include <sys/time.h>

namespace Timing
{
  timeval now();
  double elapsedMsec(const timeval& tp0, const timeval& tp1);
}

class CountdownTimer
{
public:
  CountdownTimer() { reset(); }

  void reset();

  double elapsedMsec() const;

  double elapsedMsecAndReset();

private:
  timeval itsStartTime;
};

static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
