#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

void zip_chars(int start, int end, char *input_chars, int *char_frequency, struct zipped_char *z_chars, int *zipped_chars_count, int *offsets);

/**
 * pzip() - zip an array of characters in parallel
 *
 * Inputs:
 * @n_threads:		   The number of threads to use in pzip
 * @input_chars:		   The input characters (a-z) to be zipped
 * @input_chars_size:	   The number of characaters in the input file
 *
 * Outputs:
 * @zipped_chars:       The array of zipped_char structs
 * @zipped_chars_count:   The total count of inserted elements into the zippedChars array.
 * @char_frequency[26]: Total number of occurences
 *
 * NOTE: All outputs are already allocated. DO NOT MALLOC or REASSIGN THEM !!!
 *
 */
void zip_chars(int start, int end, char *input_chars, int *char_frequency, struct zipped_char *zipped_chars, int *zipped_chars_count, int *offsets){
	int len = end - start;
	int zc_count = 0;
	struct zipped_char *z_chars;
	//lock
	char_frequency[input_chars[start]-97]++;
	struct zipped_char *temp = malloc(len);
	z_chars = temp;
	//unlock
	z_chars[start].character = input_chars[start];
	z_chars[start].occurence = 1;

	for(int i = start+1; i < end; i++){
		if(input_chars[i] == input_chars[i-1]){
			z_chars[zc_count].occurence++;
		}
		else{
			zc_count++;
			z_chars[zc_count].character = input_chars[i];
			z_chars[zc_count].occurence = 1;
		}
		//lock
		char_frequency[input_chars[i]-97]++;
		//unlock
	}
	printf("finished");
	//z_chars = realloc(z_chars, zc_count+1);
	
}

void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	int offsets[n_threads];
	zip_chars(0, input_chars_size, input_chars, char_frequency, zipped_chars, zipped_chars_count, offsets);

	

	/*
	int zc_count = 0;
	zipped_chars[zc_count].character = input_chars[0];
	zipped_chars[zc_count].occurence = 1;
	//lock
	char_frequency[input_chars[0]-97]++;
	//unlock

	for(int i = 1; i < input_chars_size; i++){
		if(input_chars[i] == input_chars[i-1]){
			zipped_chars[zc_count].occurence++;
		}
		else{
			zc_count++;
			zipped_chars[zc_count].character = input_chars[i];
			zipped_chars[zc_count].occurence = 1;
		}
		//lock
		char_frequency[input_chars[i]-97]++;
		//unlock
	}
	*/
	
}
