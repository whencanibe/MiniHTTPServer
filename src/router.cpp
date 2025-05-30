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

Response Router::dispatch(const Request& req) const {
    auto it = routes_.find(make_key(req.method, req.path));
    if (it != routes_.end()) {
        return it->second(req);
    }
    // 미등록 경로 → 404
    return {404, "text/plain", "Not Found"};
}