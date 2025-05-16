#ifndef __THREAD_POOL_HPP__
#define __THREAD_POOL_HPP__

#include <condition_variable>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>

namespace lightHTTPServer {
    /**
     * Class to manage multithreading for the HTTP Server
     * Huge thanks to this Stack Overflow answer: https://stackoverflow.com/a/32593825
     */
    class ThreadPool {
        public:
            /**
             * @brief Creates a thread pool with specified number of threads
             * 
             * @param The number of threads to use
             */
            ThreadPool(int numThreads = 1);

            /**
             * @brief Destroys the the thread pool and stops the workers
             */
            ~ThreadPool();

            /**
             * @brief Adds a task to the pool to execute
             * 
             * @param task The task/function to execute asynchronously
             */
            void addTask(const std::function<void()> &task);

            /**
             * @brief Is the pool busy
             * 
             * @return True if the pool is busy, false if not
             */
            bool busy();

            /**
             * @brief Stops the pool and worker threads
             */
            void stop();

        private:
            /**
             * @brief The loop that the threads use busy wait for new tasks
             */
            void threadLoop();

            /**
             * @brief The condition variable used to notify the pool workers
             */
            std::condition_variable poolCondition; 

            /**
             * @brief The mutex to allow for thread safe access to the tasks queue
             */
            std::mutex poolMutex;

            /**
             * @brief Should the pool keep running?
             */
            volatile bool run;

            /**
             * @brief Queue of the tasks for the workers run 
             */
            std::queue<std::function<void()>> tasks;

            /**
             * @brief The worker threads used to compute/execute tasks
             */
            std::vector<std::thread> threads;

    };
};

#endif /*__THREAD_POOL_HPP__*/