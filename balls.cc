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

#include "graphics.h"
#include "params.h"
#include "timing.h"

#include "trace.h"
#include "debug.h"

namespace
{
  typedef unsigned char ubyte;

  void makeBallPixmap(std::vector<ubyte>& vec, int size,
                      double radius, double sigma,
                      ubyte bkg_r, ubyte bkg_g, ubyte bkg_b)
  {
    DOTRACE("<balls.cc>::makeBallPixmap");

    const double tint_r = 0.0;
    const double tint_g = 1.0;
    const double tint_b = 0.5;

    const int bytes_per_pixel = 4;

    const int num_bytes = size*size*bytes_per_pixel;

    vec.resize(num_bytes);

    const double bound = radius*radius;

    for (int i=0; i<size; ++i)
      {
        for (int j=0; j<size; ++j)
          {
            const double x   = double(i - size/2 + 0.5);
            const double y   = double(j - size/2 + 0.5);

            const double rsq = x*x + y*y;

            const size_t base_loc = bytes_per_pixel*(i*size + j);

            if (rsq < bound)
              {
                const double t = exp(-rsq/sigma);

                vec[base_loc + 0] = ubyte(255 * t * tint_r);
                vec[base_loc + 1] = ubyte(255 * t * tint_g);
                vec[base_loc + 2] = ubyte(255 * t * tint_b);
                vec[base_loc + 3] = ubyte(255);
              }
            else
              {
                vec[base_loc + 0] = bkg_r;
                vec[base_loc + 1] = bkg_g;
                vec[base_loc + 2] = bkg_b;
                vec[base_loc + 3] = ubyte(255);
              }
          }
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
  const double angle = 2 * M_PI * drand48();

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

timeval Balls::runTrial(Graphics& gfx, Timing& timing,
                        TrialType ttype)
{
DOTRACE("Balls::runTrial");

  Timer timer;

  timer.reset();

  // FIXME can't the caller just do this?
  const timeval starttime = Timing::getTime();

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

  initialize(gfx);

  std::vector<ubyte> pixmap;
  std::vector<ubyte> hilitemap;

  makeBallPixmap(hilitemap, itsParams.ballPixmapSize,
                 itsParams.ballRadius, itsParams.ballSigma2,
                 255, 255, 255);
  makeBallPixmap(pixmap, itsParams.ballPixmapSize,
                 itsParams.ballRadius, itsParams.ballSigma2,
                 0, 0, 0);

  gfx.gfxWait(timer, itsParams.pauseSeconds);

  // Show the initial position of the balls
  timer.reset();

  gfx.clearFrontBuffer();
  gfx.drawCross();

  drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0]);

  if (ttype == Balls::CHECK_ALL ||
      ttype == Balls::CHECK_ONE)
    {
      drawNHiBalls(gfx, 0, itsParams.ballTrackNumber, &hilitemap[0]);
      gfx.drawCross();
    }

  gfx.swapBuffers();

  gfx.gfxWait(timer, itsParams.remindSeconds);

  if (ttype == Balls::CHECK_ALL ||
      ttype == Balls::CHECK_ONE)
    {
      gfx.clearFrontBuffer();
      gfx.drawCross();
      drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0]);
      gfx.swapBuffers();
    }

  for (int i=0; i<itsParams.remindsPerEpoch; ++i)
    {
      for (int j=0; j<itsParams.framesPerRemind; ++j)
        {
          nextBalls(gfx);

          gfx.clearBackBuffer();

          for (int i = 0; i < itsParams.ballNumber; ++i)
            {
              itsBalls[i].draw(gfx, &pixmap[0],
                               itsParams.ballPixmapSize);
            }

          gfx.drawCross();
          gfx.swapBuffers();

          copyBalls();
        }

      timer.reset();

      if (ttype == Balls::CHECK_ALL)
        {
          gfx.clearBackBuffer();

          gfx.drawCross();
          drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0]);
          drawNHiBalls(gfx, 0, itsParams.ballTrackNumber, &hilitemap[0]);

          gfx.swapBuffers();

          timing.addToStimulusStack(BUTTON1);

          gfx.gfxWait(timer, itsParams.remindSeconds);
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
          drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0]);
          drawNHiBalls(gfx, random_ball, random_ball+1, &hilitemap[0]);

          gfx.swapBuffers();

          // Note what the correct response should be for the random ball
          if (random_ball < itsParams.ballTrackNumber)
            timing.addToStimulusStack(BUTTON1);
          else
            timing.addToStimulusStack(BUTTON2);

          gfx.gfxWait(timer, itsParams.remindSeconds / 2.0);

          timer.reset();

          // Redraw the balls, but now with the correct balls
          // highlighted in order to cue the next trial
          gfx.clearBackBuffer();

          gfx.drawCross();
          drawNHiBalls(gfx, random_ball, random_ball+1, &pixmap[0]);
          drawNBalls(gfx, itsParams.ballTrackNumber, itsParams.ballNumber, &pixmap[0]);
          drawNHiBalls(gfx, 0, itsParams.ballTrackNumber, &hilitemap[0]);

          gfx.swapBuffers();

          gfx.gfxWait(timer, itsParams.remindSeconds / 2.0);
      }
      else
        {
          gfx.gfxWait(timer, itsParams.remindSeconds);
        }

      gfx.clearFrontBuffer();
      gfx.drawCross();

      drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0]);
    }

  gfx.clearFrontBuffer();
  gfx.clearBackBuffer();
  gfx.drawCross();
  gfx.swapBuffers();

  return starttime;
}

static const char vcid_balls_cc[] = "$Header$";
#endif // !BALLS_CC_DEFINED
