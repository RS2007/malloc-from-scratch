#include "mymem.h"
#include <stdio.h>
#include <sys/mman.h>

extern heap_t *global_heap_var;

void _search_ptr(void *ptr, heap_t **res_heap, heap_block_t **res_block) {
  heap_t *runner = global_heap_var;
  while (runner) {
    heap_block_t *block = HEAP_SHIFT(runner);
    while (block) {
      if (BLOCK_SHIFT(block) == ptr) {
        *res_block = block;
        *res_heap = runner;
        return;
      }
      block = block->next;
    }
    runner = runner->next;
  }
}

heap_block_t *_merge_block_right(heap_t *heap, heap_block_t *block) {
  if (heap && block && block->next && block->next->freed) {
    block->data_size += block->next->data_size + sizeof(heap_block_t);
    if (block->next->next)
      block->next->next->prev = block;
    block->next = block->next->next;
    heap->block_count -= 1;
  }
  return block;
}
heap_block_t *_merge_block_left(heap_t *heap, heap_block_t *block) {
  heap_block_t *res_block;
  if (heap && block && block->prev && block->prev->freed) {
    block->prev->data_size += block->data_size + sizeof(heap_block_t);
    block->prev->next = block->next;
    if (block->next)
      block->next->prev = block->prev;
    heap->block_count--;
    return block->prev;
  }
  return block;
}

heap_block_t *_merge_blocks(heap_t *heap, heap_block_t *block) {
  // check if the block adjacent to the block(both left and right) are free, if
  // free merge them reorder the doubly linked list accordingly, reduce the
  // block count by 1
  _merge_block_right(heap, block);
  return _merge_block_left(heap, block);
}

void _remove_block_if_last(heap_block_t *block, heap_t *heap) {
  if (block->freed && block->next == NULL) {
    if (block->prev)
      block->prev->next = NULL;
    heap->free_size += block->data_size + sizeof(heap_block_t);
    heap->block_count--;
    printf("Last block\n");
  }
}

bool _last_remaining_heap(heap_t *heap) {
  heap_t *runner = global_heap_var;
  size_t count = 0;
  heap_group_t group = heap->group;
  while (runner) {
    if (runner->group == group)
      count++;
    runner = runner->next;
  }
  return (count == 1);
}

void _delete_heap_if_empty(heap_t *heap) {
  if (heap->block_count == 0) {
    if (heap->prev)
      heap->prev->next = heap->next;
    if (heap->next)
      heap->next->prev = heap->prev;
    if (!_last_remaining_heap(heap)) {
      if (heap == global_heap_var)
        global_heap_var = heap->next;
      munmap(heap, heap->total_size);
      printf("Last remaining heap freed\n");
    }
  }
}

void my_free(void *ptr) {
  heap_t *heap;
  heap_block_t *block;
  if (!ptr || !global_heap_var)
    return;

  _search_ptr(ptr, &heap, &block);
  if (block && heap) {
    block->freed = true;
    // adj blocks are merged
    heap_block_t *ret_merge = _merge_blocks(heap, block);
    printf("Merged a block\n");
    _remove_block_if_last(block, heap);
    /* Suppose this was the last block in the heap, then once this
     * block is freed the entire heap should be
     * freed(What if there is a previous block? In that case freeing
     * might cause a performance hit since the create block function
     * has to run again in case subsequent mem allocations happen) */

    // TODO: have an option to have implicit free list(ref:
    // https://my.eng.utah.edu/~cs4400/malloc-2.pdf)

    _delete_heap_if_empty(heap);
  }
}
