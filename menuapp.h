///////////////////////////////////////////////////////////////////////
//
// menuapp.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:55:05 2000
// written: Wed Sep  3 14:19:42 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MENUAPP_H_DEFINED
#define MENUAPP_H_DEFINED

#include "application.h"
#include "defs.h"

class Graphics;

class MenuApplication : public Application
{
public:
  MenuApplication(Graphics& gfx);
  virtual ~MenuApplication();

protected:
  virtual void wrap();
  virtual void onExpose();

  void makeMenu();

  virtual void fillMenu(char menu[][STRINGSIZE], int nitems) = 0;
};

static const char vcid_menuapp_h[] = "$Header$";
#endif // !MENUAPP_H_DEFINED
