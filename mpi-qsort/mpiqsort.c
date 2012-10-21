#include "header.h"
#define DEBUG_1 0

// This is a comparison function that treats keys as signed ints

int compare(const void* key1, const void* key2) {
    int key1_v = *((int*)key1);
    int key2_v = *((int*)key2);
    return key1_v - key2_v;
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

int *merge(int* array1,int len1, int *array2, int len2)
{
    int *merged = (int*)malloc((len1 + len2) * sizeof(int)); 
    int i = 0, j = 0, k = 0;
    while(i < len1 && j < len2) 
    {
        if(array1[i] <= array2[j])
        {
            merged[k++] = array1[i++];
        }
        else
        {
            merged[k++] = array2[j++];
        }
    }
    memcpy(merged+k,array1+i,sizeof(int)*(len1-i));
    memcpy(merged+k,array2+j,sizeof(int)*(len2-j));
    return merged;
}
int* mpiqsort(int* input, int globalNumElements, int* dataLengthPtr, MPI_Comm comm, int commRank, int commSize) {
    
    //Run local sort
    int returnCode = MPI_SUCCESS;
    qsort(input,*dataLengthPtr,sizeof(int),compare);
    int i = 0;
    for(i = 1; i < commSize; i <<= 1)
    {
        int median;
        int pDivI = commSize / i;
        if(commRank % pDivI == 0)
        {
            median = input[*dataLengthPtr / 2];
            int dest = commRank + 1;
            for(; dest < commRank + pDivI; ++dest)
            {
                returnCode = MPI_Send(&median,1,MPI_INT,dest,0,comm);
                if(MPI_SUCCESS != returnCode && DEBUG_1)
                {
                    printf("Failed while sending median %d from process %d to process %d\n",median,commRank,dest); 
                }
            }
        }
        else
        {
            int sourceId = commRank - (commRank % pDivI);
            returnCode = MPI_Recv(&median, 1, MPI_INT, sourceId, MPI_ANY_TAG, comm,NULL);
            if(MPI_SUCCESS != returnCode && DEBUG_1)
            {

                printf("Failed while receiving median %d from process %d to process %d\n",median,sourceId,commRank); 
            }
        }
        int index = binary_search(input, 0, *dataLengthPtr - 1, median);    
        int lessThanMedianCount, moreThanMedianCount;
        if(index >= 0)
        {
            lessThanMedianCount = index + 1;
            moreThanMedianCount = *dataLengthPtr - lessThanMedianCount;
        }
        else
        {
            int realIndex = -index - 1;
            moreThanMedianCount = *dataLengthPtr - realIndex;
            lessThanMedianCount = *dataLengthPtr - moreThanMedianCount;
        }
        if(commRank % pDivI < pDivI / 2)
        {
            //Transmit greater than median part.
            int dest = commRank + pDivI / 2;
            //first send element count 
            returnCode = MPI_Send(&moreThanMedianCount, 1, MPI_INT, dest, 0, comm);
            returnCode = MPI_Send(input + lessThanMedianCount, moreThanMedianCount, MPI_INT, dest, 0, comm);
            if(MPI_SUCCESS != returnCode && DEBUG_1)
            {
                printf("Failed while transmitting the GreaterThanMedian part from %d to %d lessThanCount %d morethancount %d \n",commRank,dest,lessThanMedianCount,moreThanMedianCount);
            }
            //remove the tranmitted elements.
            *dataLengthPtr = lessThanMedianCount;

            //Receive elements
            int source = dest;
            int numElementRecvd = 0;
            int *rcvdEl;
            returnCode = MPI_Recv(&numElementRecvd, 1, MPI_INT, source, MPI_ANY_TAG, comm, NULL);
            if(numElementRecvd > 0)
            {
                rcvdEl = (int*)malloc(numElementRecvd * sizeof(int));
                if(DEBUG_1)
                {
                    printf("numElementRecvd %d\n",numElementRecvd);
                }
                returnCode = MPI_Recv(rcvdEl, numElementRecvd, MPI_INT, source, MPI_ANY_TAG, comm, NULL);
                if(MPI_SUCCESS != returnCode && DEBUG_1)
                {
                    printf("Failed while receiving the  from %d to %d numElRcvd%d \n",source,commRank,numElementRecvd);
                }
                //Merge received elements with existing elements.
                input = merge(input, *dataLengthPtr, rcvdEl, numElementRecvd);
                //TODO free the previous input array.
                *dataLengthPtr += numElementRecvd;
            }
        }
        else
        {
            int source = commRank - pDivI / 2;
            int numElementRecvd = 0;
            int *rcvdEl;
            returnCode = MPI_Recv(&numElementRecvd, 1, MPI_INT, source, MPI_ANY_TAG, comm, NULL);
            if(numElementRecvd > 0)
            {
                rcvdEl = (int*)malloc(numElementRecvd * sizeof(int));
            
                if(DEBUG_1)
                {
                    printf("numElementRecvd %d\n",numElementRecvd);
                }
                returnCode = MPI_Recv(rcvdEl, numElementRecvd, MPI_INT, source, MPI_ANY_TAG, comm, NULL);
                if(MPI_SUCCESS != returnCode && DEBUG_1)
                {
                    printf("Failed while receiving the  from %d to %d numElRcvd%d \n",source,commRank,numElementRecvd);
                }
            }
            //Transmit elements which are less than median.
            MPI_Send(&lessThanMedianCount, 1,  MPI_INT, source, 0, comm);
            MPI_Send(input, lessThanMedianCount, MPI_INT, source, 0, comm);
            //adjust existing element array.
            *dataLengthPtr = moreThanMedianCount; 
            input += lessThanMedianCount;
            //TODO free the previous input array.
            input = merge(input, *dataLengthPtr, rcvdEl, numElementRecvd);
            *dataLengthPtr += numElementRecvd;
        }
    }
    return input;
}

