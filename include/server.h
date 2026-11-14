#ifndef SERVER_H
#define SERVER_H

#define BACKLOG      32
#define DEFAULT_PORT 8080

/* se crea el socket, bind y listen*/
int server_init(int port);

#endif
