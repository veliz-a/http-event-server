#include "router.hpp"

void Router::add(const std::string& method,
                 const std::string& path,
                 Handler handler) {
    routes_[method + " " + path] = std::move(handler);
}

HttpResponse Router::dispatch(const HttpRequest& request) const {
    // Buscar match exacto primero
    std::string key = request.method + " " + request.path;
    auto it = routes_.find(key);

    if (it != routes_.end())
        return it->second(request);

    // Verificar si la ruta existe con otro método (405 vs 404)
    for (const auto& [route_key, handler] : routes_) {
        size_t space = route_key.find(' ');
        std::string route_path = route_key.substr(space + 1);

        if (route_path == request.path)
            return make_error(405, "Method Not Allowed");
    }

    return make_error(404, "Not Found");
}