///////////////////////////////////////////////////////////////////////
//
// timepoint.h
// Rob Peters rjpeters@klab.caltech.edu
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

  unsigned long tv_sec() const { return m_tv.tv_sec; }

private:
  timeval m_tv;
};

static const char vcid_timing_h[] = "$Id$ $HeadURL$";
#endif // !TIMING_H_DEFINED
