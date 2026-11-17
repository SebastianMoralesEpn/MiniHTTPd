# MiniHTTPd

Implementacion de un servidor HTTP/1.1 en c usando epoll.

**Autor:** Sebastian Morales 
**Repositorio:** https://github.com/SebastianMoralesEpn/MiniHTTPd

---

## Caracteristicas

- Sirve con archivos HTML, CSS, JS e imagenes.
- Concurrencia con bucle `epoll` no bloqueante.
- Soporte de conexiones persistentes (`keep-alive`).
- Tipos MIME automaticos por extensión.
- Seguridad anti directory traversal usando `realpath()`.
- Codigos HTTP: 200, 400, 403, 404, 405 y 500.

---

## Estructura

```
MiniHTTPd/
├── include/
│   ├── files.h
│   ├── http.h
│   ├── mime.h
│   └── server.h
├── src/
│   ├── files.c
│   ├── http.c
│   ├── main.c
│   ├── mime.c
│   └── server.c
├── www/
│   ├── image.png
│   ├── index.html
│   ├── script.js
│   └── style.css
├── Makefile
└── README.md
```
---

## Compilación y Ejecución

Compilar:
```bash
make
```

Ejecutar(puerto 8080):
```bash
./minihttpd
```

En el navegador:
```bash
http://localhost:8080/
```

---

## Pruebas de codigo de estado

```bash
# 200 OK
curl -i http://localhost:8080/

# 404 Not Found
curl -i http://localhost:8080/noexiste.html

# 403 Forbidden
curl -i --path-as-is http://localhost:8080/../../etc/passwd

# 405 Method Not Allowed
curl -i -X POST http://localhost:8080/

# Keep-Alive
curl -v http://localhost:8080/ http://localhost:8080/style.css

# Concurrencia
ab -n 1000 -c 50 http://localhost:8080/
```

