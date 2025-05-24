#include "../include/session.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sstream>
#include <iostream>

std::string Response::toString() const {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " OK\r\n"
        << "Content-Type: " << content_type << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "\r\n"
        << body;
    return oss.str();
}

// -- Session 생성/소멸 --
Session::Session(int fd)
    : client_fd_(fd) {}

Session::~Session() {
    if (client_fd_ >= 0) ::close(client_fd_);
}

// -- handle(): 요청 읽고, 처리하고, 응답 보내기 --
void Session::handle() {
    // 1) 요청 파싱
    Request req = read_request();

    // 2) 경로에 따라 핸들러 호출
    Response res = dispatch(req);

    // 3) 응답 전송
    send_response(res);
}

// -- read_request(): 간단한 라인 기반 파서 예시 --
Request Session::read_request() {
    Request req;
    std::string line;
    std::ostringstream body_buf;
    char buf[1024];

    // ① Request-Line 읽기
    recv(client_fd_, buf, sizeof(buf), 0);
    std::istringstream iss(buf);
    std::getline(iss, line);
    std::istringstream rl(line);
    rl >> req.method >> req.path >> req.version;

    // ② 헤더 읽기 (간단히 빈 줄 전까지)
    while (std::getline(iss, line) && line != "\r") {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            req.headers[key] = val;
        }
    }

    // ③ 바디 읽기 (Content-Length 있을 때만)
    auto it = req.headers.find("Content-Length");
    if (it != req.headers.end()) {
        int len = std::stoi(it->second);
        std::vector<char> body(len);
        recv(client_fd_, body.data(), len, 0);
        req.body.assign(body.begin(), body.end());
    }

    return req;
}

// -- dispatch(): 라우팅 테이블 대신 if-else 예시 --
Response Session::dispatch(const Request& req) {
    if (req.method == "GET" && req.path == "/hello") {
        return {200, "text/plain", "Hello"};
    }
    // 그 외 404
    return {404, "text/plain", "Not Found"};
}

// -- send_response(): 직렬화된 문자열 전송 --
void Session::send_response(const Response& res) {
    std::string raw = res.toString();
    ::send(client_fd_, raw.data(), raw.size(), 0);
}