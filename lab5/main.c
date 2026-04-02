#include  <stdio.h> /* printf */
#include  <sys/types.h> /* pid_t */
#include <unistd.h> /* get_pid */
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <sys/wait.h>  /* wait */
#include <pthread.h>
#include <sys/stat.h>

void * count(void *);
int globalNumber = 0;
//Create a mutex
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int main(void) {
    int i;
    pthread_t counterThread[5];
    int returnValues[5];

    //Create the 5 threads
    if( (returnValues[0]=pthread_create( &counterThread[0], NULL, &count, NULL)) )
    {
        printf("Thread creation failed: %d\n", returnValues[0]);
    }
    if( (returnValues[1]=pthread_create( &counterThread[1], NULL, &count, NULL)) )
    {
        printf("Thread creation failed: %d\n", returnValues[1]);
    }

    if( (returnValues[2]=pthread_create( &counterThread[2], NULL, &count, NULL)) )
    {
        printf("Thread creation failed: %d\n", returnValues[2]);
    }

    if( (returnValues[3]=pthread_create( &counterThread[3], NULL, &count, NULL)) )
    {
        printf("Thread creation failed: %d\n", returnValues[3]);
    }

    if( (returnValues[4]=pthread_create( &counterThread[4], NULL, &count, NULL)) )
    {
        printf("Thread creation failed: %d\n", returnValues[4]);
    }

    //Wait for all treads to finish
    for (int i = 0; i < 5; i++) pthread_join(counterThread[i], NULL);

    return (0);
}

void * count(void * junk) {
    int loopCount = 0;
    pthread_mutex_lock(&mutex1);
    while (loopCount < 10) {
        int tmpNumber = globalNumber;
        printf("counter: %d, Thread: %ld, PID: %d\n",
                tmpNumber, pthread_self(), getpid());
        tmpNumber = globalNumber;
        tmpNumber++;
        usleep(random() % 2);
        globalNumber = tmpNumber;
        loopCount++;
    }
    pthread_mutex_unlock(&mutex1);
    return (0);
}
