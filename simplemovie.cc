///////////////////////////////////////////////////////////////////////
//
// simplemovie.cc
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Feb 28 12:21:07 2001
// written: Wed Sep  3 12:54:24 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIMPLEMOVIE_CC_DEFINED
#define SIMPLEMOVIE_CC_DEFINED

#include "simplemovie.h"

#include <cstdlib>
#include <iostream>

void SimpleMovie::handleError(const char* message)
{
  std::cerr << message << '\n';
  exit(1);
}

SimpleMovie::SimpleMovie(const char* filename, MVfileformat fileFormat,
                         int width, int height) :
  itsWidth(width),
  itsHeight(height),
#ifndef NO_MOVIE
  itsMovie(0),
  itsMovieParams(0),
  itsImageTrack(0),
  itsImageParams(0),
#endif
  itsFrameSize(0),
  itsTempFrameBuffer(0),
  itsFrameRate(0.0)
{
#ifndef NO_MOVIE
  if ( dmParamsCreate( &itsMovieParams ) != DM_SUCCESS ) {
    handleError("couldn't create movie params");
  }

  if ( mvSetMovieDefaults( itsMovieParams, fileFormat )
       != DM_SUCCESS ) {
    handleError("couldn't set movie defaults");
  }

  if ( mvCreateFile( filename, itsMovieParams, NULL, &itsMovie )
       != DM_SUCCESS ) {
    handleError("couldn't create movie file");
  }

  if ( dmParamsCreate( &itsImageParams ) != DM_SUCCESS ) {
    handleError("couldn't create image track params");
  }

  if ( mvSetImageDefaults( itsImageParams, width, height, fileFormat )
       != DM_SUCCESS ) {
    handleError("couldn't set image defaults");
  }

  dmParamsSetString(itsImageParams, DM_IMAGE_COMPRESSION, DM_IMAGE_QT_ANIM);

  itsFrameSize = dmImageFrameSize( itsImageParams );

  if ( mvAddTrack( itsMovie,
                   DM_IMAGE,
                   itsImageParams,
                   NULL,
                   &itsImageTrack ) != DM_SUCCESS ) {
    handleError("couldn't add track");
  }

  if ( mvSetImageRate(itsImageTrack, 15.0) != DM_SUCCESS )
    handleError("couldn't set frame rate");

  itsFrameRate = mvGetImageRate( itsImageTrack );
#else
  itsFrameSize = 4*itsWidth*itsHeight;
  itsFrameRate = 15.0;
#endif
}

SimpleMovie::~SimpleMovie()
{
  delete [] itsTempFrameBuffer;
#ifndef NO_MOVIE
  dmParamsDestroy(itsImageParams);
  dmParamsDestroy(itsMovieParams);
  mvClose(itsMovie);
#endif
}

void SimpleMovie::appendFrames(int frameCount, void* buffer)
{
#ifndef NO_MOVIE
  if ( mvAppendFrames( itsImageTrack,
                       frameCount,
                       frameCount * itsFrameSize,
                       buffer )
       != DM_SUCCESS )
    handleError("couldn't append frames");
#endif
}

void SimpleMovie::flush()
{
#ifndef NO_MOVIE
  // Flushes all changes that have been made and makes sure that they
  // are written to the file.
  mvWrite(itsMovie);
#endif
}

char* SimpleMovie::tempFrameBuffer()
{
  if (itsTempFrameBuffer == 0)
    itsTempFrameBuffer = new char [itsFrameSize];
  return itsTempFrameBuffer;
}

void SimpleMovie::appendTempBuffer()
{
  appendFrames(1, tempFrameBuffer());
}

void SimpleMovie::addSillyFrames()
{
  char* buf1 = new char[itsFrameSize];
  char* buf2 = new char[itsFrameSize];

  for (unsigned int k = 0; k < itsFrameSize; ++k)
    {
      buf1[k] = char(k / 256);
      buf2[k] = ~buf1[k];
    }

  for (int i = 0; i < 10; ++i)
    {
      std::cerr << i << '\n';
      appendFrames(1, buf1);
      appendFrames(1, buf2);
    }

  delete [] buf2;
  delete [] buf1;
}

static const char vcid_simplemovie_cc[] = "$Header$";
#endif // !SIMPLEMOVIE_CC_DEFINED
