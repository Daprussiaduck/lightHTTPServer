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
            ThreadPool(int numThreads = 1);
            ~ThreadPool();
            void addTask(const std::function<void()> &task);
            bool busy();
            void stop();
        private:
            void threadLoop();
            std::condition_variable poolCondition; 
            std::mutex poolMutex;
            volatile bool run;
            std::queue<std::function<void()>> tasks;
            std::vector<std::thread> threads;
    };
};

#endif /*__THREAD_POOL_HPP__*/