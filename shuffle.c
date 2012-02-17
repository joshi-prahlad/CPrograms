#include<stdio.h>
/*
 * =====================================================================================
 *
 *       Filename:  shuffle.c
 *
 *    Description: Given an array and a valid index 'i' within the array, we need to shuffle
 *    		   it in such a way that all the elements from 0 to 'i-1' and 'i' to 
 *    		   arrLength-1. So if the array elements are a1,a2,a3,b1,b2,b3 and 
 *    		   'i' is 3 then the array should become a1,b1,a2,b2,a3,b3.
 *
 *        Version:  1.0
 *        Created:  02/17/12 12:55:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Prahlad K. Joshi (mn), 
 *        Complexity: O(n*n)  
 *
 * =====================================================================================
 */


/*
 * Shift all the elements of array 'arr' by one towards
 * the right. Elements from the beginIndex and upto endIndex - 1
 * are shifted, making a hole at beginIndex.
 */
void shiftRight(int arr[],int beginIndex,int endIndex)
{
	while(endIndex != beginIndex)
	{
		arr[endIndex] = arr[--endIndex];
	}
}
/*
 * Shuffle is done by the following algo:
 * Let firstPartIndex points to first element of the
 * first part e.g. 'a1' and secondPartIndex points
 * to shuffleIndex 'i' e.g. 'b1'. Then first the 
 * 'b1' is saved and then all the elements from 'a2'
 * to 'i-1' are shifted right by one position creating
 * space for 'b1' just after 'a1'. Then the saved 'b1'
 * is copied to this location.
 * After that firstPartIndex and secondPartIndex are 
 * updated appropiately.
 */
void shuffle(int arr[],int len,int shuffleIndex)
{
	int firstPartIndex = 0;
	int secondPartIndex = shuffleIndex;
	int iterationCount = (shuffleIndex < (len - shuffleIndex )) 
				? shuffleIndex:(len - shuffleIndex );
	while(iterationCount--)
	{
		int bEl = arr[secondPartIndex];
		shiftRight(arr,firstPartIndex+1,secondPartIndex);
		arr[firstPartIndex+1] = bEl;
	 	firstPartIndex += 2;	
		secondPartIndex += 1;
	}
}

void printArr(int arr[],int len)
{
	int i =0;
	while(i<len)
	{
		printf("%d ",arr[i++]);
	}
}
int main(void)
{
	int arr[] = {1,3,5,7,9,2,4,6,8,10};	
	int arrSize = 10;
	printf("Original: \n");
	printArr(arr,arrSize);
	shuffle(arr,arrSize,8);
	printf("Shuffled: \n");
	printArr(arr,arrSize);
	return 0;
}
