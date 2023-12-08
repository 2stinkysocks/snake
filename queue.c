#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void enqueue(node_t **head, void* val) {
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return;

    new_node->val = val;
    new_node->next = *head;

    *head = new_node;
}

void* peekFront(node_t **head) {
    node_t *current, *prev = NULL;
    void* retval = NULL;

    if (*head == NULL) return NULL;

    current = *head;
    while (current->next != NULL) {
      prev = current;
      current = current->next;
    }

    retval = current->val;

    return retval;
}

void* peekBack(node_t **head) {
   return (*head)->val;
}

void* dequeue(node_t **head) {
   node_t *current, *prev = NULL;
   void* retval = NULL;

   if (*head == NULL) return NULL;

   current = *head;
   while (current->next != NULL) {
      prev = current;
      current = current->next;
   }

   retval = current->val;
   free(current);

   if (prev)
      prev->next = NULL;
   else
      *head = NULL;

   return retval;
}