#ifndef AAQ_ARENA_H_
#define AAQ_ARENA_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// TODO: Make the arena into a linked list of regions

typedef struct Region* Arena;

Arena arena_init_cap(size_t numOfBytes);

#define DEFAULT_ARENA_CAP 1 << 30                       // 1 GiB
#define arena_init() arena_init_cap(DEFAULT_ARENA_CAP); // Alloc 1 GiB

// These functions should not be used directly
void* arena_alloc_with_custom_alignment(Arena a, size_t size_of_type,
                                        size_t type_alignment, size_t count);

void* arena_zalloc_with_custom_alignment(Arena a, size_t size_of_type,
                                         size_t type_alignment, size_t count);

// Macro that should actually be used
#define arena_alloc(arena, type, count)                                        \
  (type*)arena_alloc_with_custom_alignment(arena, sizeof(type),                \
                                           _Alignof(type), count)

// Like `arena_alloc` but also zeroes out memory
#define arena_zalloc(arena, type, count)                                       \
  (type*)arena_zalloc_with_custom_alignment(arena, sizeof(type),               \
                                            _Alignof(type), count)

void arena_reset(Arena a); // Set count to 0, effectively freeing memory but not
                           // giving it back to OS
void arena_free(Arena a);  // Returns memory to the OS

size_t arena_get_capacity(Arena a); // In bytes
size_t arena_get_count(Arena a);    // In bytes

// Get the pointer the arena would bump from
// Equivalent to arena->data[arena->count]
// Does not care about padding/free list
// UNSAFE TO WRITE TO THIS LOCATION
uintptr_t arena_get_head_ptr(Arena a);

// void arena_info_print(Arena a);
// void arena_mem_print(Arena a);
void arena_debug_print(Arena a);

#endif
