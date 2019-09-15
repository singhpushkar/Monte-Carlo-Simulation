#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#define FILE_SIZE 1000000
int main(){
    FILE* r = fopen("data.dat","w");
    
    for(int i=0;i<FILE_SIZE;i++){
        int x = rand()%10000;
        int y = rand()%10000;
        fprintf(r,"%d %d",x,y);
    }
    fclose(r);
    return 0;
}