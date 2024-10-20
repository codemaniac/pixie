#include "include/threadpool.h"

// Constructor to initialize the thread pool with a given number of threads
ThreadPool::ThreadPool(size_t threads) :
    stop(false) {
    for (size_t i = 0; i < threads; ++i)
    {
        this->workers.emplace_back([this] { workerThread(); });
    }
}

// Destructor to join all threads
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(this->queueMutex);
        this->stop = true;
    }
    this->condition.notify_all();
    for (std::thread& worker : this->workers)
        worker.join();
}

void ThreadPool::workerThread() {
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(this->queueMutex);
            condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

            if (this->stop && this->tasks.empty())
                return;

            task = std::move(this->tasks.front());
            this->tasks.pop();
        }
        task();
    }
}
