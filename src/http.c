
#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "mime.h"
#include "files.h"


static int enviar_todo(int socket_cl, const void *datos, size_t longi) {
    const char *ptr = (const char *)datos;
    size_t env = 0;
    while (env < longi) {
        ssize_t res = write(socket_cl, ptr + env, longi - env);
        if (res < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(1000);
                continue;
            }
            if (errno == EINTR) continue;
            return -1;
        }
        env += (size_t)res;
    }
    return 0;
}

void enviar_error(int socket_cl, int cod, const char *mensj) {
    char cuerpo[512];
    int len_cuerpo = snprintf(cuerpo, sizeof(cuerpo),
        "<html><body><h1>%d %s</h1></body></html>\n", cod, mensj);
    
    char cabecera[256];
    int len_cabecera = snprintf(cabecera, sizeof(cabecera),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: text/html\r\n"
	"Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        cod, mensj, len_cuerpo);

    enviar_todo(socket_cl, cabecera, (size_t)len_cabecera);
    enviar_todo(socket_cl, cuerpo, (size_t)len_cuerpo);
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

    char cabecera[512];
    int len_cabecera = snprintf(cabecera, sizeof(cabecera),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: %s\r\n"
        "\r\n",
        mime, arch.tam, keep_alive ? "keep-alive" : "close");

    enviar_todo(socket_cl, cabecera, (size_t)len_cabecera);
    enviar_todo(socket_cl, arch.cont, arch.tam); //para archivos binarios

    liberar_archivo(&arch);
    return keep_alive;
}
