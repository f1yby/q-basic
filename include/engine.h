#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace engine {
class MiniBasic {
 public:
  void load_source(std::istream& in);
  void generate_ast();
  void run_code();

  void clear();

  [[nodiscard]] std::string get_source_copy() const {
    return string_lines_into_string(source);
  }
  [[nodiscard]] std::string get_ast_copy() const {
    return string_lines_into_string(ast);
  }
  [[nodiscard]] std::string get_result_copy() const { return result; }

  MiniBasic() = default;

 private:
  std::vector<std::string> source;
  std::vector<std::string> ast;
  std::string result;

  static std::string string_lines_into_string(
      const std::vector<std::string>& in);
};

}  // namespace engine