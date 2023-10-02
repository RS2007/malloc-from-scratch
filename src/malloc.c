#include "mymem.h"

void *malloc(size_t size) {
  heap_block_t *block;
  heap_t *heap;
  if (!size)
    return NULL;
  if ((block = _try_filling_available_block(size))) {
    return BLOCK_SHIFT(block);
  }
  if (!(heap = _get_heap_with_block_size(size))) {
    return NULL;
  }
  void *res = _append_empty_block(
      heap, size); // This will get hit only when a new heap is created, we
                   // create a block in the new heap with this function
  return res;
}
