#pragma once

#include <string>
#include <unordered_map>

struct HttpRequest {
    std::string method;   // "GET", "POST", etc.
    std::string path;     // "/events"
    std::string version;  // "HTTP/1.1"
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse {
    int status_code;
    std::string status_text;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    // Serializa el response completo a string listo para enviar por socket
    std::string serialize() const;
};

// Helpers para construir responses comunes
HttpResponse make_response(int code, const std::string& body,
                           const std::string& content_type = "application/json");

HttpResponse make_error(int code, const std::string& message);

class HttpParser {
public:
    // Parsea un request crudo. Retorna false si el formato es inválido.
    bool parse(const std::string& raw, HttpRequest& out);

private:
    bool parse_request_line(const std::string& line, HttpRequest& out);
    bool parse_header_line(const std::string& line, HttpRequest& out);
};