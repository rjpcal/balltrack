///////////////////////////////////////////////////////////////////////
//
// xstuff.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Thu Feb 24 14:21:27 2000
// written: Mon Feb 28 17:38:07 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XSTUFF_H_DEFINED
#define XSTUFF_H_DEFINED

#include <X11/Xlib.h>
#include <X11/Xutil.h>

class XHints;

struct XStuff {
public:
  XStuff(const XHints& hints);

  void openWindow(const XHints& hints);
  void mapWindow(const char* name);
  void printWindowInfo();
  void wrapX();

  Display* display() { return itsDisplay; }
  Window window() const { return itsWindow; }
  XVisualInfo& visInfo() { return itsVisInfo; }
  void storeColor(unsigned int index, double red, double green, double blue);

  void setPrefVisInfo(const XVisualInfo* vinfo);

private:
  void openDisplay();
  void createVisual(const XHints& hints);
  void createColormap(const XHints& hints);
  void createWindow(const char* name);
  void selectInput();

  void setWmProperty(char* name);
  void setWmProtocol();

  int itsArgc;
  char** itsArgv;

  int itsWidth;
  int itsHeight;

  Display* itsDisplay;
  int itsScreen;
  Window itsWindow;
  Visual* itsVisual;
  int itsDepth;
  Colormap itsColormap;
  XColor itsMeanColor;
  XVisualInfo itsVisInfo;
  XVisualInfo itsPrefVisInfo;
  bool itsHasPrefVisInfo;
};

static const char vcid_xstuff_h[] = "$Header$";
#endif // !XSTUFF_H_DEFINED
