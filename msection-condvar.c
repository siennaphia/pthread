#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define M 3  // Maximum concurrent threads within m-section
#define N 10  // Total thread count

// Structure encapsulating shared variables for synchronization
typedef struct {
    pthread_mutex_t mutex;  // Mutex for counter protection
    pthread_cond_t cond;  // Condition variable for m-section access control
    int count;  // Track number of threads in m-section
} sync_data_t;

// Structure to hold the thread ID and sync_data pointer for each thread
typedef struct {
    int tid;  // Thread ID
    sync_data_t* sync_data;  // Pointer to shared sync_data
} thread_data_t;

// Critical function performed within m-section
void critical_function(int thread_id, int count) {
    printf("Thread ID: %d performing critical task. Active threads within m-section: %d\n", thread_id, count);
    sleep(1);  // Simulate processing time
}

// Function to enter m-section, waits if more than M threads are already in
void enter_section(sync_data_t *sync_data) {
    pthread_mutex_lock(&sync_data->mutex);
    while (sync_data->count >= M) {
        pthread_cond_wait(&sync_data->cond, &sync_data->mutex);
    }
    sync_data->count++;
    pthread_mutex_unlock(&sync_data->mutex);
}

// Function to exit m-section, allows another thread to enter
void exit_section(sync_data_t *sync_data) {
    pthread_mutex_lock(&sync_data->mutex);
    sync_data->count--;
    pthread_cond_broadcast(&sync_data->cond);
    pthread_mutex_unlock(&sync_data->mutex);
}

// Work function for each thread to repeatedly enter and exit m-section
void* work(void* arg) {
    // Unpack thread data
    thread_data_t* thread_data = (thread_data_t*)arg;
    int tid = thread_data->tid;
    sync_data_t* sync_data = thread_data->sync_data;

    while (1) {
        enter_section(sync_data);  // Attempt to enter m-section
        critical_function(tid, sync_data->count);  // Perform critical task
        exit_section(sync_data);  // Exit m-section
        sleep(1);  // Simulate external work time
    }

    return NULL;
}

int main() {
    pthread_t threads[N];  // Array to hold thread data
    thread_data_t thread_data[N];  // Array to hold thread data for each thread
    sync_data_t sync_data = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};  // Shared sync_data

    // Create N threads
    for (int i = 0; i < N; i++) {
        thread_data[i].tid = i;
        thread_data[i].sync_data = &sync_data;  // Point to shared sync_data
        pthread_create(&threads[i], NULL, work, &thread_data[i]);  // Create thread
    }

    // Wait for all threads to finish (which never happens in this case)
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up mutex and condition variable
    pthread_mutex_destroy(&sync_data.mutex);
    pthread_cond_destroy(&sync_data.cond);

    return 0;
}
