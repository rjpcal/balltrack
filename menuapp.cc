///////////////////////////////////////////////////////////////////////
//
// applic.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:06:33 2000
// written: Tue Feb  1 16:41:53 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef APPLIC_C_DEFINED
#define APPLIC_C_DEFINED

#include "applic.h"

#include <cstdlib>
#include <cstring>
#include <sys/time.h>

#include "balls.h"
#include "image.h"
#include "defs.h"
#include "main.h"
#include "params.h"

char  PROGRAM[STRINGSIZE];

void WhoAreYou( int argc, char** argv )
{
  if( argc < 2 )
    {
		printf( " Who are you?\n" );
		Exit(0);
    }

  strcpy( PROGRAM,  argv[0] );
  strcpy( OBSERVER, argv[1] );
  strcpy( FILENAME, argv[1] );
}

void InitApplication()
{
  int stimT;

  struct timeval tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  srand48( tp.tv_sec );

  ReadParams( "sta" );
}

void WrapApplication()
{
  WriteParams( "sta" );

  RestoreColormap();
}

void SwitchApplication( char c )
{
  switch( c )
    {
	 case 'q':
		Exit(0);
		break;

	 case 'r':
		RunApplication();
		MakeMenu();
		break;

	 case 'x':
		SetParameters1();
		break;

	 case 'y':
		SetParameters2();
		break;

	 case 'p':
		ListParams();
		break;

	 default:
		MakeMenu();
		break;
    }
}

void MakeMenu()
{
  int nmenu;
  char menu[10][STRINGSIZE];

  strcpy( menu[0], "r     run experiment");
  strcpy( menu[1], "x     set parameters 1");
  strcpy( menu[2], "y     set parameters 2");
  strcpy( menu[3], "p     show parameters");
  strcpy( menu[4], "q     quit program");

  nmenu = 5;

  ClearWindow();

  ShowMenu( menu, nmenu );
}

static const char vcid_applic_c[] = "$Header$";
#endif // !APPLIC_C_DEFINED
