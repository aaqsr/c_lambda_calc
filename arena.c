#include "arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

// TODO: Make an Arena a linked list of Regions
// TODO: Padding leads to memory waste, add a free list that keeps track of
// these bytes and gives them out as well

struct Region
{
  // First index of array that is available
  size_t count;
  // Total size of array
  size_t capacity;
  // Data array itself stored as an u8
  // This means every array entry is one byte which makes our life easy
  uint8_t data[];
};

Arena arena_init_cap(size_t numOfBytes)
{
  size_t sz_bytes = sizeof(struct Region) + sizeof(uint8_t) * numOfBytes;
  Arena res = mmap(NULL, sz_bytes, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  assert(res != MAP_FAILED);
  res->count = 0;
  res->capacity = numOfBytes;
  return res;
}

// Align parameter allows the arena to handle any alignment
void* arena_alloc_with_custom_alignment(Arena a, size_t size_of_type,
                                        size_t type_alignment, size_t count)
{
  // To align the allocated data, we need to compute the number of bytes to
  // advance the address (padding) until the alignment evenly divides the
  // address. The modulo with align computes the number of bytes it’s since the
  // last alignment:
  //                          extra = addr % align
  // We can’t operate numerically on an address like this, so in the code we
  // first convert to uintptr_t. Alignment is always a power of two, which
  // notably excludes zero, so no worrying about division by zero. That also
  // means we can compute modulo by subtracting one and masking with AND:
  //                        extra = addr & (align - 1)
  //                         // TODO: Proof Required.
  // However, we want the number of bytes to advance to the next alignment,
  // which is the inverse: padding = -addr & (align - 1).
  ptrdiff_t padding = -(uintptr_t)(&a->data[a->count]) & (type_alignment - 1);

  // Check if we have space
  assert(a->count + padding + size_of_type * count <= a->capacity);

  void* res = &a->data[a->count + padding]; // Give user the aligned ptr

  a->count += padding + size_of_type * count; // Bump the count appropriately

  return res;
}

void* arena_zalloc_with_custom_alignment(Arena a, size_t size_of_type,
                                         size_t type_alignment, size_t count)
{
  void* res =
    arena_alloc_with_custom_alignment(a, size_of_type, type_alignment, count);
  memset(res, 0, count * size_of_type);
  return res;
}

void arena_reset(Arena a)
{
  a->count = 0;
}

void arena_free(Arena a)
{
  size_t size_bytes = sizeof(struct Region) + sizeof(uint8_t) * a->capacity;
  int ret = munmap(a, size_bytes);
  assert(ret == 0);
}

size_t arena_get_capacity(Arena a)
{
  return a->capacity;
}

size_t arena_get_count(Arena a)
{
  return a->count;
}

uintptr_t arena_get_head_ptr(Arena a)
{
  return (uintptr_t)&a->data[a->count];
}

void arena_info_print(Arena a)
{
  printf("Count: %zu\n", a->count);
  printf("Capacity: %zu\n", a->capacity);
}

void arena_mem_print(Arena a)
{
  printf("Mem Array:\n");
  for (size_t i = 0; i < a->capacity; ++i) {
    #pragma clang diagnostic ignored "-Wformat"
    printf("  %12lu:  %3hhu\n", &a->data[i], a->data[i]);

    // Break every 4 bytes
    if (((i + 1) % 4) == 0) {
      printf("\n");
    }
  }
}

void arena_debug_print(Arena a)
{
  arena_info_print(a);
  arena_mem_print(a);
}

// Testing Main Function
/*
#include <limits.h>
int main(void)
{
  Arena a = arena_init_cap(32);

  char* A = arena_alloc(a, char, 1);
  *A = 'a';
  char* B = arena_alloc(a, char, 1);
  *B = 'b';
  uintptr_t* Int1 = arena_alloc(a, uintptr_t, 1);
  *Int1 = 12345678910;
  int* int2 = arena_alloc(a, int, 1);
  *int2 = 12345678;

  arena_debug_print(a);

  printf("%lu %lu %lu %lu\n", (uintptr_t)A % _Alignof(char),
         (uintptr_t)B % _Alignof(char), (uintptr_t)Int1 % _Alignof(uintptr_t),
         (uintptr_t)int2 % _Alignof(int));

  printf("%lu %lu\n", &a->data[0], &a->data[1]);
  printf("%lu %lu\n", _Alignof(int), _Alignof(uintptr_t));
  printf("%lu %lu\n", (uintptr_t)&a->data[0] % _Alignof(uintptr_t),
         (uintptr_t)&a->data[1] % _Alignof(uintptr_t));
  printf("%lu\n", sizeof(uint8_t));

  arena_free(a);
}
*/
