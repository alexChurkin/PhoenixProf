#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>

#ifndef TOOL_NAME
#error "TOOL_NAME is not defined"
#endif

#include "shared_library.h"
#include "tool.h"

static std::string GetLibFileName() {
  return std::string("lib") + TOSTRING(TOOL_NAME) + ".so";
}

static bool IsFileExists(const char *file_name) {
  ASSERT(file_name != nullptr);
  FILE *file = nullptr;
  file = fopen(file_name, "rb");
  if (file != nullptr) {
    fclose(file);
    return true;
  }
  return false;
}

int main(int argc, char *argv[]) {
  // Loading tool library via LD_PRELOAD
  std::string library_file_name = GetLibFileName();
  std::string executable_path = utils::GetExecutablePath();

  std::string library_file_path = executable_path + library_file_name;
  if (!IsFileExists(library_file_path.c_str())) {
    library_file_path = library_file_name;
  }

  SharedLibrary *lib = SharedLibrary::Create(library_file_path);
  if (lib == nullptr) {
    std::cout << "[ERROR] Failed to load " << library_file_name << " library"
              << std::endl;
    return 0;
  }

  utils::SetEnv("LD_PRELOAD", library_file_path.c_str());

  // Checking that tool library is correct (exports required functions outside)
  // and preparing the environment
  decltype(ShowHelp) *show_help = lib->GetSym<decltype(ShowHelp) *>("ShowHelp");
  if (show_help == nullptr) {
    std::cout << "[ERROR] Failed to find Usage function in "
              << library_file_name << std::endl;
    delete lib;
    return 0;
  }

  if (argc < 2) {
    show_help();
    delete lib;
    return 0;
  }

  decltype(ProcessArgs) *process_args =
      lib->GetSym<decltype(ProcessArgs) *>("ProcessArgs");
  if (process_args == nullptr) {
    std::cout << "[ERROR] Failed to find ProcessArgs function in "
              << library_file_name << std::endl;
    delete lib;
    return 0;
  }

  decltype(PrepareEnv) *prepare_env =
      lib->GetSym<decltype(PrepareEnv) *>("PrepareEnv");
  if (prepare_env == nullptr) {
    std::cout << "[ERROR] Failed to find PrepareEnv function in "
              << library_file_name << std::endl;
    delete lib;
    return 0;
  }

  prepare_env();

  // Processing tool args and target app args

  int app_index = process_args(argc, argv);
  if (app_index <= 0 || app_index >= argc) {
    if (app_index >= argc) {
      std::cout << "[ERROR] Target application to run is not specified"
                << std::endl;
      show_help();
    } else if (app_index < 0) {
      std::cout << "[ERROR] Invalid command line" << std::endl;
      show_help();
    }
    delete lib;
    return 0;
  }
  std::vector<char *> app_args;

  for (int i = app_index; i < argc; ++i) {
    app_args.push_back(argv[i]);
  }
  app_args.push_back(nullptr);

  // Running the target application
  if (execvp(app_args[0], app_args.data())) {
    std::cout << "[ERROR] Failed to launch the target application: "
              << app_args[0] << std::endl;
    show_help();
    delete lib;
    return 0;
  }

  delete lib;
  return 0;
}