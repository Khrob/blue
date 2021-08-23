#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


// Functions in the platform layer
void (*push_rect) (float, float, float, float, float, float, float, float, uint16_t);
void (*open_window) ();
void (*start_app) ();

#define KB(value) (  (value)*1024LL)
#define MB(value) (KB(value)*1024LL)
#define GB(value) (MB(value)*1024LL)
#define TB(value) (GB(value)*1024LL)

typedef struct Memory_Arena
{
	size_t		bytes_used;
	void*       bytes; 	
	size_t		size;
} 
Memory_Arena;

typedef struct Input
{
	float 		mx,my;
	bool  		mouse_down;
	uint16_t 	window_width,window_height;
} 
Input;

typedef struct Colour
{
	float r,g,b,a;
} 
Colour;

typedef struct State 
{	
	float 			mx,my;
	float 			sx,sy,ex,ey;
	bool 			just_down,just_up;
	bool 			mouse_down;	
	uint32_t		hot_element;
	Memory_Arena   *permanent;
	Memory_Arena   *temporary;
} 
State;

State state;

typedef struct UI_Element
{
	float 		x,y;
	float 		w,h;
	float		r,g,b,a;
	uint32_t   	id;
}
UI_Element;

typedef struct Elements
{
	uint32_t	count;
	void*		elements;	 
} 
Elements;


Memory_Arena *create_memory_arena (size_t size)
{
	Memory_Arena *arena = malloc(sizeof(Memory_Arena));

	assert (arena != NULL);

	arena->bytes_used = 0;
	arena->bytes = malloc(size);
	arena->size = size;

	assert(arena->bytes != NULL);

	return arena;
}

#define push_struct (arena, type) (type *)push_size_(arena, sizeof(type))

void* push_size_ (Memory_Arena *arena, size_t size)
{
	assert (arena->bytes_used+size < arena->size);
	void *base = arena->bytes+arena->bytes_used;
	arena->bytes_used += size;
	return base;
}

#pragma function (memcpy)
void* memcpy (void *destination, void const *source, size_t size)
{
    unsigned char *s = (unsigned char *)source;
    unsigned char *d = (unsigned char *)destination;
    while(size--) *d++ = *s++;

    return(destination);
}

bool button (float x, float y, float w, float h)
{
	Colour c = {};
	bool clicked = false;

	if (state.mx > x && state.mx < x+w  &&  state.my > y && state.my < y+h) {
		c.r = state.mouse_down ? 0.75 : 0.5;
		clicked = state.just_up;
	} else { 
		c.r = 0.2; 
	}
	push_rect(x,y, w,h, c.r, c.g, c.b, c.a, 0);
	return clicked;
}

void setup_ui ()
{

}

void update (float t, void *input)
{
	Input *i = ((Input *)input);

	state.mx = i->mx;
	state.my = i->my;

	if (state.just_up && !i->mouse_down) {
		state.just_up = false;
	}

	if (i->mouse_down) {

		if (!state.mouse_down) {
			printf("mouse down!\n");
			state.just_down  = true;
			state.mouse_down = true;
		} 
		else if (state.just_down) {
			state.just_down = false;
		}
	} else {

		if (state.mouse_down) {
			state.just_up    = true;
			state.mouse_down = false;
			printf("mouse up\n");
		}
	}

	// Test the dragging functionality

	if (state.just_down) {
		state.sx = i->mx;
		state.sy = i->my;
	}

	if (state.mouse_down) {
		state.ex = i->mx;
		state.ey = i->my;
	}

	setup_ui ();

	state.just_up = false;
	state.just_down = false;
}


void render_timeline ()
{
	if (state.my > 0.7) {
		push_rect(0,0.7, 1,0.3, 0.5,0.5,0.5,1, 0);
	} else { 
		push_rect(0,0.7, 1,0.3, 0.75,0.75,0.75,1, 0);
	}

	if (button (0.2,0.2, 0.6,0.1)) {
		printf("Button pressed!\n");
	}
}

void render ()
{
	render_timeline();
	push_rect(state.sx,state.sy, state.ex-state.sx, state.ey-state.sy, 0,0,1,.1, 0);
}


int main ()
{
	void *handle = dlopen("stuff.dylib", RTLD_NOW);
	
	open_window  = dlsym(handle, "open_window");
	push_rect	 = dlsym(handle, "push_rect");
	start_app    = dlsym(handle, "start_app");

	state.permanent = create_memory_arena(MB(16));
	state.temporary = create_memory_arena(MB(16));

	start_app();
	open_window(update, render);

	return 0;
}




















