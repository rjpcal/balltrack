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

class ParamFile;

#define      LEFTBUTTON            'l'
#define      RIGHTBUTTON           'r'
#define      MIDDLEBUTTON          'm'

class Timer
{
public:
  void set();
  void wait(double delay_seconds);

private:
  double itsSec;
  double itsUsec;
};

class Timing
{
public:
  // Used in Balls::runTrial
  static void addToStimulusStack(int correct_nbutton);

  // Used in Application::timeButtonEvent
  static void addToResponseStack(double xtime, int nbutton);

  static void addToResponseStack(long sec, long usec, int nbutton);

  static Timer mainTimer;
  static Timer logTimer;

  static void getTime(timeval* tp);
  static double elapsedMsec(timeval* tp0, timeval* tp1);
  static void initTimeStack(double xtime, timeval* tp);
  static void tallyReactionTime(ParamFile& f, float remind_duration);

  static double recentPercentCorrect();
};


static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
