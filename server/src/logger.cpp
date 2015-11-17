#include "include/logger.h"

logger::logger(std::string path) {
    log_path = path;
    log_file.open(log_path,  std::ofstream::out | std::ofstream::app);
}

logger::~logger() {
    log_file.close();
}
