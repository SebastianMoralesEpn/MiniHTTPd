#ifndef FILES_H
#define FILES_H
#include <stddef.h>

#define DIR_RAIZ "www"

typedef enum {
    ARCHIVO_OK = 0,
    ARCHIVO_NOT_FOUND, //404
    ARCHIVO_FORBIDDEN, //403
    ARCHIVO_INTERNAL_SERVER_ERROR //500
} estado_archivo_t;

typedef struct {
    char   *cont;
    size_t  tam;
    int     encont;
    estado_archivo_t estado;
} archivo_t;

archivo_t leer_archivo(const char *ruta_arch);

void liberar_archivo(archivo_t *arch);

#endif
