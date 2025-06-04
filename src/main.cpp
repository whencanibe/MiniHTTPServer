// main.cpp 예시

#include "../include/util.h"
#include "../include/server.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// GET handler: “/hello”
Response hello_handler(const Request&) {
    return {200, "text/plain", "Hello, World!"};
}

// POST handler: “/api/echo”
Response echo_handler(const Request& req) {
    try {
        auto j = json::parse(req.body);
        return {200, "application/json", j.dump()};
    } catch (...) {
        return {400, "text/plain", "Bad JSON"};
    }
}

// 정적 파일 제공 핸들러 팩토리
// ex) /static/index.html 요청 시 프로젝트 루트의 static/index.html 파일 반환
std::function<Response(const Request&)> make_static_handler(const std::string& doc_root) {
    return [doc_root](const Request& req) -> Response {
        // req.path 예: "/static/index.html"
        // doc_root은 프로젝트 루트 기준 "static"
        std::string rel = req.path.substr(8); // "/static/" 이후 부분만
        std::string full_path = doc_root + "/" + rel; // "static/index.html"
        bool ok;
        std::string body = read_file(full_path, ok);
        if (!ok) {
            return {404, "text/plain", "File Not Found"};
        }
        // 확장자 추출
        size_t pos = full_path.find_last_of('.');
        std::string ext = (pos == std::string::npos) ? "" : full_path.substr(pos + 1);
        return {200, mime_type(ext), std::move(body)};
    };
}

int main() {
    constexpr uint16_t PORT = 8080; // 16 bit for TCP header
    constexpr auto DOC_ROOT = "static";  // static file directory

    Server server(PORT);

    // GET /hello
    server.add_route("GET", "/hello", hello_handler);

    // POST /api/echo
    server.add_route("POST", "/api/echo", echo_handler);

    // serve static files: /static/*
    auto static_handler = make_static_handler(DOC_ROOT);
    server.add_route("GET", "/static", static_handler);

    server.run();
    return 0;
}