#include "include/request_strategy.h"


request_strategy * request_strategy::get_strategy(std::string method) {
    std::string m = method.substr(0, method.find_first_of(' '));
    if(m == "GET") {
        return new get_request;
    } else if(m == "POST") {
        return new post_request;
    }
    return nullptr;
}


void get_request::do_request(server *serv, int connfd) {
    std::cout << "This is a get request" << std::endl;
}

void post_request::do_request(server *serv, int connfd) {
    std::cout << "This is a post request" << std::endl;
}
