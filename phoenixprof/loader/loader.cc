#include <iostream>
#include <vector>

#include <stdio.h>
#include <string.h>

#ifndef TOOL_NAME
#error "TOOL_NAME is not defined"
#endif

#include "shared_library.h"
#include "utils.h"

static std::string GetLibFileName() {
  return std::string("lib") + TOSTRING(TOOL_NAME) + ".so";
}

static bool IsFileExists(const char* file_name) {
  ASSERT(file_name != nullptr);
  FILE* file = nullptr;
  file = fopen(file_name, "rb");
  if (file != nullptr) {
    fclose(file);
    return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
  std::string library_file_name = GetLibFileName();
  std::string executable_path = utils::GetExecutablePath();

  std::string library_file_path = executable_path + library_file_name;
  if (!IsFileExists(library_file_path.c_str())) {
    library_file_path = library_file_name;
  }

  SharedLibrary* lib = SharedLibrary::Create(library_file_path);
  if (lib == nullptr) {
    std::cout << "[ERROR] Failed to load " << library_file_name <<
      " library" << std::endl;
    return 0;
  }

  utils::SetEnv("LD_PRELOAD", library_file_path.c_str());
  utils::SetEnv("PTI_ENABLE", "1");

  // if (execvp(app_args[0], app_args.data())) {
  //   std::cout << "[ERROR] Failed to launch target application: " <<
  //     app_args[0] << std::endl;
  //   usage();
  //   delete lib;
  //   return 0;
  // }

  delete lib;
  return 0;
}