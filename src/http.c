
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mime.h"
#include "files.h"

void enviar_error(int socket_cl, int cod, const char *mensj) {
    char resp[512];
    
    int len = snprintf(resp, sizeof(resp),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>%d %s</h1></body></html>\n",
        cod, mensj, cod, mensj);

    write(socket_cl, resp, len);
}

int solicitud(int socket_cl, const char *buffer, int leidos) {
    (void)leidos;

    char metodo[16];
    char uri[MAX_URI];
    char version[16];

    // se parsea la solicitud
    if (sscanf(buffer, "%15s %2047s %15s", metodo, uri, version) != 3) {
        enviar_error(socket_cl, 400, "Bad Request");
        return 0;
    }

    printf("[HTTP] Peticion: %s %s %s\n", metodo, uri, version);

    // se acepta solo get
    if (strcmp(metodo, "GET") != 0) {
        enviar_error(socket_cl, 405, "Method Not Allowed");
        return 0;
    }

    if (strcmp(version, "HTTP/1.1") == 0) {
        if (strstr(buffer, "Host:") == NULL && strstr(buffer, "host:") == NULL) {
            enviar_error(socket_cl, 400, "Bad Request");
            return 0;
        }
    }

    //persistencia
    int keep_alive = 0;
    if (strcmp(version, "HTTP/1.1") == 0) {
        keep_alive = 1;
        if (strstr(buffer, "Connection: close") != NULL) {
            keep_alive = 0;
        }
    } else if (strstr(buffer, "Connection: keep-alive") != NULL) {
        keep_alive = 1;
    }

    char ruta_ok[MAX_URI];
    if (strcmp(uri, "/") == 0) {
        snprintf(ruta_ok, sizeof(ruta_ok), "/index.html");
    } else {
        snprintf(ruta_ok, sizeof(ruta_ok), "%s", uri);
    }

    char ruta_archivo[MAX_URI + 8];
    snprintf(ruta_archivo, sizeof(ruta_archivo), "www%s", ruta_ok);

    archivo_t arch = leer_archivo(ruta_archivo);
    if (!arch.encont) {
        enviar_error(socket_cl, 404, "Not Found");
        return 0;
    }

    const char *mime = obtener_mime(ruta_archivo);

    int len_cuerpo = strlen(arch.cont);

    char cabecera[512];
    int len_cabecera = snprintf(cabecera, sizeof(cabecera),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: %s\r\n"
        "\r\n",
        mime, len_cuerpo, keep_alive ? "keep-alive" : "close");

    write(socket_cl, cabecera, len_cabecera);
    write(socket_cl, arch.cont, len_cuerpo);

    liberar_archivo(&arch);
    return keep_alive;
}
