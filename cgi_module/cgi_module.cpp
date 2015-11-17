#include <unistd.h>
#include <iostream>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>

#define COMMAND strtol(argv[1], NULL, 10)
#define JAIL_PATH argv[2]
#define SCRIPT_PATH argv[3]

int main(int argc, char ** argv) {
    sigset_t sigset;
    siginfo_t sig;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    struct timespec limit;
    limit.tv_sec = 10; // 10 секунд на подумать
    limit.tv_nsec = 0;

    pid_t ppid = getppid();

    kill(ppid, SIGUSR1);

    int status = sigtimedwait(&sigset, &sig, &limit);

    if(status == -1) {
        std::cout << "Can't launch cgi_script! Server is not responding " << std::endl;
        exit(0);
    }

    if(COMMAND == 0) {
        int user_id = getuid();
        if(user_id == 0) {
            std::cout << "Don't launch server with root permitions!" << std::endl;
            exit(0);
        }
        int st = setuid(0);
        if(st == -1) {
            std::cout << "Impossible to create jail! Please set SUID flag on this server" << std::endl;
            exit(0);
        }

        chroot(JAIL_PATH);
        chdir("/");

        setuid(user_id);

        execle(SCRIPT_PATH, SCRIPT_PATH, NULL, environ);
    } else if (COMMAND == 1) {
        mkdir((std::string(JAIL_PATH)+"/dev").c_str(), 0777);
        std::vector< std::string > devices {
            "/dev/random",
            "/dev/urandom",
            "/dev/zero",
            "/dev/null",
            "/dev/tty"
        };
        int user_id = getuid();
        setuid(0);
        for(auto dev : devices) {
            struct stat buf;
            stat(dev.c_str(), &buf);
            mknod( (std::string(JAIL_PATH)+dev).c_str(), S_IFCHR | 0777, buf.st_rdev );
        }
        setuid(user_id);
        exit(0);
    }
    return 0;
}
