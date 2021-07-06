#include "socket.h"

ssize_t write_to_socket(int socket, const void *buffer, size_t size)
{
  size_t total_bytes{};
  ssize_t bytes_written{};
  while (total_bytes < size)
  {
    bytes_written = write(socket, (char *) buffer + total_bytes, size - total_bytes);

    if (bytes_written >= 0)
      total_bytes += bytes_written;

    if (bytes_written == 0 || (bytes_written == -1 && errno != EINTR))
      return bytes_written;
  }

  return total_bytes;
}

ssize_t read_from_socket(int socket, const void *buffer, size_t size)
{
  size_t total_bytes{};
  ssize_t new_bytes{};
  while (total_bytes < size)
  {
    new_bytes = read(socket, (char *) buffer + total_bytes, size - total_bytes);

    if (new_bytes > 0)
      total_bytes += new_bytes;

    if (new_bytes == 0 || (new_bytes == -1 && errno != EINTR))
      return new_bytes;
  }

  return total_bytes;
}