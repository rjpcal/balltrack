///////////////////////////////////////////////////////////////////////
// trace.cc
// Rob Peters
// created: Jan-99
// written: Wed Sep  3 12:55:00 2003
// $Id$
///////////////////////////////////////////////////////////////////////

#ifndef TRACE_CC_DEFINED
#define TRACE_CC_DEFINED

#define LOCAL_PROF
#include "trace.h"

#include <iomanip>

int MAX_TRACE_LEVEL = 6;

int TRACE_LEVEL = 0;

Util::Prof::~Prof()
{
  using namespace std;
  cout << setw(10) << long(avgTime()) << " " << setw(5) << count()
                 << " " << setw(12) << long(avgTime()) * count()
                 << " " << funcName << endl;
}

static const char vcid_trace_cc[] = "$Header$";
#endif // TRACE_CC_DEFINED
