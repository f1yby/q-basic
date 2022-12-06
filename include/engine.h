#pragma once
#include <sstream>
#include <string>
#include <vector>

#include "parser.h"
#include "type.h"
#include "ui_behavior.h"
namespace parser::ast_node {
class LineNoStmt;
}
namespace engine {
class MiniBasic {
 public:
  void load_source(std::istream& in);

  UIBehavior handle_command(const Str& command, Str& output);

  void clear();

  [[nodiscard]] std::string get_source_copy() const {
    return string_lines_into_string(source);
  }
  [[nodiscard]] std::string get_ast_copy() const;

  void reset_pc();

  UIBehavior step_run(Str& output);
  bool handle_input(const Str& input) {
    if (variant_need_input_.empty()) {
      return true;
    }
    auto v = variant_env.find(variant_need_input_);
    int64_t value;

    try {
      value = std::stoll(input);
    } catch (std::invalid_argument const& ex) {
      return false;
    } catch (std::out_of_range const& ex) {
      return false;
    }
    if (v == variant_env.end()) {
      variant_env.insert(std::make_pair(variant_need_input_, value));
    } else {
      v->second = value;
    }
    return true;
  }

  MiniBasic() = default;

 private:
  Map<int64_t, Str> source;
  Map<int64_t, Rc<parser::ast_node::LineNoStmt>> ast;

  // Runner
  int64_t pc_{-1};
  Map<Str, int64_t> variant_env;
  Str variant_need_input_;

  static Str string_lines_into_string(const Map<int64_t, Str>& in);
};

}  // namespace engine