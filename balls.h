///////////////////////////////////////////////////////////////////////
//
// balls.h
// Rob Peters rjpeters@klab.caltech.edu
//   created by Achim Braun
// created: Tue Feb  1 15:58:20 2000
// written: Wed Feb 23 15:49:30 2000
// $Id$
//
///////////////////////////////////////////////////////////////////////

#ifndef BALLS_H_DEFINED
#define BALLS_H_DEFINED

class Application;

class Ball {
public:
  void randomPosition(int width, int height);
  bool isTooClose(const Ball& other) const;
  void randomVelocity();
  void nextPosition(int width, int height);
  void collideIfNeeded(Ball& other);
  void twist();
  void move(int fildes);
  void copy();
  void draw(int fildes, unsigned char* bitmap);

private:
  void collide(Ball& other, int xij, int yij);

  int itsXpos;
  int itsYpos;
  int itsNx;
  int itsNy;
  int itsXvel;
  int itsYvel;
};

class Balls {
private:
  void initialize(Application* app);

  void nextBalls(Application* app);
  void moveBalls(Application* app);
  void copyBalls();

  void drawNBalls(int n, int fildes, unsigned char* bitmap);
  void drawNHiBalls(int n, int fildes, unsigned char* bitmap);

public:
  static const int MAX_BALLS = 25;

  void prepare(Application* app);

  void runTrial(Application* app, bool cue_track_balls = true);

private:
  Ball itsBalls[MAX_BALLS];
};

static const char vcid_balls_h[] = "$Header$";
#endif // !BALLS_H_DEFINED
