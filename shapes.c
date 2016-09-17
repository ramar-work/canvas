/*shapes.c*/

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
