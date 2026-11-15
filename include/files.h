#ifndef FILES_H
#define FILES_H
#include <stddef.h>

#define DIR_RAIZ "www"

typedef struct {
    char   *cont;
    size_t  tam;
    int     encont;
} archivo_t;

archivo_t leer_archivo(const char *ruta_arch);

void liberar_archivo(archivo_t *arch);

#endif
