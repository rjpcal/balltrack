///////////////////////////////////////////////////////////////////////
//
// ballsexpt.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:20 2000
// written: Wed Sep  3 14:19:42 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLSEXPT_H_DEFINED
#define BALLSEXPT_H_DEFINED

class Graphics;
class Params;

class BallsExpt
{
public:
  BallsExpt(Graphics& gfx, Params& p);
  virtual ~BallsExpt();

  void run();

protected:
  static void onExpose(void* cdata);

  // Returns true if application should quit
  static bool onKey(void* cdata, double xtime, char c);

  static void onButton(void* cdata, double xtime, int button_number);

  void makeMenu();

private:
  void runFixationCalibration();
  void runExperiment(double xtime);

  void runFmriExpt();
  void runEyeTrackingExpt();
  void runTrainingExpt();

  struct Impl;
  Impl* rep;
};

static const char vcid_ballsexpt_h[] = "$Header$";
#endif // !BALLSEXPT_H_DEFINED
