#include "include/server.h"

#define LISTENQ 1024
#define SA struct sockaddr

server::server() {
    try {
        conf_serv = new config_helper;
        log = new logger(conf_serv->get_param("log"));
        file_serv = new file_helper(conf_serv->get_param("root_directory"));
        err_serv = new error_helper;
    } catch (std::runtime_error e) {
        std::cerr << "Something went wrong: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    } catch (...) {
        std::cerr << "Something went wrong" << std::endl;
        exit(EXIT_FAILURE);
    }
}

server::~server() {
    delete conf_serv;
    delete log;
    delete file_serv;
    delete err_serv;
}

void server::start() {
    int listenfd /*слушающий*/, connfd;
    int port = conf_serv->get_iparam("port");
    struct sockaddr_in servaddr;
    char buff[200];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0); // создание нового сокета
    // AF_INET - сокет для интернета
    // SOCK_STREAM - потоковый сокет
    // возвращает дескриптор

    bzero(&servaddr, sizeof(servaddr)); // очищаем servvaddr
    servaddr.sin_family = AF_INET; // для интернета
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // для любого интерфейса
    servaddr.sin_port = htons(port); // порт

    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
    // соединяем

    listen(listenfd, LISTENQ);
    // запускаем прослушку

    for(;;) {
        connfd = accept(listenfd, (SA *)NULL, NULL);

        handle_request(connfd);
        // обрабатываем
    }

}


void server::handle_request(int connfd) {
    std::cout << "New connection!" << std::endl;

    std::string hel = "Hello, user!";

    std::string m = "HTTP/1.1 200 OK\nContent-Length: " + std::to_string(hel.size()) + "\n\n" + hel;

    char _b[50];
    int _n;
    std::string headers;

    do {
        _n = recv(connfd, _b, sizeof(_b)-1 , 0);
        headers.insert(headers.end(), _b, _b+_n);
    }while(_b[_n-4] != '\r' && _b[_n-3] != '\n' && _b[_n-2] != '\r' && _b[_n-1] != '\n');//_b[_n-1] != '\0');

    std::cout << headers << std::endl;

    std::cout << std::endl << "--------" << std::endl;

    send(connfd, m.c_str(), m.size(), 0);

    close(connfd);
}
