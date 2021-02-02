#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

struct __Node{
  struct __Node * next;
  size_t datasize;
};

typedef struct __Node Node;

void *ff_malloc(size_t size);
void ff_free(void *ptr);

void *bf_malloc(size_t size);
void bf_free(void *ptr); 


