
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"
#include "http.h"
#define BUFSIZE 4096

int main(int argc, char *argv[]){

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

	set_nonblocking(server_fd); //no bloqueante

	int epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		perror("error en epoll_create1");
		close(server_fd);
		exit(1);
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
		perror("error al registrar server_fd en epoll");
		close(epoll_fd);
		close(server_fd);
		exit(1);
	}

	struct epoll_event eventos[MAX_EVEN];

	//concurrencia
	while (1) {
		int nfds = epoll_wait(epoll_fd, eventos, MAX_EVEN, -1);
		if (nfds < 0) {
			if (errno == EINTR)
				continue;
			perror("error en epoll_wait");
			break;
		}

		for (int i = 0; i < nfds; i++) {
			int fd = eventos[i].data.fd;

			//nueva conexion
			if (fd == server_fd) {
				tam_dir = sizeof(dir_client);
				client_fd = accept(server_fd, (struct sockaddr *)&dir_client, &tam_dir);
				if (client_fd < 0) {
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						perror("error en accept");
					continue;
				}
				
				set_nonblocking(client_fd);
				
				struct epoll_event ev_cli; //nuevo cliente
				ev_cli.events = EPOLLIN;
				ev_cli.data.fd = client_fd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev_cli);
			}

			// datos para leer
			else {
				n = recv(fd, buf_pet, sizeof(buf_pet) - 1, 0);
				if (n <= 0) {
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL); //cerro o error
					close(fd);
				} else {
					buf_pet[n] = '\0';
					int keep_alive = solicitud(fd, buf_pet, n);
					if (!keep_alive) {
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
						close(fd);
					}
				}
			}
		}
	}
	close(epoll_fd);
	close(server_fd);
	return 0;
}
