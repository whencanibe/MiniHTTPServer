#ifndef ROUTER_H
#define ROUTER_H

#include <string>
#include <functional>
#include <unordered_map>
#include "session.h"   // Request, Response 정의

class Router {
public:
    // Handler 타입: Request → Response
    using Handler = std::function<Response(const Request&)>;

    // 라우트 등록: ex) add_route("GET", "/hello", hello_handler);
    void add_route(const std::string& method,
                   const std::string& path,
                   Handler handler);

    // Request를 받아서 적절한 Handler 실행
    Response dispatch(const Request& req) const;

private:
    // 키는 "METHOD PATH" (예: "GET /hello")
    std::unordered_map<std::string, Handler> routes_;
};


#endif //ROUTER_H
