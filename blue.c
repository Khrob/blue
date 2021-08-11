#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>


void update (float t);
void render ();
void (*push_rect)(float, float, float, float, float, float, float);
void (*open_window)();

int main ()
{
	void  *handle = dlopen("stuff.dylib", RTLD_NOW);
	
	open_window = dlsym(handle, "open_window");
	push_rect = dlsym(handle, "push_rect");

	open_window(update, render);
	
	printf("window has been opened. Are we still running here?\n");

	return 0;
}

void update (float t)
{
	// printf("elapsed: %f\n", t);
}

void render ()
{
	// printf("render called\n");
	push_rect(0.1,0.1, 0.2,0.2, 1.0,0.0,0.0);
	push_rect(0.0,0.0, 0.02,0.02, 1.0,0.0,0.0);
	push_rect(0.9,0.5, 0.1,0.5, 1.0,0.0,0.0);
}

