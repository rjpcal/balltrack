///////////////////////////////////////////////////////////////////////
//
// applic.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:55:05 2000
// written: Tue Feb  1 15:55:12 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLIC_H_DEFINED
#define APPLIC_H_DEFINED

void WhoAreYou( int argc, char** argv );
void InitApplication();
void WrapApplication();
void SwitchApplication( char c );
void MakeMenu();

static const char vcid_applic_h[] = "$Header$";
#endif // !APPLIC_H_DEFINED
