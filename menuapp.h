///////////////////////////////////////////////////////////////////////
//
// applic.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:55:05 2000
// written: Wed Feb 23 15:54:26 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLIC_H_DEFINED
#define APPLIC_H_DEFINED

#include "application.h"

class MenuApplication : public Application {
public:
  MenuApplication(int argc, char** argv);
  virtual ~MenuApplication();

protected:
  virtual void wrap();
  virtual void onExpose();

  virtual void runExperiment() = 0;

private:
  void makeMenu();
  void onMenuChoice(char c);
};

static const char vcid_applic_h[] = "$Header$";
#endif // !APPLIC_H_DEFINED
