#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void update (float t);
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

void update (float t)
{
	// printf("elapsed: %f\n", t);
}

void render ()
{
	// printf("render called\n");

	push_rect(0.1,0.1, 0.2,0.2, 1.0,0.0,0.0,1.0, 0);
	push_rect(0.0,0.0, 0.1,0.1, 0.0,1.0,0.0,1.0, 0);
	push_rect(0.9,0.5, 0.1,0.5, 1.0,0.0,1.0,1.0, 0);
}

