///////////////////////////////////////////////////////////////////////
//
// applic.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:06:33 2000
// written: Tue Feb 29 12:12:56 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLIC_C_DEFINED
#define APPLIC_C_DEFINED

#include "applic.h"

#include <cstring>

#include "balls.h"
#include "defs.h"
#include "graphics.h"
#include "params.h"
#include "timing.h"

#include "trace.h"
#include "debug.h"

MenuApplication::MenuApplication(const XHints& hints) :
  Application(hints)
{
DOTRACE("MenuApplication::MenuApplication");

  graphics()->initWindow();

  ReadParams(this, "sta");
}

MenuApplication::~MenuApplication() {
DOTRACE("MenuApplication::~MenuApplication");
}

void MenuApplication::wrap() {
DOTRACE("MenuApplication::wrap");
  WriteParams(this, "sta");

  graphics()->wrapGraphics();
}

void MenuApplication::onExpose() {
DOTRACE("MenuApplication::onExpose");

  makeMenu();
}

void MenuApplication::makeMenu() {
DOTRACE("MenuApplication::makeMenu");

  int nmenu;
  char menu[10][STRINGSIZE];

  strcpy( menu[0], "r     run experiment");
  strcpy( menu[1], "x     set parameters 1");
  strcpy( menu[2], "y     set parameters 2");
  strcpy( menu[3], "p     show parameters");
  strcpy( menu[4], "q     quit program");
  strcpy( menu[5], "");
  sprintf( menu[6], "recent percent correct: %d",
			  int(Timing::recentPercentCorrect()) );

  nmenu = 7;

  graphics()->clearWindow();

  for (int ii = 0; ii < 2; ++ii) {
	 graphics()->clearBackBuffer();
	 graphics()->swapBuffers();
  }

  graphics()->showMenu(menu, nmenu);

  graphics()->swapBuffers();
}

void MenuApplication::onMenuChoice(char c) {
DOTRACE("MenuApplication::onMenuChoice");
  switch( c ) {
  case 'q':
	 this->quit(0);
	 break;

  case 'r':
	 runExperiment();
	 makeMenu();
	 break;

  case 'x':
	 SetParameters1(this);
	 break;

  case 'y':
	 SetParameters2(this);
	 break;

  case 'p':
	 ListParams(this);
	 break;

  default:
	 makeMenu();
	 break;
  }
}

static const char vcid_applic_c[] = "$Header$";
#endif // !APPLIC_C_DEFINED
