/*A program to run tests and do all sorts of other things.*/
#include <SDL/SDL.h>
#include <signal.h>
#include <stdlib.h>
#include "canvas.h"
#include <math.h>

/*Use timing...*/
#define USETIMING

#define opt(sht,lng) \
	(sht && strcmp(*argv, sht) == 0) || (lng && strcmp(*argv, lng) == 0) 

/*Check that option is set and cause the screen to refresh via 'co'*/
#define optset(longName) \
	((strcmp(eval->lng, longName) == 0) && (co = eval->set))

/*Different shapes...*/
#define Triangle(Pd,Pa,x1,y1,x2,y2,x3,y3) \
	Pt Pa[] = {{x1,y1},{x2,y2},{x3,y3}}; \
	Pd.length = sizeof(Pa)/sizeof(Pt); \
	Pd.points = Pa

#define Square(Pd,Pa,x1,y1,dist) \
	Pt Pa[] = { \
		{x1-(dist/2),y1-(dist/2)}, \
		{x1-(dist/2),y1+(dist/2)}, \
		{x1+(dist/2),y1+(dist/2)}, \
		{x1+(dist/2),y1-(dist/2)}, \
		{x1-(dist/2),y1-(dist/2)}, \
	}; \
	Pd.length = sizeof(Pa)/sizeof(Pt); \
	Pd.points = Pa

#define Rectangle(Pd,Pa,x1,y1,dist) \
	Pt Pa[] = { \
		{x1-(dist/2),y1-(dist/2)}, \
		{x1-(dist/2),y1+(dist/2)}, \
		{x1+(dist/2),y1+(dist/2)}, \
		{x1+(dist/2),y1-(dist/2)}, \
		{x1-(dist/2),y1-(dist/2)}, \
	}; \
	Pd.length = sizeof(Pa)/sizeof(Pt); \
	Pd.points = Pa

#define Hexagon(Pd,Pa,x1,y1,dist) \
	Pt Pa[] = { \
		{x1-(dist/2),y1-(dist/2)}, \
		{x1-(dist/2),y1+(dist/2)}, \
		{x1+(dist/2),y1+(dist/2)}, \
		{x1+(dist/2),y1-(dist/2)}, \
		{x1-(dist/2),y1-(dist/2)}, \
	}; \
	Pd.length = sizeof(Pa)/sizeof(Pt); \
	Pd.points = Pa

#define Trapezoid

#define Parallelogram 

#define Star 

#define Convex

#define NonConvex

#define draw

typedef struct Value Value;
struct Value {
	union { int32_t n; char *s; char c; void *v; } value;
};

struct Settings {
	_Bool   slow;
	int32_t drawSpeed;
	int32_t height;
	int32_t width;
} settings;


struct Resolutions { char *name; int32_t w, h; } resolutions[] = {
	{ "320x240",    320, 240 },
	{ "640x480",    640, 480 },
	{ "800x600",    800, 600 },
	{ "1024x600",  1024, 600 },
	{ "1280x1000", 1024, 600 },
	{ NULL,           0,   0 }
};

typedef struct Opt Opt;
struct Opt {
	_Bool set;  /*If set is not bool, it can define the order of demos*/
	char  *sht, *lng; 
	char  *description;
	_Bool arg;
	Value v;
	void  (*callback)(char *av);
	_Bool sentinel;
};

void drlin (Surface *win) {
		//{{{30,400},{220,420},{210,440},{500,440},{180,420},{-1,-1}}, 0xffffff},// eww...
	line(win, 30, 400, 220, 420, 0xffffff, 0xff);
	line(win, 220,420, 210, 440, 0xffffff, 0xff);
	line(win, 210,440, 500, 440, 0xffffff, 0xff);
	line(win, 500,440, 180, 420, 0xffffff, 0xff);
	SDL_UpdateRect(win->win, 0, 0, 0, 0);
	getchar();
}

/* ------------------------------------------------------- *
   -------------       DISPLAY            ----------------
 * ------------------------------------------------------- */

/*transit - moves to the next frame and executes a function possibly (like playing music)*/
#if 1
 #define transit(f) \
		pause(2, 3000000);
#else
 #define transit() getchar(); 
#endif	


/*Control a fade effect*/
void fade_between (Surface *bg, int32_t start, int32_t end, int32_t step) 
{
	/*Pixel density should probably be done...*/
	int32_t c=start;
	while (c > end) {
		fill_display(bg, (c += step));
		SDL_UpdateRect(bg->win, 0, 0, 0, 0);
		pause(0, 70000000);
	}
}


/* ------------------------------------------------------- *
   -------------       LINES              ----------------
 * ------------------------------------------------------- */
void drawlines (Surface *bg) {
	int32_t tw=bg->w;
	int32_t th=bg->h;
	int32_t ww=settings.width; 
	int32_t hh=settings.width; 
	struct LineP { Pt c0, c1; int32_t color; _Bool sentinel; } Lines[] = {
		/*These lines are all green because drawing should work.*/
		{{tw/2,  0},{tw/2,th}, 0x00ff00},  
		{{ 0,   th},{tw,   0}, 0x00ff00},  
		{{ 0, th/2},{tw,th/2}, 0x00ff00},  
		{{ 0,    0},{tw,  th}, 0x00ff00},  
	#if 0
		{{ 40,   0},{ ww, hh}, 0x00ff00},  
		{{ 50,   0},{ ww, hh}, 0x00ff00},  
		{{ 60,   0},{ ww, hh}, 0x00ff00},  
		{{ 70,   0},{ ww, hh}, 0x00ff00},  
	#endif
		/*These lines are red because some will fail.*/
	#if 1
		{{-120,-10},{-120,99}, 0xff0000}, /*Both X's are negative, should fail*/
		{{-120,-10},{  30,99}, 0xff0000}, /*Should draw, but find the right intc*/
		{{  10,-10},{  1,-80}, 0xff0000}, /*Both Y's are negative, should fail*/
		{{ -40, 10},{  30,99}, 0xff0000}, /*Should draw, */
		{{ 120,-10},{-300,82}, 0xff0000}, /*Should draw, */
	#endif
		{{200, 200},{201, 400}, 0xffffff},
		{{100, 200},{130, 400}, 0xffffff},
		{{380, 200},{390, 400}, 0xffffff},
		{{510, 200},{450, 400}, 0xffffff},
		{{200, 200},{201, 400}, 0xffffff},
		{{-100,200},{130, 400}, 0xffffff},
		{{380, 200},{390, 400}, 0xffffff},
		{{-510,200},{450, 400}, 0xffffff},
		{.sentinel=1},
	};

	struct LineP *l=Lines;
	/*I want to see the difference in speeds between lines that can be drawn and those that can't...*/
	while (!l->sentinel) {
		//Also show me adjustments when necessary
		//fprintf(stdout, "Points %d,%d -> %d,%d\n", l->c0.x, l->c0.y, l->c1.x, l->c1.y);
		//Find x or y intercept...
		// sline(bg, l->c0.x, l->c0.y, l->c1.x, l->c1.y, l->color, 0xff, 0, NULL);
		l++;
	}
	SDL_UpdateRect(bg->win, 0, 0, 0, 0);
	transit();	
}


/* ------------------------------------------------------- *
   -------------       CIRCLES            ----------------
 * ------------------------------------------------------- */
/*Just draw random circles under a certain number.*/
void drawcircles (Surface *bg) 
{
	for (int i=0; i<100; i++) {
		int32_t x = rwr(0, bg->w);
		int32_t y = rwr(0, bg->h);
		uint32_t rad = urwr(1, 100);
		uint32_t clr = urwr(0x000000, 0xffffff);
		circle(bg, x, y, rad, clr, 0xff);
	}

	SDL_UpdateRect(bg->win, 0, 0, 0, 0);
	transit();
}


void drawtriangles (Surface *bg) {

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
/*find max or min from a series*/
static int32_t sortPts (Pt *pts, _Bool axis/*True uses Y*/, _Bool order/*True returns max*/) {
	Pt *p=pts;
	int32_t *cmp; 
	int32_t b=(!axis) ? p->x : p->y;
	fprintf(stderr, "Sorting axis: %s\n", (!axis) ? "x" : "y");
	while (*(cmp = (!axis) ? &p->x : &p->y) > -1) {
		(order) ? ((*cmp > b) ? b = *cmp : 0): ((*cmp < b) ? b = *cmp : 0); 
		p++;
	}
	return b;
}

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


#define hold() \
	while ((SDL_PollEvent(&se) == 0)) ;

/*Let's do a bunch of simple shapes*/
void drawngons (Surface *win) 
{
	/*Filled and unfilled*/
	/*Let's just try fills, NOTE: these are all CONVEX polygons*/
	/*Nonconvex may cause crashes...*/
	struct PolyP { Pt p[20]; int32_t color; _Bool sentinel; } Polys[] = {
		{{{100,98},{50,200} ,{150,230},{-1,-1}}, 0xffffff},                    // triangles work
		{{{300,300},{200,300},{200,200},{300,200},{-1,-1}}, 0xffffff},          // squares work
#if 0
		{{{ 30,400},{500,420},{210,440},{ 50,450},{ 20,420},{-1,-1}}, 0xffffff},// eww...
		{{{200,400},{220,420},{210,440},{190,440},{180,420},{-1,-1}}, 0xffffff},// eww...
#endif

		#if 0 /*These test points outside of the surface.*/
		{{{30,400},{220,420},{210,9999},{500,10321},{180,10024},{-1,-1}}, 0xffffff},// eww...
		{{{30,400},{20,420},{610,9999},{500,10321},{-1,-1}},0xffffff},// eww...
		#endif
		{.sentinel=1}
	};

	/*Set things that should be calc'd once and references*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	struct PolyP *p=Polys;
	SDL_Event se;

	/*Experiment with a datatype for this*/
	struct polytype {
		int32_t length;     /*Height of the polygon*/
		int32_t *trace;     /*....*/
		int32_t tlen;
	//we also know that there will be at least a multiple of two points
	//(height * 2)  = the amount of x points that we'll need to track
		int32_t beg, end;   /*These should only be one point.*/
		int32_t top, bot;   /*These should only be one point.*/
	} Py[4];
	int pyc=0;

	while (!p->sentinel) {
		Pt *c=p->p;

		/*Sort points and find length*/
		int32_t min = sortPts(c, 1, 0), max = sortPts(c, 1, 1);	
		int32_t mn = sortPts(c, 0, 0), mx = sortPts(c, 0, 1);	
		Py[pyc].length=(max - min);
		Py[pyc].top=min;
		Py[pyc].bot=max;
			
		/*Allocate 2x height and space for top and bottom points */
		int32_t ints[((max - min) * 2) + 2];
		memset(&ints, 0, sizeof(ints)/sizeof(int32_t));
		Py[pyc].trace = &ints[0];
		Py[pyc].tlen = sizeof(ints)/sizeof(int32_t);

		/*Debug*/
		fprintf(stderr, "y pos: %d, %d\n", min, max);
		fprintf(stderr, "x pos: %d, %d\n", mn, mx);
		//fprintf(stderr, "length: %d, sizeof(ints): %d\n", Py[pyc].length, Py[pyc].tlen);
		getchar();
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
				Py[pyc].trace[i]=y0, Py[pyc].trace[i+1]=y1; 
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
				//hold();
				plot(win,*x,*y,p->color,0xff);
		
				/*Only save if a certain criteria match*/
				if (yIsMajor) {
					// save the same x multiple times...
					Py[pyc].trace[i] = *x;
					fprintf(stderr, "Saving point x, plotting point y [%d, %d]\n", *x, *y); 
					i++;
				}
				else { 
					// Only save x when y changes?
					if (oy != *y) {
						Py[pyc].trace[i] = *x;
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
		int bl=Py[pyc].length-1, br=Py[pyc].length;
		int start=Py[pyc].bot;

		/*Echo eveyrthing*/
		fprintf(stderr, "height of gon: %d\n", Py[pyc].length);		
		fprintf(stderr, "find end of gon:    %d\n", get_end(Py[pyc].trace));
		fprintf(stderr, "start point:        %d\n", Py[pyc].bot);
		fprintf(stderr, "total length:       %d\n", Py[pyc].length);
		fprintf(stderr, "left side up:       %d\n", bl); 
		fprintf(stderr, "right side up:      %d\n", br); 

		#if 0
		int cuz=Py[pyc].bot;
		for (int p=0;p<Py[pyc].length;p++,cuz--)
			fprintf(stderr, "%d,%d - ", p, cuz);
		#endif
		//hold();

		/*Shade the polygon*/
		while (Py[pyc].length--) {
			line(win, Py[pyc].trace[bl], start, Py[pyc].trace[br], start, p->color, 0xff); 
			bl--, br++, start--;
			SDL_UpdateRect(win->win, 0, 0, 0, 0);
			//hold();
		}
			
		hold();	
		p++;
		pyc++;
	}

	SDL_UpdateRect(win->win, 0, 0, 0, 0);
	transit();
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

	SDL_UpdateRect(bg->win, 0, 0, 0, 0);
	transit();
}


void drawglyphs (Surface *win) {
	return;
}

	
/* ------------------------------------------------------- *
   -------------       END TESTS          ----------------
 * ------------------------------------------------------- */
Opt opts[] = {
	{ 0, NULL, "--log-file",  "Log all results to <file>", 1 },
	{ 0, "-l", "--lines",     "Draw only lines." },
	{ 0, "-p", "--polygons",  "Draw only polygons." },
	{ 0, NULL, "--fills",     "Draw filled convex polygons." },
	{ 0, "-g", "--glyphs",    "Draw glyphs." },
	{ 0, "-c", "--circles",   "Draw only circles." },
	{ 0, "-t", "--triangles", "Draw only triangles." },
	{ 0, "-i", "--input",     "Test SDL input and KeySet routines." },
	{ 0, "-s", "--slow",      "Draw everything slowly." },
	{ 0, NULL, "--screen",    "Perform screen tests." },
	{ 0, NULL, "--xxx",       "Find intercepts of lines bigger than surface area." },
	{ 0, "-v", "--convex",    "Draw only convex polygons.         " },
	{ 0, "-n", "--nonconvex", "Draw nonconvex polygons.           " },
	{ 0, "-f", "--fade",      "Perform fade color tests.          " },
	{ 0, NULL, "--scaling",   "Perform shape scale tests.         " },
	{ 0, NULL, "--expansion", "Perform shape expansion tests.     " },
	{ 0, NULL, "--motion",    "Perform shape motion tests.        " },
	{ 0, NULL, "--transform", "Perform shape transformation tests." },
	{ 0, NULL, "--rotate",    "Perform shape rotation tests.      " },
	{ 0, NULL, "--resolution","Perform tests at specified resolution.", 1},
	{ .sentinel=1 }
};


/*A usage function.*/
void usage (Opt *opts, int status) {
	while (!opts->sentinel) { 
		if (!opts->arg) 
			fprintf(stderr, "%-2s, %-20s %s\n", opts->sht ? opts->sht : " " , opts->lng, opts->description);
		else {
			char argn[1024]; memset(&argn, 0, 1024);
			sprintf(argn, "%s <arg>", opts->lng);
			fprintf(stderr, "%-2s, %-20s %s\n", opts->sht ? opts->sht : " " , argn, opts->description);
		}
		opts++;
	}
	exit(status);
}




/* Main */
int 
main (int argc, char *argv[]) 
{
	if (argc < 2)
		usage(opts, 0);
	/*Evaulate options*/
	while (*argv) {
		Opt *o=opts;
		while (!o->sentinel) {
			//Find the option, then set the boolean, and optionally run some function or do that here...
			if ((o->sht && strcmp(*argv, o->sht) == 0) || (o->lng && strcmp(*argv, o->lng) == 0)) {
				o->set=1;
				if (o->callback)
					o->callback(*argv); 
			}
			o++;
		}
		argv++;
	}

	/*Just for testing options*/
	Opt *all=opts;
	while (!all->sentinel) {
		fprintf(stderr, "%20s: %s\n", all->lng, all->set ? "true" : "false");
		all++;
	}

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

	/*Select log file.*/
	//std = (opts.log_filename) ? fopen("canvas", "w+") : stderr;
	
	/*Event and input setup*/
	SDL_Event event;

	/*Evaluate each thing*/
	Opt *eval=opts;
	while (!eval->sentinel) {
		/*Option evaluation*/
		int co=0;
		if (optset("--lines"))
			drawlines(&Win);	
		else if (optset("--circles")) 
			drawcircles(&Win);
		else if (optset("--polygons")) 
			drawngons(&Win);
		else if (optset("--triangles")) 
			drawtriangles(&Win);
		else if (optset("--glyphs")) 
			drawglyphs(&Win);
		else if (optset("--fills")) 
			fillConvexGon(&Win);
		else if (optset("--fade")) 
			fade_between(&Win, 0xffffff, 0x000000, -0x111111);

		/*Do it if we got something*/
		if (co) {
			/*I want the total clock time, a pause and a fade before the next test*/
			fade_between(&Win, 0xffffff, 0x000000, -0x111111);
		}
		eval++;
	}

	/*Close log file and free anything*/
	//if (opts.log_filename) fclose(std);
	SDL_FreeSurface(Win.win);
	SDL_Quit();
	return 0;
}
