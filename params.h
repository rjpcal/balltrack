
extern int   DISPLAY_X;
extern int   DISPLAY_Y;
extern int   BORDER_X;
extern int   BORDER_Y;
extern int   CYCLE_NUMBER;
extern float EPOCH_DURATION;
extern float PAUSE_DURATION;
extern float WAIT_DURATION;
extern float REMIND_DURATION;
extern int   FRAMES_PER_REMIND;
extern int   REMINDS_PER_EPOCH;
extern int   BALL_NUMBER;
extern int   BALL_TRACK_NUMBER;
extern int   BALL_VELOCITY;
extern int   BALL_ARRAY_SIZE;
extern float BALL_RADIUS;
extern float BALL_SIGMA2;
extern int   BALL_MIN_DISTANCE;
extern float BALL_TWIST_ANGLE;
extern char  PROGRAM[];
extern char  FILENAME[];
extern char  OBSERVER[];
extern double FRAMETIME;
extern int width, height, fildes;

/**************

    REMINDS_PER_EPOCH     = 72;
    FRAMES_PER_REMIND   = 11;  

    BORDER_X    = ( width - 120 ) / 2; 
    BORDER_Y    = ( height - 120 ) / 2;
    BALL_MIN_DISTANCE     = 24;        
    BALL_NUMBER    = 8;                
    BALL_TRACK_NUMBER   = 2;           

    BALL_ARRAY_SIZE     = 16;          
    BALL_VELOCITY = 3;
    BALL_TWIST_ANGLE    = PI/16.;

****************/

#define      YES             	   'y'
#define      NO              	   'n'
			     	      
#define      READY           	   ' '
#define      QUIT            	   'q'
			     	      
#define      LEFTBUTTON      	   'l'
#define      RIGHTBUTTON     	   'r'
#define      MIDDLEBUTTON    	   'm'
			     	      
#define      READ            	   'r'
#define      WRITE           	   'w'
#define      APPEND          	   'a'
#define      OPTIONAL        	   'o'
			     	      
#define GETINT(name) {fgets(line,120,fp);sscanf(line,"%s %d",text,&(name));}
#define GETCHAR(name) {fgets(line,120,fp);sscanf(line,"%s %c",text,&(name));}
#define GETFLOAT(name) {fgets(line,120,fp);sscanf(line,"%s %f",text,&(name));}
#define GETTEXT(name) {fgets(line,120,fp);sscanf(line,"%s %s",text,(name));}
#define GETINTL(name) {fgets(line,120,fp);sscanf(line,"%s %d %d %d %d",text,&(name)[0],&(name)[1],&(name)[2],&(name)[3]);}
#define GETTEXTL(name) {fgets(line,120,fp);sscanf(line,"%s %s %s %s %s",text,(name)[0],(name)[1],(name)[2],(name)[3]);}

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

extern char line[], text[];


