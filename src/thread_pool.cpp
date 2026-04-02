#include "thread_pool.hpp"
#include <thread>

ThreadPool::ThreadPool(size_t num_threads) : stop_(false) {
    if (num_threads == 0)
        num_threads = std::thread::hardware_concurrency();

    for (size_t i = 0; i < num_threads; ++i)
        workers_.emplace_back(&ThreadPool::worker_loop, this);
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();

    for (auto& worker : workers_)
        worker.join();
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        tasks_.push(std::move(task));
    }
    condition_.notify_one();
}

void ThreadPool::worker_loop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [this] {
                return stop_ || !tasks_.empty();
            });

            if (stop_ && tasks_.empty())
                return;

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        task();
    }
}