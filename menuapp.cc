
#include "incl.h"

char  PROGRAM[STRINGSIZE];

WhoAreYou( argc, argv )
	  int argc;
	  char **argv;
{
  if( argc < 2 )
    {
		printf( " Who are you?\n" );
		Exit();
    }

  strcpy( PROGRAM,  argv[0] );
  strcpy( OBSERVER, argv[1] );
  strcpy( FILENAME, argv[1] );
}

InitApplication()
{
  int stimT;

  struct timeval tp;
  struct timezone tzp;

  gettimeofday( &tp, &tzp );

  srand48( tp.tv_sec );

  ReadParams( "sta" );
}

WrapApplication()
{
  WriteParams( "sta" );

  RestoreColormap();
}

SwitchApplication( c )
	  char c;
{
  switch( c )
    {
	 case 'q':
		Exit();
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

MakeMenu()
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






