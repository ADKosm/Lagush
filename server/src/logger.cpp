#include "include/logger.h"

logger::logger(std::string path) {
    log_path = path;
    log_file.open(log_path,  std::ofstream::out | std::ofstream::app);
}

logger::~logger() {
    log_file.close();
}

std::string logger::get_time() {
    time_t tim = time(NULL);
    std::string now = std::string(asctime(localtime(&tim)));
    now.erase(now.end()-1, now.end());
    return now;
}

void logger::add(const char * text, ...) {
    char mess[8192];

    va_list args;
    va_start (args, text);
    vsprintf(mess, text, args);
    va_end(args);

    log_file << get_time() << ":   " << mess << std::endl;
}

void logger::error(const char *text, ...) {
    char mess[8192];

    va_list args;
    va_start (args, text);
    vsprintf(mess, text, args);
    va_end(args);

    std::cerr << "!ERROR - " << get_time() << ":   " << mess << std::endl;
    log_file << "!ERROR - " << get_time() << ":   " << mess << std::endl;
}
