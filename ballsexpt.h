///////////////////////////////////////////////////////////////////////
//
// ballsexpt.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:20 2000
// written: Tue Jun 13 14:13:00 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLSEXPT_H_DEFINED
#define BALLSEXPT_H_DEFINED

#ifndef APPLIC_H_DEFINED
#include "applic.h"
#endif

class BallsExpt : public MenuApplication {
public:
  BallsExpt(const XHints& hints);
  virtual ~BallsExpt();

protected:
  virtual void fillMenu(char menu[][STRINGSIZE], int nitems);

private:
  void runFixationCalibration();
  void runExperiment();

  void runFmriExpt();
  void runEyeTrackingExpt();
  void runTrainingExpt();

  virtual void onMenuChoice(char c);

  struct Impl;
  Impl* itsImpl;
};

static const char vcid_ballsexpt_h[] = "$Header$";
#endif // !BALLSEXPT_H_DEFINED
