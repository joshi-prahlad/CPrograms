#include<stdio.h>
#include<stdlib.h>

/*
 * Given two input numbers 'n' and 'k' print all the asceding sequence of
 * length 'k' having numbers in the range 1 to n.
 */
/*
 * It is same as having a sorted sequence of numbers from 1 to n and then
 * picking up the subsequences of length k.
 */
/*
 * Following solution does it recursively. e.g. if n is 5 and k is 3 then
 * the first position can have elements from 1 to 5-(3-(0+1)) and the second
 * positions can have elements from firstPosEl+1 to 5-(3-(1+1)) and so on.
 * so a position  'i' can have elements in the range [elementAtPrevPos + 1, n-(k-(i+1))]
 * where i varies from 0 to n-1.
 */
void printAscSeq(int max, int length, int curLength, int seq[],int prevEl,unsigned long* totalCount);

int main(int argc, char*argv[])
{
	
	if(3 != argc)
	{
		return -1;
	}
	int k = 2;
	int n = 4;
	sscanf(argv[1],"%d",&n);
	sscanf(argv[2],"%d",&k);
	printf("n is %d and k is %d\n",n,k);
	int *arr = (int*)malloc(sizeof(int)*k);
	unsigned long totalCount = 0;
	printAscSeq(n,k,0,arr,0,&totalCount);
	printf("Toal seq %lu\n",totalCount);
	return 0;
}
void printAscSeq(int max, int length, int curLength, int seq[],int prevEl,unsigned long*totalCount)
{
	if(length == curLength)
	{
		int i =0 ;
		for(;i<length;++i)
		{
			printf("%d ",seq[i]);
		}
		printf("\n");
		++*totalCount;
		return;
	}
	int curEl = prevEl+1;
	for(;curEl <= max - (length -(curLength + 1));++curEl)
	{
		seq[curLength] = curEl;
		printAscSeq(max,length,curLength+1,seq,curEl,totalCount);
	}

}
