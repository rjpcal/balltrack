///////////////////////////////////////////////////////////////////////
//
// xhints.h
// Rob Peters rjpeters@klab.caltech.edu
// created: Fri Feb 25 13:11:45 2000
// written: Wed Sep  3 14:19:41 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef XHINTS_H_DEFINED
#define XHINTS_H_DEFINED

class XHints
{
public:
  XHints() :
         itsName("xstuff"),
         itsWidth(1280),
         itsHeight(1024),
         itsDepth(8)
  {}

  XHints& argc(int hint) { itsArgc = hint; return* this; }
  int argc() const { return itsArgc; }

  XHints& argv(char** hint) { itsArgv = hint; return* this; }
  char** argv() const { return itsArgv; }

  XHints& name(const char* hint) { itsName = hint; return *this; }
  const char* name() const { return itsName; }

  XHints& width(int hint) { itsWidth = hint; return *this; }
  int width() const { return itsWidth; }

  XHints& height(int hint) { itsHeight = hint; return *this; }
  int height() const { return itsHeight; }

  XHints& depth(int hint) { itsDepth = hint; return *this; }
  int depth() const { return itsDepth; }

private:
  int itsArgc;
  char** itsArgv;

  const char* itsName;
  int itsWidth;
  int itsHeight;
  int itsDepth;
};


static const char vcid_xhints_h[] = "$Header$";
#endif // !XHINTS_H_DEFINED
