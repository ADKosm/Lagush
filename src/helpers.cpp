#include "include/helpers.h"

// ------- Config Helper ----------

config_helper::config_helper() {
    config_path = "";

    std::vector<std::string> possible_paths = {
      "config",
      "../config",
      "../CGI/config",
      "../Lagush/config"
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

std::string file_helper::status(int ercode) { //TODO: сделать нормальную обработку ошибок
    std::string result = "HTTP/1.1 ";
    if(ercode == 0) {
        result.append("200 OK");
    } else {
        result.append("404 Not Found");
    }
    return result + "\r\n";
}

std::string file_helper::file_info(std::string path) { //TODO: доделать нормальные хедеры
    std::string result;
    struct stat buff;
    int st = stat(path.c_str(), &buff);
    if(st == 0 && S_ISREG(buff.st_mode)) {

        result.append("Content-Length: " + std::to_string(buff.st_size) + "\r\n");
        result.append("Connection: close\r\n");

    } else {
        return file_info(error_helper::get_resp("404")); // TODO: потенциально лишняя проверка
    }

    return result;
}

void file_helper::send_data(std::string path, int fd) {
    int filefd = open(path.c_str(), O_RDONLY);
    int BLOCK_SIZE = 8192;
    char buffer[BLOCK_SIZE];
    int read_bytes = 0;
    int write_bytes = 0;

    while( (read_bytes = read(filefd, buffer, BLOCK_SIZE)) > 0 ) {
        while(write_bytes < read_bytes) {
            write_bytes += send(fd, buffer + write_bytes, read_bytes - write_bytes , 0);
        }
        write_bytes = 0;
    }
}

std::string file_helper::choose_path(std::string path, int &ercode) {
    path = root_dir + path;
    struct stat buff;
    int st = stat(path.c_str(), &buff);
    if(st == 0 && S_ISREG(buff.st_mode)) {
        return path;
    } else {
        std::string index_path = path + "index.html";
        st = stat(index_path.c_str(), &buff);
        if(st == 0) {
            return index_path;
        } else {
            return error_helper::get_resp("404");
        }
    }
}

// ------- Message Helper ---------

std::string message_helper::end_of_headers = "\r\n";

message_helper::message_helper() {

}

message_helper::~message_helper() {

}

bool message_helper::is_end(size_t point, std::string &s) {
    return ( s[point-3] == '\r' && s[point-2] == '\n' && s[point-1] == '\r' && s[point] == '\n' );
}

std::string message_helper::get_headers(int connfd) {
    socket_reader reader(connfd, _is_ok);
    if(!_is_ok) exit(0); // считать хедеры не удалось - закрываем соединение
    std::string headers;
    size_t p = 0;

    for(; p <= 3; p++) headers.push_back(reader.get(_is_ok));
    p--;

    for(; !is_end(p, headers); p++) {
        headers.push_back(reader.get(_is_ok));
        if(!_is_ok) break;
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

std::string message_helper::get_path() {
    std::string m = get_head("method");
    size_t begin = m.find(' ');
    size_t end = m.find(' ', begin + 1);
    return m.substr(begin + 1, end - begin - 1);
}

message_helper * message_helper::combine_headers(std::string part) {
    response_headers.append(part);
    return this;
}

std::string message_helper::server_info() {
    std::string result;
    result.append("Server: Lagush/0.1 (Linux)\r\n");
    return result;
}

void message_helper::send_headers(int connfd) {
    size_t bytes = 0;
    size_t BLOCK_SIZE = 4096;
    while(bytes < response_headers.size()) {
        bytes += send(connfd, response_headers.c_str() + bytes, std::min( BLOCK_SIZE, response_headers.size() - bytes ), 0);
    }
    response_headers.clear();
}

bool message_helper::is_ok() {
    return _is_ok;
}

// --------- Error Helper ----------

std::string error_helper::responses_path = "";

error_helper::error_helper(std::string p) {
    responses_path = p;
}

error_helper::~error_helper(){

}

std::string error_helper::get_resp(std::string code) {
    return responses_path + "/" + code + ".html";
}

// ---------- socket_reader -----------

socket_reader::socket_reader(int sock_f, bool &ok_flag) {
    sock_fd = sock_f;
    pointer = 0;
    buffer = new char[BUFF_SIZE];
    bytes = recv(sock_fd, buffer, BUFF_SIZE, 0);
    ok_flag = (bytes > 0);
    std::cout << "OK FLAG! = " << ok_flag << std::endl;
}

socket_reader::~socket_reader() {
    delete[] buffer;
}

char socket_reader::get(bool &ok_flag) {
    if(pointer == bytes) {
        pointer = 0;
        bytes = recv(sock_fd, buffer, BUFF_SIZE, 0);
    }

    ok_flag = (bytes > 0);
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
