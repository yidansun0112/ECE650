#include "my_malloc.h"
#include <pthread.h>

Node * head=NULL;
__thread Node* head_nolock=NULL;
//unsigned long data_segment_size=0;
//unsigned long data_segment_free_size=0;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;


Node *call_sbrk(size_t size){
  size_t allocated_size=sizeof(Node)+size;
  pthread_mutex_lock(&lock);
  Node * ptr=sbrk(allocated_size);
  pthread_mutex_unlock(&lock);
  ptr->next=NULL;                        
  ptr->datasize=size;
  return ptr;
}


void edit_lls(Node **temp,size_t size){
  if((*temp)->datasize<=sizeof(Node)+size){                 
    Node * del=*temp;                                       
    *temp=(*temp)->next;                                    
    del->next=NULL;                                         
    //data_segment_free_size=data_segment_free_size-sizeof(Node)-del->datasize;
  }                                                         
  else{
    Node *del=*temp;
    void *new_node=(void*)(*temp)+sizeof(Node)+size;        
    size_t new_datasize=(*temp)->datasize-sizeof(Node)-size;
    Node * next=(*temp)->next;                              
    *temp=(Node*)new_node;                                         
    (*temp)->datasize=new_datasize;                         
    (*temp)->next=next;                                     
    //data_segment_free_size=data_segment_free_size-sizeof(Node)-size;
    del->datasize=size;
  }
}

void *ts_malloc_lock(size_t size){
  pthread_mutex_lock(&lock);
  if(head==NULL){
   Node *ptr= call_sbrk(size);
   pthread_mutex_unlock(&lock);
   return ptr+1;
  }
  Node ** temp=&head;
  Node ** min=NULL;
  while(*temp!=NULL){  
    if((*temp)->datasize>=size){            
      if(min==NULL){
        min=temp;
      }
      else{
        if((*min)->datasize>(*temp)->datasize){
          min=temp;
        }
      }
      if((*min)->datasize==size){
        break;
      }
    }
    if((*temp)->next==NULL){ 
      break;                 
    }                        
    temp=&(*temp)->next;                    
  }
  
  if(min!=NULL){
    void *ptr=(void*)(*min)+sizeof(Node);   
    edit_lls(min,size);
    pthread_mutex_unlock(&lock);
    return ptr;
  }
  Node * ptr=call_sbrk(size);
  pthread_mutex_unlock(&lock);
  return ptr+1;                             
}

void ts_free_lock(void *ptr){
  void * p=ptr-sizeof(Node);
  Node * n=(Node*)p;
  void * end=ptr+n->datasize;
  pthread_mutex_lock(&lock);
  //data_segment_free_size=data_segment_free_size+sizeof(Node)+n->datasize;
  if(head==NULL||end<(void*)head){
    n->next=head;
    head=n;
    pthread_mutex_unlock(&lock);
    return;
  }
  Node *temp=head;
  while(temp->next!=NULL){
    void * end_of_temp=(void*)temp+sizeof(Node)+temp->datasize;
    void * temp_next=temp->next;
    //check whether in middle
    if(end_of_temp<=p&&p<=temp_next){
      //chech whether merge right
      if(end==temp_next){
        //merge right
        n->next=temp->next->next;
        n->datasize=n->datasize+sizeof(Node)+temp->next->datasize;
        temp->next=n;
        //check whether merge left then
        if(end_of_temp==p){
          temp->datasize=temp->datasize+sizeof(Node)+n->datasize;
          temp->next=n->next;
        }
      }
      else if(end_of_temp==p){
        //merge left
        temp->datasize=temp->datasize+sizeof(Node)+n->datasize;
      }
      else{
        n->next=temp->next;
        temp->next=n;
      }
      pthread_mutex_unlock(&lock);
      return;
    }
    temp=temp->next;
  }
    temp->next=n;
    n->next=NULL;
    pthread_mutex_unlock(&lock);
 }

__thread Node **temp_nolock=NULL;
__thread Node **min_nolock=NULL;
__thread Node* ptr_nolock=NULL;


Node *call_sbrk_nolock(size_t size){
  size_t allocated_size=sizeof(Node)+size;
  pthread_mutex_lock(&lock);
  ptr_nolock=sbrk(allocated_size);
  pthread_mutex_unlock(&lock);
  ptr_nolock->next=NULL;                        
  ptr_nolock->datasize=size;
  return ptr_nolock;
}

__thread Node *del_nolock;
__thread void* new_node_nolock;
__thread Node* next_nolock;

void edit_lls_nolcok(size_t size){
  if((*temp_nolock)->datasize<=sizeof(Node)+size){                 
    del_nolock=*temp_nolock;                                       
    *temp_nolock=(*temp_nolock)->next;                                    
    del_nolock->next=NULL;                                         
    //data_segment_free_size=data_segment_free_size-sizeof(Node)-del->datasize;
  }                                                         
  else{
    del_nolock=*temp_nolock;
    new_node_nolock=(void*)(*temp_nolock)+sizeof(Node)+size;        
    size_t new_datasize=(*temp_nolock)->datasize-sizeof(Node)-size;
    next_nolock=(*temp_nolock)->next;                              
    *temp_nolock=(Node*)new_node_nolock;                                         
    (*temp_nolock)->datasize=new_datasize;                         
    (*temp_nolock)->next=next_nolock;                                     
    //data_segment_free_size=data_segment_free_size-sizeof(Node)-size;
    del_nolock->datasize=size;
  }
}

__thread void* ptr_void=NULL;


void *ts_malloc_nolock(size_t size){
  if(head_nolock==NULL){
    ptr_nolock= call_sbrk(size);
    return ptr_nolock+1;
  }
  temp_nolock=&head;
  min_nolock=NULL;
  while(*temp_nolock!=NULL){  
    if((*temp_nolock)->datasize>=size){            
      if(min_nolock==NULL){
        min_nolock=temp_nolock;
      }
      else{
        if((*min_nolock)->datasize>(*temp_nolock)->datasize){
          min_nolock=temp_nolock;
        }
      }
      if((*min_nolock)->datasize==size){
        break;
      }
    }
    if((*temp_nolock)->next==NULL){ 
      break;                 
    }                        
    temp_nolock=&(*temp_nolock)->next;                    
  }
  
  if(min_nolock!=NULL){
    ptr_void=(void*)(*min_nolock)+sizeof(Node);   
    edit_lls(min_nolock,size);                     
    return ptr_void;
  }
  ptr_nolock=call_sbrk(size);                 
  return ptr_nolock+1;                             
}

__thread void* end_nolock=NULL;
__thread Node* temp_no=NULL;
__thread void * end_of_temp_nolock=NULL;
__thread void* temp_next_nolock=NULL;

void ts_free_nolock(void *ptr){
  ptr_void=ptr-sizeof(Node);
  ptr_nolock=(Node*)ptr_void;
  end_nolock=ptr+ptr_nolock->datasize;
  //data_segment_free_size=data_segment_free_size+sizeof(Node)+n->datasize;
  if(head_nolock==NULL||end_nolock<(void*)head){
    ptr_nolock->next=head;
    head_nolock=ptr_nolock;
    return;
  }
  temp_no=head_nolock;
  while(temp_no->next!=NULL){
    end_of_temp_nolock=(void*)temp_no+sizeof(Node)+temp_no->datasize;
    temp_next_nolock=temp_no->next;
    //check whether in middle
    if(end_of_temp_nolock<=ptr_void&&ptr_void<=temp_next_nolock){
      //chech whether merge right
      if(end_nolock==temp_next_nolock){
        //merge right
        ptr_nolock->next=temp_no->next->next;
        ptr_nolock->datasize=ptr_nolock->datasize+sizeof(Node)+temp_no->next->datasize;
        temp_no->next=ptr_nolock;
        //check whether merge left then
        if(end_of_temp_nolock==ptr_void){
          temp_no->datasize=temp_no->datasize+sizeof(Node)+ptr_nolock->datasize;
          temp_no->next=ptr_nolock->next;
        }
      }
      else if(end_of_temp_nolock==ptr_void){
        //merge left
        temp_no->datasize=temp_no->datasize+sizeof(Node)+ptr_nolock->datasize;
      }
      else{
        ptr_nolock->next=temp_no->next;
        temp_no->next=ptr_nolock;
      }
      return;
    }
    temp_no=temp_no->next;
  }
    temp_no->next=ptr_nolock;
    ptr_nolock->next=NULL;
 }
