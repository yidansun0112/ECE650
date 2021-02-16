#include "my_malloc.h"
#include <pthread.h>

Node * head=NULL;
__thread Node* head_nolock=NULL;
//unsigned long data_segment_size=0;
//unsigned long data_segment_free_size=0;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;


Node *call_sbrk(size_t size){
  size_t allocated_size=sizeof(Node)+size; 
  Node * ptr=sbrk(allocated_size);
  ptr->next=NULL;                        
  ptr->datasize=size;
  return ptr;
}

Node *call_sbrk_nolock(size_t size){
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
  }                                                         
  else{
    Node *del=*temp;
    void *new_node=(void*)(*temp)+sizeof(Node)+size;        
    size_t new_datasize=(*temp)->datasize-sizeof(Node)-size;
    Node * next=(*temp)->next;
    *temp=(Node*)new_node;                                         
    (*temp)->datasize=new_datasize;                         
    (*temp)->next=next;                                     
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
  pthread_mutex_lock(&lock);
  void * p=ptr-sizeof(Node);
  Node * n=(Node*)p;
  void * end=ptr+n->datasize;
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


void *ts_malloc_nolock(size_t size){
  if(head_nolock==NULL){
   Node *ptr= call_sbrk_nolock(size);
   return ptr+1;
  }
  Node ** temp=&head_nolock;
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
    return ptr;
  }
  Node * ptr=call_sbrk_nolock(size);                 
  return ptr+1;  
}

                               

void ts_free_nolock(void *ptr){
  void * p=ptr-sizeof(Node);
  Node * n=(Node*)p;
  void * end=ptr+n->datasize;
  if(head_nolock==NULL||end<(void*)head_nolock){
    n->next=head_nolock;
    head_nolock=n;
    return;
  }
  Node *temp=head_nolock;
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
      return;
    }
    temp=temp->next;
  }
    temp->next=n;
    n->next=NULL;
}
