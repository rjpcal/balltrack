///////////////////////////////////////////////////////////////////////
//
// graphics.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:01:32 2000
// written: Wed Feb 28 14:36:58 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_CC_DEFINED
#define GRAPHICS_CC_DEFINED

#include "graphics.h"

#include "timing.h"

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

void Graphics::startRecording() {}
void Graphics::stopRecording() {}

void Graphics::gfxWait(double delaySeconds)
{
  Timing::mainTimer.wait(delaySeconds);
}

static const char vcid_graphics_cc[] = "$Header$";
#endif // !GRAPHICS_CC_DEFINED
