/* server.c - inicializacion del socket del servidor */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"

int server_init(int port)
{
	struct sockaddr_in dir_server;
	int sock_fd, opt = 1;

	memset(&dir_server, 0, sizeof(dir_server)); //limpiado como en passivesock 
	dir_server.sin_family = AF_INET;
	dir_server.sin_addr.s_addr = INADDR_ANY;
	dir_server.sin_port = htons(port);

	sock_fd = socket(PF_INET, SOCK_STREAM, 0); //socket 
	if (sock_fd < 0) {
		perror("error al crear socket");
		return -1;
	}

	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(sock_fd, (struct sockaddr *)&dir_server, sizeof(dir_server)) < 0) {
		perror("error en bind");
		close(sock_fd);
		return -1;
	}

	if (listen(sock_fd, BACKLOG) < 0) {
		perror("error en listen");
		close(sock_fd);
		return -1;
	}

	return sock_fd;
}
