#include "include/server.h"
#include "include/request_strategy.h"

#define LISTENQ 1024
#define SA struct sockaddr

server::server() {
    try {
        conf_serv = new config_helper;
        log = new logger(conf_serv->get_param("log"));
        file_serv = new file_helper(conf_serv->get_param("root_directory"));
        err_serv = new error_helper(conf_serv->get_param("responses"));
        mess_serv = new message_helper(conf_serv->get_iparam("timelimit"));

        struct sigaction arg;
        arg.sa_handler = SIG_IGN;
        arg.sa_flags = SA_NOCLDWAIT;
        sigaction(SIGCHLD, &arg, NULL); // чтобы форкнутые процессы не засоряли таблицу

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

    int pid;

    for(;;) {
        connfd = accept(listenfd, (SA *)NULL, NULL);

        if( (pid = fork()) == 0 ) {
            close(listenfd);
            handle_request(connfd);
            close(connfd);
            exit(0);
        }

        close(connfd);
        // обрабатываем
    }

}


void server::handle_request(int connfd) {
    std::cout << "New connection!" << std::endl;

    mess_serv->read_headers(connfd);

    request_strategy * strategy = request_strategy::get_strategy(mess_serv->get_head("method"));

    strategy->do_request(this, connfd);

    delete strategy;
}
