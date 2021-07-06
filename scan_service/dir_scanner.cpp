#include "dir_scanner.h"

#define THREAD_COUNT 3
#define SUS_TYPE_COUNT 3

enum suspicious_types_enum
{
    NONE = -1,
    js = 0,
    unix = 1,
    maxOS = 2
};

suspicious_types_enum suspicion_check(const std::string &str, const std::string &ext);
void check_file(const std::filesystem::path &path);

static std::atomic<size_t> error_counter{};
static std::atomic<size_t> suspicious_files_counters[SUS_TYPE_COUNT]{};
// 0 - js
// 1 - unix
// 2 - macOS
static std::atomic<int> available_threads_counter = THREAD_COUNT;
static std::condition_variable cv{};

int scan_directory(const char *dir, scan_results &results)
{
  auto start_time{std::chrono::high_resolution_clock::now()};
  std::mutex m{};
  std::vector<std::thread> thread_vector{};
  std::unique_lock<std::mutex> lock{m};
  std::filesystem::path path{dir};

  if (!std::filesystem::exists(path))
  {
    return SCAN_ERROR_DIR_NOT_EXIST;
  }

  if (!std::ifstream{path}.is_open())
  {
    return SCAN_ERROR_CANT_READ_DIR;
  }

  for (const auto &iter : std::filesystem::directory_iterator{path})
  {
    cv.wait(lock, [] {
        return available_threads_counter > 0;
    });
    ++results.file_counter;
    --available_threads_counter;
    thread_vector.emplace_back(std::thread(check_file, iter.path()));
  }

  for (auto &thr : thread_vector)
    thr.join();

  auto duration{std::chrono::high_resolution_clock::now() - start_time};
  results.duration_s = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  results.duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;
  results.duration_us = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000;

  results.js_counter = suspicious_files_counters[js];
  results.unix_counter = suspicious_files_counters[unix];
  results.macOS_counter = suspicious_files_counters[maxOS];
  results.error_counter = error_counter;

  suspicious_files_counters[js] = 0;
  suspicious_files_counters[unix] = 0;
  suspicious_files_counters[maxOS] = 0;
  error_counter = 0;
  return SCAN_SUCCESS;
}

suspicious_types_enum suspicion_check(const std::string &str, const std::string &ext)
{
  static const std::string js_string = "<script>evil_script()</script>";
  static const std::string unix_string = "rm -rf ~/Document";
  static const std::string macOS_string = "system(\"launchctl load /Library/LaunchAgents/com.malware.agent\")";

  if (ext == ".js" && str.find(js_string) != -1)
    return suspicious_types_enum::js;

  if (str.find(unix_string) != -1)
    return suspicious_types_enum::unix;

  if (str.find(macOS_string) != -1)
    return suspicious_types_enum::maxOS;

  return NONE;
}

void check_file(const std::filesystem::path &path)
{
  std::ifstream file{path};

  if (!file.is_open())
  {
    ++error_counter;
    ++available_threads_counter;
    cv.notify_all();
    return;
  }

  std::string line;
  const std::string &extension = path.extension().string();
  while (getline(file, line))
  {
    auto result = suspicion_check(line, extension);
    if (result == NONE)
      continue;

    ++suspicious_files_counters[result];
    break;
  }

  ++available_threads_counter;
  cv.notify_all();
}