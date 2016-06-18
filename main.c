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

/* ------------------------------------------------------- *
   -------------       SHAPES             ----------------
 * ------------------------------------------------------- */
/*Let's do a bunch of simple shapes*/
void drawngons (Surface *bg) 
{
	/*Filled and unfilled*/
	/*Let's just try fills*/
	struct PolyP { Pt p[20]; int32_t color; _Bool sentinel; } Polys[] = {
		{{{100,100},{50,200},{150,200},{-1,-1}}, 0xffffff},   
		{{{300,300},{200,300},{200,200},{300,200},{-1,-1}}, 0xffffff},
		{{{200,400},{220,420},{210,440},{190,440},{180,420},{-1,-1}}, 0xffffff},
		{.sentinel=1}
	};

	/*Set things that should be calc'd once and references*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	struct PolyP *p=Polys;

	/*Sort each set of points to:*/
	//Find the y axis length
	while (!p->sentinel) {
		int on=1;
		Pt *curr=p->p, *next=p->p+1, *home=&p->p[0];
		while (on) {
			/*Move through all points until you reach "home"*/
			on = ((next = ((curr+1)->x > -1 || (curr+1)->y > -1) ? curr+1 : home) == home) ? 0 : 1;
			
		}
	}
	return; 

	/*Draw each polygon*/
	while (!p->sentinel) {
		int on=1;
		Pt *curr=p->p, *next=p->p+1, *home=&p->p[0];
		while (on) {
			/*Move through all points until you reach "home"*/
			on = ((next = ((curr+1)->x > -1 || (curr+1)->y > -1) ? curr+1 : home) == home) ? 0 : 1;
			/*Set beginning x,y and end x,y of line in question*/
			int32_t ex1=curr->x, ey1=curr->y;
			int32_t ex2=next->x, ey2=next->y;
			//fprintf(stderr, "(%d,%d) -> (%d, %d)\n", ex1, ey1, ex2, ey2);
			//line(bg, ex1, ey1, ex2, ey2, p->color, 0xff);

			/*Line algo calcs are here, but this may be a defne later*/
			if ((ex1<0 && ex2<0)||(ey1<0 && ey2<0)||(ex1>W && ex2>W)||(ey1>H && ey2>H)) {
				fprintf(stderr, "Points of polygon outside of drawable range.\n");
				curr++;
				continue;	
			}

			/*Define stuff...*/
			int32_t pixCount, errInc, errDec, errInd, mn, mj; 
			int32_t *x=NULL, *y=NULL;
			int32_t dx=0, dy=0, dx2, dy2;
			int mnc, mjc;
			
			/*Figure out minor & major axes, and increment point*/
			get_slope(dx, dy, x0, y0, x1, y1); 
			dx2 = dx*2, dy2 = dy*2;

			/*Set depending on slope*/			
			if (dx>dy) {
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
			}
			else {
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

			/*Save the first point?*/
			/*Save only the x's*/
			while (pixCount--) {
				//plot(win,*x,*y,clr,opacity);
				// ptarr[i]=*x;
				if (errInd >= 0) {
					// should "reset" the error index.
					errInd += (errInc - errDec);
					mn += mnc;   // minor index should go up
				} 
				else {
					errInd += errInc;	
				}
				mj += mjc;
			}
			curr++;
		}
		p++;
	}

	SDL_UpdateRect(bg->win, 0, 0, 0, 0);
	transit();
}


#define AllocateConvexGon

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
