///////////////////////////////////////////////////////////////////////
//
// params.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:30:51 2000
// written: Tue Feb  1 16:38:46 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PARAMS_C_DEFINED
#define PARAMS_C_DEFINED

#include "params.h"

#include <cstring>
#include <cstdio>

#include "image.h"
#include "defs.h"
#include "main.h"

int   FUDGEFRAME = 7;

int   DISPLAY_X;
int   DISPLAY_Y;
int   CYCLE_NUMBER;
float WAIT_DURATION;
float EPOCH_DURATION;
float PAUSE_DURATION;
float REMIND_DURATION;
int   REMINDS_PER_EPOCH;
int   BALL_NUMBER;
int   BALL_TRACK_NUMBER;
int   BALL_VELOCITY;
int   BALL_ARRAY_SIZE;
float BALL_RADIUS;
float BALL_SIGMA2;
int   BALL_MIN_DISTANCE;
float BALL_TWIST_ANGLE;
char  OBSERVER[STRINGSIZE];
char  FILENAME[STRINGSIZE];

char line[STRINGSIZE], text[STRINGSIZE];

int   BORDER_X;
int   BORDER_Y;
int   FRAMES_PER_REMIND;

// Prototypes
void EnterInt( int* pi );
void EnterFloat( float* pf );
void EnterText( char* ps );
void date( char* p);
void process_id( char pid[] );


/************************************************/

void ReadParams( char extension[] )
{
  FILE *fp;
  float fdummy;
  int   idummy;

  Openfile( &fp, OPTIONAL, extension );

  if( fp == NULL )
	 return;

  GETINT(   (DISPLAY_X) );         
  GETINT(   (DISPLAY_Y) );         
  GETINT(   (CYCLE_NUMBER) );
  GETFLOAT( (WAIT_DURATION) );       
  GETFLOAT( (EPOCH_DURATION) );       
  GETFLOAT( (PAUSE_DURATION) );       
  GETFLOAT( (REMIND_DURATION) );
  GETINT(   (REMINDS_PER_EPOCH) );
  GETINT(   (FRAMES_PER_REMIND) );
  GETINT(   (BALL_NUMBER) );
  GETINT(   (BALL_TRACK_NUMBER) );
  GETINT(   (BALL_VELOCITY) );
  GETINT(   (BALL_ARRAY_SIZE) );
  GETINT(   (BALL_MIN_DISTANCE) );
  GETFLOAT( (BALL_RADIUS) );
  GETFLOAT( (BALL_SIGMA2) );
  GETFLOAT( (BALL_TWIST_ANGLE) );
  GETTEXT(  (OBSERVER) );
  GETTEXT(  (FILENAME) );

  Closefile( fp );

  CheckParams();
}

void CheckParams()
{
  float time_between_reminds;

  BORDER_X    = ( width - DISPLAY_X ) / 2; 
  BORDER_Y    = ( height - DISPLAY_Y ) / 2;

  time_between_reminds = ( EPOCH_DURATION - PAUSE_DURATION - REMIND_DURATION )
	 / REMINDS_PER_EPOCH;

  FRAMES_PER_REMIND    = (int)( 1000.0*(time_between_reminds-REMIND_DURATION)
										  / FRAMETIME ) - FUDGEFRAME;
}

void WriteParams( char extension[] )
{
  FILE *fp;

  Openfile( &fp, WRITE, extension );

  PUTINT(   (DISPLAY_X),        ("DISPLAY_X") );         
  PUTINT(   (DISPLAY_Y),        ("DISPLAY_Y") );         
  PUTINT(   (CYCLE_NUMBER),     ("CYCLE_NUMBER") );
  PUTFLOAT( (WAIT_DURATION),    ("WAIT_DURATION") );       
  PUTFLOAT( (EPOCH_DURATION),   ("EPOCH_DURATION") );       
  PUTFLOAT( (PAUSE_DURATION),   ("PAUSE_DURATION") );       
  PUTFLOAT( (REMIND_DURATION),  ("REMIND_DURATION") );
  PUTINT(   (REMINDS_PER_EPOCH),("REMINDS_PER_EPOCH") );
  PUTINT(   (FRAMES_PER_REMIND),("FRAMES_PER_REMIND") );
  PUTINT(   (BALL_NUMBER),      ("BALL_NUMBER") );
  PUTINT(   (BALL_TRACK_NUMBER),("BALL_TRACK_NUMBER") );
  PUTINT(   (BALL_VELOCITY),    ("BALL_VELOCITY") );
  PUTINT(   (BALL_ARRAY_SIZE),  ("BALL_ARRAY_SIZE") );
  PUTINT(   (BALL_MIN_DISTANCE),("BALL_MIN_DISTANCE") );
  PUTFLOAT( (BALL_RADIUS),      ("BALL_RADIUS") );
  PUTFLOAT( (BALL_SIGMA2),      ("BALL_SIGMA2") );
  PUTFLOAT( (BALL_TWIST_ANGLE), ("BALL_TWIST_ANGLE") );
  PUTTEXT(  (OBSERVER),         ("OBSERVER") );
  PUTTEXT(  (FILENAME),         ("FILENAME") );

  Closefile( fp );
}

void LogParams( FILE* fl )
{
  FILE *fp;
  char line[480], Cname[ 120 ];

  WriteParams( "cur" );

  Openfile( &fp, READ, "cur" );

  date( text);
  fprintf( fl, "\n\n%s\n\n", text);

  while( fgets( line, 120, fp) !=  NULL)
    {
		fputs( line, fl);
    }

  Closefile( fp );

  fprintf( fl, "\n\n", text);
}

void ListParams()
{
  FILE *fp;
  int nparams = 0;
  char params[60][STRINGSIZE];

  WriteParams( "sta" );

  Openfile( &fp, READ,   "sta" );

  while( fgets( params[nparams], STRINGSIZE, fp) !=  NULL && nparams < 60 )
    {    
		params[nparams][ strlen( params[nparams] ) - 1 ] = '\0';
		nparams++;
    }

  Closefile( fp );

  ClearWindow();

  ShowParams( params, nparams );
}

void Openfile( FILE** fp, char mode, char extension[] )
{
  char fname[STRINGSIZE];

  sprintf( fname, "%s.%s", FILENAME, extension );

  if( mode==WRITE )
    {
		if( ( *fp = fopen( fname, "w") ) == NULL )
        {
			 printf( "cannot write %s\n", fname);
			 Exit(0);
        }
    }
  else
    if( mode==APPEND )
		{
        if( ( *fp = fopen( fname, "a") ) == NULL )
			 {
            printf( "cannot append to %s\n", fname);
				Exit(0);
			 }
		}
    else
		if( mode==READ )
		  {
			 if( ( *fp = fopen( fname, "r") ) == NULL )
				{
				  printf( "cannot read from %s\n", fname );
				  Exit(0);
				}
		  }
		else
		  if( mode==OPTIONAL )
			 {
				if( ( *fp = fopen( fname, "r") ) == NULL )
				  {
					 printf( "cannot read from %s, will create it\n", fname );
					 *fp = NULL;
				  }
			 }
}

void Closefile( FILE* fp )
{
  if( fp != NULL)
	 fclose( fp);
}

void PrintParams()
{
  PRINTINT(   (DISPLAY_X),        ("DISPLAY_X") );         
  PRINTINT(   (DISPLAY_Y),        ("DISPLAY_Y") );         
  PRINTINT(   (CYCLE_NUMBER),     ("CYCLE_NUMBER") );
  PRINTFLOAT( (WAIT_DURATION),    ("WAIT_DURATION") );       
  PRINTFLOAT( (EPOCH_DURATION),   ("EPOCH_DURATION") );       
  PRINTFLOAT( (PAUSE_DURATION),   ("PAUSE_DURATION") );       
  PRINTFLOAT( (REMIND_DURATION),  ("REMIND_DURATION") );
  PRINTINT(   (REMINDS_PER_EPOCH),("REMINDS_PER_EPOCH") );
  PRINTINT(   (FRAMES_PER_REMIND),("FRAMES_PER_REMIND") );
  PRINTINT(   (BALL_NUMBER),      ("BALL_NUMBER") );
  PRINTINT(   (BALL_TRACK_NUMBER),("BALL_TRACK_NUMBER") );
  PRINTINT(   (BALL_VELOCITY),    ("BALL_VELOCITY") );
  PRINTINT(   (BALL_ARRAY_SIZE),  ("BALL_ARRAY_SIZE") );
  PRINTINT(   (BALL_MIN_DISTANCE),("BALL_MIN_DISTANCE") );
  PRINTFLOAT( (BALL_RADIUS),      ("BALL_RADIUS") );
  PRINTFLOAT( (BALL_SIGMA2),      ("BALL_SIGMA2") );
  PRINTFLOAT( (BALL_TWIST_ANGLE), ("BALL_TWIST_ANGLE") );
  PRINTTEXT(  (OBSERVER),         ("OBSERVER") );
  PRINTTEXT(  (FILENAME),         ("FILENAME") );
}

void SetParameters1()
{
  char word[STRINGSIZE], text[4][STRINGSIZE];

  ClearWindow();

  sprintf( text[0], " BALL  NUMBER TRACK  VELOC  SIZE   MINDIS RADIUS SIGMA2 TWIST" );
  sprintf( text[1], "" );
  sprintf( text[2], "" );
  sprintf( text[3], "       %-6d %-6d %-6d %-6d %-6d %-6.1f %-6.1f %-6.3f", 
			  BALL_NUMBER, BALL_TRACK_NUMBER, BALL_VELOCITY,
			  BALL_ARRAY_SIZE, BALL_MIN_DISTANCE, BALL_RADIUS,
			  BALL_SIGMA2, BALL_TWIST_ANGLE );

  ShowMenu( text, 4);

  EnterInt( &BALL_NUMBER );
  sprintf( word, "       %-6d", BALL_NUMBER );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterInt( &BALL_TRACK_NUMBER );
  sprintf( word, " %-6d", BALL_TRACK_NUMBER );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterInt( &BALL_VELOCITY );
  sprintf( word, " %-6d", BALL_VELOCITY );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterInt( &BALL_ARRAY_SIZE );
  sprintf( word, " %-6d", BALL_ARRAY_SIZE );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterInt( &BALL_MIN_DISTANCE );
  sprintf( word, " %-6d", BALL_MIN_DISTANCE );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterFloat( &BALL_RADIUS );
  sprintf( word, " %-6.1f", BALL_RADIUS );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterFloat( &BALL_SIGMA2 );
  sprintf( word, " %-6.1f", BALL_SIGMA2 );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterFloat( &BALL_TWIST_ANGLE );
  sprintf( word, " %-6.3f", BALL_TWIST_ANGLE );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  CheckParams();

}

void SetParameters2()
{
  char word[STRINGSIZE], text[4][STRINGSIZE];

  ClearWindow();

  sprintf( text[0], "       CYCL_NUM WAIT_DUR EPCH_DUR PAUS_DUR RMND_NUM RMND_DUR" );
  sprintf( text[1], "" );
  sprintf( text[2], "" );
  sprintf( text[3], "       %-8d %-8.2f %-8.2f %-8.2f %-8d %-8.2f", 
			  CYCLE_NUMBER, WAIT_DURATION, EPOCH_DURATION,
			  PAUSE_DURATION, REMINDS_PER_EPOCH, REMIND_DURATION );

  ShowMenu( text, 4);

  EnterInt( &CYCLE_NUMBER );
  sprintf( word, "       %-8d", CYCLE_NUMBER );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterFloat( &WAIT_DURATION );
  sprintf( word, " %-8.2f", WAIT_DURATION );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterFloat( &EPOCH_DURATION );
  sprintf( word, " %-8.2f", EPOCH_DURATION );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterFloat( &PAUSE_DURATION );
  sprintf( word, " %-8.2f", PAUSE_DURATION );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterInt( &REMINDS_PER_EPOCH );
  sprintf( word, " %-8d", REMINDS_PER_EPOCH );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  EnterFloat( &REMIND_DURATION );
  sprintf( word, " %-8.2f", REMIND_DURATION );
  strcat( text[1], word );
  ShowMenu( text, 4 );

  CheckParams();
}

void EnterInt( int* pi )
{
  int n;
  char c, word[STRINGSIZE];

  n = 0;

  while( ( c = GetKeystroke() ) != ' '  && 
			n                     < STRINGSIZE )
    {
		word[n++] = c;        
    }
    
  word[n] = '\0';

  sscanf( word, "%d", pi );
}

void EnterFloat( float* pf )
{
  int n;
  char c, word[STRINGSIZE];

  n = 0;

  while( ( c = GetKeystroke() ) != ' '  && 
			n                     < STRINGSIZE )
    {
		word[n++] = c;        
    }
    
  word[n] = '\0';

  sscanf( word, "%f", pf );
}

void EnterText( char* ps )
{
  int n;
  char c, word[STRINGSIZE];

  n = 0;

  while( ( c = GetKeystroke() ) != ' '  && 
			n                     < STRINGSIZE )
    {
		word[n++] = c;        
    }
    
  word[n] = '\0';

  sscanf( word, "%s", ps );
}

void date( char* p)
{
  FILE *fp;

  if( ( fp =	popen( "date", "r")) ==	 NULL) 
    {
		printf( "cannot access date");
		return;
    }
  if( fgets( p, 50, fp) ==  NULL) 
    {
		printf( "cannot read date");
		return;
    }
  pclose(fp);
}

void process_id( char pid[] )
{
  FILE *fp;
  char line[ STRINGSIZE ];

  sprintf( line, "ps | grep %s", PROGRAM );
  
  if( ( fp =	popen( line, "r")) ==	 NULL) 
    {
		printf( "cannot access PID");
		return;
    }
  if( fgets( line, STRINGSIZE, fp) ==  NULL) 
    {
		printf( "cannot read PID");
		return;
    }
  pclose(fp);

  sscanf( line, "%s", pid );
}


static const char vcid_params_c[] = "$Header$";
#endif // !PARAMS_C_DEFINED
