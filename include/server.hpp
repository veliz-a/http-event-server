#pragma once

#include "router.hpp"
#include "thread_pool.hpp"
#include <string>

class Server {
public:
    Server(int port, size_t num_threads = 0);
    ~Server();

    // Bloquea hasta recibir señal de parada
    void run();

    // Registra handlers en el router interno
    void add_route(const std::string& method,
                   const std::string& path,
                   Handler handler);

private:
    int port_;
    int server_fd_;        // file descriptor del socket servidor
    ThreadPool pool_;
    Router router_;

    void handle_connection(int client_fd);
    int  create_socket();
};