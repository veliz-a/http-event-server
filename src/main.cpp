#include "event_store.hpp"
#include "server.hpp"
#include <iostream>
#include <sstream>

int main() {
    EventStore store;
    Server server(8080, 4);

    // GET /events — retorna todos los eventos
    server.add_route("GET", "/events", [&store](const HttpRequest&) {
        auto events = store.get_all();
        return make_response(200, EventStore::to_json(events));
    });

    // GET /events/1 — retorna un evento por id
    server.add_route("GET", "/events/1", [&store](const HttpRequest& req) {
        // Extraer id del path: "/events/42" → 42
        size_t last_slash = req.path.rfind('/');
        if (last_slash == std::string::npos)
            return make_error(400, "Invalid path");

        int id;
        try {
            id = std::stoi(req.path.substr(last_slash + 1));
        } catch (...) {
            return make_error(400, "Invalid id");
        }

        auto event = store.get_by_id(id);
        if (!event)
            return make_error(404, "Event not found");

        return make_response(200, EventStore::to_json(*event));
    });

    // POST /events — registra un nuevo evento
    server.add_route("POST", "/events", [&store](const HttpRequest& req) {
        if (req.body.empty())
            return make_error(400, "Body required");

        int id = store.add("generic", req.body);

        std::string json = "{\"id\":" + std::to_string(id) +
                           ",\"status\":\"created\"}";
        return make_response(201, json);
    });

    server.run();
    return 0;
}