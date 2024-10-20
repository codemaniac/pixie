#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
   private:
    // Thread workers
    std::vector<std::thread> workers;

    // Task queue
    std::queue<std::function<void()>> tasks;

    // Synchronization
    std::mutex              queueMutex;
    std::condition_variable condition;
    bool                    stop;

    void workerThread();

   public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>;
};

// Function to add tasks to the thread pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
    using returnType = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<returnType()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<returnType> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

#endif