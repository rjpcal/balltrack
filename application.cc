///////////////////////////////////////////////////////////////////////
//
// application.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Tue Feb 22 20:10:02 2000
// written: Tue Feb  3 16:56:40 2004
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLICATION_CC_DEFINED
#define APPLICATION_CC_DEFINED

#include "application.h"

#include "timing.h"
#include "xstuff.h"

#include <cstdlib>              // for srand48()
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "trace.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////
//
// Application member definitions
//
///////////////////////////////////////////////////////////////////////

Application::Application(XStuff& x) :
  itsXStuff(x)
{
DOTRACE("Application::Application");
}

Application::~Application()
{
DOTRACE("Application::~Application");
}

static const char vcid_application_cc[] = "$Header$";
#endif // !APPLICATION_CC_DEFINED
