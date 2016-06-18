/*lineslow.c*/
/*----------------------------------
A first attempt at a Bresenham algorithim.
It's probably going to be slow because:
1) plot() checks that every point is within
the drawable range.
2) plot() is not a define nor is it an inlined
function.
3) line() is not being intelligent as far as
checking to see if lines are vertical or horizontal
without a lot of branching
*-----------------------------------*/
/*Plot points, checking that each is within range*/
inline void 
plot (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op) {
	/*Check that we're within bounds.*/ 
	if (((y * win->w) + x >= win->w * win->h) || x<0 || y<0) {
		fprintf(stderr, "ERR: Value %d too great for plot!\n", (y * win->w) + x);
		return;
	} 
	
 #ifdef DEBUG 
	/*Debugging*/
	LinePoint lp = { x, y, (y*win->w) + x };
	SetDebug(plot, &lp, plotfail);
 #endif

	/*Color the point*/
	int *pix = ((int*)win->win->pixels) + (y * win->w) + x;
	*pix = fillPix(c, 0xffffff); 

 #ifdef SLOW
	/*Move super slowly*/
	pause(0, SPEED);
	SDL_UpdateRect(win->win, 0,0,0,0);
 #endif

 #ifdef ENTER
	/*Move at keypress - this can get more complex and move on callback or something...*/
	getchar();
	SDL_UpdateRect(win->win, 0,0,0,0);
 #endif
}


inline void 
nplot (Surface *win, int32_t x, int32_t y, uint32_t c, uint32_t op, Pt *ptlist) {
	if (((y * win->w) + x >= win->w * win->h) || x<0 || y<0) {
		fprintf(stderr, "ERR: Value %d too great for plot!\n", (y * win->w) + x);
		return;
	} 

 #ifdef DEBUG 
	/*Debugging*/
	LinePoint lp = { x, y, (y*win->w) + x };
	SetDebug(plot, &lp, plotfail);
 #endif

	/*Color the point*/
	int *pix = ((int*)win->win->pixels) + (y * win->w) + x;
	*pix = fillPix(c, 0xffffff); 

 #ifdef SLOW
	/*Move super slowly*/
	pause(0, SPEED);
	SDL_UpdateRect(win->win, 0,0,0,0);
 #endif

 #ifdef ENTER
	/*Move at keypress - this can get more complex and move on callback or something...*/
	getchar();
	SDL_UpdateRect(win->win, 0,0,0,0);
 #endif
}


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
void line (Surface *win, 
           int32_t x0, int32_t y0, 
           int32_t x1, int32_t y1, 
           uint32_t clr, uint32_t opacity)
{
	/*Display stuff ought to be global.  That's all that there is...*/
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
	int mnc, mjc;
	
	/*Figure out minor & major axes, and increment point*/
	get_slope(dx, dy, x0, y0, x1, y1); 
	dx2 = dx*2, dy2 = dy*2;
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
	plot(win,x0,y0,clr,opacity);

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


/*Do allocation of scan structure within line*/
void sline (Surface *win, 
           int32_t x0, int32_t y0, 
           int32_t x1, int32_t y1, 
           uint32_t clr, uint32_t opacity, Pt *ptlist)
{
	/*Display stuff ought to be global.  That's all that there is...*/
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
