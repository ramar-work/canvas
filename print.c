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
