///////////////////////////////////////////////////////////////////////
//
// graphics.cc
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 13:01:32 2000
// written: Wed Sep  3 12:53:10 2003
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
#ifndef I686
  if (itsFrameTime < 0)
    itsFrameTime = computeFrameTime();
#else
  itsFrameTime = 13;
#endif
  return itsFrameTime;
}

double Graphics::computeFrameTime()
{
DOTRACE("Graphics::computeFrameTime");

  struct timeval tp[2];

  clearFrontBuffer();

  waitFrameCount( 1 );

  Timing::getTime( &tp[0] );

  waitFrameCount( 99 );

  Timing::getTime( &tp[1] );

  double frametime = Timing::elapsedMsec( &tp[0], &tp[1] ) / 100.0;

  return frametime;
}

static const char vcid_graphics_cc[] = "$Header$";
#endif // !GRAPHICS_CC_DEFINED
