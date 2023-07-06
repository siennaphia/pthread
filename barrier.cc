#include <iostream>
#include <unistd.h>
#include <pthread.h>


// Barrier class acts as a synchronization point for multiple threads
class Barrier {
private:
    // A pthread mutex is used to ensure thread-safe modifications to the barrier's count
    pthread_mutex_t mutex;
    // A pthread condition variable is used to block threads until the barrier is opened
    pthread_cond_t cond;
    // This count keeps track of the number of threads that have entered the barrier
    int count;
    // total number of threads that need to enter the barrier before it opens     int num_threads;


public:
    // Barrier constructor initializes the count to 0 and sets the required number of threads
    Barrier(int n) : count(0), num_threads(n) {
        // mutex is initialized here.
        pthread_mutex_init(&mutex, NULL);
        // condition variable is initialized here.
        pthread_cond_init(&cond, NULL);
    }


    // Barrier destructor cleans up the mutex and condition variable when the barrier is no longer needed
    ~Barrier() {
        // mutex is destroyed here.
        pthread_mutex_destroy(&mutex);
        // condition variable is destroyed here
        pthread_cond_destroy(&cond);
    }


    // Threads call this function to enter the barrier, Theyll be blocked until the required number of threads have all called this function
    void wait() {
        // mutex is locked to prevent concurrent modification of the count
        pthread_mutex_lock(&mutex);
        // One more thread has reached the barrier
        count++;
        // If all the req threads have reached barrier, reset the count and wake up all threads
        if (count == num_threads) {
            count = 0; // Prepare for the next usage
            // Broadcast to all waiting threads to proceed
            pthread_cond_broadcast(&cond);
        } else {
            // If not all threads have reached the barrier, wait on the condition variable
            pthread_cond_wait(&cond, &mutex);
        }
        // After waking up, unlock the mutex
        pthread_mutex_unlock(&mutex);
    }
};

void* thread_fun(void* param);

int main() {
    const int N = 6; // Define the total number of threads

    Barrier barrier(N); // Create the Barrier object

    pthread_t threads[N];
    int thread_ids[N];

    // Create and start the threads
    for (int i = 0; i < N; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_fun, &barrier); // Pass the barrier object as an argument
    }
    // Wait for all threads to finish
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

void* thread_fun(void* param) {
    Barrier* barrier = static_cast<Barrier*>(param); // Cast the parameter back to Barrier pointer

    int thread_id = *static_cast<int*>(param);

    while (true) {
        // Perform some work
        std::cout << "Thread " << thread_id << " is performing work..." << std::endl;
        sleep(1); // Simulating some work being done by the thread


        // Call the barrier's wait() method
        barrier->wait();

        // All threads resume execution after the Nth thread calls wait()
        std::cout << "Thread " << thread_id << " resumed after the barrier." << std::endl;
        // Perform some additional work
        std::cout << "Thread " << thread_id << " is performing additional work..." << std::endl;
        sleep(1); // Simulating additional work being done by the thread
    }
    return nullptr;
}
