#ifndef SERVER_H
#define SERVER_H

#include "headers.h"
#include "logger.h"
#include "helpers.h"

class server {
    friend class get_request;
    friend class post_request;
protected:
    int port;
    std::string root_path;

    config_helper* conf_serv;
    file_helper* file_serv;
    message_helper* mess_serv;
    error_helper* err_serv;
    cgi_helper* cgi_serv;

    void handle_request(int connfd);

public:
    server();
    ~server();
    static logger* log;
    void start();
};

#endif // SERVER_H
