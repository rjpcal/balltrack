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

#include "application.h"
#include "defs.h"

class Graphics;

class BallsExpt : public Application
{
public:
  BallsExpt(Graphics& gfx);
  virtual ~BallsExpt();

protected:
  virtual void wrap();
  virtual void onExpose();
  virtual void onKey(char c);

  void makeMenu();

  virtual void fillMenu(char menu[][STRINGSIZE], int nitems);

private:
  void runFixationCalibration();
  void runExperiment();

  void runFmriExpt();
  void runEyeTrackingExpt();
  void runTrainingExpt();

  struct Impl;
  Impl* itsImpl;
};

static const char vcid_ballsexpt_h[] = "$Header$";
#endif // !BALLSEXPT_H_DEFINED
