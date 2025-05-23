#ifndef SERVER_H
#define SERVER_H
#include <cstdint>

class Server {
private:
    void acceptLoop();
    uint16_t port_;
    int listen_fd_;
    bool running_;
public:
    explicit Server(uint16_t port);
    ~Server();

    void run();
    void stop();
};



#endif //SERVER_H
