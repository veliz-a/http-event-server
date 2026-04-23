#pragma once

#include <shared_mutex>
#include <string>
#include <vector>
#include <optional>

struct Event {
    int id;
    std::string type;
    std::string payload;
    std::string timestamp;
};

class EventStore {
public:
    // Agrega un evento. Retorna el id asignado.
    int add(const std::string& type, const std::string& payload);

    // Retorna todos los eventos.
    std::vector<Event> get_all() const;

    // Retorna un evento por id. std::nullopt si no existe.
    std::optional<Event> get_by_id(int id) const;

    // Serializa un evento a JSON manualmente.
    static std::string to_json(const Event& event);

    // Serializa un vector de eventos a JSON.
    static std::string to_json(const std::vector<Event>& events);

private:
    mutable std::shared_mutex mutex_;
    std::vector<Event> events_;
    int next_id_ = 1;

    static std::string current_timestamp();
};