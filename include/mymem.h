#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum heap_group_t { TINY, SMALL, LARGE } heap_group_t;

typedef struct heap_t {
  struct heap_t *prev;
  struct heap_t *next;
  // doubly linked list
  heap_group_t group;
  size_t total_size;
  size_t free_size;
  size_t block_count;
} heap_t;

typedef struct heap_block_t {
  struct heap_block_t *prev;
  struct heap_block_t *next;
  size_t data_size;
  bool freed;
} heap_block_t;

/*
 * skip from metadata to storage
 */
#define HEAP_SHIFT(start) ((void *)start + sizeof(heap_t))
#define BLOCK_SHIFT(start) ((void *)start + sizeof(heap_block_t))

/*
 * Heap allocation sizes
 */
#define TINY_HEAP_ALLOC_SIZE (4 * getpagesize())
#define TINY_BLOCK_SIZE (TINY_HEAP_ALLOC_SIZE / 128)
#define SMALL_HEAP_ALLOC_SIZE (16 * getpagesize())
#define SMALL_BLOCK_SIZE (SMALL_HEAP_ALLOC_SIZE / 128)
#define LARGE_HEAP_ALLOC_SIZE (64 * getpagesize())
#define LARGE_BLOCK_SIZE (SMALL_HEAP_ALLOC_SIZE / 128)

/*
 * API prototypes
 */

void *malloc(size_t size);

/*
 * private prototypes
 */

heap_block_t *_try_filling_available_block(size_t size);
heap_t *_get_heap_with_block_size(size_t);
void *_append_empty_block(heap_t *, size_t);
