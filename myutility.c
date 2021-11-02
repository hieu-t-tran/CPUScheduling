/**
 * Implementation of functions used for the scheduling program, which includes queue operations and other
 * needed utility functions.
 *
 * - Author: Hieu Tran
 * - Course: COMP 354: Operating Systems
 * - Instructor: Shikha Mittal
 * - Date created: 05/15/2020
 * - Last updated: 05/28/2020
 */

#include "mydispatcher.h"

/*
 * Create a new process from a specified ID, arrival time and service time.
 *
 * process_id:		ID of the process
 * process_arrival:	arrival time of the process
 * process_service:	service time of the process
 *
 * Returns:	a pointer to the created process
 */
struct process* create_process(int process_id, int process_arrival, int process_service) {
	struct process* created_process = (struct process*) malloc(sizeof(struct process));
	created_process->id = process_id;
	created_process->arrival_time = process_arrival;
	created_process->service_time = process_service;

	/* Since the process is not run yet, remaining time is initialized to be the same as service time. */
	created_process->remaining_time = process_service;

	return created_process;
}

/*
 * Create a new queue of processes.
 *
 * Returns: a pointer to the created queue
 */
struct queue* create_queue() {
	struct queue* created_queue = (struct queue*) malloc(sizeof(struct queue));

	/* Initialize the head of rear of the queue to NULL. Set the size of the queue to 0. */
	created_queue->head = NULL;
	created_queue->rear = NULL;
	created_queue->size = 0;

	return created_queue;
}

/*
 * Add a specified process to the end of a specified queue.
 *
 * current_queue:	a pointer to the queue to add the process to
 * process_to_add:	a pointer to the process to be added
 */
void enqueue(struct queue* current_queue, struct process* process_to_add) {
	/* Create a node from the process. Initialize both its previous and next nodes to NULL. */
	struct node* node_to_add = (struct node*) malloc(sizeof(struct node));
	node_to_add->data = process_to_add;
	node_to_add->prev = NULL;
	node_to_add->next = NULL;

	if (current_queue->size == 0) {
		/* If the queue is empty, then the node to be added will be both the head and the rear of the
		 * queue. */
		current_queue->head = node_to_add;
		current_queue->rear = node_to_add;
	} else {
		/* If the queue is not empty, then the node to be added will be the next node after the current
		 * rear of the queue. Update all needed pointers and make the rear become the added node. */
		current_queue->rear->next = node_to_add;
		node_to_add->prev = current_queue->rear;
		current_queue->rear = node_to_add;
	}

	/* Increase the size of the queue after adding. */
	current_queue->size++;
}

/*
 * Re-sort a specified queue by the remaining time of processes. This function should be called right after
 * adding a process to the queue, and it takes advantage of the fact that the queue is already sorted before
 * the process is added.
 *
 * current_queue:	a pointer to the queue to be re-sorted
 */
void resort_after_adding(struct queue* current_queue) {
	struct node* added_node = current_queue->rear;			// a pointer to the node just added to the queue
	while (added_node->prev != NULL) {
		if (added_node->prev->data->remaining_time > added_node->data->remaining_time) {
			/* If the remaining time of the process refered to by the previous node is longer than that of
			 * the added process, move the added process towards the front of the queue by swapping the
			 * positions of its node and the previous node. */
			struct node* node_to_swap = added_node->prev;

			/* Update all needed pointers. */
			if (node_to_swap->prev != NULL) {
				node_to_swap->prev->next = added_node;
			} else {
				current_queue->head = added_node;
			}
			if (added_node->next != NULL) {
				added_node->next->prev = node_to_swap;
			} else {
				current_queue->rear = node_to_swap;
			}
			node_to_swap->next = added_node->next;
			added_node->prev = node_to_swap->prev;
			node_to_swap->prev = added_node;
			added_node->next = node_to_swap;
		} else {
			/* If the added process is already at the head of the queue, or the remaining time of the process
			 * refered to by the previous node is not longer than that of the added process, then the queue
			 * is already sorted. */
			break;
		}
	}
}

/*
 * Add a specified process to the end of a specified queue and then re-sort the queue. This function should
 * be called for adding a process if the queue needs to be sorted at all times.
 *
 * current_queue:	a pointer to the queue to add the process to
 * process_to_add:	a pointer to the process to be added
 */
void enqueue_with_sorting(struct queue* current_queue, struct process* process_to_add) {
	enqueue(current_queue, process_to_add);
	resort_after_adding(current_queue);
}

/*
 * Retrive and remove the first (frontmost) process of a queue.
 *
 * current_queue:	a pointer to the queue to remove the frontmost process from
 *
 * Returns: a pointer to the removed process
 */
struct process* dequeue(struct queue* current_queue) {
	if (current_queue->size == 0) {
		/* If the queue is empty, returns NULL. */
		return NULL;
	} else {
		struct process* process_to_return = current_queue->head->data;	// hold the process to be returned
		struct node* node_to_remove = current_queue->head;				// hold the node to be removed

		/* Make the head of the queue become the node after the current head, or NULL if no such node
		 * exists. */
		current_queue->head = node_to_remove->next;
		if (current_queue->head == NULL) {
			/* If the head becomes NULL, make the rear also become NULL since the queue is now empty. */
			current_queue->rear = NULL;
		} else {
			/* If the head is not NULL, change the previous node of the head to NULL. */
			current_queue->head->prev = NULL;
		}

		/* Decrease the size of the queue and deallocate unneeded memory after removing. */
		current_queue->size--;
		free(node_to_remove);
		return process_to_return;
	}
}

/*
 * Create a list (represented by a queue) of processes to be scheduled from the data in a specified input
 * file.
 *
 * input_file:		a pointer to the file to read input from
 *
 * Returns: a pointer to the created list of processes
 */
struct queue* read_input(FILE* input_file) {
	struct queue* process_list = create_queue();		// create a queue to represent the list of processes
	int read_arrival_time;								// hold the arrival time read from the input file
	int read_service_time;								// hold the service time read from the input file
	while (feof(input_file) == 0) {
		fscanf(input_file, "%d", &read_arrival_time);	// read the arrival time
		fscanf(input_file, "%d", &read_service_time);	// read the service time

		/* Create a new process from the arrival time and service time, then add it to the list. Since the
		 * process ID starts from 1, it is always 1 more than the size of the list before adding. */
		struct process* current_process = create_process(process_list->size+1, read_arrival_time, read_service_time);
		enqueue(process_list, current_process);
	}
	return process_list;
}
