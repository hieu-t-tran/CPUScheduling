/*
 * Implementation of the SRT scheduling algorithm.
 *
 * - Author: Hieu Tran
 * - Course: COMP 354: Operating Systems
 * - Instructor: Shikha Mittal
 * - Date created: 05/15/2020
 * - Last updated: 05/28/2020
 */

#include "mydispatcher.h"

/*
 * Produce the results of the SRT scheduling policy for a specified list of processes.
 *
 * process_list:	a pointer to the list of processes to be scheduled
 * output_file:		a pointer to the file to write output to
 */
void srt(struct queue* process_list, FILE* output_file) {
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
			if (process_list->head != NULL && process_list->head->data->arrival_time == current_time) {
				/* First, add all processes with arrival time equal to the current time to the ready queue
				 * and remove such processes from the list. */
				enqueue_with_sorting(ready_queue, dequeue(process_list));

			} else if (running_process == NULL) {
				/* If there is no running process and no process to be added: */

				if (ready_queue->head != NULL) {
					/* If the ready queue is not empty, dispatch the first (frontmost) process in the ready
					 * queue. Store the current time as the time this process is dispatched. */
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
				/* If there is a running process and no process to be added: */

				if (running_process->remaining_time == 0) {
					/* If the remaining time of the running process is 0, it completes its execution at this
					 * time. */

					/* Calculate the turnaround time and waiting time of the completed process, then write
					 * the needed information to the output file. */
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

				} else if (ready_queue->head != NULL && ready_queue->head->data->remaining_time < running_process->remaining_time) {
					/* If the running process is not yet finished and its remaining time is longer than that
					 * of the first (frontmost) process in the ready queue, move the running process back to
					 * the ready queue after writing the needed information to the output file. Then,
					 * dispatch the first process in the ready queue and store the current time as the time
					 * this process is dispatched. */
					fprintf(output_file, "%d runs %d-%d\n", running_process->id, start_time, current_time);
					enqueue_with_sorting(ready_queue, running_process);
					running_process = dequeue(ready_queue);
					start_time = current_time;

				} else if (process_list->head != NULL && process_list->head->data->arrival_time - current_time <= running_process->remaining_time) {
					/* If there is no process in the ready queue with a shorter remaining time than the
					 * running process, check if there is any process arriving between the current time and
					 * the time the running process finishes. If there is, skip to the time that process
					 * arrives, after updating the remaining time of the running process.
					 */
					int next_arrival_time = process_list->head->data->arrival_time;
					running_process->remaining_time -= next_arrival_time - current_time;
					current_time = next_arrival_time;

					/* What happens right after this is that that process arrives and gets added to the ready
					 * queue, then the condition is checked again to see if the running process has to be
					 * moved back to the ready queue. */

				} else {
					/* If there is no process arriving between the current time and the time the running
					 * process finishes, skip to the time the running process finishes and change its
					 * remaining time to 0. */
					current_time += running_process->remaining_time;
					running_process->remaining_time = 0;

					/* What happens right after this is that the running process completes its execution. */
				}
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
