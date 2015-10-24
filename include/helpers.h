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

class file_helper {
private:
    std::string root_dir;
public:
    file_helper(std::string path);
    ~file_helper();
};

class message_helper {
private:

public:
    message_helper();

};

class error_helper {
private:

public:
    error_helper();

};

#endif // HELPERS_H
