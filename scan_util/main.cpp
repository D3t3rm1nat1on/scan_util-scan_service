#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <thread>

#define SERVER_PATH "/tmp/scan_service"
#define EXIT_MESSAGE "CLOSE_SERVER"
#define SCAN_SUCCESS 0
#define SCAN_ERROR_DIR_NOT_EXIST 1
#define SCAN_ERROR_CANT_READ_DIR 2

struct scan_results
{
    size_t file_counter;
    size_t js_counter;
    size_t unix_counter;
    size_t macOS_counter;
    size_t error_counter;
    size_t duration_s;
    size_t duration_ms;
    size_t duration_us;
};

void print_scan(scan_results &results);

int main(int argc, const char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "wrong param count, expected path only\n");
    return -1;
  }

  const char *message = argv[1];
  int socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);
  struct sockaddr_un name{.sun_family=PF_LOCAL, .sun_path=SERVER_PATH};

  if ((connect(socket_fd, (struct sockaddr *) &name, SUN_LEN (&name))) == -1)
  {
    perror("connect: ");
    exit(errno);
  }

  size_t str_size = strlen(message) + 1;
  write(socket_fd, &str_size, sizeof(str_size));
  write(socket_fd, message, str_size);

  scan_results results{};
  int scan_code;

  // if server closed connection
  if (read(socket_fd, &scan_code, sizeof(scan_code)) == 0)
  {
    printf("server died\n");
    close(socket_fd);
    return 0;
  }

  if (scan_code == SCAN_SUCCESS)
  {
    read(socket_fd, &results, sizeof(results));
    print_scan(results);
  }

  if (scan_code == SCAN_ERROR_DIR_NOT_EXIST)
    fprintf(stderr, "directory doesn't exist: %s\n", message);

  if (scan_code == SCAN_ERROR_CANT_READ_DIR)
    fprintf(stderr, "operation not permitted for directory: %s\n", message);

  close(socket_fd);
  return 0;
}

void print_scan(scan_results &results)
{
  printf("========== Scan result ==========\n");
  printf("Processed files: %zu\n", results.file_counter);
  printf("JS detects: %zu\n", results.js_counter);
  printf("Unix detects: %zu\n", results.unix_counter);
  printf("macOS detects: %zu\n", results.macOS_counter);
  printf("Errors: %zu\n", results.error_counter);
  printf("Execution time: %02zus:%02zums:%02zuus\n", results.duration_s, results.duration_ms, results.duration_us);
  printf("=================================\n");
}