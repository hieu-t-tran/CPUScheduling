/*
 * Implementation of the SPN scheduling algorithm.
 *
 * - Author: Hieu Tran
 * - Course: COMP 354: Operating Systems
 * - Instructor: Shikha Mittal
 * - Date created: 05/15/2020
 * - Last updated: 05/28/2020
 */

#include "mydispatcher.h"

/*
 * Produce the results of the SPN scheduling policy for a specified list of processes.
 *
 * process_list:	a pointer to the list of processes to be scheduled
 * output_file:		a pointer to the file to write output to
 */
void spn(struct queue* process_list, FILE* output_file) {
	int num_process = process_list->size;						  	// hold the total number of processes

	/* Continue only if the list of processes is not empty. */
	if (process_list->head != NULL) {
		/* Hold the total turnaround time, normalized turnaround time and waiting time of all processes to
		 * calculate the averages after all processes have been scheduled. */
		double sum_turnaround = 0;
		double sum_normalized_turnaround = 0;
		double sum_waiting = 0;

		/* Create a queue for processes in the Ready state. Initialize the current time to be the arrival time
		 * of the first process in the list, which should be the earliest arriving process. Create a pointer
		 * for a process in the Running state. Create a variable to hold the time a process is dispatched. */
		struct queue* ready_queue = create_queue();
		int current_time = process_list->head->data->arrival_time;
		struct process* running_process = NULL;
		int start_time;

		while (1) {
			if (running_process == NULL) {
				/* If there is no running process: */

				if (process_list->head != NULL && process_list->head->data->arrival_time <= current_time) {
					/* First, add all processes with arrival time less than or equal to the current time to
					 * the ready queue and remove such processes from the list. Technically, a process with
					 * arrival time less than the current time should have already arrived. However, since SPN
					 * is non-preemptive, it makes no difference to the results of this scheduling algorithm.
					 * Additionally, since remaining time is never updated throughout this algorithm, it is
					 * always equal to service time and can be used for sorting instead of service time. */
					enqueue_with_sorting(ready_queue, dequeue(process_list));

				} else if (ready_queue->head != NULL) {
					/* If there is no process to be added and the ready queue is not empty, dispatch the first
					 * (frontmost) process in the ready queue. Store the current time as the time this process
					 * is dispatched. */
					running_process = dequeue(ready_queue);
					start_time = current_time;

				} else if (process_list->head != NULL) {
					/* If the ready queue is empty but there still exists some process in the list (that is
					 * not yet scheduled), set the current time to the arrival time of the first process in
					 * the list, since nothing can happen until that time. */
					current_time = process_list->head->data->arrival_time;

				} else {
					/* If the list is also empty, then all processes have been scheduled. */
					break;
				}
			} else {
				/* If there is a running process, skip to the time it completes its execution. Technically,
				 * other processes can arrive while this process is running, but since SPN is non-preemptive,
				 * adding them to the ready queue after the process finishes makes no difference to the results
				 * of this scheduling algorithm as long as the ready queue remains sorted at all times. */
				current_time += running_process->service_time;

				/* Calculate the turnaround time and waiting time of the completed process, then write the
				 * needed information to the output file. */
				int turnaround_time = current_time - running_process->arrival_time;
				int waiting_time = turnaround_time - running_process->service_time;
				fprintf(output_file, "%d runs %d-%d: A=%d, S=%d, W=%d, F=%d, T=%d\n", running_process->id, start_time, current_time, running_process->arrival_time, running_process->service_time, waiting_time, current_time, turnaround_time);

				/* Add the turnaround time, normalized turnaround time and waiting time to the total to
				 * calculate the averages later. */
				sum_turnaround += turnaround_time;
				sum_normalized_turnaround += ((double) turnaround_time) / running_process->service_time;
				sum_waiting += waiting_time;

				/* Deallocate unneeded memory and set the running process to NULL. */
				free(running_process);
				running_process = NULL;
			}
		}

		/* Calculate and print out the average turnaround time, average normalized turnaround time and average
		 * waiting time across all processes. */
		printf("Average turnaround time = %f\n", sum_turnaround/num_process);
		printf("Average normalized turnaround time = %f\n", sum_normalized_turnaround/num_process);
		printf("Average waiting time = %f\n", sum_waiting/num_process);

		/* Deallocate unneeded memory. */
		free(ready_queue);
	}
}
