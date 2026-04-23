#include "server.hpp"
#include "http_parser.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <string>

static const int BUFFER_SIZE = 4096;

Server::Server(int port, size_t num_threads)
    : port_(port), server_fd_(-1), pool_(num_threads) {
    server_fd_ = create_socket();
}

Server::~Server() {
    if (server_fd_ >= 0)
        close(server_fd_);
}

void Server::add_route(const std::string& method,
                       const std::string& path,
                       Handler handler) {
    router_.add(method, path, std::move(handler));
}

void Server::run() {
    std::cout << "Server listening on port " << port_ << "\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        // Bloqueante — espera hasta que llegue una conexión
        int client_fd = accept(server_fd_,
                               reinterpret_cast<sockaddr*>(&client_addr),
                               &client_len);
        if (client_fd < 0) {
            std::cerr << "accept() failed\n";
            continue;
        }

        // Encola la conexión al thread pool — no bloqueamos el accept loop
        pool_.enqueue([this, client_fd] {
            handle_connection(client_fd);
            close(client_fd);
        });
    }
}

void Server::handle_connection(int client_fd) {
    char buffer[BUFFER_SIZE] = {};
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

    if (bytes_read <= 0)
        return;

    std::string raw(buffer, bytes_read);

    HttpParser parser;
    HttpRequest request;

    HttpResponse response;
    if (!parser.parse(raw, request)) {
        response = make_error(400, "Bad Request");
    } else {
        response = router_.dispatch(request);
    }

    std::string serialized = response.serialize();
    write(client_fd, serialized.c_str(), serialized.size());
}

int Server::create_socket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        throw std::runtime_error("socket() failed");

    // Permite reusar el puerto inmediatamente después de reiniciar
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port_);

    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");

    if (listen(fd, 10) < 0)
        throw std::runtime_error("listen() failed");

    return fd;
}