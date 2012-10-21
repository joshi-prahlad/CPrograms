#include<stdio.h>
#include<stdlib.h>
/*
 * =====================================================================================
 *
 *       Filename:  MergeSortedLL.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/11/12 13:45:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Prahlad K. Joshi (mn), 
 *        Company:  
 *
 * =====================================================================================
 */

struct node{
    int el;
    struct node*next;
};



struct node*newLinkedList(int *arr,int size){
    if(size<=0)
        return 0;
    struct node*head = (struct node*)malloc(sizeof(struct node));
    head->el = arr[0];
    head->next = 0;
    int i=1;
    struct node*prevNode = head;
    for(;i< size;++i){
        struct node*newNode = (struct node*)malloc(sizeof(struct node));
        newNode->el = arr[i];
        newNode->next = 0;
        prevNode->next = newNode;
        prevNode = newNode;
    }
    return head;
}
void printLinkedList(struct node*head){
    while(head){
        printf(" %d ",head->el);
        head = head->next;
    }
    printf("\n");
}
void merge(struct node*one, struct node*two, struct node **merged,struct node*curMerged)
{
    if(two == 0){
        if(*merged == 0){
            *merged = one;
            return;
        }
        curMerged->next = one;
        return;
    }
    if(one == 0){
        if(curMerged == 0){
            return;
        }
        curMerged->next = two;
        return;
    }
    if(one->el<two->el){
        if(*merged == 0){
            *merged = one;
            curMerged = one; 
        }else{
            curMerged->next = one;
            curMerged = one;
        }
        merge(one->next,two,merged,curMerged);
    }else{
        if(*merged == 0){
            *merged = two;
            curMerged = two;
        }else{
            curMerged->next = two;
            curMerged = two;
        }
        merge(one, two->next, merged, curMerged); 
    }
}
void mergeSort(struct node*head,struct node*tail){
    if(head == tail){
        return;
    }

}
int main(){
    int arr[] = {0,5,15,38,59,69,79};
    int arr2[] = {20,45,50};
    struct node*head1 = newLinkedList(arr,7);
    struct node*head2 = newLinkedList(arr2,3);
    printLinkedList(head1);
    printLinkedList(head2);
    struct node*merged=0,*curMerged=0;
    merge(head2,head1,&merged,curMerged);
    printLinkedList(merged);
    return 0;
}
