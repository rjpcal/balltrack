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

static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
