#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

archivo_t leer_archivo(const char *ruta_arch) {
    archivo_t arch;

    arch.cont  = NULL;
    arch.tam   = 0;
    arch.encont = 0;
    arch.estado = ARCHIVO_OK;

    char ruta_raiz[PATH_MAX];
    char ruta_can[PATH_MAX];

    //obtener desde www
    if (realpath(DIR_RAIZ, ruta_raiz) == NULL) {
        arch.estado = ARCHIVO_INTERNAL_SERVER_ERROR;
        return arch;
    }

    if (realpath(ruta_arch, ruta_can) == NULL) {
        arch.estado = ARCHIVO_NOT_FOUND;
        return arch;
    }

    //validar seguridad
    size_t len_raiz = strlen(ruta_raiz);
    if (strncmp(ruta_can, ruta_raiz, len_raiz) != 0 ||
        (ruta_can[len_raiz] != '/' && ruta_can[len_raiz] != '\0')) {
        arch.estado = ARCHIVO_FORBIDDEN;
        return arch;
    }

    FILE *fp = fopen(ruta_can, "rb");
    if (fp == NULL) {
	arch.estado = ARCHIVO_NOT_FOUND;
        return arch;
    }

    //tam del archivo
    fseek(fp, 0, SEEK_END);
    long tam_archivo = ftell(fp);
    rewind(fp);

    if (tam_archivo < 0) {
        fclose(fp);
	arch.estado = ARCHIVO_INTERNAL_SERVER_ERROR;
        return arch;
    }

    //archivo vacio
    if (tam_archivo == 0) {
        fclose(fp);
        arch.cont = (char *)calloc(1, 1);
        arch.tam = 0;
        arch.encont = 1;
        arch.estado = ARCHIVO_OK;
        return arch;
    }

    arch.cont = (char *)malloc((size_t)tam_archivo + 1);
    if (arch.cont == NULL) {
        fclose(fp);
	arch.estado = ARCHIVO_INTERNAL_SERVER_ERROR;
        return arch;
    }

    size_t l = fread(arch.cont, 1, (size_t)tam_archivo, fp); //leidos
    fclose(fp);

    arch.cont[l] = '\0';
    arch.tam = l;
    arch.encont = 1;
    arch.estado = ARCHIVO_OK;

    return arch;
}

void liberar_archivo(archivo_t *arch) {
    if (arch != NULL && arch->cont != NULL) {
        free(arch->cont);

        // Reseteo
        arch->cont = NULL;
        arch->tam = 0;
        arch->encont = 0;
    }
}
