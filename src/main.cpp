#include "thread_pool.hpp"
#include <iostream>
#include <chrono>

int main() {
    ThreadPool pool(4);

    for (int i = 0; i < 8; ++i) {
        pool.enqueue([i] {
            std::cout << "Task " << i
                      << " running on thread "
                      << std::this_thread::get_id()
                      << "\n";
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
        });
    }

    // Pool se destruye aquí — el destructor espera que terminen todas las tareas
    std::cout << "All tasks enqueued\n";
    return 0;
}