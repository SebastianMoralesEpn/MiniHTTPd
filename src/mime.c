#include "mime.h"
#include <string.h>

typedef struct {
    const char *ext;
    const char *tipo;
} entrada_mime;


static const entrada_mime tabla_mime[] = {
    {".html", "text/html"},
    {".css",  "text/css"},
    {".js",   "application/javascript"},
    {".png",  "image/png"},
    {".jpeg", "image/jpeg"},
    {".jpg",  "image/jpeg"},
    {".ico",  "image/x-icon"},
    {NULL,    NULL}
};

const char *obtener_mime(const char *ruta) {
    if (ruta == NULL) {
        return "application/octet-stream";
    }

    const char *extension = strrchr(ruta, '.');
    if (extension == NULL) {
        return "application/octet-stream";
    }
    for (int i = 0; tabla_mime[i].ext != NULL; i++) {
        if (strcmp(extension, tabla_mime[i].ext) == 0) {
            return tabla_mime[i].tipo;
        }
    }
    
    //arhivos desconocidos
    return "application/octet-stream";
}
