#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#define ERROR_MALLOC NULL
#define ERROR_MALLOC_INT -1
#define ERROR_NULL_DAT -1
#define ERROR_NOTFOUND -1
#define SUCCESS 0;

//queue implemented as linked list
typedef struct Qnode{
   void *stored_data;
   struct  Qnode *next_node;
}Qnode;

struct queue {
  Qnode *head;
  Qnode *tail;
  int Qsize;
};

queue_t queue_create(void)  //allocates memory for new queue
{
  queue_t new_queue;
  //returns address if malloc success
  if((new_queue = malloc(sizeof(queue_t))) != NULL){
     new_queue->head = NULL;
     new_queue->tail = NULL;
     new_queue->Qsize = 0;
     return new_queue;
  }
  return (void*)ERROR_MALLOC;
}

int queue_destroy(queue_t queue) //deletes empty queue
{
   if(queue->head != NULL) return ERROR_NULL_DAT;
   if(queue == NULL)return ERROR_NULL_DAT;

   free(queue);
   return SUCCESS;
}

int queue_enqueue(queue_t queue, void *data) //puts new node at tail end of queue
{
   if(queue == NULL || data == NULL) return ERROR_NULL_DAT; 
   
   //makes new node and allocs mem
   Qnode *insert_node;     
   if( (insert_node = malloc(sizeof(Qnode))) == NULL) return ERROR_MALLOC_INT;
   if( (insert_node ->stored_data = malloc(sizeof(data))) == NULL) return ERROR_MALLOC_INT;

   insert_node -> stored_data = data;
   insert_node -> next_node = NULL; //next always NULL since end of queue
  
   //input node at tail and update tail 
   //update head if empty
   if(queue->tail != NULL) queue->tail->next_node = insert_node;
   queue->tail = insert_node;
   queue->Qsize +=1;
   if(queue -> head == NULL) queue->head = insert_node;

   return SUCCESS;

}

int queue_dequeue(queue_t queue, void **data) //sets data as first stored data
{
   if(queue == NULL || data == NULL) return ERROR_NULL_DAT;
   if(queue->head == NULL) return ERROR_NULL_DAT;

   Qnode *cleaner_node = queue->head; 
   *data = cleaner_node->stored_data;
   queue->head = cleaner_node->next_node;
   free(cleaner_node);  //clean used node
   queue->Qsize -= 1;


   if(queue->head == NULL) queue->tail = NULL;

   return SUCCESS;

}

//deletes node with specified data
int queue_delete(queue_t queue, void *data)   
{
   if(queue == NULL || data == NULL) return ERROR_NULL_DAT;

   Qnode *iter_node = queue->head;
   Qnode *prev_node = iter_node;
//searches queue for parameter data
   while(iter_node != NULL ){
      //clears found node and updates links
      if(iter_node->stored_data == data){ 
	 if(iter_node == queue->head) queue-> head = iter_node->next_node;
	 if(iter_node == queue->tail) queue-> tail = prev_node;
	 prev_node->next_node = iter_node->next_node;
         free(iter_node);
	 queue->Qsize -=1;
	 return SUCCESS;
      }
      prev_node = iter_node;
      iter_node = iter_node->next_node;
  }

   return ERROR_NOTFOUND;
}

//runs func on all nodes in queue
int queue_iterate(queue_t queue, queue_func_t func)
{
   if(queue == NULL || func == NULL) return ERROR_NULL_DAT;

   Qnode *iter_node = queue->head;
   Qnode *save_node;
   while(iter_node != NULL ){
     save_node = iter_node->next_node;
     func(queue, iter_node->stored_data);
     iter_node = save_node;
   }

   return SUCCESS;
}

//returns queues length
int queue_length(queue_t queue)
{
   return queue->Qsize;
}



