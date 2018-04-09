#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include<stdlib.h>
#include<time.h>

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

typedef struct {
    char name[256];
    int interval;
    int repeat;
    int rep;
} role;


void *Supplier(void *arg){
    role sup = {
        .name = "coke",
        .interval = 1,
        .repeat = 3,
        .rep = 0
    };
    buffer_t *buffer = (buffer_t*)arg;    
    while(1){    
        sleep(sup.interval);
        pthread_mutex_lock(&buffer->mutex);
        while(buffer->len == BUF_SIZE){
            if(sup.rep < sup.repeat){
                sup.rep++;
                pthread_mutex_unlock(&buffer->mutex);
                sleep(sup.interval);
                pthread_mutex_lock(&buffer->mutex);
                //pthread_cond_timedwait(&buffer->full_slot,&buffer->mutex,&sup.interval);
            }
            else{
                if((sup.interval * 2) < 60){
                    sup.interval *= 2;
                }
                sup.rep = 0;
            }
        }

        ++buffer->len;
        printf("---------------------------\n");
        printf("Supplied 1\n");
        printf("Having: %d\n",buffer->len);
        printf("---------------------------\n");
        //pthread_cond_timedwait(&buffer->full_slot,&buffer->mutex,&sup.interval);
        pthread_mutex_unlock(&buffer->mutex);
        
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
    buffer_t *buffer = (buffer_t*)arg;

    while(1){
        sleep(cons.interval);
        pthread_mutex_lock(&buffer->mutex);
        while(buffer->len == 0){
            if(cons.rep < cons.repeat){
                cons.rep++;
                printf("wait for produce\n");
                pthread_mutex_unlock(&buffer->mutex);
                sleep(cons.interval);
                pthread_mutex_lock(&buffer->mutex);
                //pthread_cond_timedwait(&buffer->empty_slot,&buffer->mutex,&cons.interval);
            }
            else{
                if((cons.interval * 2) < 60){
                    cons.interval *= 2;
                }
                cons.rep = 0;
            }
        }

        --buffer->len;
        printf("---------------------------\n");
        printf("Consumed: 1\n");
        printf("Having: %d\n",buffer->len);
        printf("---------------------------\n");

        //pthread_cond_timedwait(&buffer->empty_slot,&buffer->mutex,&cons.interval);

        pthread_mutex_unlock(&buffer->mutex);
        
    }
    return NULL;
}


int main(){
    pthread_t sup[SUP];
    pthread_t cons[CONS];

    buffer_t buffer[5] = {
        {
            .len = 0,
            .mutex = PTHREAD_MUTEX_INITIALIZER,
            .full_slot = PTHREAD_COND_INITIALIZER,
            .empty_slot = PTHREAD_COND_INITIALIZER
        }
    };

    //test one product -------------------------------------
    for(int i = 0; i < 5; i++)
        pthread_create(&sup[i], NULL, Supplier, (void*)&buffer[0]);
    
    for(int i = 0; i < 8; i++)
        pthread_create(&cons[i], NULL, Consumer, (void*)&buffer[0]);

    pthread_exit(0);
    return 0;
}