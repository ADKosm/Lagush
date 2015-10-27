#ifndef SERVER_H
#define SERVER_H

#include "headers.h"
#include "logger.h"
#include "helpers.h"

class server {
    friend class request_strategy;
protected:
    int port;
    std::string root_path;

    logger* log;
    config_helper* conf_serv;
    file_helper* file_serv;
    message_helper* mess_serv;
    error_helper* err_serv;

    void handle_request(int connfd);

public:
    server();
    ~server();
    void start();
};

#endif // SERVER_H
