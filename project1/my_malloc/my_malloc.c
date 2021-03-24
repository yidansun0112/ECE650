#include "my_malloc.h"

Node * head=NULL;
unsigned long data_segment_size=0;
unsigned long data_segment_free_size=0;

unsigned long get_data_segment_size(){
  return data_segment_size;
}

unsigned long get_data_segment_free_space_size(){
  return data_segment_free_size;
}


Node *call_sbrk(size_t size){
    size_t allocated_size=sizeof(Node)+size;
    Node * ptr=sbrk(allocated_size);       
    ptr->next=NULL;                        
    ptr->datasize=size;
    data_segment_size=data_segment_size+size+sizeof(Node);
    return ptr;
}


void edit_lls(Node **temp,size_t size){
  if((*temp)->datasize<=sizeof(Node)+size){                 
    Node * del=*temp;                                       
    *temp=(*temp)->next;                                    
    del->next=NULL;                                         
    data_segment_free_size=data_segment_free_size-sizeof(Node)-del->datasize;
  }                                                         
  else{
    Node *del=*temp;
    void *new_node=(void*)(*temp)+sizeof(Node)+size;        
    size_t new_datasize=(*temp)->datasize-sizeof(Node)-size;
    Node * next=(*temp)->next;                              
    *temp=(Node*)new_node;                                         
    (*temp)->datasize=new_datasize;                         
    (*temp)->next=next;                                     
  data_segment_free_size=data_segment_free_size-sizeof(Node)-size;
  del->datasize=size;
  }
}

void *ff_malloc(size_t size){
   Node ** temp=&head;
  while(*temp!=NULL){
    if((*temp)->datasize>=size){
      void *ptr=(void*)(*temp)+sizeof(Node);
      edit_lls(temp,size);
      return ptr;
    }
    if((*temp)->next==NULL){
      break;
    }
    temp=&(*temp)->next;
  } 
  Node * ptr=call_sbrk(size);
  return ptr+1;
}




                                 
void *bf_malloc(size_t size){
  if(head==NULL){
   Node *ptr= call_sbrk(size);
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
    return ptr;
  }
  Node * ptr=call_sbrk(size);                 
  return ptr+1;                             
}

void bf_free(void *ptr){
  void * p=ptr-sizeof(Node);
  Node * n=(Node*)p;
  void * end=ptr+n->datasize;
  data_segment_free_size=data_segment_free_size+sizeof(Node)+n->datasize;
  if(head==NULL||end<(void*)head){
    n->next=head;
    head=n;
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
      return;
    }
    temp=temp->next;
  }
    temp->next=n;
    n->next=NULL;
 }

void ff_free(void *ptr){                                         
  bf_free(ptr);                 
}                               
