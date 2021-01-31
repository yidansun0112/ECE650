#include <stddef.h>

typedef struct __Node{
  struct Node * next;
  int datasize;
}Node;

Node head;

void *ff_malloc(size_t size);
void ff_free(void *ptr);

void *bf_malloc(size_t size);
void bf_free(void *ptr); 


