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

#define max(a,b) (a > b ? a : b)
#define min(a,b) (a > b ? b : a)

#define MAX_UI_DRAWABLES 256

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

typedef struct UI_Drawable
{
	float 		x,y,w,h;
	float		r,g,b,a;
}
UI_Drawable;

typedef struct UI_Drawables
{
	uint32_t	count;
	void*		drawables;	 
} 
UI_Drawables;

struct Layout
{
	float divider;
	float divider_height_in_pixels;
};

typedef struct State 
{	
	// Memory
	Memory_Arena   *permanent;
	Memory_Arena   *temporary;

	// Input
	float 			mx,my;
	float 			sx,sy,ex,ey;
	float			dx,dy;
	float			ox,oy;
	bool 			just_down,just_up;
	bool 			dragging;

	// UI Elements
	uint32_t		hot_element;
	UI_Drawables 	drawables;

	// Layout
	int 			window_width,window_height;
	struct Layout 	layout;	
} 
State;

State *state = NULL;

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

#define push_struct(arena, type) 		(type *)push_size_(arena, sizeof(type))
#define push_array(arena, type, count) 	(type *)push_size_(arena, sizeof(type)*count)

void*   push_size_ (Memory_Arena *arena, size_t size)
{
	assert (arena->bytes_used+size < arena->size);
	void *base = arena->bytes+arena->bytes_used;
	arena->bytes_used += size;
	return base;
}

void* memcpy (void *destination, void const *source, size_t size)
{
    unsigned char *s = (unsigned char *)source;
    unsigned char *d = (unsigned char *)destination;
    while(size--) *d++ = *s++;

    return(destination);
}

void push_ui_drawable (float x, float y, float w, float h, float r, float g, float b, float a, UI_Drawables *drawables)
{
	assert(drawables->count < MAX_UI_DRAWABLES);

	int count = drawables->count;
	size_t size = sizeof(UI_Drawable);

	UI_Drawable *d = drawables->drawables;
	d += count * size;

	d->x = x;
	d->y = y;
	d->w = w;
	d->h = h;
	d->r = r;
	d->g = g;
	d->b = b;
	d->a = a;

	drawables->count++;
}

bool button (float x, float y, float w, float h, uint16_t id, State *state)
{
	Colour c = {0.2,0,0,1};
	bool clicked = false;

	if (state->mx > x && state->mx < x+w  &&  state->my > y && state->my < y+h) {
		c.r = state->dragging ? 0.75 : 0.5;
		if (state->just_down) { state->hot_element = id; }
		if (state->just_up && state->hot_element == id) { clicked = true; }
	}

	push_ui_drawable(x,y, w,h, c.r, c.g, c.b, c.a, &state->drawables);
	return clicked;
}

void dragger (uint16_t id, State *state)
{
	Colour c = {0,0.5,0.5,1};

	float height = state->layout.divider_height_in_pixels/state->window_height;
	float y = state->layout.divider;

	if (state->hot_element == id) {
		state->layout.divider = state->my - state->oy;
		state->layout.divider = max(0,state->layout.divider);
		state->layout.divider = min(1-height,state->layout.divider);
	}

	if (state->my > y && state->my < y+height) {

		if (state->just_down) {
			state->hot_element = id;
			state->oy = state->my - y;
			printf("Started dragging\n");
		}
	}
	
	push_ui_drawable(0,y, 1,height, c.r, c.g, c.b, c.a, &state->drawables);
}

void render_timeline (State *state)
{
	Colour c = {0.5,0.5,0.5,1};
	float divider = state->layout.divider;
	if (state->my > divider) { c.r = 0.75; c.g = 0.75; c.b = 0.75; };

	push_ui_drawable(0,divider, 1,1.0-divider, c.r, c.g, c.b, c.a, &state->drawables);
	
	dragger(10002u, state);

	if (button (0.2,0.2, 0.6,0.1, 10001u, state)) {
		printf("Button pressed!\n");
	}
}


void update_ui (State *state, Input *i)
{
	state->window_width  = i->window_width; 
	state->window_height = i->window_height;

	state->dx = i->mx - state->mx;
	state->dy = i->my - state->my;

	state->mx = i->mx;
	state->my = i->my;

	if (state->just_up && !i->mouse_down) {
		state->just_up = false;
	}

	if (i->mouse_down) {

		if (!state->dragging) {
			printf("mouse down!\n");
			state->just_down = true;
			state->dragging  = true;
			state->sx = i->mx;
			state->sy = i->my;
		} 
		
		state->ex = i->mx;
		state->ey = i->my;
	} 
	else {

		if (state->dragging) {
			state->just_up  = true;
			state->dragging = false;
			printf("mouse up\n");
		}
	}

	state->drawables.count = 0;

	render_timeline(state);

	//push_ui_drawable(state->sx,state->sy, state->ex-state->sx, state->ey-state->sy, 0,0,1,.1, &state->drawables);

	state->just_down = false;
	if (state->just_up) {
		state->hot_element = 0;
		state->just_up = false;
	}
}

void update (float t, void *input)
{
	update_ui(state, (Input *)input);
}

void render ()
{
	UI_Drawable *e = state->drawables.drawables;

	for (int i=0; i<state->drawables.count; i++) {
		push_rect(e->x,e->y,e->w,e->h,e->r,e->g,e->b,e->a,0);
		e += sizeof(UI_Drawable);
	}
}

int main (void)
{
	void *handle = dlopen("platform.dylib", RTLD_NOW);
	
	open_window  = dlsym(handle, "open_window");
	push_rect	 = dlsym(handle, "push_rect");
	start_app    = dlsym(handle, "start_app");

	Memory_Arena *bootstrap = create_memory_arena(MB(16));
	state = push_struct(bootstrap, State);
	state->permanent = bootstrap;
	state->temporary = create_memory_arena(MB(16));

	state->drawables.drawables = push_array(state->temporary, UI_Drawable, MAX_UI_DRAWABLES);
	state->layout.divider = 0.5;
	state->layout.divider_height_in_pixels = 40.0;

	start_app();
	open_window(update, render);

	return 0;
}


