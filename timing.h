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

class Timepoint
{
private:
  Timepoint() { reset(); }
public:

  static Timepoint now() { return Timepoint(); }

  void reset();

  double elapsedSecSince(const Timepoint& start) const;
  double elapsedSec() const;
  double elapsedSecAndReset();

  unsigned long tv_sec() const { return itsStartTime.tv_sec; }

private:
  timeval itsStartTime;
};

static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
