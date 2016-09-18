/*canvas.c*/
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include "canvas.h"
#include "lite.h"

#ifdef CV_DEBUG  /*Eventually this is all DEBUG or NDEBUG or something*/
 inline void __pause (uint32_t secs, uint32_t nsecs) { 
	//struct timespec ts = { .tv_nsec = nsecs };
	struct timespec ts = { .tv_sec = secs, .tv_nsec = nsecs };
	nanosleep(&ts, NULL);
 }

 inline void pauseX (Pause *time) {
 	/*Math is needed to take this to a regular level*/
 	struct timespec ts; //= { .tv_sec = time->time.secs, .tv_nsec = time->time.nsecs };
 	if (!time->type) {
 		ts.tv_nsec = time->time.nsecs;
 		ts.tv_sec = 0;
 	}
 	else {
 		ts.tv_nsec = time->time.nsecs;
 		ts.tv_sec = time->time.secs;
 	}
 	nanosleep(&ts, NULL);
 }
 
 #if 0
 #define pause(sc, nsc) \
 	pauseX(&((Pause){.type = 0, time.nsecs = nsc}));
 #endif


 /*Catch - to catch keystrokes without having to do much extra*/
 inline void __catch (SDL_Event *ev, char *msg, int only, SDLKey key) {
	if (msg) { fprintf(stderr, "%s\n", msg); }
	while (1) {
		if (SDL_PollEvent(ev)) {
			if (ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_RETURN)
				break;
		}
	}
 }
 
 void print_debug () {
 	fprintf(stderr, "ERROR OCCURRED (at %s:%d", debug.file, debug.line);
 	(debug.fn) ? fprintf(stderr, " within function '%s')\n", debug.fn) : fprintf(stderr, ")\n");
 	/*Your custom handler is called here.*/
 	debug.fp();	
 	exit(1);
 }
 
 void set_sigsegv () {
 	struct sigaction sa;
 	sa.sa_handler = print_debug; 
 	sigemptyset(&sa.sa_mask);
 	if (sigaction(SIGSEGV, &sa, NULL) == -1) {
 		fprintf(stderr, "%s\n", strerror(errno));
 		return;
 	}
 }

 typedef struct LinePoint LinePoint;
 struct LinePoint { uint32_t x, y, translated; };

 void plotfail (void) {
	LinePoint pt = *(LinePoint *)debug.data;
	fprintf(stderr, "Failed to plot point at: (%d, %d)\n", pt.x, pt.y); 
 }
#endif


/*Random number generator functions*/
#define randseed() \
	struct timeval now; \
	gettimeofday(&now, NULL); \
	srand(now.tv_usec)

/*Find quadrant*/
static uint8_t find_quadrant (Pt *of, Pt *len, int32_t xmp, int32_t ymp) {
	int32_t x=of->x, y=of->y;

	/*Check midpoint points*/
	if (xmp && xmp == x)
		return (y < (len->y/2)) ? 2 : 7;
	else if (ymp && ymp == y)
		return (x < (len->x/2)) ? 4 : 5;
	else if (ymp && xmp && xmp == x && ymp == y)
		return 0; // origin, don't do anything...

	int8_t xx = (x < (len->x/2)) ? 0 : 1;
	int8_t yy = (y < (len->y/2)) ? 0 : 1;

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

/*Return random number within a range.*/
inline int32_t rwr(int32_t l, int32_t h) {
	randseed();
	int32_t mx = !h ? 2 : h;
	int32_t i = rand() % mx;
	while (i < l || i > mx)
		i = rand() % mx;
	return i;
}

/*....*/
inline uint32_t urwr (uint32_t l, uint32_t h) {
	randseed();
	uint32_t mx = !h ? 2 : h;
	uint32_t i = rand() % mx;
	while (i < l || i > mx)
		i = rand() % mx;
	return i;
}

/*Fill a window or surface*/
void fill_display (Surface *win, uint32_t sc) {
	if (sc > 0xffffff) return;
	for (int i=0; i<win->w*win->h; i++) {
		int *pix = ((int*)win->win->pixels) + i;
		*pix = fillPix(sc, 0xff); 
	}
}

/*An even faster fill display*/
inline void fast_fill_display (Surface *win, uint32_t sc) {
	if (sc > 0xffffff) return;
	int *end = ((int*)win->win->pixels) + ((win->w*win->h) - 1);
	*end = -1; // This should do...
	int *pix = ((int*)win->win->pixels); 
	while (*pix > -1)
		*pix++ = fillPix(sc, 0xff); 
}


/*Draw from point array*/
void draw_from_point_array (Surface *srf, Pt *pt, void (*callback)(Pt *)) {
	Pt *c=pt, *n=pt+1, *h=&pt[0];
	_Bool on=1;
	while (on) {
		/*When the n pointer (n) equals h, stop.*/
		on = ((n = ((c+1)->x > -1 || (c+1)->y > -1) ? c+1 : h) == h) ? 0 : 1;
		fprintf(stderr, "line [%d, %d] => [%d, %d]\n", c->x, c->y, n->x, n->y);
		line(srf, c->x, c->y, n->x, n->y, 0xffffff, 0xff);
		c++;
	}
}


/*surface transforms*/
void transform (Surface *s) {
	#if 0
	/* Some rotations (for tile based surfaces) */
	for (c=0; c<(sizeof(compass)/sizeof(int)); c++) {
		int lim=compass[c];
		int cc, dd;
		/* north */
		if (c==0) {	
			for (cc=0;cc<lim;cc++) 
				fprintf(stdout, "%c", bb->body[cc]);
		}
		/* ... */
		else if (c==1) {
			dd=sb.st_size;
			for (cc=dd;cc>=lim;cc--) 
				fprintf(stdout, "%c", bb->body[cc]);
		}
		/* */
		else if (c==2) {
			dd=tw;
			while (dd<=lim) {
				for (cc=(dd-1);cc>=(dd-tw);cc--)
					fprintf(stdout, "%c", bb->body[cc]);
				dd+=tw;
			}
		}
		/* */
		else if (c==3) {
			dd=sb.st_size;
			while (dd>0) {
				for (cc=(dd-tw);cc<dd;cc++)
					fprintf(stdout, "%c", bb->body[cc]);
				dd-=tw;
			}
		}

		/* delimiter map */
		fprintf(stdout, "\n");
		for (cc=0;cc<tw;cc++)
			fprintf(stdout, "%c", '=');
		fprintf(stdout, "\n");
	}
	#endif
}


/*Functions that should be static can go here*/
/*When this has reached stability they get removed*/
/*find max or min from a series*/
#if 1
int32_t 
#else
static int32_t 
#endif
sortPts (Pt *pts, _Bool axis/*True = Y*/, _Bool order/*True = get max*/) {
	Pt *p=pts;
	int32_t *cmp; 
	int32_t b=(!axis) ? p->x : p->y;
	//fprintf(stderr, "Sorting axis: %s\n", (!axis) ? "x" : "y");
	while (*(cmp = (!axis) ? &p->x : &p->y) > -1) {
		(order) ? ((*cmp > b) ? b = *cmp : 0): ((*cmp < b) ? b = *cmp : 0); 
		p++;
	}
	return b;
}


/*Plot points, checking that each is within range*/
inline void 
plot (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op) {
	#ifdef CV_BOUND_CHECK
	/*Check that we're within bounds.*/ 
	if (((y * win->w) + x >= win->w * win->h) || x<0 || y<0)	
		#ifdef CV_VERBOSITY
	{
		fprintf(stderr, "ERR: Value %d too great for plot!\n", (y * win->w) + x);
		return;
	}
		#else
		return;
		#endif
	#endif
	
	/*Color the point*/
	int *pix = ((int *)win->win->pixels) + (y * win->w) + x;
	*pix = fillPix(c, 0); 

	/*Choose whether or not to control plotting speed by hand or by timer*/
 #ifdef CV_SLOW_PLOT
	__pause(0, CV_SPEED);
	SDL_UpdateRect(win->win, 0,0,0,0);
 #endif
 #ifdef CV_INTERRUPT_PLOT 
	//__pause(0, CV_SPEED);
	//hold();
	fprintf(stderr, "Waiting on input event...\n"); getchar();
	SDL_UpdateRect(win->win, 0,0,0,0);
 #endif
}

#if 1
inline void 
nplot (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op, Pt *ptlist) {
	if (((y * win->w) + x >= win->w * win->h) || x<0 || y<0) {
		//fprintf(stderr, "ERR: Value %d too great for plot!\n", (y * win->w) + x);
		return;
	} 
	#if 0
 #ifdef DEBUG 
	/*Debugging*/
	LinePoint lp = { x, y, (y*win->w) + x };
	SetDebug(plot, &lp, plotfail);
 #endif
  #endif

	/*Color the point*/
	int *pix = ((int*)win->win->pixels) + (y * win->w) + x;
	*pix = fillPix(c, 0xffffff); 
}
#endif

/*...*/
inline void 
nscan (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op, Pt *ptlist) {
	if (((y * win->w) + x >= win->w * win->h) || x<0 || y<0) {
		fprintf(stderr, "ERR: Value %d too great for plot!\n", (y * win->w) + x);
		return;
	} 

	/*Save points and quit.*/
	ptlist->x = x;
	ptlist->y = y;
}


/*Draw lines*/
void line (Surface *win, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t clr, uint8_t opacity)
{
	/*Display stuff ought to be global.  That's all that there is...*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	if ((x0<0 && x1<0)||(y0<0 && y1<0)||(x0>W && x1>W)||(y0>H && y1>H)) {
		fprintf(stderr, "Points outside of drawable range.\n");
		return;
	}

	fprintf(stderr, "Drawing line from %d,%d -> %d,%d\n", x0, y0, x1, y1);

	/*Define stuff...*/
	/*A struct will allow this to be oneline*/
	//struct line_metadata { int32_t pixCount, errInc, errDec, errInd, mn, mj, *x, *y; };
	int32_t pixCount, errInc, errDec, errInd, mn, mj; 
	int32_t *x=NULL, *y=NULL;
	int32_t dx=0, dy=0, dx2, dy2;
	int mnc, mjc;
	
	/*Figure out minor & major axes, and increment point*/
	get_slope(dx, dy, x0, y0, x1, y1); 
	dx2 = dx*2, dy2 = dy*2;

	/*Can I figure out intersection?*/
	#if 0
	int32_t xi,yi;
	fprintf(stderr, "xint: %d\n", (xi = (-y0/(dy/dx))+ x0));
	fprintf(stderr, "yint: %d\n", (yi = (-x0*(dy/dx))+ y0));
	#endif
	
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

	/*Plot first point*/
	nplot(win,x0,y0,clr,opacity,NULL);
	//plot(win,x0,y0,clr,opacity);

	/*Loop and create the line.*/
	while (pixCount--) { 
		plot(win,*x,*y,clr,opacity);
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
}

#if 0
/*Do allocation of scan structure within line*/
Pt * aline (Surface *win, 
           int32_t x0, int32_t y0, 
           int32_t x1, int32_t y1, 
           uint32_t clr, uint32_t opacity)
{
	/*Display stuff ought to be global.  That's all that there is...*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	if ((x0<0 && x1<0)||(y0<0 && y1<0)||(x0>W && x1>W)||(y0>H && y1>H)) {
		fprintf(stderr, "Points outside of drawable range.\n");
		return NULL;
	}

	/*Define stuff...*/
	int32_t pixCount, errInc, errDec, errInd, mn, mj; 
	int32_t *x=NULL, *y=NULL;
	int32_t dx=0, dy=0, dx2, dy2;
	int mnc, mjc;
	get_slope(dx, dy, x0, y0, x1, y1); 
	dx2 = dx*2, dy2 = dy*2;

	/*Figure out minor & major axes, and increment point*/
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

	/*Define fp and choose*/
	void (*plotf[2])(Surface *, int32_t, int32_t, uint32_t, uint32_t, Pt *) = { nplot, nscan };

	/*Plot first point*/
	// nplot(win,x0,y0,0,0,ptlist);

	/*Loop and create the line.*/
	while (pixCount--) { 
		//nplot(win,*x,*y,clr,opacity,ptlist);
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
	return NULL;
}
#endif

/*Do allocation of scan structure within line*/
void sline (Surface *win, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t clr, uint8_t opacity, _Bool save, Pt *ptlist)
{
	/*Display stuff ought to be global.  That's all that there is...*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	if ((x0<0 && x1<0)||(y0<0 && y1<0)||(x0>W && x1>W)||(y0>H && y1>H)) {
		/*fprintf(stderr, "Points outside of drawable range.\n");*/
		return;
	}

	/*Define stuff...*/
	int32_t pixCount, errInc, errDec, errInd, mn, mj; 
	int32_t *x=NULL, *y=NULL;
	int32_t dx=0, dy=0, dx2, dy2;
	int mnc, mjc;
	get_slope(dx, dy, x0, y0, x1, y1); 
	dx2 = dx*2, dy2 = dy*2;

	/*Figure out minor & major axes, and increment point*/
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

	/*Define fp and choose*/
	void (*plotf[2])(Surface *, int32_t, int32_t, uint32_t, uint32_t, Pt *) = { nplot, nscan };

	/*Plot first point*/
	nplot(win,x0,y0,0,0,ptlist);

	/*Loop and create the line.*/
	while (pixCount--) { 
		nplot(win,*x,*y,clr,opacity,ptlist);
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
}


/*Let's draw some circles per Wikipedia*/
void circle (Surface *win, uint16_t x, uint16_t y, uint16_t radius, uint32_t c, uint16_t o)
{
	int xx = radius;
	int yy = 0;
	int do2 = 1 - xx;

	while (yy<=xx) {
		plot(win,  xx +  x,  yy + y, c, o);  // 0ctant 0
		plot(win,  yy +  x,  xx + y, c, o);  
		plot(win, -xx +  x,  yy + y, c, o);
		plot(win, -yy +  x,  xx + y, c, o);
		plot(win, -xx +  x, -yy + y, c, o);  // Octant 4
		plot(win, -yy +  x, -xx + y, c, o);
		plot(win,  xx +  x, -yy + y, c, o);
		plot(win,  yy +  x, -xx + y, c, o);
		yy++;
		if (do2 <= 0)
			do2 += 2 * yy + 1;
		else {
			xx--;
			do2 += 2 * (yy-xx) + 1;	
		}
	}
}


/*Polygon handling*/
void ngon (Surface *win, Poly *poly, uint32_t stroke, uint32_t fill, uint32_t opacity) {
	/*Draw all points*/
	for (int i=0; i<poly->length; i++) {
		Pt *pc = &poly->points[i], *pn = &poly->points[(i==(poly->length-1)) ? 0 : i+1];
		printf("%d, %d -> %d, %d\n", pc->x, pc->y, pn->x, pn->y);
		line(win, pc->x, pc->y, pn->x, pn->y, stroke, 0xff);
	}
}


/*Just draw a filled polygon.*/
void fgon (Surface *win, Pt *pts, uint32_t color, uint32_t op) {
	/*Set things that should be calc'd once and references*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;

	/*Experiment with a datatype for this*/
	struct polytype {
		int32_t length;     /*Height of the polygon*/
		int32_t *trace;     /*....*/
		int32_t tlen;
	//we also know that there will be at least a multiple of two points
	//(height * 2)  = the amount of x points that we'll need to track
		int32_t beg, end;   /*These should only be one point.*/
		int32_t top, bot;   /*These should only be one point.*/
	} Py;


	/*Sort points and find length*/
	Pt *c=pts;
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
	Pt *curr=c, *next=c+1, *home=&c[0];

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
			//hold();
			plot(win,*x,*y,color,op);
	
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
		};
		curr++;
	}

	/*Set up the shading*/
	int bl=Py.length-1, br=Py.length;
	int start=Py.bot;

	/*Echo eveyrthing*/
	fprintf(stderr, "height of gon: %d\n", Py.length);		
	//fprintf(stderr, "find end of gon:    %d\n", get_end(Py.trace));
	fprintf(stderr, "start point:        %d\n", Py.bot);
	fprintf(stderr, "total length:       %d\n", Py.length);
	fprintf(stderr, "left side up:       %d\n", bl); 
	fprintf(stderr, "right side up:      %d\n", br); 

	#if 0
	int cuz=Py.bot;
	for (int p=0;p<Py.length;p++,cuz--)
		fprintf(stderr, "%d,%d - ", p, cuz);
	#endif
	//hold();

	/*Shade the polygon*/
	while (Py.length--) {
		line(win, Py.trace[bl], start, Py.trace[br], start, color, op); 
		bl--, br++, start--;
		//SDL_UpdateRect(win->win, 0, 0, 0, 0);
		//hold();
	}
} 



int 
FillConvexNGon (Surface *win, Poly *poly, uint32_t stroke, uint32_t fill, uint32_t opacity) 
{
	/*The fill lines need to be no bigger than the height (or width) of the window*/
	/*Define stuff*/
	Pt *vp = poly->points;

	/*Scan the gon for top and bottom*/
	if (!poly->length)
		return 0;

	/*Generate a data structure large enough to hold points comprising sides*/
	struct LineVertices {
		uint32_t ccount;  /*Coordinate count*/
		Pt *pts;          /*Coordinates on a particular line*/
	};
	struct LineVertices lv[poly->length];

	/*Generate additional data structures to */
	for (int i=0; i<poly->length; i++) {
		Pt *pc = &vp[i], *pn = &vp[(i==(poly->length-1)) ? 0 : i+1]; 
		uint32_t dx=0, dy=0;
		get_slope(dx, dy, pc->x, pc->y, pn->x, pn->y);
		//printf("rise over run: (%3d/%3d)\n", dy, dx);
	
		lv[i].ccount = (dx>dy) ? dx : dy;

		/*Pass this data structure to the line algorthim*/
		Pt pts[(dx>dy) ? dx : dy]; 	
			
	} 

	/*Find top-edge points*/
	/*Find left edge  and right edge of polygon*/
	/*Set scan line # (to perform the fill)*/		
	/*Put all vertices into an array*/
	/*Draw each line to fill the polygon*/
	return 1;
}


/* ---------------- INPUT ----------------------- */
/*Keysym table*/
static int keysyms[] = {
	['\b']= SDLK_BACKSPACE,['\t']= SDLK_TAB,        ['\r']= SDLK_RETURN,
	[' '] = SDLK_SPACE,    ['!'] = SDLK_EXCLAIM,    ['"'] = SDLK_QUOTEDBL,
	['#'] = SDLK_HASH,     ['$'] = SDLK_DOLLAR,     ['&'] = SDLK_AMPERSAND,
	['('] = SDLK_LEFTPAREN,[')'] = SDLK_RIGHTPAREN, ['*'] = SDLK_ASTERISK, 
	['+'] = SDLK_PLUS,     [','] = SDLK_COMMA,      ['-'] = SDLK_MINUS,
	['.'] = SDLK_PERIOD,   ['/'] = SDLK_SLASH,      ['0'] = SDLK_0,
	['1'] = SDLK_1,        ['2'] = SDLK_2,          ['3'] = SDLK_3,
	['4'] = SDLK_4,        ['5'] = SDLK_5,          ['6'] = SDLK_6,
	['7'] = SDLK_7,        ['8'] = SDLK_8,          ['9'] = SDLK_9,
	[':'] = SDLK_COLON,    [';'] = SDLK_SEMICOLON,  ['<'] = SDLK_LESS,
	['='] = SDLK_EQUALS,   ['>'] = SDLK_GREATER,    ['?'] = SDLK_QUESTION,
	['@'] = SDLK_AT,       ['['] = SDLK_LEFTBRACKET,[']'] = SDLK_RIGHTBRACKET,
	['^'] = SDLK_CARET,    ['_'] = SDLK_UNDERSCORE, ['`'] = SDLK_BACKQUOTE,
	['a'] = SDLK_a,        ['b'] = SDLK_b,          ['c'] = SDLK_c,
	['d'] = SDLK_d,        ['e'] = SDLK_e,          ['f'] = SDLK_f,
	['g'] = SDLK_g,        ['h'] = SDLK_h,          ['i'] = SDLK_i,
	['j'] = SDLK_j,        ['k'] = SDLK_k,          ['l'] = SDLK_l,
	['m'] = SDLK_m,        ['n'] = SDLK_n,          ['o'] = SDLK_o, 
	['p'] = SDLK_p,        ['q'] = SDLK_q,          ['r'] = SDLK_r,
	['s'] = SDLK_s,        ['t'] = SDLK_t,          ['u'] = SDLK_u,
	['v'] = SDLK_v,        ['w'] = SDLK_w,          ['x'] = SDLK_x,
	['y'] = SDLK_y,        ['z'] = SDLK_z,          ['.'] = SDLK_KP_PERIOD,
	['/'] = SDLK_KP_DIVIDE,['*'] = SDLK_KP_MULTIPLY,['-'] = SDLK_KP_MINUS,
	['+'] = SDLK_KP_PLUS,  ['\r']= SDLK_KP_ENTER,   ['='] = SDLK_KP_EQUALS,
};

/*Dummy function*/
void dummy (SDL_Event *event, Surface *bg) {
	printf("%s\n", "I do nothing.");
}


/*Set all the keys in the KeySet struct to their SDLK equivalents.*/
void set_all_keys (KeySet *keys) {
	/*Set all to dummy*/
	preinitialize(bnd, dummy);
	/*Set up a SIGSEGV handler when testing.*/
	set_sigsegv();	
	/*Then set the rest*/
	while (keys->key) {
		bnd[keys->key] = keys->fp;
		keys++;
	}
}


/* ---------------- TIMER ----------------------- */
void __timer_start (Timer *t, const char *file, int line) {
	t->file = file;
	t->linestart = line;
}

void __timer_end (Timer *t, const char *file, int line) {
	t->file = file;
	t->lineend = line;
}

void __timer_elap (const char *file, int line) {
}

