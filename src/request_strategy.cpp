#include "include/request_strategy.h"


request_strategy * request_strategy::get_strategy(std::string method) {
    std::string m = method.substr(0, method.find_first_of(' '));
    if(m == "GET") {
        return new get_request;
    }
    else if(m == "POST") {
        return new post_request;
    }
    return nullptr;
}


void get_request::do_request(server *serv, int connfd) {
    int ercode;
    std::string path = serv->file_serv->choose_path(serv->mess_serv->get_path(), ercode);

    serv->mess_serv
            ->combine_headers(serv->file_serv->status(ercode))
            ->combine_headers(serv->file_serv->file_info(path))
            ->combine_headers(serv->mess_serv->server_info())
            ->combine_headers(message_helper::end_of_headers);

    std::cout << serv->mess_serv->response_headers;
    serv->mess_serv->send_headers(connfd);
    serv->file_serv->send_data(path, connfd);
}

void post_request::do_request(server *serv, int connfd) {
    std::cout << "This is a post request" << std::endl;
}
