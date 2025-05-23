#include "../include/server.h"

Server::Server(uint16_t port) : port_(port), listen_fd_(-1), running_(false)
{

}
