#include "catch2/catch_all.hpp"
#include "tokenizer.h"

namespace {
Str lex_result_into_string(const Vec<Rc<tokenizer::Token>>& tokens) {
  std::stringstream ss;
  for (const auto& token : tokens) {
    token->dump(ss);
  }
  Str s;
  ss >> s;
  return s;
}
}  // namespace

SCENARIO("tokenizer can lex single character tokens") {
  auto tokenizer = tokenizer::Tokenizer();
  auto ans = tokenizer::Tokenizer();
  GIVEN("letter +") {
    REQUIRE(lex_result_into_string(tokenizer.lex("+")) == Str("+"));
  }
  GIVEN("letter -") {
    REQUIRE(lex_result_into_string(tokenizer.lex("-")) == Str("-"));
  }
  //  GIVEN("letter *") {
  //    REQUIRE(lex_result_into_string(tokenizer.lex("*")) == Str("*"));
  //  }
  GIVEN("letter /") {
    REQUIRE(lex_result_into_string(tokenizer.lex("/")) == Str("/"));
  }
  GIVEN("letter (") {
    REQUIRE(lex_result_into_string(tokenizer.lex("(")) == Str("("));
  }
  GIVEN("letter )") {
    REQUIRE(lex_result_into_string(tokenizer.lex(")")) == Str(")"));
  }
}