///////////////////////////////////////////////////////////////////////
//
// balls.cc
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 16:12:25 2000
// written: Wed Sep  3 14:08:29 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_CC_DEFINED
#define BALLS_CC_DEFINED

#include "balls.h"

#include <cmath>
#include <cstdlib>
#include <vector>

#include "defs.h"
#include "graphics.h"
#include "params.h"
#include "timing.h"

#include "trace.h"
#include "debug.h"

namespace
{
  std::vector<unsigned char> theirBallmap;
  std::vector<unsigned char> theirHimap;
}

namespace Local
{
  void makeBallMap(std::vector<unsigned char>& vec, int size,
                   float radius, float sigma,
                   unsigned char background);

  bool colorsAlreadyGenerated = false;
}

///////////////////////////////////////////////////////////////////////
//
// Local function definitions
//
///////////////////////////////////////////////////////////////////////

void Local::makeBallMap(std::vector<unsigned char>& vec, int size,
                        float radius, float sigma,
                        unsigned char background)
{
DOTRACE("Local::makeBallMap");

  Balls::generateColors();

  const int bytes_per_pixel = 4;

  const int num_bytes = size*size*bytes_per_pixel;

  vec.resize(num_bytes);

  for (int i=0; i<size; ++i)
    {
      for (int j=0; j<size; ++j)
        {
          const float x   = float(i - size/2 + 0.5);
          const float y   = float(j - size/2 + 0.5);

          const float rsq = x*x + y*y;

          unsigned char index;

          if (x*x+y*y < radius*radius)
            {
              index =
                (unsigned char)
                (BALL_COLOR_MIN
                  + (BALL_COLOR_MAX - BALL_COLOR_MIN) * exp(-rsq/sigma));
            }
          else
            {
              index = (unsigned char)(background);
            }

          const size_t base_loc = bytes_per_pixel*(i*size + j);

          vec[base_loc + 0] = (unsigned char)(0xff * Balls::theColors[index][0]);
          vec[base_loc + 1] = (unsigned char)(0xff * Balls::theColors[index][1]);
          vec[base_loc + 2] = (unsigned char)(0xff * Balls::theColors[index][2]);
          vec[base_loc + 3] = (unsigned char)(0xff);
        }
    }
}

///////////////////////////////////////////////////////////////////////
//
// Ball member definitions
//
///////////////////////////////////////////////////////////////////////

void Ball::randomPosition(int width, int height,
                          int xborder, int yborder,
                          int arraysize)
{
DOTRACE("Ball::randomPosition");

  this->xpos = xborder + (width - arraysize - 2*xborder) * drand48();
  this->ypos = yborder + (height - arraysize - 2*yborder) * drand48();
}

bool Ball::isTooClose(const Ball& other, int min_dist) const
{
DOTRACE("Ball::isTooClose");

  return (fabs(this->xpos - other.xpos) < min_dist &&
          fabs(this->ypos - other.ypos) < min_dist);
}

void Ball::randomVelocity(int vel)
{
DOTRACE("Ball::randomVelocity");

  // Pick a random direction for the velocity
  float angle = TWOPI * drand48();

  this->xvel = vel * cos(angle);
  this->yvel = vel * sin(angle);
}

void Ball::nextPosition(int width, int height,
                        int xborder, int yborder,
                        int arraysize)
{
DOTRACE("Ball::nextPosition");

  this->xnext = this->xpos + this->xvel;
  this->ynext = this->ypos + this->yvel;

  if (this->xnext < xborder || this->xnext > width - xborder - arraysize)
    {
      this->xvel  = -this->xvel;
      this->xnext = this->xpos + this->xvel;
    }

  if (this->ynext < yborder || this->ynext > height - yborder - arraysize)
    {
      this->yvel  = -this->yvel;
      this->ynext = this->ypos + this->yvel;
    }
}

void Ball::collideIfNeeded(Ball& other, int min_dist)
{
DOTRACE("Ball::collideIfNeeded");

  const double dx = this->xnext - other.xnext;
  const double dy = this->ynext - other.ynext;

  if (fabs(dx) < min_dist && fabs(dy) < min_dist)
    {
      collide(other, dx, dy);
    }
}

void Ball::collide(Ball& other, double xij, double yij)
{
DOTRACE("Ball::collide");

  const double d    =  sqrt(xij*xij + yij*yij);
  const double xa   =  xij/d;
  const double ya   =  yij/d;
  const double xo   = -ya;
  const double yo   =  xa;

  const double vai  = this->xvel*xa + this->yvel*ya;
  const double vaj  = other.xvel*xa + other.yvel*ya;

  if (vai - vaj < 0.0)
    {
      const float voi  = this->xvel*xo + this->yvel*yo;
      const float voj  = other.xvel*xo + other.yvel*yo;
      /*
        ovi2 = vai*vai + voi*voi;
        ovj2 = vaj*vaj + voj*voj;
      */
      const float nvi2 = vaj*vaj + voi*voi;
      const float nvj2 = vai*vai + voj*voj;

      const float vij2 = vai*vai - vaj*vaj;

      const float fi   = sqrt(1. + vij2 / nvi2);
      const float fj   = sqrt(1. - vij2 / nvj2);

      this->xvel = fi * (voi * xo + vaj * xa);
      this->yvel = fi * (voi * yo + vaj * ya);

      other.xvel = fj * (voj * xo + vai * xa);
      other.yvel = fj * (voj * yo + vai * ya);
    }

  this->xnext = this->xpos + this->xvel;
  this->ynext = this->ypos + this->yvel;

  other.xnext = other.xpos + other.xvel;
  other.ynext = other.ypos + other.yvel;

}

void Ball::twist(double angle)
{
DOTRACE("Ball::twist");

  const double a11  =  cos(angle);
  const double a12  =  sin(angle);
  const double a21  = -sin(angle);
  const double a22  =  cos(angle);

  const double x = this->xvel;
  const double y = this->yvel;

  if (drand48() < 0.5)
    {
      this->xvel = a11*x + a12*y;
      this->yvel = a21*x + a22*y;
    }
  else
    {
      this->xvel =  a11*x - a12*y;
      this->yvel = -a21*x + a22*y;
    }
}

void Ball::copy()
{
DOTRACE("Ball::copy");

  this->xpos = this->xnext;
  this->ypos = this->ynext;
}

void Ball::draw(Graphics& gfx, unsigned char* bitmap, int size)
{
DOTRACE("Ball::draw");

  gfx.writePixmap(int(round(this->xpos)),
                  int(round(this->ypos)),
                  bitmap, size);
}

///////////////////////////////////////////////////////////////////////
//
// Balls member definitions
//
///////////////////////////////////////////////////////////////////////

Balls::Balls(const Params& params) : itsParams(params) {}

const int Balls::MAX_BALLS;
const int Balls::COLOR_NUMBER;
float Balls::theColors[Balls::COLOR_NUMBER][3];

void Balls::initialize(Graphics& gfx)
{
DOTRACE("Balls::initialize");

  for (int i=0; i < itsParams.ballNumber; ++i)
    {

      // Pick a random initial location that is not too close to the other balls
      bool too_close;

      do
        {
          too_close = false;

          itsBalls[i].randomPosition(gfx.width(), gfx.height(),
                                     itsParams.borderX,
                                     itsParams.borderY,
                                     itsParams.ballPixmapSize);

          for (int j=0; j<i; ++j)
            {
              if (itsBalls[i].isTooClose(itsBalls[j],
                                         itsParams.ballMinDistance))
                {
                  too_close = true;
                  break;
                }
            }
        } while (too_close);

    itsBalls[i].randomVelocity(itsParams.ballVelocity);
  }

}

void Balls::nextBalls(Graphics& gfx)
{
DOTRACE("Balls::nextBalls");

  for (int i=0; i<itsParams.ballNumber; ++i)
    {
      itsBalls[i].nextPosition(gfx.width(), gfx.height(),
                               itsParams.borderX, itsParams.borderY,
                               itsParams.ballPixmapSize);
    }

  for (int i=0; i<itsParams.ballNumber-1; ++i)
    {
      for (int j=i+1; j<itsParams.ballNumber; ++j)
        {
          itsBalls[i].collideIfNeeded(itsBalls[j],
                                      itsParams.ballMinDistance);
        }
    }

  for (int i=0; i<itsParams.ballNumber; ++i)
    {
      itsBalls[i].twist(itsParams.ballTwistAngle);
    }
}

void Balls::moveBalls(Graphics& gfx)
{
DOTRACE("Balls::moveBalls");

  gfx.clearBackBuffer();

  for (int i = 0; i < itsParams.ballNumber; ++i)
    {
      itsBalls[i].draw(gfx, &theirBallmap[0], itsParams.ballPixmapSize);
    }

  gfx.drawCross();

  gfx.swapBuffers();
}

void Balls::copyBalls()
{
DOTRACE("Balls::copyBalls");

  for (int i=0; i<itsParams.ballNumber; ++i)
    {
      itsBalls[i].copy();
    }
}

void Balls::drawNBalls(Graphics& gfx, int first, int last,
                       unsigned char* bitmap)
{
DOTRACE("Balls::drawNBalls");

  while (first != last)
    {
      itsBalls[first].draw(gfx, bitmap, itsParams.ballPixmapSize);
      ++first;
    }
}

void Balls::drawNHiBalls(Graphics& gfx, int first, int last,
                         unsigned char* bitmap)
{
DOTRACE("Balls::drawNHiBalls");

  drawNBalls(gfx, first, last, bitmap);
}

void Balls::prepare(Graphics& gfx)
{
DOTRACE("Balls::prepare");

  initialize(gfx);

  Local::makeBallMap(theirHimap, itsParams.ballPixmapSize,
                     itsParams.ballRadius, itsParams.ballSigma2, 255);
  Local::makeBallMap(theirBallmap, itsParams.ballPixmapSize,
                     itsParams.ballRadius, itsParams.ballSigma2, 0);
}

void Balls::generateColors()
{
DOTRACE("Balls::generateColors");

  if (Local::colorsAlreadyGenerated) return;

  float lmin =   0.;
  float lmax = 196.;

  int n;
  float ratio;

  for (n=0; n<BALL_COLOR_MIN; ++n)
    theColors[n][0] = theColors[n][1] = theColors[n][2] = 0.;

  for (n=BALL_COLOR_MIN; n<=BALL_COLOR_MAX; ++n)
    {
      ratio = lmin/lmax +
        ((lmax-lmin)*(n-BALL_COLOR_MIN)/
         (lmax*(BALL_COLOR_MAX-BALL_COLOR_MIN)));

      theColors[n][0] = 1.0*ratio; DebugEval(theColors[n][0]);
      theColors[n][1] = 0.5*ratio; DebugEval(theColors[n][1]);
      theColors[n][2] = 0.0*ratio; DebugEvalNL(theColors[n][2]);
    }

  for (n=BALL_COLOR_MAX+1; n<COLOR_NUMBER; ++n)
    theColors[n][0] = theColors[n][1] = theColors[n][2] = 1.;

  Local::colorsAlreadyGenerated = true;
}

void Balls::runTrial(Graphics& gfx, timeval* starttime, TrialType ttype)
{
DOTRACE("Balls::runTrial");

  Timing::mainTimer.set();

  Timing::getTime(starttime);

  gfx.clearFrontBuffer();
  gfx.clearBackBuffer();

  if (ttype == Balls::PASSIVE)
    {
      gfx.drawMessage("PASSIVE");
    }
  else if (ttype == Balls::CHECK_ALL)
    {
      gfx.drawMessage(" ALL");
    }
  else if (ttype == Balls::CHECK_ONE)
    {
      gfx.drawMessage("TRACK");
    }

  gfx.swapBuffers();

  prepare(gfx);

  gfx.gfxWait(itsParams.pauseSeconds);

  // Show the initial position of the balls
  Timing::mainTimer.set();

  gfx.clearFrontBuffer();
  gfx.drawCross();

  drawNBalls(gfx, 0, itsParams.ballNumber, &theirBallmap[0]);

  if (ttype == Balls::CHECK_ALL ||
      ttype == Balls::CHECK_ONE)
    {
      drawNHiBalls(gfx, 0, itsParams.ballTrackNumber, &theirHimap[0]);
      gfx.drawCross();
    }

  gfx.swapBuffers();

  gfx.gfxWait(itsParams.remindSeconds);

  if (ttype == Balls::CHECK_ALL ||
      ttype == Balls::CHECK_ONE)
    {
      gfx.clearFrontBuffer();
      gfx.drawCross();
      drawNBalls(gfx, 0, itsParams.ballNumber, &theirBallmap[0]);
      gfx.swapBuffers();
    }

  for (int i=0; i<itsParams.remindsPerEpoch; ++i)
    {
      for (int j=0; j<itsParams.framesPerRemind; ++j)
        {
          nextBalls(gfx);
          moveBalls(gfx);
          copyBalls();
        }

      Timing::mainTimer.set();

      if (ttype == Balls::CHECK_ALL)
        {
          gfx.clearBackBuffer();

          gfx.drawCross();
          drawNBalls(gfx, 0, itsParams.ballNumber, &theirBallmap[0]);
          drawNHiBalls(gfx, 0, itsParams.ballTrackNumber, &theirHimap[0]);

          gfx.swapBuffers();

          Timing::addToStimulusStack(LEFTBUTTON);

          gfx.gfxWait(itsParams.remindSeconds);
        }
      else if (ttype == Balls::CHECK_ONE)
        {
          // Randomly choose whether the highlighted ball will be a
          // target or a non-target
          bool pick_target = (drand48() > 0.5);

          // Pick a random ball
          int random_ball;
          if (pick_target)
            random_ball = int(itsParams.ballTrackNumber * drand48());
          else
            random_ball = int((itsParams.ballNumber - itsParams.ballTrackNumber) * drand48())
              + itsParams.ballTrackNumber;

          // Redraw the balls with the random ball highlighted
          gfx.clearBackBuffer();

          gfx.drawCross();
          drawNBalls(gfx, 0, itsParams.ballNumber, &theirBallmap[0]);
          drawNHiBalls(gfx, random_ball, random_ball+1, &theirHimap[0]);

          gfx.swapBuffers();

          // Note what the correct response should be for the random ball
          if (random_ball < itsParams.ballTrackNumber)
            Timing::addToStimulusStack(LEFTBUTTON);
          else
            Timing::addToStimulusStack(MIDDLEBUTTON);

          gfx.gfxWait(itsParams.remindSeconds / 2.0);

          Timing::mainTimer.set();

          // Redraw the balls, but now with the correct balls
          // highlighted in order to cue the next trial
          gfx.clearBackBuffer();

          gfx.drawCross();
          drawNHiBalls(gfx, random_ball, random_ball+1, &theirBallmap[0]);
          drawNBalls(gfx, itsParams.ballTrackNumber, itsParams.ballNumber, &theirBallmap[0]);
          drawNHiBalls(gfx, 0, itsParams.ballTrackNumber, &theirHimap[0]);

          gfx.swapBuffers();

          gfx.gfxWait(itsParams.remindSeconds / 2.0);
      }
      else
        {
          gfx.gfxWait(itsParams.remindSeconds);
        }

      gfx.clearFrontBuffer();
      gfx.drawCross();

      drawNBalls(gfx, 0, itsParams.ballNumber, &theirBallmap[0]);
    }

  gfx.clearFrontBuffer();
  gfx.clearBackBuffer();
  gfx.drawCross();
  gfx.swapBuffers();
}

static const char vcid_balls_cc[] = "$Header$";
#endif // !BALLS_CC_DEFINED
