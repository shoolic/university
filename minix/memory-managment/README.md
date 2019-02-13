# Task
Implement in the Minix operating system the Worst Fit memory allocation algorithm with the option of choosing between it and the standard First Fit algorithm.

Implement two additional system calls:
- **HOLE_MAP** with function signature _int hole_map( void *buffer, size_t nbytes )_
returning in the _buffer_ information about free memory blocks (pattern:
_size1, address1, size2, address2, ..., 0_)
- **WORST_FIT** with function signature *int worst_fit( int w )*
 choosing the allocation algorithm depending on the *w* value (1 for worst, 0 for first fit)