///////////////////////////////////////////////////////////////////////
//
// timing.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:52:28 2000
// written: Wed Feb 23 15:21:35 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef TIMING_H_DEFINED
#define TIMING_H_DEFINED

#include <cstdio>
#include <sys/time.h>

class Application;

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
  // Used in Balls::runTrial and Balls::runDummy
  static void addToStimulusStack(Application* app);

  // Used in Application::timeButtonEvent
  static void addToResponseStack(Application* app, double xtime, int nbutton);

  // Used in Image::initWindow
  static void checkFrameTime(Application* app);

  static Timer mainTimer;
  static Timer logTimer;

  static void getTime( timeval* tp );
  static double elapsedMsec( timeval* tp0, timeval* tp1 );
  static void initTimeStack( double xtime, timeval* tp );
  static void tallyReactionTime( FILE* fl );
};


static const char vcid_timing_h[] = "$Header$";
#endif // !TIMING_H_DEFINED
