///////////////////////////////////////////////////////////////////////
//
// applic.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:55:05 2000
// written: Wed Feb 23 14:15:19 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLIC_H_DEFINED
#define APPLIC_H_DEFINED

class Application;

void InitApplication(Application* app);
void WrapApplication(Application* app);
void SwitchApplication( Application* app, char c );
void MakeMenu(Application* app);

static const char vcid_applic_h[] = "$Header$";
#endif // !APPLIC_H_DEFINED
