#include "pqsort.h"
#include "pqueue.h"
#include "aligned_malloc_free.h"

pthread_mutex_t mutex1= PTHREAD_MUTEX_INITIALIZER;

int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}
int compare_heap_key( const void*a,const void *b)
{
    return ((struct sample_size*)a)->addr[((struct sample_size*)a)->current_index] - ((struct sample_size*)b)->addr[((struct sample_size*)b)->current_index];
}
void p_way_merge(struct sample_size*arr,int p,int *result_offset,int thread_count,int thread_id)
{
    PQueue* heap = pqueue_new(compare_heap_key,p);
    int i = 0;
    int total_el = 0;
    for(;i<p;++i)
    {
        if(arr[i].count > 0)
        {
            pqueue_enqueue(heap,arr+i);
            total_el += arr[i].count;
        }
    }
    int *merged = (int*)malloc(sizeof(int) * total_el);
    i = 0;
    struct sample_size *ptr ; 
    while((ptr = pqueue_dequeue(heap)) != NULL)
    {
       if(DEBUG_1)
        {
            pthread_mutex_lock(&mutex1);
            printf(" Got value %d current_index is %d ptr is %p\n",ptr->addr[ptr->current_index],ptr->current_index,ptr); 
            printf("\n");
            pthread_mutex_unlock(&mutex1);
        }
        merged[i++] =  ptr->addr[ptr->current_index++];
    //    result_offset[i++] = ptr->addr[ptr->current_index++]; 
       if(ptr->current_index < ptr->count)
       {
           pqueue_enqueue(heap,ptr);
       }
    }
    int j = 0;
    if(DEBUG_1)
    {
        pthread_mutex_lock(&mutex1);
       printf("\n");
        pthread_mutex_unlock(&mutex1);
    }
     mylib_barrier(barr, thread_count);
    memcpy(result_offset,merged,total_el*sizeof(int));
    
}
void *sort_task(void*ptr)
{
    struct thread_arg *args = (struct thread_arg*) ptr;
    int begin_index = 0;
    int param = args->input_size / args->thread_count;
    int extraElem = args->input_size % args->thread_count;
    int elementCount = 0;
    if(args->thread_id < (extraElem)){
        begin_index = args->thread_id * (param + 1);
        elementCount = param + 1;
    }else{
        begin_index = args->thread_id * param + extraElem;
        elementCount = param;
    }
    qsort(args->input+begin_index , elementCount,sizeof(int),compare);
    int i = 0 ;
   if(DEBUG_1) 
    {
        pthread_mutex_lock(&mutex1);
        printf("In thread %d \n",args->thread_id);
        for(;i<elementCount;++i)
        {
            printf("%d ",args->input[i+begin_index]);
        }
        printf("\n");
        pthread_mutex_unlock(&mutex1);
     }
    i = 0;
    int factor = args->input_size/(args->thread_count * args->thread_count);
    int *samples = (int*)malloc(sizeof(int)*args->thread_count);
    int sample_index = 0;
    for(i=0;i < args->thread_count; ++i)
    {
        sample_index = begin_index + factor * i;
        samples[i] = args->input[sample_index]; 
    }
    samples_arr[args->thread_id].addr = samples;
    samples_arr[args->thread_id].count = i;
    samples_arr[args->thread_id].current_index = 0;

    mylib_barrier_calculate_pivots(barr,args->thread_count);

    partition_array[args->thread_id].partition_index = (int **)aligned_malloc(sizeof(int*) * args->thread_count,CACHE_LINE);
    partition_array[args->thread_id].partition_element_count = (int*)aligned_malloc(sizeof(int) * args->thread_count,CACHE_LINE);
    memset(partition_array[args->thread_id].partition_index,0,sizeof(int*) * args->thread_count);
    memset(partition_array[args->thread_id].partition_element_count,0,sizeof(int) * args->thread_count);

    int current_index = begin_index;
    int end_index = 0;
    for(i=0;i<total_pivots && current_index < begin_index + elementCount;++i)
    {
        if(args->input[current_index] > pivots[i]){
            partition_array[args->thread_id].partition_index[i] = NULL;
            partition_array[args->thread_id].partition_element_count[i] = 0;
        }
        else
        {
            int key_index = binary_search(args->input,current_index,begin_index+elementCount-1,pivots[i]);
            partition_array[args->thread_id].partition_index[i] = args->input + current_index;
            end_index = key_index;
            if(key_index < 0 )
            {
                int real_end_index = -1*key_index - 1 ;

                partition_array[args->thread_id].partition_element_count[i] = real_end_index - current_index;
                current_index =real_end_index ;
            }
            else
            {
                partition_array[args->thread_id].partition_element_count[i] = end_index - current_index + 1;
                current_index = end_index + 1;
            }
        }
    }
    if(current_index <= begin_index + elementCount - 1)
    {
        partition_array[args->thread_id].partition_index[i] = args->input + current_index;
        partition_array[args->thread_id].partition_element_count[i] = begin_index + elementCount -  current_index;
    }
    
    if(DEBUG_1) 
    {
        pthread_mutex_lock(&mutex1);
        printf("In thread %d \n",args->thread_id);
        printf("Total Pivots %d\n",total_pivots);
        for(i = 0;i<=total_pivots;++i)
        {
           if(i < total_pivots)
           {
               printf("Pivot %d\n",pivots[i]);
           }
           if(partition_array[args->thread_id].partition_index[i] != NULL){
            printf("Begin Index %d\n",*(partition_array[args->thread_id].partition_index[i]));
            printf("Size %d\n",partition_array[args->thread_id].partition_element_count[i]);
           }
           else{
           printf("Begin Index for partition %d is NULL\n",i);
           printf("Element Count is %d\n",partition_array[args->thread_id].partition_element_count[i]);
          }
        }
        printf("\n");
        pthread_mutex_unlock(&mutex1);
     }

     mylib_barrier(barr, args->thread_count);
     int my_els_count = 0;
     struct sample_size*partition_info_for_merge = (struct sample_size*)malloc(sizeof(struct sample_size)*args->thread_count);
     for(i = 0;i < args->thread_count; ++i)
     {
        my_els_count += partition_array[i].partition_element_count[args->thread_id]; 
        partition_info_for_merge[i].addr =partition_array[i].partition_index[args->thread_id]; 
        partition_info_for_merge[i].count =partition_array[i].partition_element_count[args->thread_id]; 
        partition_info_for_merge[i].current_index = 0;
     }
    if(DEBUG_2) 
    {
        pthread_mutex_lock(&mutex1);
        printf("In thread %d \n",args->thread_id);
        printf("Total els_count %d\n",my_els_count);
        //for(i = 0;i<my_els_count;++i)
        //{
         //   printf(" %d ",my_els[i]); 
       // }
        printf("\n");
        pthread_mutex_unlock(&mutex1);
     }
     prefix_sum_msg_arr[args->thread_id].msg = my_els_count;
     prefix_sum_msg_arr[args->thread_id].sum = 0;
     mylib_barrier(barr, args->thread_count);
     prefix_sum(args->thread_count,args->thread_id);
     int offset = prefix_sum_msg_arr[args->thread_id].sum;
     if(DEBUG_1)
     {
         //printf("In thread %d with offset %d offsetAddr %p base is %p\n",args->thread_id,offset,result+offset,result);
     }
     p_way_merge(partition_info_for_merge,args->thread_count,args->input+offset,args->thread_count,args->thread_id);

}
long timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
    long msec;
    msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
    msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
    return msec;
}
int binary_search(int *a, int low, int high, int key) {
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int midVal = a[mid];
        if (midVal < key)
            low = mid + 1;
        else if (midVal > key)
            high = mid - 1;
        else
            return mid; // key found
    }
    return -(low + 1);  // key not found.
}
int *pqsort(int* input, int num_elements, int num_threads)
{
  // YOUR CODE GOES HERE
    struct timeval start, finish;
/*    if(1 == num_threads)
    {
        gettimeofday(&start, NULL);
        qsort(input,num_elements,sizeof(int),compare);
        gettimeofday(&finish, NULL);
        printf("Time diff %ld\n",timevaldiff(&start,&finish));
        return input;
    }*/
    if(num_elements < num_threads)
    {
        num_threads = num_elements;
    }
    pthread_t *thread_arr = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);
    struct thread_arg *arg_arr = (struct thread_arg*)malloc(sizeof(struct thread_arg)*num_threads);
   
    
    //Phase 2: each thread samples its locally sorted array from
    //phase 1 and selects p elements.
    //Allocate an array of 'sample_size' where each thread writes
    //its base address of the array where its p elements are stored
    //and the size of the array(which might be less than 'p')
    samples_arr = (struct sample_size*)aligned_malloc(sizeof(struct sample_size) * num_threads,CACHE_LINE);
    int thread_id = 0;
    for(;thread_id < num_threads; ++thread_id)
    {
        arg_arr[thread_id].thread_id = thread_id;
        arg_arr[thread_id].input = input;
        arg_arr[thread_id].input_size = num_elements;
        arg_arr[thread_id].thread_count = num_threads;
    }
  
    barr = (struct barrier_node*)aligned_malloc(sizeof(struct barrier_node), CACHE_LINE);
    
    partition_array = (struct partition_info*)aligned_malloc(sizeof(struct partition_info) * num_threads,CACHE_LINE);
    prefix_sum_msg_arr = (struct prefix_sum_msg*)aligned_malloc(sizeof(struct prefix_sum_msg)*num_threads, CACHE_LINE);
    memset(prefix_sum_msg_arr,0,sizeof(struct prefix_sum_msg) * num_threads);

    mylib_init_barrier (barr);

    int temp_threads = num_threads;
    while(temp_threads)
    {
        ++dimension;
        temp_threads >>=1;
    }
    dimension -=1;

    if(1<<dimension < num_threads)
    {
        ++dimension;
    }
    if(DEBUG_1)
    {
        printf(" Dimension %d\n",dimension);
    }
    int i = 0;
    gettimeofday(&start, NULL);
    for(i=0;i< num_threads; ++i)
    {
        pthread_create(thread_arr+i,NULL,sort_task,arg_arr+i);
    }
    for(i=0;i< num_threads; ++i)
    {
        pthread_join(thread_arr[i],NULL);
    }
    gettimeofday(&finish, NULL);
    printf("Timediff %ld\n",timevaldiff(&start,&finish));
    if(DEBUG_1)
    {
        for(i=0;i<num_threads;++i)
        {
            printf("Thread %d sum %d \n",i,prefix_sum_msg_arr[i].sum);
        }
    }
    if(DEBUG_1)
    {
        printf("Output\n");

        for(i=0;i<num_elements;++i)
        {
            printf(" %d ",input[i]);
        }
        printf("\n");
    }
    return input; //return appropriately
}

