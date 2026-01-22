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
    new->next = NULL;
    if (PQ_get_head() == NULL){
        head = new;
    } else {
        Node_ptr_t current = head;
        Node_ptr_t prev = NULL;
        while (current->next != NULL){
            prev = current;
            current = current->next;

            if (current->priority < new->priority){
                break;
            }
        }
        
        new -> next = current -> next;
        prev -> next = new;
    }
    return;
}
/**
 * Delete and return the node with the highest priority.
 * @return The highest priority Node.
 */
Node_ptr_t PQ_delete() {
    Node_ptr_t highest = head;
    
    Node_ptr_t current = head;
    while (current->next != NULL){
        current = current->next;
        if (current->priority > highest->priority){
            highest = current;
        }
    }

    current = head;
    Node_ptr_t previous = NULL;
    while (current != highest){
        previous = current;
        current = current -> next;
    }

    if (previous != NULL){
        previous -> next = current -> next;    
    }
    else {
        head = current -> next;
    }
    return highest;
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


