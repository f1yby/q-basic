#include "catch2/catch_all.hpp"
#include "tokenizer.h"

namespace {
Str lex_result_into_string(const Vec<Rc<tokenizer::Token>>& tokens) {
  std::stringstream ss;
  for (const auto& token : tokens) {
    token->dump(ss);
  }
  return ss.str();
}
}  // namespace

SCENARIO("tokenizer can lex single character tokens") {
  auto tokenizer = tokenizer::Tokenizer();
  GIVEN("single letter word") {
    WHEN("+") { REQUIRE(lex_result_into_string(tokenizer.lex("+")) == "+"); }
    WHEN("-") { REQUIRE(lex_result_into_string(tokenizer.lex("-")) == "-"); }
    WHEN("*") { REQUIRE(lex_result_into_string(tokenizer.lex("*")) == "*"); }
    WHEN("/") { REQUIRE(lex_result_into_string(tokenizer.lex("/")) == "/"); }
    WHEN(">") { REQUIRE(lex_result_into_string(tokenizer.lex(">")) == ">"); }
    WHEN("<") { REQUIRE(lex_result_into_string(tokenizer.lex("<")) == "<"); }
    WHEN("=") { REQUIRE(lex_result_into_string(tokenizer.lex("=")) == "="); }
    WHEN("(") { REQUIRE(lex_result_into_string(tokenizer.lex("(")) == "("); }
    WHEN(")") { REQUIRE(lex_result_into_string(tokenizer.lex(")")) == ")"); }
    WHEN("invalid characters") {
      REQUIRE(lex_result_into_string(tokenizer.lex("[];")) == "[];");
    }
  }
  GIVEN("Parentheses") {
    WHEN("(())") {
      REQUIRE(lex_result_into_string(tokenizer.lex("(())")) == "(())");
    }
    WHEN("(()())") {
      REQUIRE(lex_result_into_string(tokenizer.lex("(()())")) == "(()())");
    }
  }

  GIVEN("Single Letters separated by space") {
    WHEN("* *") {
      REQUIRE(lex_result_into_string(tokenizer.lex("* *\n")) == "**");
    }
    WHEN("* (") {
      REQUIRE(lex_result_into_string(tokenizer.lex("* (\n")) == "*(");
    }
  }
}

SCENARIO("tokenizer can lex number expressions") {
  auto tokenizer = tokenizer::Tokenizer();
  GIVEN("single number") {
    WHEN("0") { REQUIRE(lex_result_into_string(tokenizer.lex("0\n")) == "0"); }
    WHEN("123456") {
      REQUIRE(lex_result_into_string(tokenizer.lex("123456\n")) == "123456");
    }
    WHEN("-123456") {
      REQUIRE(lex_result_into_string(tokenizer.lex("-123456\n")) == "-123456");
    }
  }
  GIVEN("expression") {
    WHEN("123**456") {
      REQUIRE(lex_result_into_string(tokenizer.lex("123**456\n")) ==
              "123**456");
    }
    WHEN("12+34*56") {
      REQUIRE(lex_result_into_string(tokenizer.lex("12 + 34 * 56\n")) ==
              "12+34*56");
    }
    WHEN("12/3**(4+56)") {
      REQUIRE(lex_result_into_string(tokenizer.lex("12/3**(4+56)\n")) ==
              "12/3**(4+56)");
    }
  }
}

SCENARIO("tokenizer can lex key words (except for REM)") {
  auto tokenizer = tokenizer::Tokenizer();
  GIVEN("single keyword") {
    WHEN("LET") {
      REQUIRE(lex_result_into_string(tokenizer.lex("LET")) == "LET");
    }
    WHEN("PRINT") {
      REQUIRE(lex_result_into_string(tokenizer.lex("PRINT")) == "PRINT");
    }
    WHEN("INPUT") {
      REQUIRE(lex_result_into_string(tokenizer.lex("INPUT")) == "INPUT");
    }
    WHEN("GOTO") {
      REQUIRE(lex_result_into_string(tokenizer.lex("GOTO")) == "GOTO");
    }
    WHEN("IF") { REQUIRE(lex_result_into_string(tokenizer.lex("IF")) == "IF"); }
    WHEN("THEN") {
      REQUIRE(lex_result_into_string(tokenizer.lex("THEN")) == "THEN");
    }
    WHEN("END") {
      REQUIRE(lex_result_into_string(tokenizer.lex("END")) == "END");
    }
  }
  GIVEN("single command") {
    WHEN("RUN") {
      REQUIRE(lex_result_into_string(tokenizer.lex("RUN")) == "RUN");
    }
    WHEN("LOAD") {
      REQUIRE(lex_result_into_string(tokenizer.lex("LOAD")) == "LOAD");
    }
    WHEN("LIST") {
      REQUIRE(lex_result_into_string(tokenizer.lex("LIST")) == "LIST");
    }
    WHEN("CLEAR") {
      REQUIRE(lex_result_into_string(tokenizer.lex("CLEAR")) == "CLEAR");
    }
    WHEN("HELP") {
      REQUIRE(lex_result_into_string(tokenizer.lex("HELP")) == "HELP");
    }
    WHEN("QUIT") {
      REQUIRE(lex_result_into_string(tokenizer.lex("QUIT")) == "QUIT");
    }
  }
  GIVEN("combination fo keywords") {
    WHEN("IF 1 THEN 30") {
      REQUIRE(lex_result_into_string(tokenizer.lex("IF 1 THEN 30")) ==
              "IF1THEN30");
    }
  }
}
SCENARIO("tokenizer can lex REM") {
  auto tokenizer = tokenizer::Tokenizer();
  GIVEN("simple REM") {
    WHEN("REM hello world") {
      REQUIRE(lex_result_into_string(tokenizer.lex("REM hello world")) ==
              "REMhello world");
    }
    WHEN("100 REM Program to print the Fibonacci sequence") {
      REQUIRE(lex_result_into_string(tokenizer.lex(
                  "100 REM Program to print the Fibonacci sequence")) ==
              "100REMProgram to print the Fibonacci sequence");
    }
  }
}
