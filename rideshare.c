#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>

unsigned int countA = 0;
unsigned int countB = 0;
int sizeA, sizeB;
sem_t semA, semB;
pthread_mutex_t mutex, mutex2;
pthread_barrier_t barrier;
int driver=0;
void * rideShare(void *argv)
{
    char *args = (char*)argv;
    char team = *args;
    if(team == 'A')
    {
        pthread_mutex_lock(&mutex);
        printf("Thread ID: %ld , Team: %c, I am currently looking for a car\n", pthread_self(), team);
        countA++;
        if(countA == 2 && countB >= 2)
        {
            sem_post(&semA);
            sem_post(&semB);
            sem_post(&semA);
            sem_post(&semB);
            countA= countA-2;
            countB= countB-2;
        }
        else if(countA == 4)
        {
            sem_post(&semA);
            sem_post(&semA);
            sem_post(&semA);
            sem_post(&semA);
            countA= countA-4;
        }
        else if(countB == 4)
        {
            sem_post(&semB);
            sem_post(&semB);
            sem_post(&semB);
            sem_post(&semB);
            countB= countB-4;
        }
        pthread_mutex_unlock(&mutex);
        sem_wait(&semA);
    }
    else if(team == 'B')
    {
        pthread_mutex_lock(&mutex);
        printf("Thread ID: %ld , Team: %c, I am currently looking for a car\n", pthread_self(), team);
        countB++;
        
        if(countA >= 2 && countB == 2)
        {
            sem_post(&semB);
            sem_post(&semA);
            sem_post(&semB);
            sem_post(&semA);
            countA= countA-2;
            countB= countB-2;
        }
        else if(countB == 4)
        {
            sem_post(&semB);
            sem_post(&semB);
            sem_post(&semB);
            sem_post(&semB);
            countB= countB-4;
        }
        else if(countA == 4)
        {
            sem_post(&semA);
            sem_post(&semA);
            sem_post(&semA);
            sem_post(&semA);
            countA= countA-4;
        }
        pthread_mutex_unlock(&mutex);
        sem_wait(&semB);
    }
    
    pthread_barrier_wait(&barrier);
    if(team == 'A' || team == 'B')
    {
        pthread_mutex_lock(&mutex2);
        printf("Thread ID: %ld , Team: %c, I have found a spot on the car\n", pthread_self(), team);
        driver++;
        
       
        if(driver% 4 == 0 && driver != 0)
        {
            printf("Thread ID: %ld , Team: %c, I am the captain and driving the car\n", pthread_self(), team);
        }
        pthread_mutex_unlock(&mutex2);
    }  
}
int main(int argc, char* argv[])
{
    pthread_t *thread_A , *thread_B;
    sizeA = atoi(argv[1]);
    sizeB = atoi(argv[2]);
    char A = 'A';
    char B = 'B';
    pthread_barrier_init(&barrier, NULL, 4);
    sem_init(&semA, 0, 0);
    sem_init(&semB, 0, 0);
    if(sizeA % 2==0 && sizeB % 2 == 0 && (sizeA + sizeB)%4 == 0)
    {
        thread_A=(pthread_t *)malloc(sizeA* sizeof(pthread_t ));
        for(int i=0;i<sizeA;i++)
        {
            pthread_create(&thread_A[i],NULL,rideShare,&A);
        }

        thread_B=(pthread_t *)malloc(sizeB* sizeof(pthread_t ));
        for( int i=0;i<sizeB;i++)
        {
            pthread_create(&thread_B[i],NULL,rideShare,&B);
        }
        for(int i=0;i<sizeA;i++)
        {
            pthread_join(thread_A[i],NULL);
        }
        for(int i=0;i<sizeB;i++)
        {
            pthread_join(thread_B[i],NULL);
        }
    }
    sem_destroy(&semA);
    sem_destroy(&semB);
    pthread_barrier_destroy(&barrier);
    printf("The main terminates\n");
    return 0;
}
