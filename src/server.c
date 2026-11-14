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
	struct sockaddr_in sin;
	int s, opt = 1;

	memset(&sin, 0, sizeof(sin)); //limpiado como en passivesock 
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);

	s = socket(PF_INET, SOCK_STREAM, 0); //socket 
	if (s < 0) {
		perror("socket");
		return -1;
	}

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("bind");
		close(s);
		return -1;
	}

	if (listen(s, BACKLOG) < 0) {
		perror("listen");
		close(s);
		return -1;
	}

	return s;
}
