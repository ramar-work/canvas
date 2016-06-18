/*canvas.c*/
#define _POSIX_C_SOURCE 200809L
#include "canvas.h"

/*Some global variables that never change go here.*/
FILE *std = NULL;

#if 1  /*Eventually this is all DEBUG or NDEBUG or something*/
 #if 1
 inline void pause (uint32_t secs, uint32_t nsecs) { 
	//struct timespec ts = { .tv_nsec = nsecs };
	struct timespec ts = { .tv_sec = secs, .tv_nsec = nsecs };
	nanosleep(&ts, NULL);
 }
 #endif

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
inline void __catch (SDL_Event *ev, char *msg, int only, SDLKey key) 
{
	if (msg) { fprintf(stderr, "%s\n", msg); }
	while (1) {
		if (SDL_PollEvent(ev)) {
			if (ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_RETURN)
				break;
		}
	}
}
 
 void std_debug (void *p) { ; }
 
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

 void print_surface (Surface *s) {
	//p = sf = s->win;
	SDL_PixelFormat *p = s->win->format;
	fprintf(stderr, "Height:  %d\n", s->h);
	fprintf(stderr, "Width:   %d\n", s->w);
	//fprintf(stderr, "Height:  %d\n", s->hh; )
	//fprintf(stderr, "Height:  %d\n", s->ww; )
	fprintf(stderr, "BPP:     %d\n", s->bpp);
	fprintf(stderr, "Chroma:  %d\n", s->chromakey);
	fprintf(stderr, "BgColor: %d\n", s->bg);
	fprintf(stderr, "Resizb:  %d\n", s->resizable );
	fprintf(stderr, "HW Surf: %d\n", s->hwsw      );
	fprintf(stderr, "Fullscr: %d\n", s->fullscreen);
	fprintf(stderr, "Alpha:   %d\n", s->alpha     );
	fprintf(stderr, "Video:   %d\n", s->video     );
	fprintf(stderr, "DB Buf:  %d\n", s->dbbuf     );
	fprintf(stderr, "Position:(%d, %d)\n", s->position.x, s->position.y);
	//fprintf(stderr, "Surface: %p\n", (void *)s->srf);
	fprintf(stderr, "Userdata:%p\n", s->ud);
	fprintf(stderr, "Window:  %p\n", (void *)s->win);
	fprintf(stderr, "\t(SDL_Surface data)\n"); 
	fprintf(stderr, "\tRGB values:     %-3d, %-3d, %-3d\n", 
		p->Rmask, p->Gmask, p->Bmask);
	fprintf(stderr, "\tBits per pixel: %d\n", p->BitsPerPixel);
	fprintf(stderr, "\tAlpha mask:     %d\n", p->Amask);
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
	struct timespec gtimespec = { .tv_nsec = 10 }; \
	struct timeval now; \
	if ((nanosleep(&gtimespec, NULL) < 0)) \
		return 0; \
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


/* ---------------- LINES ----------------------- */
/*Plot points, checking that each is within range*/
inline void 
plot (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op) {
	/*Check that we're within bounds.*/ 
	if (((y * win->w) + x >= win->w * win->h) || x<0 || y<0) {
		//fprintf(stderr, "ERR: Value %d too great for plot!\n", (y * win->w) + x);
		return;
	} 
	
	/*Color the point*/
	int *pix = ((int*)win->win->pixels) + (y * win->w) + x;
	*pix = fillPix(c, 0); 

 #ifdef SLOW
	/*Move super slowly*/
	pause(0, SPEED);
	SDL_UpdateRect(win->win, 0,0,0,0);
 #endif
}

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
void 
line (Surface *win, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t clr, uint32_t opacity)
{
	/*Display stuff ought to be global.  That's all that there is...*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	if ((x0<0 && x1<0)||(y0<0 && y1<0)||(x0>W && x1>W)||(y0>H && y1>H)) {
		fprintf(stderr, "Points outside of drawable range.\n");
		return;
	}

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
void sline (Surface *win, 
           int32_t x0, int32_t y0, 
           int32_t x1, int32_t y1, 
           uint32_t clr, uint32_t opacity, 
					 _Bool save, Pt *ptlist)
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



/* Plot a point larger than a single pixel.*/
void coord (Surface *win, uint16_t x, uint16_t y, uint32_t c, uint32_t op, uint32_t w) {
// does dotting and more
	for (int xx=(x-w);xx<(x+w);xx++) {	
		for (int yy=(y-w);yy<(y+w);yy++) {	
			plot(win, xx, yy, c, op); 
		}
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


#define iforward(Index) \
	Index = (Index + 1) % poly->length;

#define ibackward(Index) \
	Index = (Index - 1 + poly->length) % poly->length;
	
#define imove(Index, Dir) \
	if (Dir > 0) \
		Index = (Index + 1) % poly->length; \
	else \
		Index = (Index - 1 + poly->length) % poly->length;


#if 0
/*Horizontal lines*/
typedef struct HLines HLines;
struct HLines {
};

/*Vertical lines*/
typedef struct VLines VLines;
struct VLines {
};
#endif

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
	['\b'] = SDLK_BACKSPACE,    ['\t'] = SDLK_TAB,      ['\r'] = SDLK_RETURN,
	[' ']  = SDLK_SPACE,        ['!']  = SDLK_EXCLAIM,  ['"']  = SDLK_QUOTEDBL,
	['#']  = SDLK_HASH,         ['$'] = SDLK_DOLLAR,    ['&'] = SDLK_AMPERSAND,
	['('] = SDLK_LEFTPAREN,     [')'] = SDLK_RIGHTPAREN,['*'] = SDLK_ASTERISK, 
	['+'] = SDLK_PLUS,          [','] = SDLK_COMMA,     ['-'] = SDLK_MINUS,
	['.'] = SDLK_PERIOD,        ['/'] = SDLK_SLASH,     ['0'] = SDLK_0,
	['1'] = SDLK_1,             ['2'] = SDLK_2, ['3'] = SDLK_3,
	['4'] = SDLK_4, ['5'] = SDLK_5, ['6'] = SDLK_6,
	['7'] = SDLK_7, ['8'] = SDLK_8, ['9'] = SDLK_9,
	[':'] = SDLK_COLON, [';'] = SDLK_SEMICOLON, ['<'] = SDLK_LESS,
	['='] = SDLK_EQUALS, ['>'] = SDLK_GREATER, ['?'] = SDLK_QUESTION,
	['@'] = SDLK_AT, ['['] = SDLK_LEFTBRACKET, [']'] = SDLK_RIGHTBRACKET,
	['^'] = SDLK_CARET, ['_'] = SDLK_UNDERSCORE,['`'] = SDLK_BACKQUOTE,
	['a'] = SDLK_a, ['b'] = SDLK_b, ['c'] = SDLK_c,
	['d'] = SDLK_d, ['e'] = SDLK_e, ['f'] = SDLK_f,
	['g'] = SDLK_g,['h'] = SDLK_h, ['i'] = SDLK_i,
	['j'] = SDLK_j,['k'] = SDLK_k,['l'] = SDLK_l,
	['m'] = SDLK_m,['n'] = SDLK_n,['o'] = SDLK_o, 
	['p'] = SDLK_p,['q'] = SDLK_q,['r'] = SDLK_r,
	['s'] = SDLK_s,['t'] = SDLK_t,['u'] = SDLK_u,
	['v'] = SDLK_v,['w'] = SDLK_w,['x'] = SDLK_x,
	['y'] = SDLK_y,['z'] = SDLK_z,['.'] = SDLK_KP_PERIOD,
	['/'] = SDLK_KP_DIVIDE,['*'] = SDLK_KP_MULTIPLY,['-'] = SDLK_KP_MINUS,
	['+'] = SDLK_KP_PLUS,['\r'] = SDLK_KP_ENTER,['='] = SDLK_KP_EQUALS,
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

