///////////////////////////////////////////////////////////////////////
//
// menuapp.cc
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:06:33 2000
// written: Wed Sep  3 14:11:45 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef MENUAPP_CC_DEFINED
#define MENUAPP_CC_DEFINED

#include "menuapp.h"

#include "defs.h"
#include "graphics.h"
#include "params.h"

#include "trace.h"
#include "debug.h"

MenuApplication::MenuApplication(Graphics& gfx) :
  Application(gfx)
{
DOTRACE("MenuApplication::MenuApplication");

  Params::readParams(this, "sta");
}

MenuApplication::~MenuApplication()
{
DOTRACE("MenuApplication::~MenuApplication");
}

void MenuApplication::wrap()
{
DOTRACE("MenuApplication::wrap");
  Params::writeParams(this, "sta");

  graphics().wrapGraphics();
}

void MenuApplication::onExpose()
{
DOTRACE("MenuApplication::onExpose");

  makeMenu();
}

void MenuApplication::makeMenu()
{
DOTRACE("MenuApplication::makeMenu");

  const int nmenu = 8;
  char menu[nmenu][STRINGSIZE];

  fillMenu(menu, nmenu);

  graphics().clearFrontBuffer();

  for (int ii = 0; ii < 2; ++ii)
    {
      graphics().clearBackBuffer();
      graphics().swapBuffers();
    }

  graphics().showMenu(menu, nmenu);

  graphics().swapBuffers();
}

static const char vcid_menuapp_cc[] = "$Header$";
#endif // !MENUAPP_CC_DEFINED
