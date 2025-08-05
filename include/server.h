#ifndef SERVER_H
#define SERVER_H
#include <cstdint>

#include "db.h"
#include "router.h"
#include "thread_pool.h"

class Server {
public:
    explicit Server(uint16_t port);
    ~Server();

    void add_route(const std::string& method,
                   const std::string& path,
                   Router::Handler handler);

    void run();
    void stop();
private:
    void acceptLoop();

    uint16_t port_;
    int listen_fd_;
    bool running_;
    Router router_;
    DB db_;
    ThreadPool pool_{8};
};



#endif //SERVER_H
