#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pzip.h"

pthread_mutex_t l;
pthread_barrier_t b;

int *g_char_frequency;
struct zipped_char *g_zipped_chars;
int *g_zipped_chars_count;

typedef struct{
	int start;
	int end;
	int id;
	int sz;

	char *input_chars;
	int *offsets;
	pthread_t *threads;

} arg_st;

void *zip_chars(void *args);
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
void *zip_chars(void *args_){
	//initialize necessary local variables
	arg_st *args = (arg_st *)args_;
	int len = args->sz;
	int zc_count = 0;
	int offset = 0;
	struct zipped_char *z_chars = malloc(sizeof(struct zipped_char) * len);

	if( z_chars == NULL ){
		fprintf(stderr, "malloc failed");
		exit(1);
	}

	//take care of first values
	z_chars[zc_count].character = args->input_chars[args->start];
	z_chars[zc_count].occurence = 1;

	//loop through the threads section of the input
	for(int i = args->start+1; i < args->end; i++){
		if(args->input_chars[i] == args->input_chars[i-1]){
			z_chars[zc_count].occurence++;
		}
		else{
			zc_count++;
			z_chars[zc_count].character = args->input_chars[i];
			z_chars[zc_count].occurence = 1;
		}
	}

	zc_count++;

	//set offset and shared globals
	pthread_mutex_lock(&l);

	args->offsets[args->id] = zc_count;
	for(int i = 0; i < zc_count; i++){
		g_char_frequency[z_chars[i].character - 97] += z_chars[i].occurence;
	}
	*g_zipped_chars_count += zc_count;
	
	pthread_mutex_unlock(&l);
	pthread_barrier_wait(&b);

	//use offsets to set this thread's section
	for(int i = 0; i < args->id; i++){
		offset += args->offsets[i];
	}
	for(int i = 0; i < zc_count; i++){
		g_zipped_chars[offset+i] = z_chars[i];
	}

	free(z_chars);
	return NULL;
}

void pzip(int n_threads, char *input_chars, int input_chars_size,
	  struct zipped_char *zipped_chars, int *zipped_chars_count,
	  int *char_frequency)
{
	//initialize barrier and mutex
	if(pthread_mutex_init(&l, 0) != 0){
		fprintf(stderr, "mutex error");
		exit(1);
	}
	if(pthread_barrier_init(&b, NULL, n_threads) != 0){
		fprintf(stderr, "barrier error");
		exit(1);
	}
	//create important argument variables
	int offsets[n_threads];
	arg_st a[n_threads];
	pthread_t threads[n_threads];
	int sz = input_chars_size/n_threads;

	//set global variables to necessary pointers
	g_char_frequency = char_frequency;
	g_zipped_chars = zipped_chars;
	g_zipped_chars_count = zipped_chars_count;

	//loop to set each argument and start threads
	for(int i = 0; i < n_threads; i++){
		a[i].id = i;
		a[i].input_chars = input_chars;
		a[i].threads = threads;
		a[i].offsets = offsets;
		a[i].sz = sz;
		a[i].start = i * sz;
		a[i].end = ((i+1) * sz);
		if( pthread_create(&threads[i], NULL, zip_chars, (void *)&a[i]) != 0){
			fprintf(stderr, "pthread_create error");
			exit(1);
		}
	}

	//wait for the threads and destroy the barrier
	for(int i = 0; i < n_threads; i++){
		pthread_join(threads[i], NULL);
	}

	if( pthread_barrier_destroy(&b) != 0){
		fprintf(stderr, "barrier error");
		exit(1);
	}
}