///////////////////////////////////////////////////////////////////////
//
// ballsexpt.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:20 2000
// written: Thu Mar 30 16:43:09 2000
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

protected:
  virtual void runFixationCalibration();

  virtual void runExperiment();

  virtual void fillMenu(char menu[][STRINGSIZE], int nitems);
};

static const char vcid_ballsexpt_h[] = "$Header$";
#endif // !BALLSEXPT_H_DEFINED
