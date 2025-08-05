// src/server.cpp
#include "../include/server.h"
#include "../include/session.h"
#include <arpa/inet.h> // sockaddr_in, htons
#include <sys/socket.h> // socket, bind, listen, accept
#include <unistd.h> // close
#include <iostream>
#include <list>

// Constructor : create socket, bind, listen
Server::Server(uint16_t port)
 : port_(port), listen_fd_(-1), running_(false), router_(), db_()
{
    //create socket
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0); // IPv4 , TCP , 프로토콜 자동선택
    if (listen_fd_ < 0) {
        perror("socket");
        return;
    }

    // Address structure
    sockaddr_in addr{}; // initialize 0
    addr.sin_family = AF_INET; //  Address Family: Internet - IPv4 주소 체계
    addr.sin_addr.s_addr = INADDR_ANY;       // 모든 IP로부터의 연결을 수락
    addr.sin_port = htons(port_);            // Port number

    // call bind - 소켓, 주소 구조체, 구조체 크기
    if (::bind(listen_fd_, (sockaddr*)(&addr), sizeof(addr)) < 0) {
        perror("bind");
        ::close(listen_fd_);
        listen_fd_ = -1;
        return;
    }

    // call listen - listen(int socket, int backlog) backlog: max capacity for request queue
    if (::listen(listen_fd_, 16) < 0) {
        perror("listen");
        ::close(listen_fd_);
        listen_fd_ = -1;
        return;
    }

    std::cout << "[Server] Listening on port " << port_ << "...\n";
}

// close socket
Server::~Server() {
    if (listen_fd_ >= 0) ::close(listen_fd_);
}

void Server::run() {
    if (listen_fd_ < 0) {
        std::cerr << "[Server] ERROR: Invalid listen_fd_\n";
        return;
    }

    running_ = true;
    acceptLoop();
}

void Server::stop() {
    running_ = false;
}


// add_route 구현: 라우터 인스턴스에 경로와 핸들러 등록
void Server::add_route(
    const std::string& method,
    const std::string& path,
    Router::Handler handler) {
    router_.add_route(method, path, std::move(handler));
}

void Server::acceptLoop() {
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    //sockfd - server socket ,
    // addr - structure that includes client's address information(IP, Port)
    // addrlen - size of addr
    while (running_) {
        //File descriptor => int
        int client_fd = ::accept(listen_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pool_.enqueue([this, client_fd] {
            Session session(client_fd, router_, db_);
            session.handle();
        });

    }
}
