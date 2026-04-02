#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    // Crea el pool con n workers. En caso de n=0, se usa el número de cores del sistema.
    explicit ThreadPool(size_t num_threads = 0);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool();

    // Encola una tarea. El primer worker libre la ejecuta.
    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_;

    void worker_loop();
};