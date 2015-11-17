#ifndef LOGGER_H
#define LOGGER_H

#include "headers.h"

class logger {
private:
    std::string log_path;

    std::ofstream log_file;
public:
    logger(std::string path);
    ~logger();

    void log_start_server(int addr, int port);
};

#endif // LOGGER_H
