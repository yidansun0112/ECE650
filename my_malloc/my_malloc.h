#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

struct __Node{
  struct __Node * next;
  size_t datasize;
};

typedef struct __Node Node;

unsigned long get_data_segment_size(); //in bytes
unsigned long get_data_segment_free_space_size(); //in bytes
Node *call_sbrk(size_t size);
void edit_lls(Node **temp,size_t size);
void *ff_malloc(size_t size);
void ff_free(void *ptr);
void *bf_malloc(size_t size);
void bf_free(void *ptr); 


