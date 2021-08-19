#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

// TODO (khrob): move the platform stuff into a header somewhere?

typedef struct Input
{
	float 		mx,my;
	bool  		mouse_down;
	uint16_t 	window_width,window_height;
} 
Input;

// TODO (khrob): colour space
typedef struct Colour
{
	float r,g,b,a;
} 
Colour;

typedef struct UI_State {
	
	float 	mx,my;
	float 	sx,sy,ex,ey;
	bool 	just_down,just_up;
	bool 	mouse_down;	
} 
UI_State;

void update (float t, void *input);
void render ();
void (*push_rect) (float, float, float, float, float, float, float, float, uint16_t);
void (*open_window) ();
void (*start_app) ();

UI_State ui_state;

#pragma function(memcpy)
void *memcpy(void *destination, void const *source, size_t size)
{
    unsigned char *s = (unsigned char *)source;
    unsigned char *d = (unsigned char *)destination;
    while(size--) *d++ = *s++;

    return(destination);
}


int main ()
{
	void *handle = dlopen("stuff.dylib", RTLD_NOW);
	
	open_window  = dlsym(handle, "open_window");
	push_rect	 = dlsym(handle, "push_rect");
	start_app    = dlsym(handle, "start_app");

	start_app();
	open_window(update, render);

	printf("All done\n");

	return 0;
}



void update (float t, void *input)
{
	Input *i = ((Input *)input);

	ui_state.mx = i->mx;
	ui_state.my = i->my;


	// TODO (khrob): move this into the render callback,
	// just update the position and the down/up status
	// in here.
	
	if (ui_state.just_up && !i->mouse_down) {
		ui_state.just_up = false;
	}

	if (i->mouse_down) {

		if (!ui_state.mouse_down) {
			printf("mouse down!\n");
			ui_state.just_down = true;
			ui_state.mouse_down = true;
		} 
		else if (ui_state.just_down) {
			ui_state.just_down = false;
		}
	} else {

		if (ui_state.mouse_down) {
			ui_state.just_up = true;
			ui_state.mouse_down = false;
			printf("mouse up\n");
		}
	}


	// Test the dragging functionality

	if (ui_state.just_down) {
		ui_state.sx = i->mx;
		ui_state.sy = i->my;
	}

	if (ui_state.mouse_down) {
		ui_state.ex = i->mx;
		ui_state.ey = i->my;
	}
}

bool button (UI_State uis, float x, float y, float w, float h)
{
	Colour c = {};

	bool clicked = false;

	if (uis.mx > x && uis.mx < x+w && 
		uis.my > y && uis.my < y+h) {
		c.r = uis.mouse_down ? 0.75 : 0.5;
		clicked = uis.just_up;
	} else { 
		c.r = 0.2; 
	}
	push_rect(x,y, w,h, c.r, c.g, c.b, c.a, 0);
	return clicked;
}

void render_timeline (UI_State uis)
{
	if (uis.my > 0.7) {
		push_rect(0,0.7, 1,0.3, 0.5,0.5,0.5,1, 0);
	} else { 
		push_rect(0,0.7, 1,0.3, 0.75,0.75,0.75,1, 0);
	}

	if (button (ui_state, 0.2,0.2, 0.6,0.1)) {
		printf("Button pressed!\n");
	}
}

void render ()
{
	render_timeline(ui_state);
	push_rect(ui_state.sx,ui_state.sy, ui_state.ex-ui_state.sx, ui_state.ey-ui_state.sy, 0,0,1,.1, 0);
}

