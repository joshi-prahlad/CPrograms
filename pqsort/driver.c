#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

int compare1 (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}
void validate(int* output, int num_elements,int*input) {
  int i = 0;
  assert(output != NULL);
  qsort(input,num_elements,sizeof(int),compare1);
  for(i = 0; i < num_elements - 1; i++) {
    if (output[i] > output[i+1] || output[i] != input[i]) {
      printf("************* NOT sorted *************\n");
      printf("Failed at %d %d\n",i,output[i]);
      return;
    }
  }
  printf("============= SORTED ===========\n"); 
}

int main(int argc, char **argv)
{
    FILE* fin = NULL;
    int* input = NULL;
    int* output = NULL;
    int num_elements, num_threads, i = 0;

    if(argc != 2)
      {printf("Usage: ./pqsort <num of threads>\n");}
    
    num_threads = atoi(argv[1]);

    //read input_size and input
    if((fin = fopen("input.txt", "r")) == NULL)
      {printf("Error opening input file\n"); exit(0);}

    fscanf(fin, "%d", &num_elements);
    if( !(input = (int *)calloc(num_elements, sizeof(int))) )
      {printf("Memory error\n"); exit(0);}

    for(i = 0; i < num_elements || feof(fin); i++)
        fscanf(fin, "%d", &input[i]);
    
    if(i < num_elements)
      {printf("Invalid input\n"); exit(0);}

    output = pqsort(input, num_elements, num_threads);

    validate(output, num_elements,input);
    return 0;
}
