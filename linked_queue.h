// linked_queue.h

#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H


// A linked list (LL) node to store a queue entry
struct QNode
{
	char key;
	struct QNode *next;
};

// The queue, front stores the front node of LL and rear stores ths
// last node of LL
struct Queue
{
	struct QNode *front, *rear;
};

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

