#ifndef LOGGER_H
#define LOGGER_H

#include "headers.h"

class logger {
private:
    std::string log_path;

    std::ofstream log_file;

    std::string get_time();
public:
    logger(std::string path);
    ~logger();

    void add(const char *text, ...);
    void error(const char *text, ...);
};

#endif // LOGGER_H
