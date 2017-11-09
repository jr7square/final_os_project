// linked_queue.h

#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

/*
 * newNode: Creates a new node 
 *
 */
 struct QNode* newNode(char c);
 
 /*
  * createQueue: create a new queue
  */
  struct Queue *createQueue();
  
/*
 * enQueue: adds a node to the linked list
 */
void enQueue(struct Queue *q, char k);

/*
 * deQueue: removes the first node from the linked list
 */
 struct QNode *deQueue(struct Queue *q);

#endif

