///////////////////////////////////////////////////////////////////////
//
// graphics.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:01:32 2000
// written: Tue Mar  6 16:55:21 2001
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
  itsHeight(hgt),
  itsFrameTime(-1.0)
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

double Graphics::frameTime() {
  if (itsFrameTime < 0)
	 itsFrameTime = computeFrameTime();
  return itsFrameTime;
}

static const char vcid_graphics_cc[] = "$Header$";
#endif // !GRAPHICS_CC_DEFINED
