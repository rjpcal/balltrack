///////////////////////////////////////////////////////////////////////
//
// applic.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:55:05 2000
// written: Tue Feb 29 16:12:22 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLIC_H_DEFINED
#define APPLIC_H_DEFINED

#ifndef APPLICATION_H_DEFINED
#include "application.h"
#endif

#ifndef DEFS_H_DEFINED
#include "defs.h"
#endif

class MenuApplication : public Application {
public:
  MenuApplication(const XHints& hints);
  virtual ~MenuApplication();

protected:
  virtual void wrap();
  virtual void onExpose();

  virtual void runExperiment() = 0;

  virtual void fillMenu(char menu[][STRINGSIZE], int nitems) = 0;

private:
  void makeMenu();
  void onMenuChoice(char c);
};

static const char vcid_applic_h[] = "$Header$";
#endif // !APPLIC_H_DEFINED
