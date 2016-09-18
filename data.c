/*data.c*/

/*Plots*/
Pt Points[] = {
	{ 300, 300 },
	{ 100, 300 },
	{ 200, 300 },
	{ 400, 300 },
	{ 300, 300 },
	{ 310, 300 },
	{ 320, 300 },
	{ 330, 300 },
	{ 340, 300 },
	{ -1, -1},
};

/*Lines*/
//Line
LineP Lines[] = {
#if 0
	/*These lines are all green because drawing should work.*/
	{{tw/2,  0},{tw/2,th}, 0x00ff00},  
	{{ 0,   th},{tw,   0}, 0x00ff00},  
	{{ 0, th/2},{tw,th/2}, 0x00ff00},  
	{{ 0,    0},{tw,  th}, 0x00ff00},  
#endif
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
	/*Line null*/
	{.sentinel=1},
};

/*Circles*/
//Circle
Pt Circles[] = { {0,0}
};

/*Convex (convex polygons)*/
//Convex { ... }

/*NonConvex (weird polygons with overlapping edges)*/
//NonConvex { ... }

PolyP Polys[] = {
#if 0
/*Simple polygons*/
{{{100,98},{50,200} ,{150,230},{-1,-1}}, 0xffffff           /*Triangle*/          },
{{{300,300},{200,300},{200,200},{300,200},{-1,-1}}, 0xffffff/*Square*/            },
{{{ 30,400},{500,420},{210,440},{ 50,450},{ 20,420},{-1,-1}}, 0xffffff},// eww...
{{{200,400},{220,420},{210,440},{190,440},{180,420},{-1,-1}}, 0xffffff},// eww...

/*Complete width and height polygons*/
{{{0,0},{639,0},{639,330},{0,200},{-1,-1}}, 0xbbbbbb        /*top half of screen*/},
{{{0,200},{0,479},{639,479},{639,330},{-1,-1}}, 0x3f3f3f    /*bot half of screen*/},
#endif
/*Weird polygons*/	
//{{{0,400},{100,479},{200,309},{-1,-1}}, 0x414141                                  },
{{{200,309},{0,400},{100,479},{-1,-1}}, 0x414141                                  },
{{{0,400},{0,479},{100,479},{-1,-1}}, 0x9998b0                           },
{{{0,200},{0,479},{639,479},{639,330},{-1,-1}}, 0x123fff             },
{{{0,0},{640,0},{640,230},{0,100},{-1,-1}}, 0x323f1f },

#if 0 /*These test points outside of the surface.*/
{{{30,400},{220,420},{210,9999},{500,10321},{180,10024},{-1,-1}}, 0xffffff},// eww...
{{{30,400},{20,420},{610,9999},{500,10321},{-1,-1}},0xffffff},// eww...
#endif
{.sentinel=1}
};
