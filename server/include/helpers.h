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
    socket_reader(int sock_f, bool &ok_flag);
    ~socket_reader();

    char get(bool &ok_flag);
    std::string get_remain();
};

// ----------------------------------------------

class message_helper {
    friend class socket_reader;
private:
    bool _is_ok;
    std::string remains;

    std::map<std::string, std::string> headers;

    bool is_end(size_t point, std::string &s);

    static timeval timelimit;
public:
    std::string response_headers; //TODO: перенести в private
    message_helper(int timelim);
    ~message_helper();

    static std::string end_of_headers;

    std::string get_headers(int connfd);
    void read_headers(int connfd);
    std::string get_head(std::string key);
    std::string get_path();

    message_helper * combine_headers(std::string part);
    std::string server_info();

    bool is_ok();
    void send_headers(int connfd);
    void send_from_fd(int fd, int sock);
    void send_string(std::string text, int sock);
    void send_post_data(int sock, int fd);
};

// ----------------------------------------------

struct pipes {
    int read[2];
    int write[2];
    int err[2];
    pipes();
    void close_all();
};

class cgi_helper {
private:
    std::string jail_path;
    std::vector<std::string> extentions;
    bool jail_enable;

    std::vector<std::string> get_dependencies(std::string path);
    std::string get_real_path(std::string path);
    std::string isolate(std::string path);
    std::string prepare_script(std::string path);
    const char * const *build_evironment(std::string req);
    void prepare_jail(std::string path);
    void copy_lib(std::string from, std::string to);
    void copy_dir(std::string from, std::string to);
    void copy_devices(std::string path);
    void cgi_exec(std::string path, const char * const *env);
    void identificate(pid_t pid);
    void clear(std::string path);
    int wait_data(int readp, int errp);

public:
    cgi_helper(std::string raw_ext);
    ~cgi_helper();

    bool is_cgi(std::string name);
    void set_jail(std::string jail);
    void run_and_send(std::string path, int fd, message_helper * m_help, bool post);
};

#endif // HELPERS_H
