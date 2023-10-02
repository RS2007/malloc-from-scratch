# Malloc

- If `mmap` exists then why do you need `malloc`?
  - Systemcalls are expensive, malloc acts as a performance wrapper around mmap
  - fetch more memory and use the unused memory for the next call

## Implementation

- memory zone called the heap
  - heap is filled with blocks
  - Both heaps and blocks have metadata at their start

- While doing malloc
  - search for an empty block in a heap which satisfies the size constraint
    - once in the heap, check if the block is free and the `block_header_size` + size of the allocated memory is lesser than the data size
    - once block is found, divide the block(the latter part should be created into a block and should be initialised and then inserted as a node in the doubly linked list between `block` and `block->next`)

- If no such heap is available,a new one has to be `mmaped` (Two cases that would cause this are: the heap with the current group does not exists or all heaps with that specific group are completely full)
  - Once heap is created, append an empty block to it and return the block as the return of malloc    
- `mmap` fetches a page(heap) and `malloc` fetches a block.
