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
#include <iostream>
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
                      double fg_r, double fg_g, double fg_b,
                      double bkg_r, double bkg_g, double bkg_b,
                      bool debug)
  {
    DOTRACE("<balls.cc>::makeBallPixmap");

    const int bytes_per_pixel = 4;

    const int num_bytes = size*size*bytes_per_pixel;

    vec.resize(num_bytes);

    for (int i=0; i<size; ++i)
      {
        for (int j=0; j<size; ++j)
          {
            const double x   = double(i - size/2 + 0.5);
            const double y   = double(j - size/2 + 0.5);

            const double rsq = x*x + y*y;
            const double r   = sqrt(rsq);

            const size_t base_loc = bytes_per_pixel*(i*size + j);

            if (debug &&
                (i == 0 || i == size-1 || j == 0 || j == size-1 ||
                 fabs(r-radius) < 2.0))
              {
                vec[base_loc + 0] = ubyte(127);
                vec[base_loc + 1] = ubyte(127);
                vec[base_loc + 2] = ubyte(127);
                vec[base_loc + 3] = ubyte(255);
              }
            else if (r < radius)
              {
                const double t = exp(-rsq/sigma);

                vec[base_loc + 0] = ubyte(255 * t * fg_r);
                vec[base_loc + 1] = ubyte(255 * t * fg_g);
                vec[base_loc + 2] = ubyte(255 * t * fg_b);
                vec[base_loc + 3] = ubyte(255);
              }
            else
              {
                vec[base_loc + 0] = ubyte(255 * bkg_r);
                vec[base_loc + 1] = ubyte(255 * bkg_g);
                vec[base_loc + 2] = ubyte(255 * bkg_b);
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
                          int xdisplay, int ydisplay,
                          int arraysize)
{
DOTRACE("Ball::randomPosition");

  const int xborder = (width-xdisplay)/2;
  const int yborder = (height-ydisplay)/2;

  this->xpos = xborder + (width - arraysize - 2*xborder) * drand48();
  this->ypos = yborder + (height - arraysize - 2*yborder) * drand48();
}

bool Ball::isTooClose(const Ball& other, int min_dist) const
{
DOTRACE("Ball::isTooClose");

  return (fabs(this->xpos - other.xpos) < min_dist &&
          fabs(this->ypos - other.ypos) < min_dist);
}

void Ball::randomSpeed(double vel)
{
DOTRACE("Ball::randomVelocity");

  // Pick a random direction for the velocity
  const double angle = 2 * M_PI * drand48();

  this->velx = vel * cos(angle);
  this->vely = vel * sin(angle);
}

void Ball::nextPosition(int width, int height,
                        int xdisplay, int ydisplay,
                        int arraysize, double lapsed_seconds)
{
DOTRACE("Ball::nextPosition");

  const int xborder = (width-xdisplay)/2;
  const int yborder = (height-ydisplay)/2;

  this->xnext = this->xpos + (lapsed_seconds * this->velx);
  this->ynext = this->ypos + (lapsed_seconds * this->vely);

  if (this->xnext < xborder || this->xnext > width - xborder - arraysize)
    {
      this->velx  = -this->velx;
      this->xnext = this->xpos + (lapsed_seconds * this->velx);
    }

  if (this->ynext < yborder || this->ynext > height - yborder - arraysize)
    {
      this->vely  = -this->vely;
      this->ynext = this->ypos + (lapsed_seconds * this->vely);
    }
}

void Ball::collideIfNeeded(Ball& other, int min_dist,
                           double lapsed_seconds)
{
DOTRACE("Ball::collideIfNeeded");

  const double xij = this->xnext - other.xnext; // units pixels
  const double yij = this->ynext - other.ynext; // units pixels

  if (fabs(xij) >= min_dist || fabs(yij) >= min_dist)
    return;

  const double d    =  sqrt(xij*xij + yij*yij); // units pixels
  const double xa   =  xij/d; // unitless
  const double ya   =  yij/d; // unitless
  const double xo   = -ya;    // unitless
  const double yo   =  xa;    // unitless

  const double vai  = this->velx*xa + this->vely*ya; // units pix/sec
  const double vaj  = other.velx*xa + other.vely*ya; // units pix/sec

  if (vai - vaj < 0.0)
    {
      const double voi  = this->velx*xo + this->vely*yo; // pix/sec
      const double voj  = other.velx*xo + other.vely*yo; // pix/sec
      /*
        ovi2 = vai*vai + voi*voi;
        ovj2 = vaj*vaj + voj*voj;
      */
      const double nvi2 = vaj*vaj + voi*voi; // pix^2/sec^2
      const double nvj2 = vai*vai + voj*voj; // pix^2/sec^2

      const double vij2 = vai*vai - vaj*vaj; // pix^2/sec^2

      const double fi   = sqrt(1. + vij2 / nvi2); // unitless
      const double fj   = sqrt(1. - vij2 / nvj2); // unitless

      this->velx = fi * (voi * xo + vaj * xa); // pix/sec
      this->vely = fi * (voi * yo + vaj * ya); // pix/sec

      other.velx = fj * (voj * xo + vai * xa); // pix/sec
      other.vely = fj * (voj * yo + vai * ya); // pix/sec
    }

  this->xnext = this->xpos + (lapsed_seconds * this->velx);
  this->ynext = this->ypos + (lapsed_seconds * this->vely);

  other.xnext = other.xpos + (lapsed_seconds * other.velx);
  other.ynext = other.ypos + (lapsed_seconds * other.vely);
}

void Ball::twist(double maxangle)
{
DOTRACE("Ball::twist");

  // Rotate the velocity vector by either +angle or -angle

  const double angle = 2 * (drand48() - 0.5) * maxangle;

  const double a11  =  cos(angle);
  const double a12  =  sin(angle);
  const double a21  = -sin(angle);
  const double a22  =  cos(angle);

  const double x = this->velx;
  const double y = this->vely;

  this->velx = a11*x + a12*y;
  this->vely = a21*x + a22*y;
}

void Ball::copy()
{
DOTRACE("Ball::copy");

  this->xpos = this->xnext;
  this->ypos = this->ynext;
}

void Ball::draw(Graphics& gfx, unsigned char* bitmap, int size,
                bool debug)
{
DOTRACE("Ball::draw");

  gfx.writePixmap(int(round(this->xpos)),
                  int(round(this->ypos)),
                  bitmap, size);

  if (debug)
    gfx.drawLine(this->xpos + size/2,
                 this->ypos + size/2,
                 this->xpos + size/2 - this->velx / 4,
                 this->ypos + size/2 - this->vely / 4);
}

///////////////////////////////////////////////////////////////////////
//
// Balls member definitions
//
///////////////////////////////////////////////////////////////////////

Balls::Balls(const Params& params) :
  itsParams(params),
  itsBalls()
{}

void Balls::pickInitialPositions(Graphics& gfx)
{
DOTRACE("Balls::pickInitialPositions");

  itsBalls.resize(itsParams.ballNumber);

  for (int i=0; i < itsParams.ballNumber; ++i)
    {
      bool too_close = true;

      // Pick a random initial location that is not too close to the
      // other balls
      while (too_close)
        {
          too_close = false;

          itsBalls[i].randomPosition(gfx.width(), gfx.height(),
                                     itsParams.displayX,
                                     itsParams.displayY,
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
        }

      itsBalls[i].randomSpeed(itsParams.ballSpeed *
                              itsParams.ballRadius);
    }
}

void Balls::pickNextPositions(Graphics& gfx, double lapsed_seconds)
{
DOTRACE("Balls::pickNextPositions");

  for (int i=0; i<itsParams.ballNumber; ++i)
    {
      itsBalls[i].nextPosition(gfx.width(), gfx.height(),
                               itsParams.displayX,
                               itsParams.displayY,
                               itsParams.ballPixmapSize,
                               lapsed_seconds);
    }

  for (int i=0; i<itsParams.ballNumber-1; ++i)
    {
      for (int j=i+1; j<itsParams.ballNumber; ++j)
        {
          itsBalls[i].collideIfNeeded(itsBalls[j],
                                      itsParams.ballMinDistance,
                                      lapsed_seconds);
        }
    }

  for (int i=0; i<itsParams.ballNumber; ++i)
    {
      itsBalls[i].twist(itsParams.ballTwistAngle);
    }

  for (int i=0; i<itsParams.ballNumber; ++i)
    {
      itsBalls[i].copy();
    }
}

void Balls::drawNBalls(Graphics& gfx, int first, int last,
                       unsigned char* bitmap, bool debug)
{
DOTRACE("Balls::drawNBalls");

  while (first != last)
    {
      itsBalls[first].draw(gfx, bitmap,
                           itsParams.ballPixmapSize, debug);
      ++first;
    }
}

void Balls::runTrial(Graphics& gfx, std::vector<Stimulus>& stimuli,
                     TrialType ttype)
{
DOTRACE("Balls::runTrial");

  Timepoint timer = Timepoint::now();

  gfx.clearBackBuffer();

  switch (ttype)
    {
    case PASSIVE:   gfx.drawMessage("PASSIVE"); break;
    case CHECK_ALL: gfx.drawMessage(" ALL"); break;
    case CHECK_ONE: gfx.drawMessage("TRACK"); break;
    }

  gfx.swapBuffers();

  pickInitialPositions(gfx);

  std::vector<ubyte> pixmap;
  std::vector<ubyte> hilitemap;

  makeBallPixmap(hilitemap, itsParams.ballPixmapSize,
                 itsParams.ballRadius, itsParams.ballSigma2,
                 0.0, 1.0, 0.5, // fg color
                 1.0, 1.0, 1.0, // bkg color
                 itsParams.showPhysics);
  makeBallPixmap(pixmap, itsParams.ballPixmapSize,
                 itsParams.ballRadius, itsParams.ballSigma2,
                 0.0, 1.0, 0.5, // fg color
                 0.0, 0.0, 0.0, // bkg color
                 itsParams.showPhysics);

  gfx.gfxWait(timer, itsParams.pauseSeconds);

  // Show the initial position of the balls
  timer.reset();

  gfx.clearBackBuffer();
  drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0],
             itsParams.showPhysics);
  if (ttype == Balls::CHECK_ALL || ttype == Balls::CHECK_ONE)
    {
      drawNBalls(gfx, 0, itsParams.ballTrackNumber, &hilitemap[0],
                 itsParams.showPhysics);
    }
  gfx.drawCross();
  gfx.swapBuffers();

  gfx.gfxWait(timer, itsParams.remindSeconds);

  if (ttype == Balls::CHECK_ALL || ttype == Balls::CHECK_ONE)
    {
      gfx.clearBackBuffer();
      drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0],
                 itsParams.showPhysics);
      gfx.drawCross();
      gfx.swapBuffers();
    }

  for (int i=0; i<itsParams.remindsPerEpoch; ++i)
    {
      const Timepoint tstart = Timepoint::now();

      Timepoint t0 = tstart;

      gfx.resetFrameTimer();

      // Here's the main loop where the balls are moving randomly
      int nframes = 0;
      while (true)
        {
          ++nframes;
          gfx.clearBackBuffer();
          drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0],
                     itsParams.showPhysics);
          gfx.drawCross();

          const double lapsed_sec = gfx.swapBuffers() / 1000.0;

          pickNextPositions(gfx, lapsed_sec);

          // amount of time lapsed in the entire motion sequence so far

          if (tstart.elapsedMsec()/1000.0 >= itsParams.ballMotionSeconds())
            break;
        }

      const double lapse = tstart.elapsedMsec();

      std::cout << " " << nframes << " frames in "
                << lapse << " msec ("
                << lapse/nframes << " msec/frame, "
                << 1000*nframes/lapse << " frames/sec)\n";

      timer.reset();

      // Now set up to get the observer's response
      if (ttype == Balls::CHECK_ALL)
        {
          gfx.clearBackBuffer();
          drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0],
                     itsParams.showPhysics);
          drawNBalls(gfx, 0, itsParams.ballTrackNumber, &hilitemap[0],
                     itsParams.showPhysics);
          gfx.drawCross();
          gfx.swapBuffers();

          stimuli.push_back(Stimulus(Timepoint::now(), BUTTON1));

          gfx.gfxWait(timer, itsParams.remindSeconds);
        }
      else if (ttype == Balls::CHECK_ONE)
        {
          // Randomly choose whether the highlighted ball will be a
          // target or a non-target
          const bool pick_target = (drand48() > 0.5);

          // Pick a random ball
          int random_ball;
          if (pick_target)
            random_ball = int(itsParams.ballTrackNumber * drand48());
          else
            random_ball =
              itsParams.ballTrackNumber
              + int((itsParams.ballNumber - itsParams.ballTrackNumber)
                    * drand48());

          // Redraw the balls with the random ball highlighted
          gfx.clearBackBuffer();
          drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0],
                     itsParams.showPhysics);
          drawNBalls(gfx, random_ball, random_ball+1, &hilitemap[0],
                     itsParams.showPhysics);
          gfx.drawCross();
          gfx.swapBuffers();

          // Note what the correct response should be for the random
          // ball
          if (random_ball < itsParams.ballTrackNumber)
            stimuli.push_back(Stimulus(Timepoint::now(), BUTTON1));
          else
            stimuli.push_back(Stimulus(Timepoint::now(), BUTTON2));

          gfx.gfxWait(timer, itsParams.remindSeconds / 2.0);

          timer.reset();

          // Redraw the balls, but now with the correct balls
          // highlighted in order to cue the next trial
          gfx.clearBackBuffer();
          drawNBalls(gfx, itsParams.ballTrackNumber,
                     itsParams.ballNumber, &pixmap[0],
                     itsParams.showPhysics);
          drawNBalls(gfx, 0, itsParams.ballTrackNumber, &hilitemap[0],
                     itsParams.showPhysics);
          gfx.drawCross();

          gfx.swapBuffers();

          gfx.gfxWait(timer, itsParams.remindSeconds / 2.0);
        }
      else
        {
          gfx.gfxWait(timer, itsParams.remindSeconds);
        }

      gfx.clearBackBuffer();
      drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0],
                 itsParams.showPhysics);
      gfx.drawCross();
      gfx.swapBuffers();
    }

  gfx.clearBackBuffer();
  gfx.drawCross();
  gfx.swapBuffers();
}

static const char vcid_balls_cc[] = "$Header$";
#endif // !BALLS_CC_DEFINED
