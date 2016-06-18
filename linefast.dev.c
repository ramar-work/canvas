/*linefast.c*/
/*----------------------------------
A second attempt at a Bresenham algorithim.

This one should be faster because:

1) plot() does not do any checks.  It expects
points that are within drawable range.

2) line() checks that points are within the 
drawable range, so this should reduce branching
and the number of checks that need to be done.

3) vert, horiz & diagonal lines ought to be 
accounted for.

4) points outside of line are recalculated
ONCE per line.  a crude implementation checked
that each point was not negative or not beyond 
the drawable area.
*-----------------------------------*/
/*Plot points, checking that each is within range*/
inline void plot (Surface *win, uint16_t x, uint16_t y, uint32_t c, uint32_t op) {
	#if BOUNDS_CHECK 
	/* This is bringing out a serious error in the Bresenham implementation below. */
	if ((y * win->w) + x >= win->w * win->h) {fprintf(stderr, "ERR: Value %d too great for plot!\n", (y * win->w) + x);return;} 
	#endif
	#ifdef DEBUG 
	LinePoint lp = { x, y, (y*win->w) + x };
	SetDebug(plot, &lp, plotfail);
	#endif
	#ifdef VERBOSE
	fprintf(stderr, "Plotting point at (%d,%d)\n", x, y);
	#endif
	int *pix = ((int*)win->win->pixels) + (y * win->w) + x;
	*pix = fillPix(c, 0xffffff); 
	#ifdef SLOW
	pause(0, SPEED);
	SDL_UpdateRect(win->win, 0,0,0,0);
	#endif
	#ifdef ENTER
	getchar();
	SDL_UpdateRect(win->win, 0,0,0,0);
	#endif
}



void line (Surface *win, 
           int32_t  x0,  int32_t  y0, 
           int32_t  x1,  int32_t  y1, 
           uint32_t clr, uint32_t opacity)
{
	/*Reject if it's obvious that points will not be drawn.*/
	uint32_t H = win->h - 1; 
	uint32_t W = win->w - 1;
	if ((x0<0 && x1<0)||(y0<0 && y1<0)||(x0>W && x1>W)||(y0>H && y1>H)) {
		fprintf(stderr, "Points outside of drawable range.\n");
		return;
	}

	/*Define stuff...*/
	int32_t pixCount, errInc, errDec, errInd, mn, mj; 
	int32_t *x=NULL, *y=NULL;
	int32_t dx=0, dy=0, dx2, dy2;
	int32_t tx=0, ty=0;
	int mnc, mjc;

#if 1
	/*Get interesting with distance calculation and find intercepts*/
	/*Swear I'm missing something...*/
	if (x0<0) {
		/*Find x-intercept*/
		dx = x1-x0;
		dy = (y1>y0)?y1-y0:y0-y1;
		fprintf(stderr, "New: (%3d/%3d), (%3d,%3d)\n", 0,y0,x1,y1);
		fprintf(stderr, "%d = (%d/%d)%d + b\n", y0,dx,dy,0); 
		exit(0);	
	}
	else if (x1<0) {
		dx = x0-x1;
		dy = (y1>y0)?y1-y0:y0-y1;
		fprintf(stderr, "New: (%3d/%3d), (%3d,%3d)\n", x0,y0,0,y1);
		fprintf(stderr, "%d = (%d/%d)%d + b\n", y0,dx,dy,0);  
	}
	else if (y0<0) {
		dy = y1-y0;
		dx = (x1>x0)?x1-x0:x0-x1;
		fprintf(stderr, "New: (%3d/%3d), (%3d,%3d)\n", x0,0,x1,y1);
		fprintf(stderr, "%d = (%d/%d)%d + b\n", y0,dx,dy,0);  
	}
	else if (y1<0) {
		dy = y0-y1;
		dx = (x1>x0)?x1-x0:x0-x1;
		fprintf(stderr, "New: (%3d/%3d), (%3d,%3d)\n", x0,y0,x1,0);
		fprintf(stderr, "%d = (%d/%d)%d + b\n", y0,dx,dy,0);  
	}

#else
	/*The old algorithim for line calculation.*/
	dx = (x1>x0)?x1-x0:x0-x1;
	dy = (y1>y0)?y1-y0:y0-y1;
	dx2 = dx*2;
	dy2 = dy*2;
#endif

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


	/*THe ugly way uses loops for now... this is bad for long term use though.*/

	/*Plot first point*/
	plot(win,x0,y0,clr,opacity);

#if 0
	/*Debugging point*/	
	int cc=0;
	fprintf(std, "Major Incrementor:  %d\n", mj);
	fprintf(std, "Minor Incrementor:  %d\n", mn);
	fprintf(std, "Major & Minor Axes: %s\n", (dx>dy) ? "x, y" : "y, x");
	fprintf(std, "Error index start:  %d\n", errInd);
	fprintf(std, "Error incrementor:  %d\n", errInc);
	fprintf(std, "Error decrementor:  %d\n", errDec);
	fprintf(std, "Line length:        %d\n", pixCount);
	fprintf(std, "x0, y0:     (%3d, %3d)\n", x0, y0);
	fprintf(std, "\n\n");
	fprintf(std, "%10s %5s %5s\n", "Error", "X", "Y"); 
#endif

#ifdef FASTLINES
	/* Optimize horizontal, vertical and diagonal lines */
	int *pix = setCursor(x0, y0); // Set initial position.
	/*We're not here yet, but would it be quicker to state machine this? fp[(!dx && !dy) ? 0 : ( ... )](clr, opac) */
	/* Maybe event seperate the three?
    	hline, vline, dline */
	if (!dx) {
		for (int i=0; i<dy; i++) {
			*pix = fillPix(clr, opacity);
			pix += win->w;	
		}
		return;
	}

	if (!dy) {
		for (int i=0; i<dx; i++) {
			*pix = fillPix(clr, opacity);
			pix += win->h;	
		}
		return;
	}

	if (dx == dy) {
		for (int i=0; i<dx; i++) {
			*pix = fillPix(clr, opacity);
			pix += mjc;	
		}
	}

	/* Run-slice lines.  
 		Better, according to Abrash, for long lines */
	/* (Xdelta % YDelta)/YDelta (switched if major is Y) */ 
	/* Faster because less calcs are involved. */
#endif

	/*Loop and create the line.*/
	while (pixCount--) { 
		// fprintf(std, "%10d %5d %5d\n", errInd,*x,*y);
		plot(win,*x,*y,clr,opacity);
		if (errInd >= 0) {
			// should "reset" the error index.
			errInd += (errInc - errDec);
			mn += mnc;   // minor index should go up
		} else {
			errInd += errInc;	
		}
		mj += mjc;
	}
}


/*Find the intercept for lines outside of the range.*/
uint32_t find_intercept (uint32_t rise, uint32_t run, uint32_t x, uint32_t y) {
	uint32_t it=0;
	return it;
}

