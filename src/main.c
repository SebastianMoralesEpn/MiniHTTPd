
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"
#include "http.h"

#define BUFSIZE 4096

int main(int argc, char *argv[])
{
	int port = DEFAULT_PORT;
	int server_fd, client_fd;
	struct sockaddr_in dir_client;
	unsigned int tam_dir;
	char buf_pet[BUFSIZE];
	int n;  //bytes leidos

	if (argc == 2)
		port = atoi(argv[1]);

	server_fd = server_init(port);
	if (server_fd < 0) {
		fprintf(stderr, "no se pudo iniciar el servidor\n");
		exit(1);
	}
	printf("minihttpd esta escuchando en el puerto %d\n", port);

	/* loop iterativo antes de implementar epoll */
	while (1) {
		tam_dir = sizeof(dir_client);
		client_fd = accept(server_fd, (struct sockaddr *)&dir_client, &tam_dir);
		if (client_fd < 0) {
			if (errno == EINTR)
				continue;
			perror("error en accept");
			continue;
		}

		n = read(client_fd, buf_pet, sizeof(buf_pet) - 1);
		if (n > 0) {
			buf_pet[n] = '\0';
			solicitud(client_fd, buf_pet, n);

			}
		close(client_fd);
	}

	return 0;
}
