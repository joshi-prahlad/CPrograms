/*
 * =====================================================================================
 *
 *       Filename:  bitReverse.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/07/12 14:21:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Prahlad K. Joshi (mn), 
 *        Company:  
 *
 * =====================================================================================
 */

#include<stdio.h>

int reverseBits(int a){
    unsigned int up = ~0;
    up >>=1;
    up = ~up;
    unsigned int low = 1;
    for(;up>low;up>>=1,low<<=1){
        int temp = a;
        if(temp&up){
            a|=low;
        }else{
            a&=~low;
        }
        if(temp&low){
            a|= up;
        }else{
            a&=~up;
        }       
    }
    printf("%u\n",a);
}


int main(void){
    reverseBits(3);
    return 0;
}
