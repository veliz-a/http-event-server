#include "http_parser.hpp"
#include <sstream>

// ─── HttpResponse ────────────────────────────────────────────────────────────

std::string HttpResponse::serialize() const {
    std::string out;
    out += "HTTP/1.1 " + std::to_string(status_code) + " " + status_text + "\r\n";

    for (const auto& [key, value] : headers)
        out += key + ": " + value + "\r\n";

    out += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    out += "\r\n";
    out += body;
    return out;
}

HttpResponse make_response(int code, const std::string& body,
                           const std::string& content_type) {
    HttpResponse res;
    res.status_code = code;
    res.body = body;
    res.headers["Content-Type"] = content_type;

    switch (code) {
        case 200: res.status_text = "OK"; break;
        case 201: res.status_text = "Created"; break;
        case 400: res.status_text = "Bad Request"; break;
        case 404: res.status_text = "Not Found"; break;
        case 405: res.status_text = "Method Not Allowed"; break;
        case 500: res.status_text = "Internal Server Error"; break;
        default:  res.status_text = "Unknown"; break;
    }
    return res;
}

HttpResponse make_error(int code, const std::string& message) {
    return make_response(code, "{\"error\":\"" + message + "\"}");
}

// ─── HttpParser ──────────────────────────────────────────────────────────────

bool HttpParser::parse(const std::string& raw, HttpRequest& out) {
    // Separar headers del body en \r\n\r\n
    const std::string separator = "\r\n\r\n";
    size_t header_end = raw.find(separator);
    if (header_end == std::string::npos)
        return false;

    std::string header_section = raw.substr(0, header_end);
    out.body = raw.substr(header_end + separator.size());

    // Parsear línea por línea
    std::istringstream stream(header_section);
    std::string line;
    bool first_line = true;

    while (std::getline(stream, line)) {
        // getline no consume \r — lo removemos manualmente
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty()) continue;

        if (first_line) {
            if (!parse_request_line(line, out))
                return false;
            first_line = false;
        } else {
            if (!parse_header_line(line, out))
                return false;
        }
    }

    return !out.method.empty() && !out.path.empty();
}

bool HttpParser::parse_request_line(const std::string& line, HttpRequest& out) {
    // Formato: "GET /events HTTP/1.1"
    std::istringstream ss(line);
    ss >> out.method >> out.path >> out.version;
    return !out.method.empty() && !out.path.empty();
}

bool HttpParser::parse_header_line(const std::string& line, HttpRequest& out) {
    // Formato: "Content-Type: application/json"
    size_t colon = line.find(':');
    if (colon == std::string::npos)
        return false;

    std::string key = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    // Remover espacios al inicio del value
    size_t value_start = value.find_first_not_of(' ');
    if (value_start != std::string::npos)
        value = value.substr(value_start);

    out.headers[key] = value;
    return true;
}