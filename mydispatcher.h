/**
 * The header file for the scheduling program, which includes the declaration of needed structures and
 * functions.
 *
 * - Author: Hieu Tran
 * - Course: COMP 354: Operating Systems
 * - Instructor: Shikha Mittal
 * - Date created: 05/15/2020
 * - Last updated: 05/28/2020
 */

#include <stdlib.h>
#include <stdio.h>

/* Representation of a process. */
struct process {
	int id;						// the ID of the process
	int arrival_time;			// the arrival time of the process
	int service_time;			// the service time of the process
	int remaining_time;			// the amount of time the process still has to run
};

/* Create a new process. */
struct process* create_process(int process_id, int process_arrival, int process_service);

/* Representation of a node used for a queue of processes (implemented with a doubly linked list). */
struct node {
	struct process* data;		// the process refered to by this node
	struct node* prev;			// the previous node in the queue
	struct node* next;			// the next node in the queue
};

/* Representation of a queue of processes (implemented with a doubly linked list). */
struct queue {
	struct node* head;			// the head (first node) of the queue
	struct node* rear;			// the rear (last node) of the queue
	int size;					// the current size (number of nodes) of the queue
};

/* Create a new queue. */
struct queue* create_queue();

/* Add a process to the end of a queue. */
void enqueue(struct queue* current_queue, struct process* process_to_add);

/* Re-sort a queue by the remaining time of processes. */
void resort_after_adding(struct queue* current_queue);

/* Add a process to the end of a queue and then re-sort the queue. */
void enqueue_with_sorting(struct queue* current_queue, struct process* process_to_add);

/* Retrive and remove the first (frontmost) process of a queue. */
struct process* dequeue(struct queue* current_queue);

/* Create a list of processes from the data in an input file. */
struct queue* read_input(FILE* input_file);

/* Produce the results of the SPN scheduling policy. */
void spn(struct queue* process_list, FILE* output_file);

/* Produce the results of the SRT scheduling policy. */
void srt(struct queue* process_list, FILE* output_file);
