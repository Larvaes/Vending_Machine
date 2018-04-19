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
#define stringTime time_t clk = time(NULL); strcpy(stime,ctime(&clk)); strtok(stime,"\n");

typedef struct {
    //product buffer
    size_t len;
    pthread_mutex_t mutex;
} buffer_t;


buffer_t buffer[5] = {
    //initialize all buffer 
    {
        .len = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER
    }
};


typedef struct {
    //attribute of both consumer and supplier
    char name[256];
    int interval;
    int repeat;
    int rep;
} role;

int getBufferIndex(char *name){
    if(!strcmp(name,"Lay"))
        return 0;
    else if(!strcmp(name,"Pote"))
        return 1;
    else if(!strcmp(name,"Testo"))
        return 2;
    else if(!strcmp(name,"Paprika"))
        return 3;
    else if(!strcmp(name,"Sunbite"))
        return 4;

    return -1;
}

void *Supplier(void *arg){
    int buffIndex = *(int*)arg;   
    //------------------ read configure file ---------------------
    char fileName[20];
    sprintf(fileName,"supplier%d.txt",buffIndex+1);
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
    char stime[30];   
    if(sup.interval == 0){
        printf("Invalid interval value, must > 0\n");
        printf("Terminate thread\n");
        pthread_exit(0);
        
    }
    while(1){    
        //sleep for interval value before attampt to add item
        sleep(sup.interval);
        pthread_mutex_lock(&buffer[buffIndex].mutex);
        while(buffer[buffIndex].len == BUF_SIZE){
            //check if buffer is full
            if(sup.rep < sup.repeat){
                //check if repeat counter is more than configured repeat value
                stringTime;
                printf("%s %s supplier going to wait\n",stime,sup.name);
                sup.rep++;  
                pthread_mutex_unlock(&buffer[buffIndex].mutex);
                sleep(sup.interval);
                pthread_mutex_lock(&buffer[buffIndex].mutex);
            }
            else{
                if((sup.interval * 2) < 60){
                    sup.interval *= 2;
                }
                sup.rep = 0;
            }
        }

        ++buffer[buffIndex].len;
        stringTime;
        printf("%s %s supplied 1 unit. stock after = %d\n",stime,sup.name,buffer[buffIndex].len);
        
        pthread_mutex_unlock(&buffer[buffIndex].mutex);
        
    }
    return NULL;
}

void *Consumer(void *arg){
    int consumerIndex = *(int*)arg;
    //------------------ read configure file ---------------------
    char fileName[20];
    sprintf(fileName,"consumer%d.txt",consumerIndex+1);
    FILE *fp = fopen(fileName,"r");
    char input[3][20];
    
    for(int i = 0; i < 3; i++){
        fgets(input[i],20,fp);
        strtok(input[i], "\n");
    }
    //-------------------------------------------------------------
    //------------------ initialize consumer parameter ------------
    role cons;
    strcpy(cons.name, input[0]);
    cons.interval = atoi(input[1]);
    cons.repeat = atoi(input[2]);
    cons.rep = 0;
    //--------------------------------------------------------------
    char stime[30];
    int buffIndex = getBufferIndex(cons.name); 
    if(buffIndex == -1){
        printf("Invalid product name\n");
        printf("Terminate thread\n");
        pthread_exit(0);
    }
    if(cons.interval == 0){
        printf("Invalid interval value, must > 0\n");
        printf("Terminate thread\n");
        pthread_exit(0);
        
    }
    while(1){
        //sleep for interval value before attampt to remove item
        sleep(cons.interval);
        pthread_mutex_lock(&buffer[buffIndex].mutex);
        while(buffer[buffIndex].len == 0){
            //check if buffer empty
            if(cons.rep < cons.repeat){
                //check if repeat counter is more than configured repeat value
                stringTime;
                printf("%s %s consumer going to wait\n",stime,cons.name);
                cons.rep++;
                pthread_mutex_unlock(&buffer[buffIndex].mutex);
                sleep(cons.interval);
                pthread_mutex_lock(&buffer[buffIndex].mutex);
            }
            else{
                if((cons.interval * 2) < 60){
                    cons.interval *= 2;
                }
                cons.rep = 0;
            }
        }

        --buffer[buffIndex].len;
        stringTime;
        printf("%s %s consumed 1 unit. stock after = %d\n",stime,cons.name,buffer[buffIndex].len);
        
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

    for(int i = 0; i < 8; i++){
        int *index = malloc(sizeof(*index));
        *index = i;
        pthread_create(&cons[i], NULL, Consumer, index);
    }

    pthread_exit(0);
    return 0;
}