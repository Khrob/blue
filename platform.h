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

void *push_size_ (Memory_Arena *arena, size_t size)
{
	assert (arena->bytes_used+size < arena->size);
	void *base = arena->bytes+arena->bytes_used;
	arena->bytes_used += size;
	return base;
}

void *memcpy (void *destination, void const *source, size_t size)
{
    unsigned char *s = (unsigned char *)source;
    unsigned char *d = (unsigned char *)destination;
    while(size--) *d++ = *s++;

    return(destination);
}

void null_terminated_string_copy (char *dest, char *source, size_t max_size)
{
	uint16_t count = 0;
	unsigned char *s = (unsigned char *)source;
    unsigned char *d = (unsigned char *)dest;

	while (true) {
		*d++ = *s++;
		if (*s == '\0') break;
		if (count++ == max_size) {
			*d++ = '\0';
			break;
		}
	}
}

size_t null_terminated_strlen (void *string)
{
	size_t count = 0;
	unsigned char *s = (unsigned char *)string;
	while (*s++ != '\0') { count++; }
	return count;
}

#include <mach-o/dyld.h>

void executable_path (char *dest) 
{
	char raw_path_name[256];
    uint32_t raw_path_size = (uint32_t)sizeof(raw_path_name);
    int res = _NSGetExecutablePath(raw_path_name, &raw_path_size);
    assert(res == 0);
	null_terminated_string_copy(dest, raw_path_name, 256);
 	size_t length = null_terminated_strlen(dest);
 	// NOTE(khrob): 5 is 'blue\0' - _should_ go up to the final '/'
	dest[length-5] = '\0';
}

#endif
