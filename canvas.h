#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <sys/time.h>
#include <time.h>

#ifndef CANVAS_H
#define CANVAS_H

#define errstat(x, ...) while (0) { \
	fprintf(stderr, "%s", "ERROR OCCURRED: "); \
	fprintf(stderr, __VA_ARGS__); }

#define errabrt(x, ...) while (0) { \
	fprintf(stderr, "%s", "ERROR OCCURRED: "); \
	fprintf(stderr, __VA_ARGS__); return x; }

#define max(a,b) (((a) > (b)) ? (a) : (b))

#define min(a,b) (((a) < (b)) ? (a) : (b))

/* Find where we want to start drawing. */
#define setCursor(x1, y1) \
	((int *)win->win->pixels + (y1 * win->w)) + x1

/*How do HTML colors work? Look closely and think about shifts.*/
#define fillPix(clr, opq) \
	(((clr>>16) & 0xFF)<<16) | (((clr>>8) & 0xFF)<<8) | ((clr>>0) & 0xFF)
#if 0
 	printf("Red:   %d\n", ((sc >> 16)&0xFF)); \
 	printf("Green: %d\n", ((sc >>  8)&0xFF)); \
 	printf("Blue:  %d\n", ((sc >>  0)&0xFF));
#endif

/*Do this for function pointers, b/c C is ridiculous*/
#define _bw(ff,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
 bnd[a]=ff;bnd[b]=ff;bnd[c]=ff;bnd[d]=ff;bnd[e]=ff;bnd[f]=ff;bnd[g]=ff;bnd[h]=ff;bnd[i]=ff;bnd[j]=ff;bnd[k]=ff;bnd[l]=ff;bnd[m]=ff;bnd[n]=ff;bnd[o]=ff;bnd[p]=ff
#define preinitialize(fn, fp) \
_bw(fp,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);\
_bw(fp,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32);\
_bw(fp,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48);\
_bw(fp,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64);\
_bw(fp,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80);\
_bw(fp,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96);\
_bw(fp,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112);\
_bw(fp,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128);\
_bw(fp,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144);\
_bw(fp,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160);\
_bw(fp,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176);\
_bw(fp,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192);\
_bw(fp,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208);\
_bw(fp,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224);\
_bw(fp,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240);\
_bw(fp,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256);\
_bw(fp,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272);\
_bw(fp,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288);\
_bw(fp,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304);\
_bw(fp,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320);\
_bw(fp,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336);\
_bw(fp,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352);\
_bw(fp,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368);\
_bw(fp,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384);\
_bw(fp,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400);\
_bw(fp,401,402,403,404,405,406,407,408,409,410,411,412,413,414,415,416);\
_bw(fp,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432);\
_bw(fp,433,434,435,436,437,438,439,440,441,442,443,444,445,446,447,448);\
_bw(fp,449,450,451,452,453,454,455,456,457,458,459,460,461,462,463,464);\
_bw(fp,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480);\
_bw(fp,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496);\
_bw(fp,497,498,499,500,501,502,503,504,505,506,507,508,509,510,511,0);\


/*Debugging*/
typedef struct Debug Debug;
struct Debug {
	const char *fn;    /*The currently executing function.*/
	void (*fp)(void);  /*A function to output data or print something.*/
	void *data;        /*Any data that you need to see.*/
	const char *file;  /*File with the error.*/
	uint32_t line;     /*Line with the error.*/
};

Debug debug;

void std_debug (void *p);

void print_debug ();

void set_sigsegv ();

void pause (uint32_t secs, uint32_t nsecs);

typedef struct Pause Pause;
struct Pause { char type; union { uint32_t secs, msecs, usecs, nsecs; } time; }; 
void pauseX (Pause *);

#if 1
void __catch (SDL_Event *ev, char *msg, int only, SDLKey key);
 #define Catch(ev) __catch(ev, NULL, 0, SDLK_RETURN)
 #define VCatch(ev, msg) __catch(ev, msg, 0, SDLK_RETURN)
#else
 #define Catch()
 #define VCatch(msg)
#endif

//I forgot how I solved this before: debug.fp = (fptr != NULL) ? &std_debug : &fptr;
//The way it's currently written, you'll have to manually define std_debug as the fallback function vs using NULL.
#define SetDebug(fname, dt, fptr) \
	debug.fn = #fname; \
	debug.fp = fptr; \
	debug.data = (void *)dt; \
	debug.file = __FILE__; \
	debug.line = __LINE__


/*Data structures*/
#if 0
typedef struct Surface Surface;
struct Surface {
	uint16_t h, w, bpp;
	SDL_Surface *win;
	void *ud;
};
#endif

typedef struct Pt Pt;
struct Pt {
	int32_t x, y;
};

/*Surface handling*/
typedef struct Surface Surface;
struct Surface {
	uint32_t     h, w; 
	uint32_t     *hh, *ww; 
	uint32_t     bpp;
	uint32_t     chromakey;
	uint32_t     bg;
	uint8_t      resizable ;
	uint8_t      hwsw      ;
	uint8_t      fullscreen;
	uint8_t      alpha     ;
	uint8_t      video     ;
	uint8_t      dbbuf     ;
	Pt           position;
	SDL_Surface *srf; /*This ought to be where an array of Surfaces is...*/
	SDL_Surface *win;
	void     *ud;
};


typedef struct Coord Coord;
struct Coord {
	uint16_t x, y, z;
};

typedef struct KeySet KeySet;
struct KeySet { 
	uint8_t key; 
	void (*fp)(SDL_Event *event, Surface *bg); 
};

typedef struct Poly Poly;
struct Poly {
	uint32_t length;
	Pt *points;
};

typedef struct Timer Timer;
struct Timer {
	timer_t *timer;
	clockid_t clockid;
	const char *label, *file;
	int32_t linestart, lineend;	
	struct timespec start, end;
};

void __timer_start (Timer *t, const char *file, int line);
void __timer_end (Timer *t, const char *file, int line);
void __timer_elap (const char *file, int line);

/*This is an idea....but eschew it for now*/
typedef struct PtFmt PtFmt;
struct PtFmt {
	Pt xy0, xy1;
	uint32_t color;
	uint32_t opacity;
	uint32_t thickness;
	uint8_t  antialias;
	Pt *points;
	void (*callback)(void);
};


/*line is a define, ray is the function?*/

/*Boiler plate*/
void dummy (SDL_Event *event, Surface *bg);

void (*bnd[512])(SDL_Event *event, Surface *bg);


/*Game mechanics*/
void fill_display (Surface *win, uint32_t sc);

void fast_fill_display (Surface *win, uint32_t sc); 

void set_all_keys (KeySet *keys);


/*Random code*/
int32_t rwr(int32_t l, int32_t h);

uint32_t urwr(uint32_t l, uint32_t h);


/*Plots & Lines*/
/*Discern between scanning functions and regular functions*/
void nplot (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op, Pt *ptlist);

void nscan (Surface *win, int32_t x, int32_t y, uint32_t, uint32_t, Pt *ptlist);

/*Do allocation within line*/
//Pt * aline (Surface *win, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t c, uint32_t o); 
/*Do allocation outside (less error prone to me and probably quicker but I want to test this)*/

void sline (Surface *win, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t c, uint8_t o, _Bool save, Pt *pts); 

#define line(w,x0,y0,x1,y1,clr,op) sline(w, x0, y0, x1, y1, clr, op, 0, NULL)


void plot (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op);

//void line (Surface *, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t clr, uint32_t opacity);

#define get_slope(rn, rz, x0, y0, x1, y1) \
	if (x0<0) \
		rn = x1-x0, rz = (y1>y0)?y1-y0:y0-y1; \
	else if (x1<0) \
		rn = x0-x1, rz = (y1>y0)?y1-y0:y0-y1; \
	else if (y0<0) \
		rn = y1-y0, rz = (x1>x0)?x1-x0:x0-x1; \
	else if (y1<0) \
		rn = y0-y1, rz = (x1>x0)?x1-x0:x0-x1; \
	else \
		rn = (x1>x0)?x1-x0:x0-x1, rz = (y1>y0)?y1-y0:y0-y1;

#define yintz

#define xintz

/*Shape stuff*/
void coord (Surface *win, uint16_t x, uint16_t y, uint32_t c, uint32_t op, uint32_t w);

void circle (Surface *win, uint16_t x, uint16_t y, uint16_t radius, uint32_t c, uint16_t o);

void ngon (Surface *win, Poly *poly, uint32_t stroke, uint32_t fill, uint32_t opacity);

int FillConvexNGon (Surface *win, Poly *poly, uint32_t stroke, uint32_t fill, uint32_t opacity);

void draw_from_point_array (Surface *srf, Pt *pt, void (*callback)(Pt *));

/*...*/
void expand_shape (Pt *s, int32_t to);
 
/*Debugging*/
void print_surface (Surface *s);

/*Peek a boo!*/
extern Surface *Win;

extern FILE *std;

extern int on; 

#ifdef SLOW
 #ifndef SPEED
	#define SPEED 5000 /*Auto sleep for 5000 nanoseconds*/	
 #endif
#endif
#endif
