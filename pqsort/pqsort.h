#ifndef PQSORT_H
#define PQSORT_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <stddef.h>

#define CACHE_LINE 64
#define DEBUG_1  0
#define DEBUG_2  0


struct thread_arg
{
    int thread_id;
    int *input;
    int input_size;
    int thread_count;
};
struct sample_size
{
    int *addr;
    int count;
    int current_index;
    char padding[52];
};
/*typedef struct barrier_node {
    pthread_mutex_t count_lock;
    pthread_cond_t ok_to_proceed_up;
    pthread_cond_t ok_to_proceed_down;
    int count;
} mylib_barrier_t_internal;*/

struct barrier_node {
    pthread_mutex_t count_lock;
    pthread_cond_t ok_to_proceed;
    int count;
};

struct partition_info
{
    int** partition_index;
    int* partition_element_count;
    char padding[56];
};

struct prefix_sum_msg
{
    int sum;
    int msg;
    char padding[56];
};
//void mylib_init_barrier(struct barrier_node* b, int MAX_THREADS);

void mylib_init_barrier(struct barrier_node *b);

void mylib_barrier (struct barrier_node*b, int num_threads);

//void mylib_logbarrier (struct barrier_node* b, int num_threads, int thread_id, int MAX_THREADS);
struct sample_size *samples_arr;
struct barrier_node *barr;
struct barrier_node *barr2;
struct partition_info* partition_array;

extern int compare (const void * a, const void * b);
extern int binary_search(int *a, int low, int high, int key);
extern void prefix_sum(int num_threads,int thread_id);
extern int compare_heap_key( const void*a,const void *b);

int *pivots;
int total_pivots;
struct prefix_sum_msg*prefix_sum_msg_arr;
int dimension;

#endif
