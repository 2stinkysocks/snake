#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

typedef struct node {
   void* val;
   struct node *next;
} node_t;

void enqueue(node_t **head, void* val);

void* dequeue(node_t **head);

void* peekFront(node_t **head);

void* peekBack(node_t **head);


#endif
