///////////////////////////////////////////////////////////////////////
//
// timing.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:52:28 2000
// written: Mon Feb 28 11:51:48 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_H_DEFINED
#define TIMING_H_DEFINED

#include <cstdio>
#include <sys/time.h>

class Application;

class Graphics;

class Timer {
public:
  void set();
  void wait(double delay_seconds);
  void logToFile(FILE* fl) const;

private:
  double itsSec;
  double itsUsec;
};

class VideoTimer {
public:
  VideoTimer(double frametime) : itsFrametime(frametime) {}

  void set()
	 { itsTimer.set(); }

  void wait(int delay_frames)
	 { itsTimer.wait(delay_frames*itsFrametime / 1000.0); }

private:
  double itsFrametime;
  Timer itsTimer;
};

class Timing {
public:
  // Used in Balls::runTrial
  static void addToStimulusStack(int correct_nbutton);

  // Used in Application::timeButtonEvent
  static void addToResponseStack(double xtime, int nbutton);

  static Timer mainTimer;
  static Timer logTimer;

  static void getTime( timeval* tp );
  static double elapsedMsec( timeval* tp0, timeval* tp1 );
  static void initTimeStack( double xtime, timeval* tp );
  static void tallyReactionTime( FILE* fl );

  static double recentPercentCorrect();
};


static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
