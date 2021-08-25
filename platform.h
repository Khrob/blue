#ifndef PLATFORM_H
#define PLATFORM_H

// Functions in the platform layer

void (*push_rect) (float, float, float, float, float, float, float, float, uint16_t);
void (*open_window) ();
void (*start_app) ();

#define KB(value) (  (value)*1024LL)
#define MB(value) (KB(value)*1024LL)
#define GB(value) (MB(value)*1024LL)
#define TB(value) (GB(value)*1024LL)

#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) > (b) ? b : a)

typedef struct Memory_Arena
{
	size_t		bytes_used;
	void*       bytes; 	
	size_t		size;
} 
Memory_Arena;

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


#endif
