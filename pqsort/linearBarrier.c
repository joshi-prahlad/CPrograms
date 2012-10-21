#include "pqsort.h"
#include "pqueue.h"



void mylib_init_barrier(struct barrier_node *b)
{
    b -> count = 0;
    pthread_mutex_init(&(b -> count_lock), NULL);
    pthread_cond_init(&(b -> ok_to_proceed), NULL);
}

void mylib_barrier_calculate_pivots (struct barrier_node*b, int num_threads)
{
    pthread_mutex_lock(&(b -> count_lock));
    b -> count ++;
    if (b -> count == num_threads) {
        b -> count = 0;
        //This code should be executed by only one thread.
        int *merged_samples = (int*)malloc(sizeof(int)*num_threads*num_threads);
        int i = 0 , k= 0;
        PQueue*heap = pqueue_new(compare_heap_key,num_threads);
        for(;i<num_threads;++i)
        {
            if(samples_arr[i].count > 0 )
            {
                pqueue_enqueue(heap,samples_arr + i);
            }
        }
        struct sample_size*ptr;
        while((ptr = pqueue_dequeue(heap)) != NULL)
        {
            merged_samples[k++] = ptr->addr[ptr->current_index++];
            if(ptr->current_index < ptr->count)
            {
                pqueue_enqueue(heap,ptr);
            }
        }
        if(DEBUG_1)
        {
            puts("In merged samples");
            for(i=0;i<k;++i)
            {
                printf("%d ",merged_samples[i]);
            }
        }
        //Find the p-1 pivots
        pivots = (int*)malloc(sizeof(int) * (num_threads - 1));
        int pby2 = num_threads/2 - 1;
        for(i = 1;i < num_threads;++i)
        {
            int index =   i*num_threads + pby2;
            if(index >= k)
            {
                break;
            }
            //Store unique pivots only.
            if(!total_pivots ||( pivots[total_pivots - 1] != merged_samples[index]))
            {
                pivots[total_pivots++] = merged_samples[index];
            }
        }
        //free(merged_samples);
        pthread_cond_broadcast(&(b -> ok_to_proceed));
    }
    else
        while (pthread_cond_wait(&(b -> ok_to_proceed),
                    &(b -> count_lock)) != 0);
    pthread_mutex_unlock(&(b -> count_lock));
}
void mylib_barrier (struct barrier_node*b, int num_threads)
{
    pthread_mutex_lock(&(b -> count_lock));
    b -> count ++;
    if (b -> count == num_threads) {
        b -> count = 0;
                pthread_cond_broadcast(&(b -> ok_to_proceed));
    }
    else
        while (pthread_cond_wait(&(b -> ok_to_proceed),
                    &(b -> count_lock)) != 0);
    pthread_mutex_unlock(&(b -> count_lock));
}
void prefix_sum (int num_threads, int thread_id)
{
   int i = 0;
   for(;i<dimension;++i)
   {
       int partner = thread_id ^ (1<<i);
       if(partner < num_threads && partner > thread_id)
       {
            if(DEBUG_1)
           {
               printf("Thread id %d  sum %d and partner %d\n",thread_id,prefix_sum_msg_arr[thread_id].sum,partner);
           }
           prefix_sum_msg_arr[partner].msg += prefix_sum_msg_arr[thread_id].msg;
           prefix_sum_msg_arr[partner].sum += prefix_sum_msg_arr[thread_id].msg;
           prefix_sum_msg_arr[thread_id].msg = prefix_sum_msg_arr[partner].msg;
           
       }
     mylib_barrier(barr, num_threads);
   }
    if(DEBUG_1)
           {
               printf("Thread id %d Prefix_sum %d\n",thread_id,prefix_sum_msg_arr[thread_id].sum);
           }
}


