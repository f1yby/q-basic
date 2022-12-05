#include "engine.h"
namespace engine {
void MiniBasic::generate_ast() {
  ast.clear();
  auto parser = parser::Parser();
  auto tokenizer = tokenizer::Tokenizer();
  for (const auto& line : source) {
    auto node = parser.parse(tokenizer.lex(line));
    if (typeid(*node) == typeid(parser::ast_node::LineNoStmt)) {
      auto l = std::static_pointer_cast<parser::ast_node::LineNoStmt>(node);

      ast.insert(std::make_pair(l->number()->value(), node));
    }
  }
}
void MiniBasic::run_code() { result = "todo"; }
void MiniBasic::clear() {
  source.clear();
  ast.clear();
  result.clear();
}
std::string MiniBasic::string_lines_into_string(
    const std::vector<std::string>& in) {
  auto out = std::string();
  for (const auto& i : in) {
    out.insert(out.end(), i.begin(), i.end());
    out.push_back('\n');
  }
  return out;
}
void MiniBasic::load_source(std::istream& in) {
  source.clear();
  auto line = std::string();
  while (std::getline(in, line)) {
    source.push_back(std::move(line));
    line = std::string();
  }
}

}  // namespace engine