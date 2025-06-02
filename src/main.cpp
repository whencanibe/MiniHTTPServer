// main.cpp 예시

#include "../include/util.h"
#include "../include/server.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 메모리 상의 간단 CRUD 저장소
std::unordered_map<int, std::string> notes;
int next_id = 1;

// “/notes” 컬렉션 GET 핸들러
Response list_notes(const Request& req) {
    json arr = json::array();
    for (auto& [id, content] : notes) {
        arr.push_back({{"id", id}, {"content", content}});
    }
    return {200, "application/json", arr.dump()};
}

// “/notes” POST 핸들러
Response create_note(const Request& req) {
    try {
        auto j = json::parse(req.body);
        if (!j.contains("content")) {
            return {400, "text/plain", "Missing 'content' field"};
        }
        int id = next_id++;
        notes[id] = j["content"].get<std::string>();
        return {201, "application/json",
                json({{"id", id}, {"content", notes[id]}}).dump()};
    } catch (...) {
        return {400, "text/plain", "Invalid JSON"};
    }
}

// “/notes/{id}” PUT 핸들러 (경로 parsing 단순화: /notes/숫자)
Response update_note(const Request& req) {
    // req.path가 "/notes/3" 같은 형식이라고 가정
    const std::string& path = req.path; // 예: "/notes/3"
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return {400, "text/plain", "Invalid path"};
    }
    int id = std::stoi(path.substr(pos + 1));
    if (notes.find(id) == notes.end()) {
        return {404, "text/plain", "Not Found"};
    }
    try {
        auto j = json::parse(req.body);
        if (!j.contains("content")) {
            return {400, "text/plain", "Missing 'content' field"};
        }
        notes[id] = j["content"].get<std::string>();
        return {200, "application/json",
                json({{"id", id}, {"content", notes[id]}}).dump()};
    } catch (...) {
        return {400, "text/plain", "Invalid JSON"};
    }
}

// “/notes/{id}” DELETE 핸들러
Response delete_note(const Request& req) {
    const std::string& path = req.path; // "/notes/3"
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return {400, "text/plain", "Invalid path"};
    }
    int id = std::stoi(path.substr(pos + 1));
    if (notes.erase(id) == 0) {
        return {404, "text/plain", "Not Found"};
    }
    return {204, "text/plain", ""}; // No Content
}

int main() {
    constexpr uint16_t PORT = 8080;

    Server server(PORT);

    // GET /hello
    server.add_route("GET", "/hello",
                     [](const Request&) -> Response {
                         return {200, "text/plain", "Hello, World!"};
                     });

    // JSON echo 예시
    server.add_route("POST", "/api/echo",
                     [](const Request& req) -> Response {
                         try {
                             auto j = json::parse(req.body);
                             return {200, "application/json", j.dump()};
                         } catch (...) {
                             return {400, "text/plain", "Bad JSON"};
                         }
                     });

    // 전체 노트 조회
    server.add_route("GET", "/notes", list_notes);

    // 새 노트 생성
    server.add_route("POST", "/notes", create_note);

    // PUT /notes/{id}
    server.add_route("PUT", "/notes", update_note);
    // DELETE /notes/{id}
    server.add_route("DELETE", "/notes", delete_note);

    server.run();
    return 0;
}
