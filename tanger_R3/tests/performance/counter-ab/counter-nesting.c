/*
 * Very simple example: Two threads concurrently update a shared counter.
 *
 * Look for comments containing TANGER to see the relevant steps
 * necessary to use transactions via Tanger.
 */

/* Copyright (C) 2008  Pascal Felber, Diogo Becker de Brum
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>


/* ################################################################### *
 * BARRIER
 * ################################################################### */

typedef struct barrier {
    pthread_cond_t complete;
    pthread_mutex_t mutex;
    int count;
    int crossing;
} barrier_t;

void barrier_init(barrier_t *b, int n)
{
    pthread_cond_init(&b->complete, NULL);
    pthread_mutex_init(&b->mutex, NULL);
    b->count = n;
    b->crossing = 0;
}

void barrier_cross(barrier_t *b)
{
    pthread_mutex_lock(&b->mutex);
    /* One more thread through */
    b->crossing++;
    /* If not all here, wait */
    if (b->crossing < b->count) {
        pthread_cond_wait(&b->complete, &b->mutex);
    } else {
        pthread_cond_broadcast(&b->complete);
        /* Reset for next time */
        b->crossing = 0;
    }
    pthread_mutex_unlock(&b->mutex);
}

///////////////////////////////////////////////////////////////////////////////////////


// Flag that signals worker threads to stop
volatile int stop = 0;

// Shared counter
long count = 0;
long count_nested = 0;

barrier_t barrier;

void inc_count_nested() __attribute__((noinline));
void inc_count_nested()
{
    __transaction {
        count_nested++;
    }
}

// Worker thread function
void *counter_function(void *data)
{
    // Go into the barrier. This makes sure that threads start roughly at
    // the same time.
    barrier_cross(&barrier);

    while (!stop) {
        /* TANGER: Increments the counter in an atomic block.
         */
        __transaction {
          count++;
          inc_count_nested();
        }
    }
    return NULL;
}


int main(int argc, char **argv)
{
    // Just use two threads for now.
    int nb_threads = 2;
    volatile int i;
    pthread_t *threads;

    // We use a barrier to make worker threads start roughly at the same time.
    barrier_init(&barrier, nb_threads + 1);

    // Start threads.
    if ((threads = (pthread_t *)malloc(nb_threads * sizeof(pthread_t))) == NULL) {
        perror("malloc");
        exit(1);
    }
    for (i = 0; i < nb_threads; i++) {
        if (pthread_create(&threads[i], NULL, counter_function, NULL) != 0) {
            fprintf(stderr, "Error creating thread\n");
            exit(1);
        }
    }

    // Go into the barrier, enables worker threads to start
    barrier_cross(&barrier);

    // After threads have started, wait for a few seconds.
    struct timespec timeout;
    int duration = 10000;
    timeout.tv_sec = duration / 1000;
    timeout.tv_nsec = (duration % 1000) * 1000000;
    if (nanosleep(&timeout, NULL) != 0) {
      perror("nanosleep");
      exit(1);
    }

    // Tell worker threads to stop
    stop = 1;

    // Wait for worker threads to stop
    for (i = 0; i < nb_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error waiting for thread completion\n");
            exit(1);
        }
    }

    printf("Counter = %ld\n", count);

    free(threads);

    return 0;
}
