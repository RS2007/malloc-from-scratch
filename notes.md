# Malloc

- If `mmap` exists then why do you need `malloc`?
  - Systemcalls are expensive, malloc acts as a performance wrapper around mmap
  - fetch more memory and use the unused memory for the next call

## Implementation

### Malloc

- memory zone called the heap

  - heap is filled with blocks
  - Both heaps and blocks have metadata at their start

- While doing malloc

  - search for an empty block in a heap which satisfies the size constraint
    - once in the heap, check if the block is free and the `block_header_size` + size of the allocated memory is lesser than the data size
    - once block is found, divide the block(the latter part should be created into a block and should be initialised and then inserted as a node in the doubly linked list between `block` and `block->next`)

- If no such heap is available,a new one has to be `mmaped` (Two cases that would cause this are: the heap with the current group does not exists or all heaps with that specific group are completely full)
  - Once heap is created, append an empty block to it and return the pointer to data corresponding to that block as the return of malloc
- `mmap` fetches a page(heap) and `malloc` fetches a block.

### Free
- Once free is called, start from the global heap pointer
- Start iterating through each block in each heap(we don't have size information so have to search through all the different heaps)
- Search: 
  1. move the pointer from heap to the first block(by skipping the metadata)
  2. skip the blocks metadata and check if the pointer value is equal to the current pointer
- If they are the same, set the block as free and merge it to the nearby blocks(if those blocks are already freed)
- If the block is the last in the heap, then remove the block(one reason for doing this is if a heap has only one block left, we wanna free everything and then free the empty heap)
- Check if the heap is empty and then free the heap(make sure to set the global heap pointer to the next heap if one exists)
> Make sure the linked list connections are adjusted while merging and dividing blocks


