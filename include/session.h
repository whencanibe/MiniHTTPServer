#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <unordered_map>

class Router;
class DB;

// 요청(request) 파싱과 응답(response) 직렬화를 위한 구조체 선언
struct Request {
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

struct Response {
    int status;
    std::string content_type;
    std::string body;

    // HTTP 응답 문자열로 직렬화
    std::string toString() const;
};

class Session {
public:
    // 생성자: 클라이언트 소켓 FD를 넘겨받음
    explicit Session(int client_fd, const Router& router, DB& db);

    // 소멸자: FD 닫기
    ~Session();

    // 하나의 요청-응답 사이클 처리
    void handle();

private:
    int client_fd_;
    const Router& router_;
    DB& db_;

    Request  read_request();
    Response dispatch(const Request& req);
    void     send_response(const Response& res);
};

#endif //SESSION_H
