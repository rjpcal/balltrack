///////////////////////////////////////////////////////////////////////
//
// xstuff.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:21:27 2000
// written: Wed Sep  3 16:12:22 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XSTUFF_H_DEFINED
#define XSTUFF_H_DEFINED

#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct XStuff
{
public:
  XStuff(int width, int height);

  void openWindow(const char* winname,
                  int width, int height, int depth);
  void wrapX();

  Display* display() { return itsDisplay; }
  Window window() const { return itsWindow; }

  void setPrefVisInfo(const XVisualInfo* vinfo);

private:
  int itsWidth;
  int itsHeight;
  Display* itsDisplay;
  Window itsWindow;
  XVisualInfo itsVisInfo;
  bool itsHasVisInfo;
};

static const char vcid_xstuff_h[] = "$Header$";
#endif // !XSTUFF_H_DEFINED
