///////////////////////////////////////////////////////////////////////
//
// menuapp.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:55:05 2000
// written: Tue Jun 13 14:47:32 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MENUAPP_H_DEFINED
#define MENUAPP_H_DEFINED

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

  void makeMenu();

  virtual void fillMenu(char menu[][STRINGSIZE], int nitems) = 0;
};

static const char vcid_menuapp_h[] = "$Header$";
#endif // !MENUAPP_H_DEFINED
