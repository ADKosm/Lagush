#ifndef REQUEST_STRATEGY_H
#define REQUEST_STRATEGY_H

#include "include/server.h"

class request_strategy {
public:
    virtual ~request_strategy() {}
    void virtual do_request(server * serv, int connfd) = 0;
    static request_strategy * get_strategy(std::string method);
};

class get_request : public request_strategy {
public:
    ~get_request() {}
    void virtual do_request(server * serv, int connfd);
};

class post_request : public request_strategy {
public:
    ~post_request() {}
    void virtual do_request(server * serv, int connfd);
};

#endif // REQUEST_STRATEGY_H
