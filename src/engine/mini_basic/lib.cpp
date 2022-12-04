#include "engine.h"
namespace engine {
void MiniBasic::generate_ast() {
  // FIXME
  ast = source;
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