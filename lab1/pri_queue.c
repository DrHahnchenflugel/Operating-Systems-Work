#include <stdlib.h>
#include <stdio.h>
#include "pri_queue.h"
/** @file pri_queue.c */
static Node_ptr_t head = NULL;
/**
 * Insert a Node into a priority queue.
 * @param priority
 * @param data
 * @author Victor Lelikov
 */
void PQ_insert(int priority, char * data) {
    Node_ptr_t new = (Node_ptr_t) malloc(sizeof(Node_t));
    new->priority = priority;
    new->data = data;
    
    if (PQ_get_size() == 0){
        
        head = new;
        new->next = NULL;
        
    } else {
        
        Node_ptr_t current = head;
        Node_ptr_t prev = NULL;
        
        while (current != NULL){
            if (current->priority > new->priority){
                prev = current;
                current = current->next;                
            }
            else {
                new -> next = current;
                if (prev != NULL) prev -> next = new;
                break;
            }
        }
        
        if (current == NULL) current -> next = new;
        
        if (current == PQ_get_head()){
            head = new;
        }
    }
    return;
}
/**
 * Delete and return the node with the highest priority.
 * @return The highest priority Node.
 */
Node_ptr_t PQ_delete() {
    if (head != NULL) head = head -> next;
}

/**
 * Do NOT modify this function.
 * @return A pointer to the head of the list.  (NULL if list is empty.)
 */
Node_ptr_t PQ_get_head() {
    return head;
}

/**
 * Do NOT modify this function.
 * @return the number of items in the queue
 */
int PQ_get_size() {
    int size = 0;
    Node_ptr_t tmp;
    for(tmp = head; tmp != NULL; tmp = tmp->next, size++)
        ;
    return size;
}


