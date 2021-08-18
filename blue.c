#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct Input
{
	float x,y;
	bool  mouse_down;
} Input;

void update (float t, void *input);
void render ();
void (*push_rect) (float, float, float, float, float, float, float, float, uint16_t);
void (*open_window) ();
void (*start_app) ();


int main ()
{
	void  *handle = dlopen("stuff.dylib", RTLD_NOW);
	
	open_window  = dlsym(handle, "open_window");
	push_rect	 = dlsym(handle, "push_rect");
	start_app    = dlsym(handle, "start_app");

	start_app();
	open_window(update, render);

	printf("All done\n");

	return 0;
}

float sx,sy,ex,ey;
bool just_down = false;

void update (float t, void *input)
{
	Input *i = ((Input *)input);

	if (!i->mouse_down) { just_down = false; }

	if (i->mouse_down && !just_down) {
		just_down = true;
		sx = i->x;
		sy = i->y;
		printf("c: %f,%f\n", i->x, i->y);
	}

	if (just_down && i->mouse_down) {
		ex = i->x;
		ey = i->y;
	}
}

void render ()
{
	// push_rect(0.1,0.1, 0.2,0.2, 1.0,0.0,0.0,1.0, 0);
	// push_rect(0.0,0.0, 0.1,0.1, 0.0,1.0,0.0,1.0, 0);
	// push_rect(0.9,0.5, 0.1,0.5, 1.0,0.0,1.0,1.0, 0);

	push_rect(sx,sy, ex-sx, ey-sy, 0,0,1,1, 0);
}

