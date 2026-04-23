#pragma once

#include "http_parser.hpp"
#include <functional>
#include <string>
#include <unordered_map>

using Handler = std::function<HttpResponse(const HttpRequest&)>;

class Router {
public:
    // Registra un handler para un método + ruta específicos
    void add(const std::string& method,
             const std::string& path,
             Handler handler);

    // Despacha el request al handler correcto
    // Retorna 404 si la ruta no existe, 405 si el método no está permitido
    HttpResponse dispatch(const HttpRequest& request) const;

private:
    // Clave: "METHOD /path"  →  "GET /events"
    std::unordered_map<std::string, Handler> routes_;
};