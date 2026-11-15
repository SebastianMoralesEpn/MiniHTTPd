#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

archivo_t leer_archivo(const char *ruta_arch) {
    archivo_t arch;

    arch.cont  = NULL;
    arch.tam   = 0;
    arch.encont = 0;

FILE *fp = fopen(ruta_arch, "rb");
    if (fp == NULL) {
        return arch;
    }

//tam del archivo
fseek(fp, 0, SEEK_END);
    long tam_archivo = ftell(fp);
    rewind(fp);

    if (tam_archivo < 0) {
        fclose(fp);
        return arch;
    }

arch.cont = (char *)malloc((size_t)tam_archivo + 1);
    if (arch.cont == NULL) {
        fclose(fp);
        return arch;
    }

size_t l = fread(arch.cont, 1, (size_t)tam_archivo, fp); //leidos
    fclose(fp);

arch.cont[l] = '\0';
    arch.tam = l;
    arch.encont = 1;

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
