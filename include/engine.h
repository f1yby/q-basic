#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "parser.h"
#include "type.h"

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
    std::stringstream ss;
    for (const auto& node : ast) {
      node.second->dump(0, ss);
    }
    return ss.str();
  }
  [[nodiscard]] std::string get_result_copy() const { return result; }

  MiniBasic() = default;

 private:
  Vec<Str> source;
  Map<int32_t, Rc<parser::AstNode>> ast;
  Str result;

  static std::string string_lines_into_string(
      const std::vector<std::string>& in);
};

}  // namespace engine