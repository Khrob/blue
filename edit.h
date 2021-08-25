#include "platform.h"


typedef struct 
{
	char	 	magic[4];			// Should always be 0xED17ED17
	int 		version;			// File Format version 
	float 		ratio_width; 		// For the ratio, not in pixels.
	float 		ratio_height;		// This is a float so you can to 1:2.39
	float 		fps;				// Frames per second
} 
Edit_File_Header;

typedef struct 
{
	float start;
	float duration;
	float length;
}
Clip;

typedef struct 
{	
	char*		title;
	float		ratio_width;
	float		ratio_height;

	uint16_t	clip_count;
	Clip* 		clips;
} 
Edit_Project;


#define MAX_CLIP_COUNT 4096

Edit_Project *create_project (Memory_Arena *arena) 
{
	Edit_Project *project = push_struct(arena, Edit_Project);
	project->clips = push_array(arena, Clip, MAX_CLIP_COUNT);
	return project;
}

void push_clip (Clip c, Edit_Project *p)
{
	assert (p->clip_count < MAX_CLIP_COUNT-1);
	Clip *next_clip_slot = p->clips+p->clip_count*sizeof(Clip);
	memcpy(next_clip_slot, &c, sizeof(Clip));
	//printf("pushed clip (%f,%f) into slot %d\n", c.start, c.duration, p->clip_count);
	p->clip_count++;
}

float length (Edit_Project *p)
{
	float end = 0.0;
	Clip *c = p->clips;
	for (int i=0; i<p->clip_count; i++) { 
		//printf("%i - %f+%f -> %f\n", i, c->start, c->duration, c->start+c->duration);
		end = max(c->start+c->duration,end); 
		c += sizeof(Clip);
	}
	return end;
}


















