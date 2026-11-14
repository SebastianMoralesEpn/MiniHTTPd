/* main.c - servidor minihttpd */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"

#define BUFSIZE 4096

int main(int argc, char *argv[])
{
	int port = DEFAULT_PORT;
	int server_fd, client_fd;
	struct sockaddr_in caddr;
	unsigned int alen;
	char buf[BUFSIZE];
	int n;

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
		alen = sizeof(caddr);
		client_fd = accept(server_fd, (struct sockaddr *)&caddr, &alen);
		if (client_fd < 0) {
			if (errno == EINTR)
				continue;
			perror("accept");
			continue;
		}

		n = read(client_fd, buf, sizeof(buf) - 1);
		if (n > 0) {
			buf[n] = '\0';
			printf("--- request ---\n%s\n", buf);

			/* respuesta temporal de prueba */
			const char *resp =
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: 44\r\n"
				"Connection: close\r\n"
				"\r\n"
				"<html><body><h1>esto es una prueba</h1></body></html>";
			write(client_fd, resp, strlen(resp));
		}
		close(client_fd);
	}

	return 0;
}
