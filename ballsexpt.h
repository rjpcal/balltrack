///////////////////////////////////////////////////////////////////////
//
// ballsexpt.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Wed Feb 23 15:41:20 2000
// written: Tue Feb 29 16:12:38 2000
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
  virtual void runExperiment();

  virtual void fillMenu(char menu[][STRINGSIZE], int nitems);
};

static const char vcid_ballsexpt_h[] = "$Header$";
#endif // !BALLSEXPT_H_DEFINED
