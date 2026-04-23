#include "event_store.hpp"
#include <chrono>
#include <iomanip>
#include <mutex> 
#include <sstream>

// ─── Escritura ────────────────────────────────────────────────────────────────

int EventStore::add(const std::string& type, const std::string& payload) {
    std::unique_lock<std::shared_mutex> lock(mutex_);

    Event event;
    event.id = next_id_++;
    event.type = type;
    event.payload = payload;
    event.timestamp = current_timestamp();

    events_.push_back(event);
    return event.id;
}

// ─── Lectura ──────────────────────────────────────────────────────────────────

std::vector<Event> EventStore::get_all() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return events_;
}

std::optional<Event> EventStore::get_by_id(int id) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);

    for (const auto& event : events_) {
        if (event.id == id)
            return event;
    }
    return std::nullopt;
}

// ─── Serialización JSON ───────────────────────────────────────────────────────

// Función helper para escapar strings JSON
static std::string json_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

std::string EventStore::to_json(const Event& event) {
    return "{"
           "\"id\":" + std::to_string(event.id) + ","
           "\"type\":\"" + json_escape(event.type) + "\","
           "\"payload\":\"" + json_escape(event.payload) + "\","
           "\"timestamp\":\"" + event.timestamp + "\""
           "}";
}

std::string EventStore::to_json(const std::vector<Event>& events) {
    std::string json = "[";
    for (size_t i = 0; i < events.size(); ++i) {
        json += to_json(events[i]);
        if (i + 1 < events.size())
            json += ",";
    }
    json += "]";
    return json;
}

// ─── Timestamp ────────────────────────────────────────────────────────────────

std::string EventStore::current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream ss;
    ss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}