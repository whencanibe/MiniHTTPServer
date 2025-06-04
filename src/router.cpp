#include "../include/router.h"

// "METHOD PATH" 형태의 키 생성 헬퍼
static std::string make_key(const std::string& method, const std::string& path) {
    return method + " " + path;
}

void Router::add_route(const std::string& method,
                       const std::string& path,
                       Handler handler) {
    routes_.emplace(make_key(method, path), std::move(handler));
}

//
Response Router::dispatch(const Request& req) const {
    auto exact = routes_.find(make_key(req.method, req.path));
    if (exact != routes_.end()) {
        return exact->second(req);
    }

    // 접두사 매칭 처리 (ex: "/static")
    for (const auto& [key, handler] : routes_) {
        auto pos = key.find(' ');
        std::string method = key.substr(0, pos);
        std::string path = key.substr(pos + 1);

        if (req.method == method && req.path.rfind(path, 0) == 0) {
            // path가 접두사로 일치하면 이 핸들러로 처리
            return handler(req);
        }
    }

    // 못 찾으면 404
    return {404, "text/plain", "Not Found"};
}