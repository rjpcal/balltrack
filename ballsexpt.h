///////////////////////////////////////////////////////////////////////
//
// ballsexpt.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:20 2000
// written: Wed Feb 23 15:41:28 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLSEXPT_H_DEFINED
#define BALLSEXPT_H_DEFINED

#include "applic.h"

class BallsExpt : public MenuApplication {
public:
  BallsExpt(int argc, char** argv) :
	 MenuApplication(argc, argv)
	 {}

  virtual void runExperiment();
};

static const char vcid_ballsexpt_h[] = "$Header$";
#endif // !BALLSEXPT_H_DEFINED
