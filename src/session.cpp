#include "../include/session.h"
#include "../include/router.h"
#include "../include/db.h"
#include <unistd.h> // close(), read(), write()
#include <sys/socket.h> // recv(), send()
#include <sstream> // std::istringstream, std::ostringstream
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstring>


// Converts the Response object to a full HTTP response string.
std::string Response::toString() const {
    //Output string stream
    std::ostringstream oss;
    oss << "HTTP/1.1 " << status << " OK\r\n"
        << "Content-Type: " << content_type << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "\r\n"
        << body;
    return oss.str();
}

// -- Session 생성/소멸 --
Session::Session(int fd, const Router& router, DB& db)
    : client_fd_(fd), router_(router), db_(db) {}

Session::~Session() {
    if (client_fd_ >= 0) ::close(client_fd_);
}

// -- handle(): 요청 읽고, 처리하고, 응답 보내기 --
void Session::handle() {
    // Parse the incoming request
    Request req = read_request();

    // call a handler for the request
    Response res = dispatch(req);

    // print log on console
    std::cout << "[SESSION] Logging request: "
              << req.method << " " << req.path
              << " -> status " << res.status << "\n";

    // save log into database
    db_.insertLog(req.method, req.path, res.status);

    // send the response
    send_response(res);
}

Request Session::read_request() {
    Request req;
    std::string data;
    char buf[1024];
    ssize_t n; // return value from recv - read bytes

    // 헤더 끝(“\r\n\r\n”)까지 누적 읽기
    while (data.find("\r\n\r\n") == std::string::npos) {
        n = ::recv(client_fd_, buf, sizeof(buf), 0);
        if (n <= 0) break;                // 연결 종료 혹은 오류
        data.append(buf, static_cast<size_t>(n)); // append n bytes
    }

    // Request-Line + 헤더 부분 분리
    auto header_end = data.find("\r\n\r\n");
    std::string header_part = (header_end != std::string::npos)
                                ? data.substr(0, header_end + 2)
                                : data;   // “헤더만 온 경우”에도 처리

    std::istringstream iss(header_part);
    std::string line;

    // 첫 줄: Request-Line
    std::getline(iss, line);
    {
        std::istringstream rl(line);
        rl >> req.method >> req.path >> req.version;
    }

    // 헤더 읽기
    while (std::getline(iss, line) && line != "\r") {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            // 앞뒤 공백 제거(간단히)
            if (!val.empty() && val[0] == ' ') val.erase(0, 1);
            req.headers[key] = val;
        }
    }

    // Body가 남아 있을 경우 읽기
    auto it = req.headers.find("Content-Length");
    if (it != req.headers.end()) {
        int content_len = std::stoi(it->second);
        size_t already = data.size() - (header_end + 4);
        // “[헤더끝]\r\n\r\n” 이후 버퍼에 이미 들어온 Body 길이
        if (already < static_cast<size_t>(content_len)) {
            // 부족한 만큼 더 읽기
            int to_read = content_len - static_cast<int>(already);
            req.body.reserve(content_len);
            if (header_end + 4 < data.size()) {
                req.body = data.substr(header_end + 4);
            }
            while (to_read > 0) {
                n = ::recv(client_fd_, buf, std::min<int>(sizeof(buf), to_read), 0);
                if (n <= 0) break;
                req.body.append(buf, static_cast<size_t>(n));
                to_read -= static_cast<int>(n);
            }
        } else {
            // 이미 모든 Body가 버퍼에 함께 읽혀온 경우
            req.body = data.substr(header_end + 4, content_len);
        }
    }

    return req;
}


Response Session::dispatch(const Request& req) {
    return router_.dispatch(req);
}


void Session::send_response(const Response& res) {
    std::string raw = res.toString();
    ::send(client_fd_, raw.data(), raw.size(), 0);
}