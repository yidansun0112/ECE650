#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

struct __Node{
  struct __Node * next;
  size_t datasize;
};

typedef struct __Node Node;

//Thread Safe malloc/free: locking version
Node *call_sbrk(size_t size);
void edit_lls(Node **temp,size_t size);
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

//Thread Safe malloc/free: non-locking version
Node *call_sbrk_nolock(size_t size);
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);
