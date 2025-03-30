#include "ThreadPool.hpp"

namespace lightHTTPServer {
    ThreadPool::ThreadPool(int numThreads){
        this -> run = true;
        if (numThreads > std::thread::hardware_concurrency() || numThreads < 0){
            numThreads = std::thread::hardware_concurrency();
        }
        for (int i = 0; i < numThreads; i++){
            threads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
        }
    }

    ThreadPool::~ThreadPool(){
        this -> stop();
    }

    void ThreadPool::addTask(const std::function<void()>& task) {
        {
            std::unique_lock<std::mutex> lock(this -> poolMutex);
            (this -> tasks).push(task);
        }
        (this -> poolCondition).notify_one();
    }

    void ThreadPool::threadLoop(){
        while (this -> run){
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(poolMutex);
                poolCondition.wait(lock, [this] {
                    return !(this -> tasks).empty() || !(this -> run);
                });
                if (!(this -> run)) {
                    return;
                }
                task = (this -> tasks).front();
                (this -> tasks).pop();
            }
            task();
        }
    }

    bool ThreadPool::busy() {
        bool poolBusy;
        {
            std::unique_lock<std::mutex> lock(this -> poolMutex);
            poolBusy = !((this -> tasks).empty());
        }
        return poolBusy;
    }

    void ThreadPool::stop() {
        {
            std::unique_lock<std::mutex> lock(this -> poolMutex);
            this -> run = false;
        }
        (this -> poolCondition).notify_all();
        for (std::thread &active_thread : (this -> threads)) {
            // std::cout << "Joining thread: " << active_thread.get_id() << std::endl;
            active_thread.join();
        }
        (this -> threads).clear();
    }
    
};