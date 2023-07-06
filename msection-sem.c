#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define M 3  // Maximum threads that can enter the m-section (critical section)
#define N 10  // Total number of threads

sem_t sem;  // Declare a semaphore

// Function for the critical work that will be done inside the m-section (critical section)
void doCriticalWork(int thread_id, int m_section_threads) {
    // Print the thread ID and the current number of threads in the m-section
    printf("Thread %d is doing critical work. Number of threads in m-section: %d\n", thread_id, m_section_threads);
    sleep(1);  // Simulate time spent doing work
}

// Function to enter m-section, will block if more than M threads are in m-section
void enter(sem_t* s) {
    sem_wait(s);  // Decrement the semaphore count, block if count is 0
}

// Function to leave m-section, allowing another thread to enter
void leave(sem_t* s) {
    sem_post(s);  // Increment the semaphore count, potentially unblocking a waiting thread
}

// Thread function which repeatedly enters and leaves the m-section (critical section)
void* doWork(void* arg) {
    int tid = *(int*)arg;  // Get the thread ID from the argument
    while (1) {
        enter(&sem);  // Try to enter the m-section
        int sem_count;
        sem_getvalue(&sem, &sem_count);  // Get the current value of the semaphore
        doCriticalWork(tid, M - sem_count);  // Do the critical work
        leave(&sem);  // Leave the m-section
        sleep(1);  // Simulate doing more work outside the m-section
    }
    return NULL;
}


int main() {
    pthread_t threads[N];  // Array to hold pthread identifiers
    int thread_ids[N];  // Array to hold custom thread IDs

    // Initialize semaphore with a value of M
    sem_init(&sem, 0, M);

    // Create N threads
    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        // Create a new thread that starts executing the doWork function, pass the address of thread id
        pthread_create(&threads[i], NULL, doWork, &thread_ids[i]);
    }

    // Wait for all threads to finish (which will not happen in this case due to the infinite loop in doWork)
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up the semaphore
    sem_destroy(&sem);

    return 0;
}
