// src/server.cpp
#include "../include/server.h"
#include "../include/session.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

Server::Server(uint16_t port)
 : port_(port), listen_fd_(-1), running_(false)
{
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    // bind/listen 생략…
}

Server::~Server() {
    if (listen_fd_ >= 0) ::close(listen_fd_);
}

void Server::run() {
    running_ = true;
    acceptLoop();
}

void Server::stop() {
    running_ = false;
}

void Server::acceptLoop() {
    while (running_) {
        int client_fd = ::accept(listen_fd_, nullptr, nullptr);
        if (client_fd < 0) continue;
        Session{client_fd}.handle();  // RAII 기반 세션 처리
    }
}
