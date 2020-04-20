/**
 * Name: Jeon Cheol Su  
 * Student id: 20673413
 * ITSC email: csjeon@connect.ust.hk
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "pthreads_smith_waterman.h"

void* update_and_max(void* args);
/*
 *  You can add helper functions and variables as you wish.
 */
int min(int a, int b){
    int min_num = a < b ? a : b;
    return min_num;
}

int max(int a, int b){
    int max_num = a > b ? a : b;
    return max_num;
}

int global_num_threads;
char* global_a;
char* global_b;
int global_a_len;
int global_b_len;

int **score;
int global_max = 0;

pthread_mutex_t mutex;
pthread_barrier_t barrier;

int smith_waterman(int num_threads, char *a, char *b, int a_len, int b_len){
    // make sure a is always a longer text than b
    if(a_len < b_len) {
        char* temp = b;
        b = a;
        a = temp;

        int temp_len = b_len;
        b_len = a_len;
        a_len = temp_len;
    }

    // create a shared memory (score matrix)
    score = new int*[b_len + 1];
    for (int col = 0; col <= b_len; col++) {
        score[col] = new int[a_len + 1];
        for (int row = 0; row <= a_len; row++) {
            score[col][row] = 0;
        }
    }

    // make globally accessible since they are read only
    global_num_threads = num_threads;
    global_a = a;
    global_b = b;
    global_a_len = a_len;
    global_b_len = b_len;

    // allocatie memory for pthreads + mutex
    pthread_t* thread_location;
    thread_location = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init (&barrier, NULL, num_threads);

    for (long t=0; t < num_threads; t++) {
        pthread_create(&thread_location[t], NULL, update_and_max, (void*) t);
    }
    for (long t = 0; t < num_threads; t++) {
        pthread_join(thread_location[t], NULL);
    }
    // create a thread pool to update score matrix, get local max
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);

    free(thread_location);
	return global_max;
}

void* update_and_max(void* rank) {
    long my_rank = (long) rank;    

    int local_max = 0;
    int num_iterations = global_a_len + global_b_len - 1;
    // main loop    
    for (int i=0; i < num_iterations; i++ ) {

        bool is_upper_triangle = i <= global_a_len - 1 ? true : false;
        // global number of job is equal to iteration number + 1 if iteration number does not a_len else it is capped at b_len 
        int global_num_jobs = is_upper_triangle ? min(global_b_len, i + 1) : global_a_len + global_b_len - (i + 1);

        // number of threads to spawn is global_num_jobs if the specified thread count is greater than the global_num_jobs
        int thread_count = global_num_jobs < global_num_threads ? global_num_jobs : global_num_threads;

        // calculate number of jobs for each thread and their initial position
        int local_num_jobs =  global_num_jobs / global_num_threads;
        int remainder  = global_num_jobs % global_num_threads;
        int local_start_r;
        int local_start_c;

        // starting position of zero index thread
        int row_start = is_upper_triangle ? i + 1 : global_a_len;
        int col_start = is_upper_triangle ? 1 : i + 1 - global_a_len + 1;

        bool is_inner_thread = (remainder - my_rank > 0) ? true : false;
        // allocate one job from remainder to threads index less than the remainder count
        local_num_jobs = is_inner_thread ? local_num_jobs + 1 : local_num_jobs;
        
        // allocate the starting row index and col index for each thread
        local_start_r = is_inner_thread ? row_start - (my_rank * local_num_jobs) : row_start - (my_rank * local_num_jobs + remainder);
        local_start_c = is_inner_thread ? col_start + (my_rank * local_num_jobs) : col_start + (my_rank * local_num_jobs + remainder);
        
        for (int i = 0; i < local_num_jobs; i++)
        {   
            int row = local_start_r - i;
            int col = local_start_c + i;
            score[col][row] = max(0,
                            max(score[col - 1][row - 1] + sub_mat(global_a[col - 1], global_b[row - 1]), 
                            max(score[col - 1][row] - GAP,
                                score[col][row - 1] - GAP)));
            local_max = max(score[col][row], local_max);
        }   
        // all threads wait before continuing the iteration
        pthread_barrier_wait(&barrier);
    }
    global_max = max(local_max, global_max);

    return NULL;
}