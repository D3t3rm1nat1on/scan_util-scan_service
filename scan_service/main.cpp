#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <thread>

#include "dir_scanner.h"

#define SERVER_PATH "/tmp/scan_service"
#define EXIT_MESSAGE "CLOSE_SERVER"

int main()
{
  int socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
  int option = 1;
  if ((setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &option, sizeof(option)) == -1))
  {
    perror("set socket option error: ");
    exit(errno);
  }

  /* Определить что это сервер */
  struct sockaddr_un name{.sun_path=SERVER_PATH, .sun_family=PF_LOCAL};
  if ((bind(socket_fd, (struct sockaddr *) &name, SUN_LEN (&name))) == -1)
  {
    perror("socket bind failed: ");
    unlink(SERVER_PATH);
    exit(errno);
  }

  listen(socket_fd, 5);

  while (true)
  {
    size_t msg_len;
    char *msg;
    int client_socket_fd = accept(socket_fd, nullptr, nullptr);

    // if client close connection
    if (read(client_socket_fd, &msg_len, sizeof(msg_len)) == 0)
    {
      close(client_socket_fd);
      break;
    }

    msg = new char[msg_len];
    read(client_socket_fd, msg, msg_len);

    if (!strcmp(msg, EXIT_MESSAGE))
    {
      close(client_socket_fd);
      delete[] msg;
      break;
    }

    scan_results results{};
    int scan_code = scan_directory(msg, results);

    write(client_socket_fd, &scan_code, sizeof(scan_code));

    if (scan_code == SCAN_SUCCESS)
      write(client_socket_fd, &results, sizeof(results));

    delete[] msg;
    close(client_socket_fd);
  }

  close(socket_fd);
  unlink(SERVER_PATH);
  return 0;

}