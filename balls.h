///////////////////////////////////////////////////////////////////////
//
// balls.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:58:20 2000
// written: Wed Sep  3 14:19:42 2003
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_H_DEFINED
#define BALLS_H_DEFINED

class Graphics;

struct timeval;

class Ball
{
public:
  void randomPosition(int width, int height);
  bool isTooClose(const Ball& other) const;
  void randomVelocity();
  void nextPosition(int width, int height);
  void collideIfNeeded(Ball& other);
  void twist();
  void move(Graphics& gfx);
  void copy();
  void draw(Graphics& gfx, unsigned char* bitmap);

private:
  void collide(Ball& other, int xij, int yij);

  int itsXpos;
  int itsYpos;
  int itsNx;
  int itsNy;
  int itsXvel;
  int itsYvel;
};

class Balls
{
private:
  void prepare(Graphics& gfx);

  void initialize(Graphics& gfx);

  void nextBalls(Graphics& gfx);
  void moveBalls(Graphics& gfx);
  void copyBalls();

  void drawNBalls(Graphics& gfx, int first, int last, unsigned char* bitmap);
  void drawNHiBalls(Graphics& gfx, int first, int last, unsigned char* bitmap);

public:
  static const int MAX_BALLS = 25;

  enum TrialType { PASSIVE, CHECK_ALL, CHECK_ONE };

  void runTrial(Graphics& gfx, timeval* starttime, TrialType ttype);

  static const int COLOR_NUMBER = 256;
  static float theColors[COLOR_NUMBER][3];
  static void generateColors();

private:
  Ball itsBalls[MAX_BALLS];
};

static const char vcid_balls_h[] = "$Header$";
#endif // !BALLS_H_DEFINED
