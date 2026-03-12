/**
 *
 * COE 628 - Operating Systems
 * Lab 6
 *
 **/

// compile:
// gcc -pthread -o main lab6.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define PRODUCER_NO 5
#define NUM_PRODUCED 20   // set to 2000 later if needed

void *generator_function(void *);
void *print_function(void *);

long sum;                 /* Sum of generated values */
long finished_producers;  /* Number of producers that finished */

// C: Mutex declaration and initialization
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

// F: Condition variable declaration and initialization
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

int main(void) {
    int i;
    pthread_t producers[PRODUCER_NO];
    pthread_t print_thread;

    srand(time(NULL));
    sum = 0;
    finished_producers = 0;

    // A: Create five generator threads
    for (i = 0; i < PRODUCER_NO; i++) {
        if (pthread_create(&producers[i], NULL, generator_function, NULL) != 0) {
            perror("pthread_create producer");
            exit(EXIT_FAILURE);
        }
    }

    // D: Create print thread
    if (pthread_create(&print_thread, NULL, print_function, NULL) != 0) {
        perror("pthread_create print_thread");
        exit(EXIT_FAILURE);
    }

    // B: Wait for all generator threads to finish
    for (i = 0; i < PRODUCER_NO; i++) {
        if (pthread_join(producers[i], NULL) != 0) {
            perror("pthread_join producer");
            exit(EXIT_FAILURE);
        }
    }

    // E: Wait for print thread to finish
    if (pthread_join(print_thread, NULL) != 0) {
        perror("pthread_join print_thread");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *generator_function(void *junk) {
    long counter = 0;
    long sum_this_generator = 0;

    while (counter < NUM_PRODUCED) {
        long rnd_number = 1;   // easier for testing; expected total = 5 * 20 = 100

        // Lock only around shared-data access
        pthread_mutex_lock(&mutex1);

        long tmpNumber = sum;
        printf("current sum of the generated number up to now is %ld going to add %ld to it.\n",
               tmpNumber, rnd_number);
        sum = tmpNumber + rnd_number;

        pthread_mutex_unlock(&mutex1);

        counter++;
        sum_this_generator += rnd_number;

        usleep(1000);
    }

    printf("--+---+----+----------+---------+---+--+---+------+----\n");
    printf("The sum of produced items for this number generator at the end is: %ld\n",
           sum_this_generator);
    printf("--+---+----+----------+---------+---+--+---+------+----\n");

    pthread_mutex_lock(&mutex1);
    finished_producers++;

    // H: If all generators have finished, fire signal for condition variable
    if (finished_producers == PRODUCER_NO) {
        pthread_cond_signal(&cond1);
    }

    pthread_mutex_unlock(&mutex1);

    return NULL;
}

void *print_function(void *junk) {
    pthread_mutex_lock(&mutex1);

    // G: Wait until all generators have finished
    while (finished_producers < PRODUCER_NO) {
        pthread_cond_wait(&cond1, &mutex1);
    }

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("The value of counter at the end is: %ld\n", sum);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    pthread_mutex_unlock(&mutex1);

    return NULL;
}