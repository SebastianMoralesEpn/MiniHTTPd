#ifndef HTTP_H
#define HTTP_H
#define MAX_URI 2048

int solicitud(int socket_cl, const char *buffer, int leidos);
void enviar_error(int socket_cl, int cod, const char *mensj);

#endif
