#ifndef SERVER_H
#define SERVER_H
#define BACKLOG 32
#define DEFAULT_PORT 8080
#define MAX_EVEN 64

/* se crea el socket, bind y listen*/
int server_init(int port);

/*no bloqueante */
int set_nonblocking(int fd);

#endif
