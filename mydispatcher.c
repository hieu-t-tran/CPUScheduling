/**
 * A program that illustrates two CPU scheduling policies, Shortest Process Next (SPN) and Shortest Remaining Time (SRT).
 *
 * - Author: Hieu Tran
 * - Course: COMP 354: Operating Systems
 * - Instructor: Shikha Mittal
 * - Date created: 05/15/2020
 * - Last updated: 05/28/2020
 */

#include <string.h>
#include "mydispatcher.h"

/*
 * This function is called when the program is run with the wrong arguments. It prints out an error statement and the
 * usage instructions.
 */
void print_error_and_instructions() {
	printf("ERROR: Invalid arguments!\n");
	printf("There should be 2 arguments:\n");
	printf("- \"input.dat\"\n");
	printf("- Either \"SPN\" or \"SRT\"\n");
}

/*
 * The main function for the program.
 */
int main(int argc, char* argv[]) {
	if (argc != 3 || strcmp(argv[1], "input.dat") != 0 || (strcmp(argv[2], "SPN") != 0 && strcmp(argv[2], "SRT") != 0)) {
		/* If the program is run with the wrong arguments, print out the instructions and exit. */
		print_error_and_instructions();
		exit(1);
	} else {
		FILE* input_file = fopen(argv[1], "r");			// open the file "input.dat" for reading
		FILE* output_file = fopen("output.dat", "w");	// open/create the file "output.dat" for writing
		if (input_file == NULL) {
			/* If a problem occurs with opening "input.dat", print out an error statement and exit. */
			printf("ERROR: Cannot open file \"%s\"!\n", argv[1]);
			exit(1);
		} else if (output_file == NULL) {
			/* If a problem occurs with opening/creating "output.dat", print out an error statement and exit. */
			printf("ERROR: Cannot open/create file \"output.dat\"\n");
			exit(1);
		} else {
			/* Get a list of processes from the file "input.dat". */
			struct queue* process_list = read_input(input_file);

			/* Close the file "input.dat" after reading and deallocate unneeded memory. */
			fclose(input_file);
			free(input_file);

			if (strcmp(argv[2], "SPN") == 0) {
				/* The SPN scheduling policy */
				spn(process_list, output_file);
			} else if (strcmp(argv[2], "SRT") == 0) {
				/* The SRT scheduling policy */
				srt(process_list, output_file);
			}

			/* Close the file "output.dat" after writing and deallocate unneeded memory. */
			free(process_list);
			fclose(output_file);
			free(output_file);
		}
	}
}
