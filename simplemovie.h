///////////////////////////////////////////////////////////////////////
//
// simplemovie.h
//
// Copyright (c) 1998-2000 Rob Peters rjpeters@klab.caltech.edu
//
// created: Wed Feb 28 12:20:41 2001
// written: Wed Feb 28 13:04:36 2001
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef SIMPLEMOVIE_H_DEFINED
#define SIMPLEMOVIE_H_DEFINED

#include <dmedia/moviefile.h>

class SimpleMovie {
public:

  SimpleMovie(const char* filename, MVfileformat fileFormat,
				  int width, int height);

  ~SimpleMovie();

  void appendFrames(int frameCount, void* buffer);

  char* tempFrameBuffer();

  // Appends the data stored in tempFrameBuffer()
  void appendTempBuffer();

  void flush();

  void addSillyFrames();

  int width() const { return itsWidth; }
  int height() const { return itsHeight; }

  size_t frameSize() const { return itsFrameSize; }

  double frameRate() const { return itsFrameRate; }

private:
  int itsWidth;
  int itsHeight;

  MVid itsMovie;
  DMparams* itsMovieParams;

  MVid itsImageTrack;
  DMparams* itsImageParams;

  size_t itsFrameSize;
  char* itsTempFrameBuffer;

  double itsFrameRate;

  static void handleError(const char* message);
};

static const char vcid_simplemovie_h[] = "$Header$";
#endif // !SIMPLEMOVIE_H_DEFINED
