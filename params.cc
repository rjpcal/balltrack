///////////////////////////////////////////////////////////////////////
//
// params.cc
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:30:51 2000
// written: Wed Sep  3 14:19:41 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef PARAMS_CC_DEFINED
#define PARAMS_CC_DEFINED

#include "params.h"

#include <cstring>
#include <cstdio>

#include "application.h"
#include "defs.h"
#include "graphics.h"

#include "trace.h"
#include "debug.h"

bool MAKING_MOVIE = false;
AppMode APPLICATION_MODE = EYE_TRACKING;
int FMRI_SESSION_NUMBER = 1;

int   FUDGEFRAME = 10;

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


FILE* ParamFile::openfile(Application* app, char mode, char extension[])
{
DOTRACE("ParamFile::openfile");

  FILE* fp;

  char fname[STRINGSIZE];

  sprintf( fname, "%s.%s", FILENAME, extension );

  char mode_string[2] = { mode, '\0' };

  if( ( fp = fopen( fname, mode_string) ) == NULL )
    {
      printf( "cannot open %s in mode '%s'\n", fname, mode_string);
      app->quit(0);
    }

  return fp;
}

void ParamFile::fetchLine()
{
  fgets(itsLine, 120, itsFile);
}

ParamFile::ParamFile(Application* app, char mode, char extension[]) :
  itsFile(openfile(app, mode, extension))
{}

ParamFile::~ParamFile()
{
  fclose(itsFile);
}

void ParamFile::getInt(int& var)
{
  fetchLine();
  sscanf(itsLine, "%s %d", itsText, &var);
}

void ParamFile::getChar(char& var)
{
  fetchLine();
  sscanf(itsLine, "%s %c", itsText, &var);
}

void ParamFile::getFloat(float& var)
{
  fetchLine();
  sscanf(itsLine, "%s %f", itsText, &var);
}

void ParamFile::getText(char* var)
{
  fetchLine();
  sscanf(itsLine, "%s %s", itsText, var);
}

void ParamFile::ignoreText()
{
  char dummy[STRINGSIZE];
  getText(dummy);
}

void ParamFile::putInt(int var, const char* name)
{
  fprintf(itsFile, "%-19s %d\n", name, var);
}

void ParamFile::putChar(char var, const char* name)
{
  fprintf(itsFile, "%-19s %c\n", name, var);
}

void ParamFile::putFloat(float var, const char* name)
{
  fprintf(itsFile, "%-19s %.2f\n", name, var);
}

void ParamFile::putText(const char* var, const char* name)
{
  fprintf(itsFile, "%-19s %+s\n", name, var);
}

void Params::readParams(Application* app, char extension[])
{
DOTRACE("Params::readParams");

  ParamFile pmfile(app, READ, extension);

  pmfile.getInt(   DISPLAY_X  );
  pmfile.getInt(   DISPLAY_Y  );
  pmfile.getInt(   CYCLE_NUMBER  );
  pmfile.getFloat( WAIT_DURATION  );
  pmfile.getFloat( EPOCH_DURATION  );
  pmfile.getFloat( PAUSE_DURATION  );
  pmfile.getFloat( REMIND_DURATION  );
  pmfile.getInt(   REMINDS_PER_EPOCH  );
  pmfile.getInt(   FRAMES_PER_REMIND  );
  pmfile.getInt(   BALL_NUMBER  );
  pmfile.getInt(   BALL_TRACK_NUMBER  );
  pmfile.getInt(   BALL_VELOCITY  );
  pmfile.getInt(   BALL_ARRAY_SIZE  );
  pmfile.getInt(   BALL_MIN_DISTANCE  );
  pmfile.getFloat( BALL_RADIUS  );
  pmfile.getFloat( BALL_SIGMA2  );
  pmfile.getFloat( BALL_TWIST_ANGLE  );
  pmfile.getText(  OBSERVER  );
  pmfile.getText(  FILENAME  );

  // this is a placeholder for the application mode, but we ignore the
  // value in the file since the actual application mode is set at
  // startup time
  pmfile.ignoreText();

  pmfile.getInt( (FMRI_SESSION_NUMBER) );

  RecomputeParams(app->graphics());
}

void RecomputeParams(Graphics* gfx)
{
DOTRACE("RecomputeParams");

  float time_between_reminds;

  BORDER_X    = ( gfx->width() - DISPLAY_X ) / 2;
  BORDER_Y    = ( gfx->height() - DISPLAY_Y ) / 2;

  time_between_reminds = ( EPOCH_DURATION - PAUSE_DURATION - REMIND_DURATION )
         / REMINDS_PER_EPOCH;

  double frametime = gfx->frameTime();

  printf( " Video frame time %7.4lf ms\n", frametime );

  FRAMES_PER_REMIND    = (int)( 1000.0*(time_between_reminds-REMIND_DURATION)
                                / frametime ) - FUDGEFRAME;

  DebugEval(time_between_reminds);
  DebugEval(frametime);
  DebugEvalNL(FUDGEFRAME);

  DebugEvalNL(FRAMES_PER_REMIND);
}

void Params::writeParams(Application* app, char extension[])
{
DOTRACE("Params::writeParams");

  ParamFile pmfile(app, WRITE, extension);

  appendParams(pmfile);
}

void Params::appendParams(ParamFile& pmfile)
{
DOTRACE("Params::appendParams");

  pmfile.putInt(   (DISPLAY_X),        ("DISPLAY_X") );
  pmfile.putInt(   (DISPLAY_Y),        ("DISPLAY_Y") );
  pmfile.putInt(   (CYCLE_NUMBER),     ("CYCLE_NUMBER") );
  pmfile.putFloat( (WAIT_DURATION),    ("WAIT_DURATION") );
  pmfile.putFloat( (EPOCH_DURATION),   ("EPOCH_DURATION") );
  pmfile.putFloat( (PAUSE_DURATION),   ("PAUSE_DURATION") );
  pmfile.putFloat( (REMIND_DURATION),  ("REMIND_DURATION") );
  pmfile.putInt(   (REMINDS_PER_EPOCH),("REMINDS_PER_EPOCH") );
  pmfile.putInt(   (FRAMES_PER_REMIND),("FRAMES_PER_REMIND") );
  pmfile.putInt(   (BALL_NUMBER),      ("BALL_NUMBER") );
  pmfile.putInt(   (BALL_TRACK_NUMBER),("BALL_TRACK_NUMBER") );
  pmfile.putInt(   (BALL_VELOCITY),    ("BALL_VELOCITY") );
  pmfile.putInt(   (BALL_ARRAY_SIZE),  ("BALL_ARRAY_SIZE") );
  pmfile.putInt(   (BALL_MIN_DISTANCE),("BALL_MIN_DISTANCE") );
  pmfile.putFloat( (BALL_RADIUS),      ("BALL_RADIUS") );
  pmfile.putFloat( (BALL_SIGMA2),      ("BALL_SIGMA2") );
  pmfile.putFloat( (BALL_TWIST_ANGLE), ("BALL_TWIST_ANGLE") );
  pmfile.putText(  (OBSERVER),         ("OBSERVER") );
  pmfile.putText(  (FILENAME),         ("FILENAME") );

  const char* app_mode = "unknown";
  switch (APPLICATION_MODE)
    {
    case TRAINING:      app_mode = "TRAINING";     break;
    case EYE_TRACKING:  app_mode = "EYE_TRACKING"; break;
    case FMRI_SESSION:  app_mode = "FMRI_SESSION"; break;
    }

  pmfile.putText(   (app_mode),         ("APPLICATION_MODE") );
  pmfile.putInt(    (FMRI_SESSION_NUMBER),("FMRI_SESSION_NUMBER") );
}

void Params::logParams(Application* app, ParamFile& logfile)
{
DOTRACE("Params::logParams");

  writeParams(app, "cur");

  char text[STRINGSIZE];
  date(text);
  fprintf( logfile.fp(), "\n\n%s\n\n", text);

  appendParams(logfile);

  fprintf( logfile.fp(), "\n\n");
}

void Params::displayParams(Application* app)
{
DOTRACE("Params::displayParams");

  const int MAXPARAMS = 60;

  int nparams = 0;
  char params[MAXPARAMS][STRINGSIZE];

  writeParams(app, "sta");

  ParamFile pmfile(app, READ, "sta");

  int curparam = MAXPARAMS - 1;

  while( curparam >= 0 &&
         fgets( params[curparam], STRINGSIZE, pmfile.fp()) !=  NULL )
    {
      --curparam;
      ++nparams;
    }

  app->graphics()->clearFrontBuffer();
  app->graphics()->clearBackBuffer();
  app->graphics()->showParams(params+curparam+1, nparams);
  app->graphics()->swapBuffers();
}

void SetParameters1(Application* app)
{
DOTRACE("SetParameters1");

  char word[STRINGSIZE], text[4][STRINGSIZE];

  Graphics* gfx = app->graphics();

  gfx->clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii)
    {
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

  RecomputeParams(gfx);
}

void SetParameters2(Application* app)
{
DOTRACE("SetParameters2");

  char word[STRINGSIZE], text[4][STRINGSIZE];

  Graphics* gfx = app->graphics();

  gfx->clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii)
    {
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

  RecomputeParams(gfx);
}

void SetParameters3(Application* app)
{
DOTRACE("SetParameters2");

  char word[STRINGSIZE], text[4][STRINGSIZE];

  Graphics* gfx = app->graphics();

  gfx->clearFrontBuffer();
  for (int ii = 0; ii < 2; ++ii)
    {
      gfx->clearBackBuffer();
      gfx->swapBuffers();
    }

  sprintf( text[0], "       SESSION_NUMBER" );
  sprintf( text[1], "" );
  sprintf( text[2], "" );
  sprintf( text[3], "       %-8d",
                          FMRI_SESSION_NUMBER );

  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  EnterInt( app, &FMRI_SESSION_NUMBER );
  sprintf( word, "       %-8d", FMRI_SESSION_NUMBER );
  strcat( text[1], word );
  gfx->showMenu(text, 4);
  gfx->swapBuffers();

  RecomputeParams(gfx);
}

void GetWord(Application* app, char* buf, int sz)
{
DOTRACE("GetWord");

  int n = 0;
  char c;

  while( ( c = app->getKeystroke() ) != ' '  && n < sz )
    {
      buf[n++] = c;
    }

  buf[n] = '\0';
}

void EnterInt( Application* app, int* pi )
{
DOTRACE("EnterInt");

  char word[STRINGSIZE];
  GetWord(app, word, STRINGSIZE);

  sscanf( word, "%d", pi );
}

void EnterFloat( Application* app, float* pf )
{
DOTRACE("EnterFloat");

  char word[STRINGSIZE];
  GetWord(app, word, STRINGSIZE);

  sscanf( word, "%f", pf );
}

void EnterText( Application* app, char* ps )
{
DOTRACE("EnterText");

  char word[STRINGSIZE];
  GetWord(app, word, STRINGSIZE);

  sscanf( word, "%s", ps );
}

void date( char* p )
{
DOTRACE("date");

  FILE *fp;

  if ( ( fp =    popen( "date", "r")) ==  NULL)
    {
      printf( "cannot access date");
      return;
    }
  if ( fgets( p, 50, fp) ==  NULL)
    {
      printf( "cannot read date");
      return;
    }
  pclose(fp);
}

void process_id( char pid[] )
{
DOTRACE("process_id");

  FILE *fp;
  char line[ STRINGSIZE ];

  sprintf( line, "ps | grep %s", PROGRAM );

  if ( ( fp =    popen( line, "r")) ==    NULL)
    {
      printf( "cannot access PID");
      return;
    }
  if ( fgets( line, STRINGSIZE, fp) ==  NULL)
    {
      printf( "cannot read PID");
      return;
    }
  pclose(fp);

  sscanf( line, "%s", pid );
}


static const char vcid_params_cc[] = "$Header$";
#endif // !PARAMS_CC_DEFINED
