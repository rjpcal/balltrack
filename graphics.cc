///////////////////////////////////////////////////////////////////////
//
// graphics.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:01:32 2000
// written: Fri Feb 25 10:47:14 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_CC_DEFINED
#define GRAPHICS_CC_DEFINED

#include "graphics.h"

#include "trace.h"

Graphics::Graphics(int wid, int hgt) :
  itsWidth(wid),
  itsHeight(hgt)
{
DOTRACE("Graphics::Graphics");
}

Graphics::~Graphics()
{
DOTRACE("Graphics::~Graphics");
}

static const char vcid_graphics_cc[] = "$Header$";
#endif // !GRAPHICS_CC_DEFINED
