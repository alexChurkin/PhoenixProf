#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "cl_api_collector.h"

class ChromeTracingGenerator {
 public:
  static void ExportToFile(const std::vector<ClFunctionCall>& calls,
                           const std::string& filename) {
    std::ofstream output_file(filename);
    if (!output_file.is_open()) {
      std::cerr << "Error: Could not open file: " << filename << std::endl;
      return;
    }

    output_file << "{\"traceEvents\":[";
    bool first_entry = true;

    for (const auto& call : calls) {
      if (!first_entry) {
        output_file << ",";
      }
      first_entry = false;

      uint64_t duration = call.end_time - call.start_time;

      output_file << "{"
                  << "\"name\":\"" << call.function_name << "\","
                  << "\"ph\":\"X\","
                  << "\"ts\":" << call.start_time << ","
                  << "\"dur\":" << duration << ","
                  << "\"pid\":1,"
                  << "\"tid\":1"
                  << "}";
    }

    output_file << "]}" << std::endl;
    output_file.close();
    std::cout << "Trace saved to: " << filename << std::endl;
  }
};