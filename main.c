/*A program to run tests and do all sorts of other things.*/
#include "lite.h"
#include <SDL/SDL.h>
#include <signal.h>
#include <stdlib.h>
#include "canvas.h"
#include <math.h>

/*Use timing...*/
#define CV_SPEED 1
#define USETIMING
#define aon(m) \
	opt_set(opts, "--all") || opt_set(opts, m)

struct Settings {
	_Bool   slow;
	int32_t drawSpeed;
	int32_t height;
	int32_t width;
} settings;

typedef struct Test {
	char *name; 
	void (*test)(Surface *win, void *data); 
	_Bool run; 
	void *data;
} Test;


/*Temporary measure while we're missing a Line structure*/
typedef	struct LineP { 
	Pt c0, c1; int32_t color; _Bool sentinel; 
} LineP;
	
struct polytype {
	int32_t length;     /*Height of the polygon*/
	int32_t *trace;     /*....*/
	int32_t tlen;
//we also know that there will be at least a multiple of two points
//(height * 2)  = the amount of x points that we'll need to track
	int32_t beg, end;   /*These should only be one point.*/
	int32_t top, bot;   /*These should only be one point.*/
};

typedef struct PolyP { 
	Pt p[20]; int32_t color; struct polytype Py; _Bool sentinel; } PolyP;

/*Control a fade effect*/
void fade_between (Surface *bg, int32_t start, int32_t end, int32_t step) {
	int32_t c=start;
	while (c > end) {
		fill_display(bg, (c += step));
		SDL_UpdateRect(bg->win, 0, 0, 0, 0);
		__pause(0, 70000000);
	}
}


/*Draws some simple plots.*/
void drawplots (Surface *bg, void *data) {
	Pt *p = (Pt *)data;
	while (p->x > -1) {
		nplot(bg, p->x, p->y, 0xffffff, 0xff, NULL);
		p++;
	}

	SDL_UpdateRect(bg->win, 0, 0, 0, 0);
}


/*Draws some simple lines*/
void drawlines (Surface *bg, void *data) {
	int32_t tw=bg->w;
	int32_t th=bg->h;
	int32_t ww=settings.width; 
	int32_t hh=settings.width; 
	LineP *l = (LineP *)data;
	/*I want to see the difference in speeds between lines that can be drawn and those that can't...*/
	while (!l->sentinel) {
		//Also show me adjustments when necessary
		//fprintf(stdout, "Points %d,%d -> %d,%d\n", l->c0.x, l->c0.y, l->c1.x, l->c1.y);
		//Find x or y intercept...
		sline(bg, l->c0.x, l->c0.y, l->c1.x, l->c1.y, l->color, 0xff, 0, NULL);
		l++;
	}
}


/* ------------------------------------------------------- *
   -------------       CIRCLES            ----------------
 * ------------------------------------------------------- */
/*Just draw random circles under a certain number.*/
void drawcircles (Surface *bg, void *data) 
{
	for (int i=0; i<100; i++) {
		int32_t x = rwr(0, bg->w);
		int32_t y = rwr(0, bg->h);
		uint32_t rad = urwr(1, 100);
		uint32_t clr = urwr(0x000000, 0xffffff);
		circle(bg, x, y, rad, clr, 0xff);
	}
}


void drawtriangles (Surface *bg, void *data) {

}


/* -------------------------------------------------- *
- find top and bottom, calc y index
- (scan order of all points, so that you don't end up with a diff shape)
-  
 * -------------------------------------------------- */

/*Object copy to not use malloc, when not truly needed (memset causes it to fail?)*/
#define ptcopy(old,new) \
	const int16_t _##old##_size=glean_size(old); \
	Pt new[_##old##_size]; \
	map_pt(old, new)


/*Getting the end of something isn't easy*/
int32_t get_end (int32_t *arr) {
	/*Move through *arr and find the -negative*/
	int32_t *p=arr;
	int32_t c=0;
	while (*p > -1)
		p++, c++; 
	return c;	
}

/* ------------------------------------------------------- *
   -------------       SHAPES             ----------------
 * ------------------------------------------------------- */

inline uint8_t 
find_quadrant (Pt *of, Pt *len, int32_t xmp, int32_t ymp) 
{
	int32_t x=of->x, y=of->y;

	/*Check midpoint points*/
	if (xmp && xmp == x)
		return (y < (len->y/2)) ? 2 : 7;
	else if (ymp && ymp == y)
		return (x < (len->x/2)) ? 4 : 5;
	else if (ymp && xmp && xmp == x && ymp == y)
		return 0; // origin, don't do anything...

	int8_t xx,yy;
	xx = (x < (len->x/2)) ? 0 : 1;
	yy = (y < (len->y/2)) ? 0 : 1;

	/*This can be a one line monstrosity*/
	if (!xx && !yy) /*1*/
		return 1;
	if (xx && !yy)  /*3*/
		return 3;
	if (!xx && yy)  /*2*/
		return 6;
	if (xx && yy)   /*4*/
		return 8;
	return 0;
}


#if 0
#define hold(h) \
	while ((SDL_PollEvent(&se) == 0) || se.key.keysym.sym != SDLK_RETURN) ;
#else	
#define hold(h) getchar()
#endif



/*Let's do a bunch of simple shapes*/
void drawngons (Surface *win, void *data) 
{
	/*Filled and unfilled*/
	/*Let's just try fills, NOTE: these are all CONVEX polygons*/

	/*Set things that should be calc'd once and references*/
	SDL_Event se;
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	PolyP *p=(PolyP *)data;

#if 0
	while (!p->sentinel) {
		Pt *c=p->p;
		fgon(win, c, p->color, 0xff); 
		p++;
	}
#else
	/*Experiment with a datatype for this*/
	struct polytype Py[4];

	while (!p->sentinel) {
		Pt *c=p->p;
		struct polytype Py = p->Py;

		/*Sort points and find length*/
		/*Find the index too, and that should solve the misdrawn poly problem.  The next step, though, is to bust out some math that will test for "convexness" of a polygon*/
		int32_t min = sortPts(c, 1, 0), max = sortPts(c, 1, 1);	
		int32_t mn = sortPts(c, 0, 0), mx = sortPts(c, 0, 1);	
		Py.length=(max - min);
		Py.top=min;
		Py.bot=max;

			
		/*Allocate 2x height and space for top and bottom points */
		int32_t ints[((max - min) * 2) + 2];
		memset(&ints, 0, sizeof(ints)/sizeof(int32_t));
		Py.trace = &ints[0];
		Py.tlen = sizeof(ints)/sizeof(int32_t);

		/*Debug*/
		fprintf(stderr, "y pos: %d, %d\n", min, max);
		fprintf(stderr, "x pos: %d, %d\n", mn, mx);
		//fprintf(stderr, "length: %d, sizeof(ints): %d\n", Py.length, Py.tlen);
		/*Check for flat top & bottom*/
		/*(I don't know how to do this yet, these tests don't solve this problem)*/

		/*Set up variables needed to trace the polygon*/
		int on=1;
		int i=0;
		Pt *curr=p->p, *next=p->p+1, *home=&p->p[0];

		/*Trace the polygon*/
		while (on) {
			/*Move through all points until you reach "home"*/
			on = ((next = ((curr+1)->x > -1 || (curr+1)->y > -1) ? curr+1 : home) == home) ? 0 : 1;
			int32_t x0=curr->x, y0=curr->y;
			int32_t x1=next->x, y1=next->y;
			//fprintf(stderr, "(%d,%d) -> (%d, %d)\n", x0, y0, x1, y1);

			/*Shut out points not within a particular range (this is a bad idea...)*/
			if ((x0<0 && x1<0)||(y0<0 && y1<0)||(x0>W && x1>W)||(y0>H && y1>H)) {
				fprintf(stderr, "Points of polygon outside of drawable range.\n");
				curr++;
				continue;	
			}

			/*Define stuff...*/
			int32_t pixCount, errInc, errDec, errInd, mn, mj; 
			int32_t *x=NULL, *y=NULL;
			int32_t dx=0, dy=0, dx2, dy2;
			int mnc, mjc;
			int32_t oy=0;
			_Bool yIsMajor=0;

			// if y0 == y1, save endpoints and continue
			if (y0 == y1) {
				Py.trace[i]=y0, Py.trace[i+1]=y1; 
				fprintf(stderr, "Got horizontal line...");
				curr++;
				continue;
			}
			
			/*Figure out minor & major axes, and increment point*/
			get_slope(dx, dy, x0, y0, x1, y1); 
			dx2 = dx*2, dy2 = dy*2;

			/*Set depending on slope*/			
			if (dx>dy) {
				yIsMajor=0;
				pixCount = dx;
				errInc = dy2;
				errDec = dx2;
				mn = y0;
				mj = x0;
				mjc = (x1>x0) ? 1 : -1;
				mnc = (y1>y0) ? 1 : -1;
				x = &mj;
				y = &mn;
				errInd = dy2-dx; 
				oy = *y;
			}
			else {
				yIsMajor=1;
				pixCount = dy;
				errInc = dx2;
				errDec = dy2;
				mn = x0;
				mj = y0;
				mjc = (y1>y0) ? 1 : -1;
				mnc = (x1>x0) ? 1 : -1;
				x = &mn;
				y = &mj;
				errInd = dx2-dy; 
			}

			/*Save the first point if it's not flat topped*/
			fprintf(stderr, "Major index is: %s\n", !yIsMajor ? "x" : "y");
			fprintf(stderr, "x0: %d, y0: %d, x1: %d, y1: %d\n", x0, y0, x1, y1); 

			/*Save only the x's from the line*/
			while (pixCount--) {
				/*Draw the point while debugging*/
				fprintf(stderr, "point at: %d, %d\n",*x,*y);
				//getchar();
				plot(win,*x,*y,p->color,0xff);
		
				/*Only save if a certain criteria match*/
				if (yIsMajor) {
					// save the same x multiple times...
					Py.trace[i] = *x;
					fprintf(stderr, "Saving point x, plotting point y [%d, %d]\n", *x, *y); 
					i++;
				}
				else { 
					// Only save x when y changes?
					if (oy != *y) {
						Py.trace[i] = *x;
						fprintf(stderr, "Saving point x, when y changes [%d, %d]\n", *x, *y); 
						oy=*y;
						i++;
					}
				}

				/*Calculate error index*/
				if (errInd >= 0) {
					// should "reset" the error index.
					errInd += (errInc - errDec);
					mn += mnc;   // minor index should go up
				}
				else {
					errInd += errInc;	
				}
				mj += mjc;
				SDL_UpdateRect(win->win, 0, 0, 0, 0);
			};
			curr++;
		}

		/*Set up the shading*/
		int bl=Py.length-1, br=Py.length;
		int start=Py.bot;

		/*Echo eveyrthing*/
		fprintf(stderr, "height of gon:      %d\n", Py.length);
		fprintf(stderr, "find end of gon:    %d\n", get_end(Py.trace));
		fprintf(stderr, "start point:        %d\n", Py.bot);
		fprintf(stderr, "total length:       %d\n", Py.length);
		fprintf(stderr, "left side up:       %d\n", bl); 
		fprintf(stderr, "right side up:      %d\n", br); 
		fprintf(stderr, "trace[mid-1]: %d\n", Py.trace[bl]); 
		fprintf(stderr, "trace[mid]:   %d\n", Py.trace[br]); 
		fprintf(stderr, "press enter to continue\n"); 
		getchar();

		#if 1
		// drawing order matters... start at top or bottom
		int cuz=Py.bot;
		for (int p=0;p<(Py.length * 2);p++,cuz--) {
			fprintf(stderr, "%d,%d - ", p, cuz);
			if ((p % 10) == 0) fprintf(stderr, "\n");
		}
		getchar();
		#endif

		/*Shade the polygon*/
		while (Py.length--) {
			line(win, Py.trace[bl], start, Py.trace[br], start, p->color, 0xff); 
			bl--, br++, start--;
			SDL_UpdateRect(win->win, 0, 0, 0, 0);
			getchar();
			//hold();
		}
			
		hold(1);	
		p++;

		fill_display(win, 0x000000);
		SDL_UpdateRect(win->win, 0, 0, 0, 0);
	}
#endif
}

/*
- get the approximate length of the gon by doing calculations
	allocate space either dynamically or statically
	(hypotenuse for most of them will work...)
	(if that number is greater than stack limit, use malloc)
	(notate this in whatever data structure is used)

- don't worry about all that and just hope that it's under a
	certain number, reallocating when necessary...?
*/
typedef struct GonPtArray GonPtArray;
struct GonPtArray {
	_Bool sh; /*Static or heap*/
	Pt *p;    /*Point array*/
};

// a^2 + b^2 = c^2
//{{{100,100},{50,200},{150,200},{-1,-1}}, 0xffffff},   



void fillConvexGon (Surface *bg) {
	struct PolyP { Pt p[20]; int32_t color; _Bool sentinel; } Polys[] = {
		{{{100,100},{50,200},{150,200},{-1,-1}}, 0xffffff},   
		{{{300,300},{200,300},{200,200},{300,200},{-1,-1}}, 0xffffff},
		{{{200,400},{220,420},{210,440},{190,440},{180,420},{-1,-1}}, 0xffffff},
		{.sentinel=1}
	};

	struct PolyP *p=Polys;
	while (!p->sentinel) {
		int on=1;
		Pt *curr=p->p, *next=p->p+1, *home=&p->p[0];

		/*Creating the convex gon point array happens here.*/
		/*We know that no single convex gon will be larger than the surface*/

		/*"Save" all the points*/
		while (on) {
			on = ((next = ((curr+1)->x > -1 || (curr+1)->y > -1) ? curr+1 : home) == home) ? 0 : 1;
			int32_t ex1=curr->x, ey1=curr->y;
			int32_t ex2=next->x, ey2=next->y;
			int32_t len=0;
			if (ex1 == ex2)
				len = (ey1>ey2) ? ey1 - ey2 : ey2 - ey1;
			else if (ey1 == ey2)
				len = (ex1>ex2) ? ex1 - ex2 : ex2 - ex1;
			else {
				int a=(ex1>ex2) ? ex1 - ex2 : ex2 - ex1;
				int b=(ey1>ey2) ? ey1 - ey2 : ey2 - ey1;
				len = (int)sqrt((a*a) + (b*b)) + 1 ;
			}
			fprintf(stderr, "allocating struct of size: %d\n", len);	
			Pt arr[len];
			// malloc(sizeof(Pt) * len);
			getchar();
			//VCatch("sss");
			//fprintf(stderr, "(%d,%d) -> (%d, %d)\n", ex1, ey1, ex2, ey2);
			//line(bg, ex1, ey1, ex2, ey2, p->color, 0xff);
			curr++;
		}

		/*Sort them*/

		/*Then render the filled polygon*/
		p++;
	}
}


void drawglyphs (Surface *win, void *data) {
	return;
}


void drawgonz (Surface *win) {
	// data
	Pt a[]={{0,0},{639,0},{639,330},{0,200},{-1,-1}};
	Pt b[]={{0,200},{0,479},{639,479},{639,330},{-1,-1}};
	Pt c[]={{410,284},{450,292},{170,479},{0,479},{40,479},{-1,-1}};
	Pt d[]={{410,224},{450,230},{450,292},{410,284},{-1,-1}};
	Pt e[]={{450,292},{450,479},{170,479},{-1,-1}};
	//Pt d[]={{0,400},{0,479},{100,479},{-1,-1}};
	//Pt d[]={{0,200},{0,479},{639,479},{639,330},{-1,-1}};
	//Pt c[]={{0,0},{640,0},{640,230},{0,100},{-1,-1}};
	//Pt d[]={{0,0},{640,0},{640,230},{0,100},{-1,-1}};
	// did it work
	// let's loop an animation
	// yay....
	fgon(win, a, 0xdddddd, 0xff);
	fgon(win, b, 0x333333, 0xff);
	fgon(win, c, 0x8729e1, 0xff);
	fgon(win, d, 0x000000, 0xff);
	fgon(win, e, 0x444444, 0xff);
	SDL_UpdateRect(win->win, 0, 0, 0, 0);
	getchar();
}

/* Selectable resolutions */
struct Resolutions { char *name; int32_t w, h; } resolutions[] = {
	{ "320x240",    320, 240 },
	{ "640x480",    640, 480 },
	{ "800x600",    800, 600 },
	{ "1024x600",  1024, 600 },
	{ "1280x1000", 1024, 600 },
	{ NULL,           0,   0 }
};
	
/* End tests */ 
Option opts[] = {
#if 0
	/*When this option is specified, command line will turn
    off tests that are compiled in via tests.h           */
	{ "-u", "--use-cli",   "Negate with flags (-DTESTS_H " \
                         "must be compiled in)."          },
#endif
	/*Test controllers*/
	{ "-a", "--all",       "Run all tests."                 },
	{ "-o", "--plots",     "Draw only plots."               },
	{ "-l", "--lines",     "Draw only lines."               },
	{ "-p", "--polygons",  "Draw only polygons."            },
	{ NULL, "--fills",     "Draw filled convex polygons."   },
	{ "-g", "--glyphs",    "Draw glyphs."                   },
	{ "-c", "--circles",   "Draw only circles."             },
	{ "-t", "--triangles", "Draw only triangles."           },
	{ NULL, "--screen",    "Perform screen tests."          },
	{ "-i", "--input",     "Test SDL input and KeySet " \
                         "routines."                      },
	{ "-v", "--convex",    "Draw only convex polygons."     },
	{ "-n", "--nonconvex", "Draw nonconvex polygons."       },
	{ "-f", "--fade",      "Perform fade color tests."      },
	{ NULL, "--scaling",   "Perform shape scale tests."     },
	{ NULL, "--expansion", "Perform shape expansion tests." },
	{ NULL, "--motion",    "Perform shape motion tests."    },
	{ NULL, "--transform", "Perform shape transformation " \
                         "tests."                         },
	{ NULL, "--rotate",    "Perform shape rotation tests."  },

	/*Test parameters*/
	{ NULL, "--log-file",  "Log all results to <file>", 1   },
	{ "-s", "--slow",      "Draw everything slowly."        },
	{ NULL, "--resolution","Perform tests at specified " \
                         "resolution.",                   },

	/*Edge case testing*/
	{ NULL, "--xxx",       "Find intercepts of lines" \
                         "bigger than surface area."      },
	{ .sentinel=1 }
};

#ifdef TESTS_H
 #include "tests.h"
#endif

#ifndef TESTS_DATA_PATH
 #include "data.c"
#else
 #error "Defining an alternate file for test data is not yet supported."
#endif

/* Main */
int 
main (int argc, char *argv[]) 
{
	if (argc < 2)
		opt_usage(opts, "Nothing to do.", 0);
	else
		opt_eval(opts, argc, argv);

#if 0
	/*Have yet to make Engine datatype*/
#else
	/*Display/Main window setup*/
	uint32_t init_mask = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD;
	uint32_t sdl_mask = SDL_SWSURFACE | SDL_DOUBLEBUF;
	if (SDL_Init(init_mask) < 0)
		errabrt(1, "Everything must go.");
	Surface Win = { .h = 480, .w = 640, .bpp = 32 }; 
	Win.win = SDL_SetVideoMode(640, 480, 32, 0);
	signal(SIGINT, SIG_DFL);
	int on = 1;
#endif

	/*Event and input setup*/
	SDL_Event event;

 #ifndef TESTS_H
	/*Define which tests to run*/
	Test tests[] = {
		{ "plots",     drawplots     , aon("--plots")    , (void*)Points   },
		{ "lines",     drawlines     , aon("--lines")    , (void*)Lines    },
		{ "circles",   drawcircles   , aon("--circles")  , NULL            },
		{ "triangles", drawtriangles , aon("--triangles"), NULL            },
		{ "gons",      drawngons     , aon("--polygons") , NULL            },
		{ "gons2",     drawngons     , aon("--convex")   , NULL            },
		{ "glyphs",    drawglyphs    , aon("--glyphs")   , NULL            },
	};
 #endif

	/*Run each test*/
	for (int i=0; i<sizeof(tests)/sizeof(Test); i++) {
		if (tests[i].run) {
			if (!tests[i].data) {
				fprintf(stderr, "No tests specified for module %s\n", tests[i].name);
				continue;
			}
			tests[i].test(&Win, tests[i].data);
			SDL_UpdateRect(Win.win, 0, 0, 0, 0);
			__pause(2, 3000000);
			fade_between(&Win, 0xffffff, 0x000000, -0x111111);
		}
	}	

	/*Close log file and free anything*/
	//if (opts.log_filename) fclose(std);
	SDL_FreeSurface(Win.win);
	SDL_Quit();
	return 0;
}
