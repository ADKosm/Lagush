#ifndef HELPERS_H
#define HELPERS_H

#include "headers.h"

class config_helper {
private:
    std::string config_path;

    std::map<std::string, std::string> parameters;
public:
    config_helper();
    ~config_helper();

    std::string get_param(std::string name);
    int get_iparam(std::string name);
};

// ----------------------------------------------

class file_helper {
private:
    std::string root_dir;

    std::string complete_path(std::string path);
public:
    file_helper(std::string path);
    ~file_helper();

    std::string status(int ercode);
    std::string file_info(std::string path);

    std::string choose_path(std::string path, int &ercode);

    void send_data(std::string path, int fd);
};

// ----------------------------------------------

class error_helper {
private:

    static std::string responses_path;
public:
    error_helper(std::string p);
    ~error_helper();

    static std::string get_resp(std::string code);

};

// ----------------------------------------------

class socket_reader {
private:
    const int BUFF_SIZE = 4048;
    size_t bytes;
    size_t pointer;
    char * buffer;
    int sock_fd;
public:
    socket_reader(int sock_f);
    ~socket_reader();

    char get();
    std::string get_remain();
};

// ----------------------------------------------

class message_helper {
private:
    std::string remains;

    std::map<std::string, std::string> headers;

    bool is_end(size_t point, std::string &s);
public:
    std::string response_headers; //TODO: перенести в private
    message_helper();
    ~message_helper();

    static std::string end_of_headers;

    std::string get_headers(int connfd);
    void read_headers(int connfd);
    std::string get_head(std::string key);
    std::string get_path();

    message_helper * combine_headers(std::string part);
    std::string server_info();

    void send_headers(int connfd);
};


#endif // HELPERS_H
