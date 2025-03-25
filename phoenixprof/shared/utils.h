#ifndef PHPROF_UTILS_H_
#define PHPROF_UTILS_H_

#ifdef NDEBUG
#undef NDEBUG
#include <assert.h>
#define NDEBUG
#else
#include <assert.h>
#endif

#include <unistd.h>

#include <string>
#include <vector>

#define ASSERT(X) assert(X)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define MAX_STR_SIZE 1024

#define PHPROF_EXPORT __attribute__((visibility("default")))

namespace utils {
inline std::string GetEnv(const char *name) {
  ASSERT(name != nullptr);
  const char *value = getenv(name);
  if (value == nullptr) {
    return std::string();
  }
  return std::string(value);
}

inline void SetEnv(const char *name, const char *value) {
  ASSERT(name != nullptr);
  ASSERT(value != nullptr);

  int status = 0;
  status = setenv(name, value, 1);

  ASSERT(status == 0);
}

inline std::string GetFilePath(const std::string &filename) {
  ASSERT(!filename.empty());

  size_t pos = filename.find_last_of("/\\");
  if (pos == std::string::npos) {
    return "";
  }

  return filename.substr(0, pos + 1);
}

inline std::string GetExecutablePath() {
  char buffer[MAX_STR_SIZE] = {0};
  ssize_t status = readlink("/proc/self/exe", buffer, MAX_STR_SIZE);
  ASSERT(status > 0);
  return GetFilePath(buffer);
}
}  // namespace utils

#endif  // PHPROF_UTILS_H_