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

int **score;
char* global_a;
char* global_b;
pthread_mutex_t mutex;
int global_max = 0;

struct thread_args {
    int local_num_jobs;
    int local_start_row;
    int local_start_col;
};

int smith_waterman(int num_threads, char *a, char *b, int a_len, int b_len){
    // all threads have access to the given arguments
    // allocate memory for the threads

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

    // make string globally accessible since they are read only
    global_a = a;
    global_b = b;

    // allocatie memory for pthreads + mutex
    pthread_t* thread_location;
    thread_location = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    
    // main loop
    int num_iterations = a_len + b_len - 1;
    for (int i=0; i < num_iterations; i++ ) {

        bool is_upper_triangle = i <= a_len - 1 ? true : false;
        // global number of job is equal to iteration number + 1 if iteration number does not a_len else it is capped at b_len 
        int global_num_jobs = is_upper_triangle ? min(b_len, i + 1) : a_len + b_len - (i + 1);

        // number of threads to spawn is global_num_jobs if the specified thread count is greater than the global_num_jobs
        int thread_count = global_num_jobs < num_threads ? global_num_jobs : num_threads;

        // calculate number of jobs for each thread and their initial position
        int local_num_jobs =  global_num_jobs / thread_count;
        int remainder  = global_num_jobs % thread_count;
        int local_num_jobs_arr[thread_count];
        int local_start_r_arr[thread_count];
        int local_start_c_arr[thread_count];

        // starting position of zero index thread
        int row_start = is_upper_triangle ? i + 1 : a_len;
        int col_start = is_upper_triangle ? 1 : i + 1 - a_len + 1;

        for (int j=0; j < thread_count ; j++) {
            bool is_inner_thread = (remainder - j > 0) ? true : false;
            // allocate one job from remainder to threads index less than the remainder count
            local_num_jobs_arr[j] = is_inner_thread ? local_num_jobs + 1 : local_num_jobs;
            
            // allocate the starting row index and col index for each thread
            local_start_r_arr[j] = is_inner_thread ? row_start - (j * local_num_jobs_arr[j]) : row_start - (j * local_num_jobs_arr[j] + remainder);
            local_start_c_arr[j] = is_inner_thread ? col_start + (j * local_num_jobs_arr[j]) : col_start + (j * local_num_jobs_arr[j] + remainder);
        }
        // create a thread pool to update score matrix, get local max
        for (int t=0; t < thread_count; t++) {
            struct thread_args *args = (thread_args*) malloc(sizeof *args);
            args->local_num_jobs = local_num_jobs_arr[t];
            args->local_start_row = local_start_r_arr[t];
            args->local_start_col = local_start_c_arr[t];
            pthread_create(&thread_location[t], NULL, update_and_max, args);
        }
        for (int t = 0; t < thread_count; t++) {
            pthread_join(thread_location[t], NULL);
        }

    }
    pthread_mutex_destroy(&mutex);
    free(thread_location);
	return global_max;
}

void* update_and_max(void* args) {
    struct thread_args* local_args = (thread_args*) args;
    int local_num_job = local_args->local_num_jobs;
    int local_start_row = local_args->local_start_row;
    int local_start_col = local_args->local_start_col;

    int local_max = 0;

    for (int i = 0; i < local_num_job; i++)
    {   
        int row = local_start_row - i;
        int col = local_start_col + i;
        // printf("%d,%d\n",col,row);
        // printf("%d",score[col][row]);
        score[col][row] = max(0,
                          max(score[col - 1][row - 1] + sub_mat(global_a[col - 1], global_b[row - 1]), 
                          max(score[col - 1][row] - GAP,
                              score[col][row - 1] - GAP)));
        local_max = max(score[col][row], local_max);
    }   
    pthread_mutex_lock(&mutex);
    global_max = max(local_max, global_max);
    pthread_mutex_unlock(&mutex);

    free(local_args);
    // printf("my jobs = %d, start row index = %d start column index = %d \n", local_num_job, local_start_row, local_start_col);
    return NULL;
}