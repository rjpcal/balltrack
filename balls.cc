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
#include "timepoint.h"

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

  this->pos.x = xborder + (width - arraysize - 2*xborder) * drand48();
  this->pos.y = yborder + (height - arraysize - 2*yborder) * drand48();
}

bool Ball::isTooClose(const Ball& other, int min_dist) const
{
DOTRACE("Ball::isTooClose");

  return (fabs(this->pos.x - other.pos.x) < min_dist &&
          fabs(this->pos.y - other.pos.y) < min_dist);
}

void Ball::randomSpeed(double vel)
{
DOTRACE("Ball::randomVelocity");

  // Pick a random direction for the velocity
  const double angle = 2 * M_PI * drand48();

  this->vel.x = vel * cos(angle);
  this->vel.y = vel * sin(angle);
}

void Ball::nextPosition(int width, int height,
                        int xdisplay, int ydisplay,
                        int arraysize, double lapsed_seconds)
{
DOTRACE("Ball::nextPosition");

  const int xborder = (width-xdisplay)/2;
  const int yborder = (height-ydisplay)/2;

  this->next.x = this->pos.x + (lapsed_seconds * this->vel.x);
  this->next.y = this->pos.y + (lapsed_seconds * this->vel.y);

  if (this->next.x < xborder || this->next.x > width - xborder - arraysize)
    {
      this->vel.x  = -this->vel.x;
      this->next.x = this->pos.x + (lapsed_seconds * this->vel.x);
    }

  if (this->next.y < yborder || this->next.y > height - yborder - arraysize)
    {
      this->vel.y  = -this->vel.y;
      this->next.y = this->pos.y + (lapsed_seconds * this->vel.y);
    }
}

bool Ball::checkInsideBorder(int width, int height,
                             int xdisplay, int ydisplay,
                             int arraysize) const
{
DOTRACE("Ball::checkInsideBorder");

  const int xborder = (width-xdisplay)/2;
  const int yborder = (height-ydisplay)/2;

  if (this->next.x < xborder || this->next.x > width - xborder - arraysize)
    {
      return false;
    }

  if (this->next.y < yborder || this->next.y > height - yborder - arraysize)
    {
      return false;
    }

  return true;
}

void Ball::bringInsideBorder(int width, int height,
                             int xdisplay, int ydisplay,
                             int arraysize)
{
DOTRACE("Ball::bringInsideBorder");

  const int xborder = (width-xdisplay)/2;
  const int yborder = (height-ydisplay)/2;

  if (this->next.x < xborder)
    this->next.x = xborder;
  else if (this->next.x > width - xborder - arraysize)
    this->next.x = width - xborder - arraysize;

  if (this->next.y < yborder)
    this->next.y = yborder;
  else if (this->next.y > height - yborder - arraysize)
    this->next.y = height - yborder - arraysize;
}

void Ball::collideIfNeeded(Ball& other, int min_dist,
                           double lapsed_seconds)
{
DOTRACE("Ball::collideIfNeeded");

  const double xij = this->next.x - other.next.x; // units pixels
  const double yij = this->next.y - other.next.y; // units pixels

  if (fabs(xij) >= min_dist || fabs(yij) >= min_dist)
    return;

  const double d    =  sqrt(xij*xij + yij*yij); // units pixels
  const double xa   =  xij/d; // unitless
  const double ya   =  yij/d; // unitless
  const double xo   = -ya;    // unitless
  const double yo   =  xa;    // unitless

  const double vai  = this->vel.x*xa + this->vel.y*ya; // units pix/sec
  const double vaj  = other.vel.x*xa + other.vel.y*ya; // units pix/sec

  if (vai - vaj < 0.0)
    {
      const double voi  = this->vel.x*xo + this->vel.y*yo; // pix/sec
      const double voj  = other.vel.x*xo + other.vel.y*yo; // pix/sec
      /*
        ovi2 = vai*vai + voi*voi;
        ovj2 = vaj*vaj + voj*voj;
      */
      const double nvi2 = vaj*vaj + voi*voi; // pix^2/sec^2
      const double nvj2 = vai*vai + voj*voj; // pix^2/sec^2

      const double vij2 = vai*vai - vaj*vaj; // pix^2/sec^2

      const double fi   = sqrt(1. + vij2 / nvi2); // unitless
      const double fj   = sqrt(1. - vij2 / nvj2); // unitless

      this->vel.x = fi * (voi * xo + vaj * xa); // pix/sec
      this->vel.y = fi * (voi * yo + vaj * ya); // pix/sec

      other.vel.x = fj * (voj * xo + vai * xa); // pix/sec
      other.vel.y = fj * (voj * yo + vai * ya); // pix/sec

      this->next.x = this->pos.x + (lapsed_seconds * this->vel.x);
      this->next.y = this->pos.y + (lapsed_seconds * this->vel.y);

      other.next.x = other.pos.x + (lapsed_seconds * other.vel.x);
      other.next.y = other.pos.y + (lapsed_seconds * other.vel.y);
    }
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

  const double x = this->vel.x;
  const double y = this->vel.y;

  this->vel.x = a11*x + a12*y;
  this->vel.y = a21*x + a22*y;
}

void Ball::copy()
{
DOTRACE("Ball::copy");

  this->pos.x = this->next.x;
  this->pos.y = this->next.y;
}

void Ball::draw(Graphics& gfx, unsigned char* bitmap, int size,
                bool debug)
{
DOTRACE("Ball::draw");

  gfx.writePixmap(int(round(this->pos.x)),
                  int(round(this->pos.y)),
                  bitmap, size);

  if (debug)
    gfx.drawLine(this->pos.x + size/2,
                 this->pos.y + size/2,
                 this->pos.x + size/2 - this->vel.x / 4,
                 this->pos.y + size/2 - this->vel.y / 4);
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

  const int maxtries = 100000;

  for (int i=0; i < itsParams.ballNumber; ++i)
    {
      bool too_close = true;

      int c = 0;

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

          if (++c > maxtries)
            {
              std::cerr << "couldn't find a satisfactory initial ball configuration\n"
                        << "(try increasing DISPLAY_X or DISPLAY_Y, or decreasing BALL_NUMBER)\n";
              exit(1);
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

      if (!itsBalls[i].checkInsideBorder(gfx.width(), gfx.height(),
                                         itsParams.displayX,
                                         itsParams.displayY,
                                         itsParams.ballPixmapSize))
        {
          std::cerr << "OOPS! A ball sneaked out of the arena (after moving)...\n"
                    << "Try increasing DISPLAY_X and DISPLAY_Y, or decreasing BALL_NUMBER\n";
          exit(1);
        }
    }

  for (int i=0; i<itsParams.ballNumber-1; ++i)
    {
      for (int j=i+1; j<itsParams.ballNumber; ++j)
        {
          itsBalls[i].collideIfNeeded(itsBalls[j],
                                      itsParams.ballMinDistance,
                                      lapsed_seconds);

          itsBalls[i].bringInsideBorder(gfx.width(), gfx.height(),
                                        itsParams.displayX,
                                        itsParams.displayY,
                                        itsParams.ballPixmapSize);

          itsBalls[j].bringInsideBorder(gfx.width(), gfx.height(),
                                        itsParams.displayX,
                                        itsParams.displayY,
                                        itsParams.ballPixmapSize);

          if (!itsBalls[j].checkInsideBorder(gfx.width(), gfx.height(),
                                             itsParams.displayX,
                                             itsParams.displayY,
                                             itsParams.ballPixmapSize))
            {
              std::cerr << "OOPS! A ball sneaked out of the arena (after colliding)...\n"
                        << "Try increasing DISPLAY_X and DISPLAY_Y, or decreasing BALL_NUMBER\n";
              exit(1);
            }

          if (!itsBalls[i].checkInsideBorder(gfx.width(), gfx.height(),
                                             itsParams.displayX,
                                             itsParams.displayY,
                                             itsParams.ballPixmapSize))
            {
              std::cerr << "OOPS! A ball sneaked out of the arena (after colliding)...\n"
                        << "Try increasing DISPLAY_X and DISPLAY_Y, or decreasing BALL_NUMBER\n";
              exit(1);
            }
        }
    }

  for (int i=0; i<itsParams.ballNumber; ++i)
    {
      itsBalls[i].twist(itsParams.ballTwistAngle);

      if (!itsBalls[i].checkInsideBorder(gfx.width(), gfx.height(),
                                         itsParams.displayX,
                                         itsParams.displayY,
                                         itsParams.ballPixmapSize))
        {
          std::cerr << "OOPS! A ball sneaked out of the arena (after twisting)...\n"
                    << "Try increasing DISPLAY_X and DISPLAY_Y, or decreasing BALL_NUMBER\n";
          exit(1);
        }
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

  if (itsParams.showPhysics)
    {
      const double left = gfx.width()/2.0 - itsParams.displayX/2.0;
      const double right = gfx.width()/2.0 + itsParams.displayX/2.0;
      const double bottom = gfx.height()/2.0 - itsParams.displayY/2.0;
      const double top = gfx.height()/2.0 + itsParams.displayY/2.0;

      gfx.drawLine(left, bottom, left, top);
      gfx.drawLine(right, bottom, right, top);
      gfx.drawLine(left, bottom, right, bottom);
      gfx.drawLine(left, top, right, top);
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
      double simulated_lapsed_sec = 0.0;
      while (true)
        {
          ++nframes;
          gfx.clearBackBuffer();
          drawNBalls(gfx, 0, itsParams.ballNumber, &pixmap[0],
                     itsParams.showPhysics);
          gfx.drawCross();

          const double lapsed_sec = gfx.swapBuffers();

          pickNextPositions(gfx, lapsed_sec);

          simulated_lapsed_sec += lapsed_sec;

          // amount of time lapsed in the entire motion sequence so far

          if (simulated_lapsed_sec >= itsParams.ballMotionSeconds())
            break;
        }

      const double lapse_secs = tstart.elapsedSec();

      std::cout << " " << nframes << " frames in "
                << 1000.0*lapse_secs << " msec ("
                << (1000.0*lapse_secs)/nframes << " msec/frame, "
                << nframes/lapse_secs << " frames/sec)\n";

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
