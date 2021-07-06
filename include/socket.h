#ifndef SCAN_SERVICE_SOCKET_READ_WRITE_H
#define SCAN_SERVICE_SOCKET_READ_WRITE_H

#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <sys/un.h>

#define BACKLOG 5
#define SERVER_PATH "/tmp/scan_service"
#define EXIT_MESSAGE "CLOSE_SERVER"

extern ssize_t write_to_socket(int socket, const void *buffer, size_t size);
extern ssize_t read_from_socket(int socket, const void *buffer, size_t size);

#endif
