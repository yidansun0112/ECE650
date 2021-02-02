#include "my_malloc.h"

Node * head=NULL;

void *ff_malloc(size_t size){
  Node ** temp=&head;
  while(*temp!=NULL&&(*temp)->next!=NULL){
    if((*temp)->datasize>=size){
      //delete temp and return temp
      void *ptr=(void*)(*temp)+sizeof(Node);
      if((*temp)->datasize<=sizeof(Node)+size){
        Node * del=*temp;
        *temp=(*temp)->next;
        del->next=NULL;
      }
      else{
        void *new_node=(void*)(*temp)+sizeof(Node)+size;
        size_t new_datasize=(*temp)->datasize-sizeof(Node)-size;
        Node * next=(*temp)->next;
        *temp=new_node;
        (*temp)->datasize=new_datasize;
        (*temp)->next=next;
      }
      return ptr;
    }
    temp=&(*temp)->next;
  }
  size_t allocated_size=sizeof(Node)+size; 
  Node * ptr=sbrk(allocated_size);
  ptr->next=NULL;
  ptr->datasize=size;
  return ptr+1;
}

void ff_free(void *ptr){
  //add ptr into linkedlist
  Node * new_node=ptr-sizeof(Node);
  if(head==NULL){
    head=new_node;
    return;
  }
  if(new_node<head){
    new_node->next=head;
    head=new_node;
    return;
  }
  Node ** temp=&head;
  while((*temp)->next!=NULL){
    if(new_node==(void*)(*temp)+sizeof(Node)+sizeof((*temp)->datasize)){
      (*temp)->datasize+=new_node->datasize+sizeof(Node);
      if((void*)(*temp)+sizeof(Node)+sizeof((*temp)->datasize)==(*temp)->next){
        (*temp)->next=(*temp)->next->next;
      }
      return;
    }
    if((void*)new_node+sizeof(Node)+sizeof(new_node->datasize)==(*temp)->next){
      new_node->datasize+=sizeof(Node)+(*temp)->next->datasize;
      new_node->next=(*temp)->next->next;
      (*temp)->next=new_node;
      return;
    }
    if((void*)new_node+sizeof(Node)+sizeof(new_node->datasize)<(void*)(*temp)->next){
      new_node->next=(*temp)->next;
      (*temp)->next=new_node;
      return;
    }
    temp=&(*temp)->next;
  }
  (*temp)->next=new_node;
  new_node->next=NULL;
}
                                 
void *bf_malloc(size_t size){

  return NULL;
}

void bf_free(void *ptr){

}
                                 
