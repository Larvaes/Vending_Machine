#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define BUF_SIZE 100
#define CONS 8
#define SUP 5

#define p_test (printf("test"))

typedef struct {
    int buff[BUF_SIZE];
    size_t len;
    pthread_mutex_t mutex;
    pthread_cond_t full_slot;
    pthread_cond_t empty_slot;
} buffer_t;


buffer_t buffer[5] = {
    {
        .len = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .full_slot = PTHREAD_COND_INITIALIZER,
        .empty_slot = PTHREAD_COND_INITIALIZER
    }
};


typedef struct {
    char name[256];
    int interval;
    int repeat;
    int rep;
} role;

void printPara(char *name, int interval, int repeat, int rep, int len){
        printf("---------------------------\n");
        printf("Supplied %s 1\n",name);
        printf("Having: %d\n",len);
        printf("Current interval %d\n",interval);
        printf("Current repeat %d\n",repeat);
        printf("Current rep %d\n",rep);
        printf("---------------------------\n");
}

void *Supplier(void *arg){
    int buffIndex = *(int*)arg;  
    //------------------ read configure file ---------------------
    char fileName[20];
    sprintf(fileName,"supplier%d.txt",buffIndex+1);
    printf("%s",fileName);
    FILE *fp = fopen(fileName,"r");
    char input[3][20];
    
    for(int i = 0; i < 3; i++){
        fgets(input[i],20,fp);
        strtok(input[i], "\n");
    }
    //-------------------------------------------------------------
    //------------------ initialize supplier parameter ------------
    role sup;
    strcpy(sup.name, input[0]);
    sup.interval = atoi(input[1]);
    sup.repeat = atoi(input[2]);
    sup.rep = 0;
    //--------------------------------------------------------------   
    while(1){    
        sleep(sup.interval);
        pthread_mutex_lock(&buffer[buffIndex].mutex);
        while(buffer[buffIndex].len == BUF_SIZE){
            if(sup.rep < sup.repeat){
                sup.rep++;  
                pthread_mutex_unlock(&buffer[buffIndex].mutex);
                sleep(sup.interval);
                pthread_mutex_lock(&buffer[buffIndex].mutex);
                //pthread_cond_timedwait(&buffer->full_slot,&buffer->mutex,&sup.interval);
            }
            else{
                if((sup.interval * 2) < 60){
                    sup.interval *= 2;
                }
                sup.rep = 0;
            }
        }

        ++buffer[buffIndex].len;
        
        
        // printPara(sup.name,sup.interval,sup.repeat,sup.rep,buffer[buffIndex].len);

        // pthread_cond_timedwait(&buffer->full_slot,&buffer->mutex,&sup.interval);
        pthread_mutex_unlock(&buffer[buffIndex].mutex);
        
    }
    return NULL;
}

void *Consumer(void *arg){
    role cons = {
        .name = "coke",
        .interval = 1,
        .repeat = 3,
        .rep = 0
    };
    int buffIndex = *(int*)arg; 

    while(1){
        sleep(cons.interval);
        pthread_mutex_lock(&buffer[buffIndex].mutex);
        while(buffer[buffIndex].len == 0){
            if(cons.rep < cons.repeat){
                cons.rep++;
                printf("wait for produce\n");
                pthread_mutex_unlock(&buffer[buffIndex].mutex);
                sleep(cons.interval);
                pthread_mutex_lock(&buffer[buffIndex].mutex);
                //pthread_cond_timedwait(&buffer->empty_slot,&buffer->mutex,&cons.interval);
            }
            else{
                if((cons.interval * 2) < 60){
                    cons.interval *= 2;
                }
                cons.rep = 0;
            }
        }

        --buffer[buffIndex].len;
        printf("---------------------------\n");
        printf("Consumed: 1\n");
        printf("Having: %d\n",buffer[buffIndex].len);
        printf("---------------------------\n");

        //pthread_cond_timedwait(&buffer->empty_slot,&buffer->mutex,&cons.interval);

        pthread_mutex_unlock(&buffer[buffIndex].mutex);
        
    }
    return NULL;
}


int main(){
    pthread_t sup[SUP];
    pthread_t cons[CONS];

    for(int i = 0; i < 5; i++){
        int *index = malloc(sizeof(*index));
        *index = i;
        pthread_create(&sup[i], NULL, Supplier, index);
    }

    


    /*for(int i = 0; i < 8; i++){
        int *index = malloc(sizeof(*index));
        *index = i;
        pthread_create(&cons[i], NULL, Consumer, index);
    }*/

    pthread_exit(0);
    return 0;
}