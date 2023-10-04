#include "mymem.h"
#include <assert.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/resource.h>

heap_t *global_heap_var = NULL;

heap_group_t _heap_group_from_block_size(size_t size) {
  if (size <= TINY_BLOCK_SIZE) {
    return TINY;
  }
  if (size <= SMALL_BLOCK_SIZE) {
    return SMALL;
  }
  return LARGE;
}

void _find_available_block(size_t size, heap_block_t **res_block,
                           heap_t **res_heap) {
  // takes in size and a double pointer to block and heap(double pointer so as
  // to write to block and heap) PS: top level memory allocations
  heap_t *heap = global_heap_var;
  heap_group_t group = _heap_group_from_block_size(size);
  heap_block_t *block;
  while (heap) {
    block = (heap_block_t *)HEAP_SHIFT(heap);
    while (heap->group == group && block) {
      while (block->freed &&
             (block->data_size) >= size + sizeof(heap_block_t)) {
        *res_block = block;
        *res_heap = heap;
        return;
      }
      block = block->next;
    }
    heap = heap->next;
  }
  *res_heap = NULL;
  *res_block = NULL;
}

void _setup_block(heap_block_t *block, size_t size) {
  // setting up a block at the pointer where block is pointing to, useful when
  // allocating a block of size and creating an empty one right after it
  block->prev = NULL;
  block->next = NULL;
  block->data_size = size;
  block->freed = false;
}

void _divide_block(heap_block_t *block, size_t size, heap_t *heap) {
  heap_block_t *next_block = BLOCK_SHIFT(block) + size;
  _setup_block(next_block, block->next - next_block);
  next_block->freed = true;
  next_block->prev = block;
  next_block->next = block->next;
  heap->block_count++;
  block->next = next_block;
  block->freed = false;
  block->data_size = size;
}

heap_block_t *_try_filling_available_block(size_t size) {
  heap_block_t *block;
  heap_t *heap;
  _find_available_block(size, &block, &heap);
  if (block && heap) {
    _divide_block(block, size, heap);
    return block;
  }
  return NULL;
}

heap_t *_get_available_heap(heap_t *start_heap, heap_group_t group,
                            size_t size) {
  heap_t *heap = start_heap;
  while (heap) {
    if (heap->group == group && heap->free_size >= size) {
      return heap;
    }
    heap = heap->next;
  }
  return NULL;
}

int _get_heap_of_block_size(heap_group_t group, size_t size) {
  switch (group) {
  case TINY:
    return TINY_HEAP_ALLOC_SIZE;
  case SMALL:
    return SMALL_HEAP_ALLOC_SIZE;
  case LARGE:
    return size + sizeof(heap_t) + sizeof(heap_block_t);
  }
  return 0;
}

void _zero_out(void *heap, size_t size) {
  // take size in bytes, typecast to byte array and then zero out memory
  uint8_t *end = (uint8_t *)heap;
  while (size--) {
    uint8_t *curr = (end + size);
    curr = 0;
  }
}

heap_t *_new_heap(heap_group_t group, size_t size) {
  int heap_size = _get_heap_of_block_size(group, size);
  struct rlimit rpl;
  getrlimit(RLIMIT_DATA, &rpl);
  if (heap_size > rpl.rlim_max) {
    return NULL; // safety check before mmaping
  }
  // adding a video here for future
  // reference: https://youtu.be/sFYFuBzu9Ow?feature=shared
  // For more info on arguments check the man page for mmap
  heap_t *heap = mmap(NULL, (size_t)heap_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (heap == MAP_FAILED)
    return NULL;
  _zero_out(heap, sizeof(heap_t));
  heap->group = group;
  heap->total_size = heap_size;
  heap->free_size = heap_size - sizeof(heap_t);
  return (heap);
}

heap_t *_get_heap_with_block_size(size_t size) {
  // Start from the current heap global variable, check for group and size
  // constraints iterate and return the heap that satisfies the same
  heap_group_t group = _heap_group_from_block_size(size);
  heap_t *required_heap = _get_available_heap(
      global_heap_var, group,
      sizeof(heap_block_t) + size); // Here the size to be passed is the size of
                                    // the block(including the header)
  if (required_heap)
    return required_heap;
  else {
    required_heap = _new_heap(
        group, size); // Here we are creating the heap so the size of the block
                      // excluding the header is only required
    required_heap->next = global_heap_var;
    if (global_heap_var) {
      required_heap->next->prev = required_heap;
    }
    global_heap_var = required_heap;
  }
  return required_heap;
}

heap_block_t *_get_last_block(heap_block_t *block) {
  // ALERT: MUTATION
  while (block->next) {
    block = block->next;
  }
  return block;
}

void *_append_empty_block(heap_t *heap, size_t size) {
  heap_block_t *new_block;
  heap_block_t *last_block;
  new_block = HEAP_SHIFT(heap);
  last_block = NULL; // In case of non initialized heaps
  if (heap->block_count) {
    last_block = _get_last_block(new_block);
    new_block =
        (heap_block_t *)(BLOCK_SHIFT(last_block) + last_block->data_size);
  }
  _setup_block(new_block, size);
  if (heap->block_count) {
    last_block->next = new_block;
    new_block->prev = last_block;
  }
  heap->block_count++;
  heap->free_size -= (sizeof(heap_block_t) + new_block->data_size);
  return ((void *)BLOCK_SHIFT(new_block));
}
