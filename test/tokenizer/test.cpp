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
  GIVEN("single letter word") {
    WHEN("+") {
      REQUIRE(lex_result_into_string(tokenizer.lex("+")) == Str("+"));
    }
    WHEN("-") {
      REQUIRE(lex_result_into_string(tokenizer.lex("-")) == Str("-"));
    }
    WHEN("*") {
      REQUIRE(lex_result_into_string(tokenizer.lex("*")) == Str("*"));
    }
    WHEN("/") {
      REQUIRE(lex_result_into_string(tokenizer.lex("/")) == Str("/"));
    }
    WHEN("(") {
      REQUIRE(lex_result_into_string(tokenizer.lex("(")) == Str("("));
    }
    WHEN(")") {
      REQUIRE(lex_result_into_string(tokenizer.lex(")")) == Str(")"));
    }
  }
  GIVEN("Parentheses") {
    WHEN("(())") {
      REQUIRE(lex_result_into_string(tokenizer.lex("(())")) == Str("(())"));
    }
    WHEN("(()())") {
      REQUIRE(lex_result_into_string(tokenizer.lex("(()())")) == Str("(()())"));
    }
  }

  GIVEN("Single Letters separated by space") {
    WHEN("* *") {
      REQUIRE(lex_result_into_string(tokenizer.lex("* *\n")) == Str("**"));
    }
    WHEN("* (") {
      REQUIRE(lex_result_into_string(tokenizer.lex("* (\n")) == Str("*("));
    }
  }
}

SCENARIO("tokenizer can lex number expressions") {
  auto tokenizer = tokenizer::Tokenizer();
  GIVEN("Single Number") {
    WHEN("0") {
      REQUIRE(lex_result_into_string(tokenizer.lex("0\n")) == Str("0"));
    }
    WHEN("123456") {
      REQUIRE(lex_result_into_string(tokenizer.lex("123456\n")) ==
              Str("123456"));
    }
    WHEN("-123456") {
      REQUIRE(lex_result_into_string(tokenizer.lex("-123456\n")) ==
              Str("-123456"));
    }
  }
  GIVEN("expression") {
    WHEN("123**456") {
      REQUIRE(lex_result_into_string(tokenizer.lex("123**456\n")) ==
              Str("123**456"));
    }
    WHEN("12+34*56") {
      REQUIRE(lex_result_into_string(tokenizer.lex("12 + 34 * 56\n")) ==
              Str("12+34*56"));
    }
    WHEN("12/3**(4+56)") {
      REQUIRE(lex_result_into_string(tokenizer.lex("12/3**(4+56)\n")) ==
              Str("12/3**(4+56)"));
    }
  }
}