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
    close(filefd);
}

std::string file_helper::choose_path(std::string path, int &ercode) {
    path = root_dir + path;
    path = path.substr(0, path.find_first_of('?'));
    ercode = 0;
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
            ercode = 404;
            return error_helper::get_resp("404");
        }
    }
}

// ------- Message Helper ---------

std::string message_helper::end_of_headers = "\r\n";
timeval message_helper::timelimit = {0, 0};

message_helper::message_helper(int timelim) {
    message_helper::timelimit.tv_sec = timelim;
    message_helper::timelimit.tv_usec = 0;
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

    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(sock_fd, &rset);
    int ready = select(sock_fd+1, &rset, NULL, NULL, &message_helper::timelimit);

    if(ready == 0) {
        std::cout << "Time is over!" << std::endl;
        ok_flag = false; // истек лимит ожидания
    } else {
        bytes = recv(sock_fd, buffer, BUFF_SIZE, 0);

        ok_flag = (bytes > 0);
    }
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

// ------------- cgi_helper --------------------

cgi_helper::cgi_helper(std::string raw_ext) {
    std::istringstream iss(raw_ext);
    std::string ext;
    while(iss >> ext) {
        extentions.push_back(ext);
    }

    jail_enable = false;
}

cgi_helper::~cgi_helper() {

}

void cgi_helper::set_jail(std::string jail) {
    jail_enable = true;
    jail_path = jail;
    prepare_jail(jail_path);
}

bool cgi_helper::is_cgi(std::string name) {
    for(unsigned int i = 0; i < extentions.size(); i++) {
        if(name.substr(name.size() - extentions[i].size()) == extentions[i]) return true;
    }
    return false;
}

std::string cgi_helper::isolate(std::string path) {
    if(!jail_enable) return path;

    std::string path_in_jail = prepare_script(path);

    return path_in_jail;
}

void cgi_helper::cgi_exec(std::string path, const char * const *env) {
    if(jail_enable) {
        execle("cgi_module", "cgi_module", "0", jail_path.c_str(), path.c_str(), NULL, env);
    } else {
        execle(path.c_str(), path.c_str(), NULL, env);
    }
}

std::string cgi_helper::prepare_script(std::string path) {

    std::string path_in_jail = path.substr(path.rfind('/')); // копирование самого скрипта
    copy_lib(path, jail_path + path_in_jail);

    std::cout << "AAAAYYYAAAAA!!: " << std::endl;
    for(auto i : prepared) {
        std::cout << i << std::endl;
    }

    if(std::count(prepared.begin(), prepared.end(), shared_string(path.c_str())) > 0) return path_in_jail; // TODO: на данный момент не работает - доделать

    std::string shebang = get_real_path(path);
    if(shebang != path) copy_lib(shebang, jail_path + shebang); // копирование интерпретатора
    auto depend = get_dependencies(path);                       // и библиотек для него
    for(auto lib : depend) copy_lib(lib, jail_path + lib);

    if(shebang != path) { // подтащить соответствующие библиотеки для скрипта

        std::vector<std::string> names;
        names.push_back(shebang.substr(shebang.rfind('/')+1));

        struct stat sb;
        char * linkname;
        if( lstat(shebang.c_str(), &sb) == 0) {
            linkname = new char[sb.st_size + 1];
            size_t r = readlink(shebang.c_str(), linkname, sb.st_size + 1);
            linkname[r] = '\0';
            names.push_back(std::string(linkname));
            delete[] linkname;
        }

        std::vector<std::string> dirs { // TODO: прикрутить больше возможных папок с библиотеками
            "/usr/lib",
            "/usr/share"
        };



        for(std::string dir : dirs) {
            for(std::string name : names) {
                copy_dir(dir+"/"+name, jail_path + dir+"/"+name );
            }
        }
    }

    prepared.push_back(shared_string(path.c_str()));

    return path_in_jail;
}

void cgi_helper::copy_dir(std::string from, std::string to) {
    std::cout << from << ' ' << to << std::endl;

    DIR * folder = opendir(from.c_str());
    struct dirent * file;

    if(folder == NULL) return;

    mkdir(to.c_str(), 0777);

    struct stat file_info;
    while( (file = readdir(folder)) != NULL ) {
        std::string cur_path;
        cur_path.append(from);
        cur_path.append("/");
        cur_path.append(file->d_name);

        int res = stat(cur_path.c_str(), &file_info);
        if(S_ISDIR(file_info.st_mode)) {
            if( std::string(file->d_name) != "." && std::string(file->d_name) != ".."  ) copy_dir(cur_path, (to + "/" + std::string(file->d_name)) );
        } else {
            copy_lib(cur_path, (to + "/" + std::string(file->d_name)) );
        }
    }
}

void cgi_helper::clear(std::string path) {
    if(!jail_enable) return;
    std::string path_in_jail = jail_path + path.substr(path.rfind('/'));
    int status = unlink(path_in_jail.c_str());
}

void cgi_helper::identificate(pid_t pid) {
    if(jail_enable) {
        sigset_t sigset;
        siginfo_t sig;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigset, NULL);

        struct timespec limit;
        limit.tv_sec = 10; // 10 секунд на подумать
        limit.tv_nsec = 0;

        int status = sigtimedwait(&sigset, &sig, &limit);
        if(status == -1) {
            std::cout << "Can't launch cgi_script! Cgi_module is not responding" << std::endl;
            exit(0);
        }
        kill(pid, SIGUSR1);
    }
}

void cgi_helper::run_and_send(std::string path, int fd, message_helper * m_help) {
    std::string r = m_help->get_head("method");
    std::string args = r.substr(r.find_first_of('?')+1);
    args = args.substr(0, args.size() - std::string("HTTP/1.1\r\n").size());
    std::string method = std::string("REQUEST_METHOD=")+r.substr(0, r.find(' '));
    std::string query_string = std::string("QUERY_STRING=")+args;
    std::string home = "HOME=/home";
    const char * const env_var[4] = { // TODO: добавить больше переменных среды
        method.c_str(),
        query_string.c_str(),
        home.c_str(),
        NULL
    };

    int readfd[2];
    int cgipid;

    pipe(readfd);

    path = isolate(path);

    if( (cgipid = fork()) == 0 ) { // child
        close(readfd[0]);

        dup2(readfd[1], 1); // перенаправляем вывод

        cgi_exec(path, env_var);

        std::cout << "Launching CGI-script is failed" << std::endl;
        close(readfd[1]);
        exit(0);
    }
    // parent // TODO: вынести перегон данных из одного дескриптора в другой в отдельный метод
    close(readfd[1]);
    
    identificate(cgipid);

    std::string ok_stat = "HTTP/1.1 200 OK"; // TODO: сделать нормальную проверку
    unsigned int b = 0;
    while(b < ok_stat.size()) b += send(fd, ok_stat.c_str() + b, ok_stat.size() - b, 0);

    int BLOCK_SIZE = 8192;
    char buffer[BLOCK_SIZE];
    int read_bytes = 0;
    int write_bytes = 0;

    while( (read_bytes = read(readfd[0], buffer, BLOCK_SIZE)) > 0 ) {
        while(write_bytes < read_bytes) {
            write_bytes += send(fd, buffer + write_bytes, read_bytes - write_bytes , 0);
        }
        write_bytes = 0;
    }

    close(readfd[0]);

    wait(NULL);
    clear(path);
}

void cgi_helper::prepare_jail(std::string path) {
    std::vector<std::string> programs = {
         "/bin/bash",
         "/bin/cp",
         "/usr/bin/dircolors",
         "/bin/ls",
         "/bin/mkdir",
         "/bin/mv",
         "/bin/rm",
         "/bin/rmdir",
         "/bin/sh"
    };

    std::vector<std::string> dirs {
      "/home",
      "/tmp"
    };

    for(auto prog : programs) {
        copy_lib(prog, path+prog);
        prepared.push_back(shared_string(prog.c_str()));
        auto depend = get_dependencies(prog);
        for(auto lib : depend) {
            copy_lib(lib, path+lib);
        }
    }
    copy_devices(path);
    for(auto dir : dirs) mkdir((path + dir).c_str(), 0777);
}

void cgi_helper::copy_devices(std::string path) {
    if(!jail_enable) return;
    pid_t id = fork();
    if(id == 0) {
        execle("cgi_module", "cgi_module", "1", jail_path.c_str(), path.c_str(), NULL, environ);
        exit(0);
    }
    identificate(id);
}


void cgi_helper::copy_lib(std::string from, std::string to) {
    struct stat buf;
    int status = stat(to.c_str(), &buf);
    if(status == 0) return;
    size_t point = 0;
    while( (point = to.find('/', point+1)) != std::string::npos ) {
        std::string p = to.substr(0, point);
        if(stat(p.c_str(), &buf) == 0) continue; // тут происходит что-то странное
        mkdir(p.c_str(), 0777); // все файлы в jail, а значит спокойно выдаем все права
    }
    int tofd = creat(to.c_str(), 0777);
    int fromfd = open(from.c_str(), O_RDONLY);

    status = stat(from.c_str(), &buf);

    sendfile(tofd, fromfd, 0, buf.st_size);

    close(tofd);
    close(fromfd);
}

std::string cgi_helper::get_real_path(std::string path) {
    std::ifstream file(path);
    char iden[3];
    file >> iden[0] >> iden[1];
    iden[2] = '\0';
    if(std::string(iden) == "#!") { // if shebang
        std::string res;
        getline(file, res);
        file.close();
        return res.substr(0, res.find_first_of(' '));
    }
    file.close();
    return path;
}

std::vector<std::string> cgi_helper::get_dependencies(std::string path) {
    path = get_real_path(path);

    int output[2];
    pipe(output);

    pid_t id = fork();
    if(id == 0) { // child
        close(output[0]);
        dup2(output[1], 1);

        execlp("ldd", "ldd", path.c_str(), NULL);

    }
    close(output[1]);
    std::string _depend;
    char buffer[4096];
    int read_bytes = 0;
    while((read_bytes = read(output[0], buffer, sizeof(buffer)-1)) > 0) {
        buffer[read_bytes] = '\0';
        _depend += std::string(buffer);
    }
    close(output[0]);

    std::istringstream iss(_depend);
    std::vector<std::string> result;
    std::string lib;
    while(getline(iss, lib)) {
        size_t beg = lib.find_first_of('/');
        if(beg != std::string::npos) {
            size_t end = lib.find(' ', beg);
            result.push_back(lib.substr(beg, end - beg));
        }
    }
    return result;
}
