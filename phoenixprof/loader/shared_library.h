#ifndef PHPROF_SHARED_LIBRARY_H_
#define PHPROF_SHARED_LIBRARY_H_

#include <dlfcn.h>

#include <cerrno>
#include <string>
#include <vector>

#include "utils.h"

class SharedLibrary {
 public:
  static SharedLibrary* Create(const std::string& name) {
#if defined(_WIN32)
    HMODULE handle = nullptr;
    handle = LoadLibraryA(name.c_str());
#else
    void* handle = nullptr;
    handle = dlopen(name.c_str(), RTLD_NOW);
#endif
    if (handle != nullptr) {
      return new SharedLibrary(handle);
    }
    return nullptr;
  }

  ~SharedLibrary() {
#if defined(_WIN32)
    BOOL completed = FreeLibrary(handle_);
    ASSERT(completed == TRUE);
#else
    int completed = dlclose(handle_);
    ASSERT(completed == 0);
#endif
  }

  template <typename T>
  T GetSym(const char* name) {
    void* sym = nullptr;
    sym = dlsym(handle_, name);
    return reinterpret_cast<T>(sym);
  }

  void* GetHandle() { return handle_; }

 private:
  SharedLibrary(void* handle) : handle_(handle) {}

  void* handle_ = nullptr;
};

#endif  // PHPROF_SHARED_LIBRARY_H_