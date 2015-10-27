#include "include/helpers.h"

// ------- Config Helper ----------

config_helper::config_helper() {
    config_path = "";

    std::vector<std::string> possible_paths = {
      "config",
      "../config",
      "../CGI/config"
    };
    for(auto path : possible_paths) {
        std::ifstream f(path);
        if(f.good()) {
            config_path = path;
            break;
        }
        f.close();
    }

    if(config_path == "") {
        throw std::runtime_error("Config file not found!");
    }

    std::ifstream config_file;
    config_file.open(config_path);

    std::string param;

    while(std::getline(config_file, param)) {
        if(param[0] != '#' && param != "") {
            std::string key = param.substr(0, param.find_first_of(':'));
            std::string val = param.substr(param.find_first_of(':')+1);
            parameters[key] = val;
        }
    }

    config_file.close();
}

std::string config_helper::get_param(std::string name) {
    return parameters.at(name);
}

int config_helper::get_iparam(std::string name) {
    return std::stoi(parameters.at(name));
}

config_helper::~config_helper() {
}

// ------ File Helper -------

file_helper::file_helper(std::string path) {
    root_dir = path;
}

file_helper::~file_helper() {
}

// ------- Message Helper ---------

message_helper::message_helper() {

}

message_helper::~message_helper() {

}

bool message_helper::is_end(size_t point, std::string &s) {
    return ( s[point-3] == '\r' && s[point-2] == '\n' && s[point-1] == '\r' && s[point] == '\n' );
}

std::string message_helper::get_headers(int connfd) {
    socket_reader reader(connfd);
    std::string headers;
    size_t p = 0;

    for(; p <= 3; p++) headers.push_back(reader.get());
    p--;

    for(; !is_end(p, headers); p++) {
        headers.push_back(reader.get());
    }

    remains = reader.get_remain();

    return headers;
}

void message_helper::read_headers(int connfd) {
    std::istringstream iss(get_headers(connfd));
    std::string line;
    std::getline(iss, line);
    headers[std::string("method")] = line;
    while(std::getline(iss, line)) {
        std::string key = line.substr(0, line.find_first_of(':'));
        std::string val = line.substr(line.find_first_of(':')+1);
        headers[key] = val;
    }
}

std::string message_helper::get_head(std::string key) {
    return headers[key];
}

// --------- Error Helper ----------

error_helper::error_helper() {

}

// ---------- socket_reader -----------

socket_reader::socket_reader(int sock_f) {
    sock_fd = sock_f;
    pointer = 0;
    buffer = new char[BUFF_SIZE];
    bytes = recv(sock_fd, buffer, BUFF_SIZE, 0);
}

socket_reader::~socket_reader() {
    delete[] buffer;
}

char socket_reader::get() {
    if(pointer == bytes) {
        pointer = 0;
        bytes = recv(sock_fd, buffer, BUFF_SIZE, 0);
    }

    if(bytes <= 0) {
        return '\0';
    }

    return buffer[pointer++];
}

std::string socket_reader::get_remain() {
    std::string result;
    result.append(buffer + pointer, buffer + bytes);
    return result;
}
