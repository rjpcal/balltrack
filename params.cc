///////////////////////////////////////////////////////////////////////
//
// params.c
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:30:51 2000
// written: Tue Feb 29 14:45:08 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PARAMS_C_DEFINED
#define PARAMS_C_DEFINED

#include "params.h"

#include <cstring>
#include <cstdio>

#include "application.h"
#include "graphics.h"
#include "defs.h"

#include "trace.h"
#include "debug.h"

// #define      YES             	   'y'
// #define      NO              	   'n'
// #define      READY           	   ' '
// #define      QUIT            	   'q'


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
char  PROGRAM[STRINGSIZE];
char  FILENAME[STRINGSIZE];
char  OBSERVER[STRINGSIZE];
double FRAMETIME;

int   BORDER_X;
int   BORDER_Y;
int   FRAMES_PER_REMIND;

// Prototypes
void GetWord(Application* app, char* buf, int sz);

void EnterInt( Application* app, int* pi );
void EnterFloat( Application* app, float* pf );
void EnterText( Application* app, char* ps );
void date( char* p);
void process_id( char pid[] );


/************************************************/


void WhoAreYou( Application* app )
{
  if( app->argc() < 2 )
    {
		printf( " Who are you?\n" );
		app->quit(0);
    }

  strcpy( PROGRAM,  app->argv(0) );
  strcpy( OBSERVER, app->argv(1) );
  strcpy( FILENAME, app->argv(1) );
}

namespace {
  char LINE[STRINGSIZE];
  char TEXT[STRINGSIZE];
}

#define GETINT(name) {fgets(LINE,120,fp);sscanf(LINE,"%s %d",TEXT,&(name));}
#define GETCHAR(name) {fgets(LINE,120,fp);sscanf(LINE,"%s %c",TEXT,&(name));}
#define GETFLOAT(name) {fgets(LINE,120,fp);sscanf(LINE,"%s %f",TEXT,&(name));}
#define GETTEXT(name) {fgets(LINE,120,fp);sscanf(LINE,"%s %s",TEXT,(name));}
#define GETINTL(name) {fgets(LINE,120,fp);sscanf(LINE,"%s %d %d %d %d",TEXT,&(name)[0],&(name)[1],&(name)[2],&(name)[3]);}
#define GETTEXTL(name) {fgets(LINE,120,fp);sscanf(LINE,"%s %s %s %s %s",TEXT,(name)[0],(name)[1],(name)[2],(name)[3]);}

#define PUTINT(name,text) {fprintf(fp,"%-19s %d\n",(text),(name));}
#define PUTCHAR(name,text) {fprintf(fp,"%-19s %c\n",(text),(name));}
#define PUTFLOAT(name,text) {fprintf(fp,"%-19s %.2f\n",(text),(name));}
#define PUTTEXT(name,text) {fprintf(fp,"%-19s %+s\n",(text),(name));}
#define PUTINTL(name,text) {fprintf(fp,"%-19s %d %d %d %d\n",(text),(name)[0],(name)[1],(name)[2],(name)[3]);}
#define PUTTEXTL(name,text) {fprintf(fp,"%-19s %+s %+s %+s %+s\n",(text),(name)[0],(name)[1],(name)[2],(name)[3]);}

#define PRINTINT(name,text) {printf( "%-19s %d\n",(text),(name));}
#define PRINTCHAR(name,text) {printf( "%-19s %c\n",(text),(name));}
#define PRINTFLOAT(name,text) {printf( "%-19s %.2f\n",(text),(name));}
#define PRINTTEXT(name,text) {printf( "%-19s %+s\n",(text),(name));}
#define PRINTINTL(name,text) {printf( "%-19s %d %d %d %d\n",(text),(name)[0],(name)[1],(name)[2],(name)[3]);}
#define PRINTTEXTL(name,text) {printf( "%-19s %+s %+s %+s %+s\n",(text),(name)[0],(name)[1],(name)[2],(name)[3]);}

void ReadParams(Application* app, char extension[]) {
DOTRACE("ReadParams");

  FILE *fp;

  Openfile(app, &fp, OPTIONAL, extension );

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

  CheckParams(app->graphics());
}

void CheckParams(Graphics* gfx) {
DOTRACE("CheckParams");

  float time_between_reminds;

  BORDER_X    = ( gfx->width() - DISPLAY_X ) / 2; 
  BORDER_Y    = ( gfx->height() - DISPLAY_Y ) / 2;

  time_between_reminds = ( EPOCH_DURATION - PAUSE_DURATION - REMIND_DURATION )
	 / REMINDS_PER_EPOCH;

  FRAMES_PER_REMIND    = (int)( 1000.0*(time_between_reminds-REMIND_DURATION)
										  / FRAMETIME ) - FUDGEFRAME;

  DebugEval(time_between_reminds);
  DebugEval(FRAMETIME);
  DebugEvalNL(FUDGEFRAME);

  DebugEvalNL(FRAMES_PER_REMIND);
}

void WriteParams(Application* app, char extension[]) {
DOTRACE("WriteParams");

  FILE *fp;

  Openfile(app, &fp, WRITE, extension );

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

void LogParams(Application* app, FILE* fl) {
DOTRACE("LogParams");

  FILE *fp;
  char line[480], Cname[ 120 ];

  WriteParams(app, "cur");

  Openfile(app, &fp, READ, "cur" );

  char text[STRINGSIZE];
  date(text);
  fprintf( fl, "\n\n%s\n\n", text);

  while( fgets( line, 120, fp) !=  NULL)
    {
		fputs( line, fl);
    }

  Closefile( fp );

  fprintf( fl, "\n\n");
}

void ListParams(Application* app) {
DOTRACE("ListParams");

  FILE *fp;
  int nparams = 0;
  char params[60][STRINGSIZE];

  WriteParams(app, "sta");

  Openfile(app, &fp, READ, "sta");

  while( fgets( params[nparams], STRINGSIZE, fp) !=  NULL && nparams < 60 )
    {    
		params[nparams][ strlen( params[nparams] ) - 1 ] = '\0';
		nparams++;
    }

  Closefile( fp );

  app->graphics()->clearFrontBuffer();
  app->graphics()->clearBackBuffer();
  app->graphics()->showParams(params, nparams);
  app->graphics()->swapBuffers();
}

void Openfile(Application* app, FILE** fp, char mode, char extension[]) {
DOTRACE("Openfile");

  char fname[STRINGSIZE];

  sprintf( fname, "%s.%s", FILENAME, extension );

  if (mode == OPTIONAL) {
	 if( ( *fp = fopen( fname, "r") ) == NULL )
		{
		  printf( "cannot read from %s, will create it\n", fname );
		  *fp = NULL;
		}
  }
  else {
	 char mode_string[2] = { mode, '\0' };

	 if( ( *fp = fopen( fname, mode_string) ) == NULL )
		{
		  printf( "cannot open %s in mode '%s'\n", fname, mode_string);
		  app->quit(0);
		}
  }
}

void Closefile( FILE* fp ) {
DOTRACE("Closefile");

  if(fp != NULL)
	 fclose(fp);
}

void PrintParams() {
DOTRACE("PrintParams");

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

void SetParameters1(Application* app) {
DOTRACE("SetParameters1");

  char word[STRINGSIZE], text[4][STRINGSIZE];

  Graphics* gfx = app->graphics();

  gfx->clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii) {
	 gfx->clearBackBuffer();
	 gfx->swapBuffers();
  }

  sprintf( text[0], " BALL  NUMBER TRACK  VELOC  SIZE   MINDIS RADIUS SIGMA2 TWIST" );
  sprintf( text[1], "" );
  sprintf( text[2], "" );
  sprintf( text[3], "       %-6d %-6d %-6d %-6d %-6d %-6.1f %-6.1f %-6.3f", 
			  BALL_NUMBER, BALL_TRACK_NUMBER, BALL_VELOCITY,
			  BALL_ARRAY_SIZE, BALL_MIN_DISTANCE, BALL_RADIUS,
			  BALL_SIGMA2, BALL_TWIST_ANGLE );
  
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &BALL_NUMBER );
  sprintf( word, "       %-6d", BALL_NUMBER );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &BALL_TRACK_NUMBER );
  sprintf( word, " %-6d", BALL_TRACK_NUMBER );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &BALL_VELOCITY );
  sprintf( word, " %-6d", BALL_VELOCITY );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &BALL_ARRAY_SIZE );
  sprintf( word, " %-6d", BALL_ARRAY_SIZE );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &BALL_MIN_DISTANCE );
  sprintf( word, " %-6d", BALL_MIN_DISTANCE );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterFloat( app, &BALL_RADIUS );
  sprintf( word, " %-6.1f", BALL_RADIUS );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterFloat( app, &BALL_SIGMA2 );
  sprintf( word, " %-6.1f", BALL_SIGMA2 );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterFloat( app, &BALL_TWIST_ANGLE );
  sprintf( word, " %-6.3f", BALL_TWIST_ANGLE );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  CheckParams(gfx);
}

void SetParameters2(Application* app) {
DOTRACE("SetParameters2");

  char word[STRINGSIZE], text[4][STRINGSIZE];

  Graphics* gfx = app->graphics();

  gfx->clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii) {
	 gfx->clearBackBuffer();
	 gfx->swapBuffers();
  }

  sprintf( text[0], "       CYCL_NUM WAIT_DUR EPCH_DUR PAUS_DUR RMND_NUM RMND_DUR" );
  sprintf( text[1], "" );
  sprintf( text[2], "" );
  sprintf( text[3], "       %-8d %-8.2f %-8.2f %-8.2f %-8d %-8.2f", 
			  CYCLE_NUMBER, WAIT_DURATION, EPOCH_DURATION,
			  PAUSE_DURATION, REMINDS_PER_EPOCH, REMIND_DURATION );

  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &CYCLE_NUMBER );
  sprintf( word, "       %-8d", CYCLE_NUMBER );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterFloat( app, &WAIT_DURATION );
  sprintf( word, " %-8.2f", WAIT_DURATION );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterFloat( app, &EPOCH_DURATION );
  sprintf( word, " %-8.2f", EPOCH_DURATION );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterFloat( app, &PAUSE_DURATION );
  sprintf( word, " %-8.2f", PAUSE_DURATION );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &REMINDS_PER_EPOCH );
  sprintf( word, " %-8d", REMINDS_PER_EPOCH );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterFloat( app, &REMIND_DURATION );
  sprintf( word, " %-8.2f", REMIND_DURATION );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  CheckParams(gfx);
}

void GetWord(Application* app, char* buf, int sz) {
DOTRACE("GetWord");

  int n = 0;
  char c;

  while( ( c = app->getKeystroke() ) != ' '  && n < sz ) {
	 buf[n++] = c;        
  }
    
  buf[n] = '\0';
}

void EnterInt( Application* app, int* pi ) {
DOTRACE("EnterInt");

  char word[STRINGSIZE];
  GetWord(app, word, STRINGSIZE);

  sscanf( word, "%d", pi );
}

void EnterFloat( Application* app, float* pf ) {
DOTRACE("EnterFloat");

  char word[STRINGSIZE];
  GetWord(app, word, STRINGSIZE);

  sscanf( word, "%f", pf );
}

void EnterText( Application* app, char* ps ) {
DOTRACE("EnterText");

  char word[STRINGSIZE];
  GetWord(app, word, STRINGSIZE);

  sscanf( word, "%s", ps );
}

void date( char* p ) {
DOTRACE("date");

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

void process_id( char pid[] ) {
DOTRACE("process_id");

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
