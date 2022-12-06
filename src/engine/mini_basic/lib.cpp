#include "engine.h"
namespace engine {
void MiniBasic::clear() {
  source.clear();
  ast.clear();
  result.clear();
}
Str MiniBasic::string_lines_into_string(const Map<int64_t, Str>& in) {
  auto out = std::string();
  for (const auto& i : in) {
    out.insert(out.end(), i.second.begin(), i.second.end());
    out.push_back('\n');
  }
  return out;
}
void MiniBasic::load_source(std::istream& in) {
  source.clear();
  ast.clear();
  auto line = std::string();
  while (std::getline(in, line)) {
    auto node = parser::Parser().parse(tokenizer::Tokenizer().lex(line));
    if (typeid(*node) == typeid(parser::ast_node::LineNoStmt)) {
      auto l = std::static_pointer_cast<parser::ast_node::LineNoStmt>(node);
      ast.insert(std::make_pair(l->number()->value(), l));
      source.insert(std::make_pair(l->number()->value(), line));
    }
  }
}
std::string MiniBasic::get_ast_copy() const {
  std::stringstream ss;
  for (const auto& node : ast) {
    node.second->dump(0, ss);
  }
  return ss.str();
}
UIBehavior MiniBasic::step_run(Str& output) {
  if (pc_ == -1) {
    return UIBehavior::FinishRun;
  }
  auto stmt = ast.find(pc_);
  if (stmt == ast.end()) {
    auto warn = Str("WARNING: unknown statement at " + std::to_string(pc_));
    return UIBehavior::FinishRun;
  }
  auto next_stmt = stmt;
  ++next_stmt;
  if (next_stmt == ast.end()) {
    pc_ = -1;
  } else {
    pc_ = next_stmt->first;
  }
  return stmt->second->run(variant_env, pc_, output, variant_need_input_);
}
void MiniBasic::start_run() {
  variant_env.clear();
  variant_need_input_.clear();
  if (!ast.empty()) {
    pc_ = ast.begin()->first;
  } else {
    pc_ = -1;
  }
}

}  // namespace engine