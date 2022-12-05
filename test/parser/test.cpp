#include <catch2/catch_all.hpp>

#include "parser.h"
namespace {
Str parser_result_into_str(const Rc<parser::AstNode>& node) {
  std::stringstream ss;
  node->dump(0, ss);
  return ss.str();
}
}  // namespace

SCENARIO("parser can parse command", "[parser]") {
  auto tokenizer = tokenizer::Tokenizer();
  auto parser = parser::Parser();

  GIVEN("RUN") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("RUN"))) ==
            "RUN\n");
  }

  GIVEN("LOAD") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("LOAD"))) ==
            "LOAD\n");
  }
  GIVEN("LIST") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("LIST"))) ==
            "LIST\n");
  }
  GIVEN("CLEAR") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("CLEAR"))) ==
            "CLEAR\n");
  }
  GIVEN("HELP") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("HELP"))) ==
            "HELP\n");
  }
  GIVEN("QUIT") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("QUIT"))) ==
            "QUIT\n");
  }

  GIVEN("INVALID") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("RUN 123"))) ==
            "INVALID\n"
            "\tunexpected token after command\n");
    REQUIRE(
        parser_result_into_str(parser.parse(tokenizer.lex("RUN and QUIT"))) ==
        "INVALID\n"
        "\tunexpected token after command\n");
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("HELL"))) ==
            "INVALID\n"
            "\tunexpected token at begin of the line\n");
  }
}

SCENARIO("parser can parse script with simple expression", "[parser]") {
  auto tokenizer = tokenizer::Tokenizer();
  auto parser = parser::Parser();

  GIVEN("REM") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex(
                "100 REM Program to print the Fibonacci sequence"))) ==
            "100\n"
            "\tREM\n"
            "\t\tProgram to print the Fibonacci sequence\n");
  }
  GIVEN("LET") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("100 LET abc123 = 123"))) ==
            "100\n"
            "\tLET\n"
            "\t\t=\n"
            "\t\t\tabc123\n"
            "\t\t\t123\n");
  }
  GIVEN("PRINT") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("80 PRINT abc123"))) ==
            "80\n"
            "\tPRINT\n"
            "\t\tabc123\n");
  }
  GIVEN("GOTO") {
    REQUIRE(
        parser_result_into_str(parser.parse(tokenizer.lex("180 GOTO 140"))) ==
        "180\n"
        "\tGOTO\n"
        "\t\t140\n");
  }
  GIVEN("IF") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("180 IF n1 > max THEN 140"))) ==
            "180\n"
            "\tIF\n"
            "\t\t>\n"
            "\t\t\tn1\n"
            "\t\t\tmax\n"
            "\tTHEN\n"
            "\t\t140\n");
  }
  GIVEN("INPUT") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("90 INPUT abc123"))) ==
            "90\n"
            "\tINPUT\n"
            "\t\tabc123\n");
  }
  GIVEN("END") {
    REQUIRE(parser_result_into_str(parser.parse(tokenizer.lex("190 END"))) ==
            "190\n"
            "\tEND\n");
  }
}

SCENARIO("parser can parse complex expression", "[parser]") {
  auto tokenizer = tokenizer::Tokenizer();
  auto parser = parser::Parser();
  GIVEN("0 LET n = 1+2+3") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("0 LET n = 1+2+3"))) ==
            "0\n"
            "\tLET\n"
            "\t\t=\n"
            "\t\t\tn\n"
            "\t\t\t+\n"
            "\t\t\t\t+\n"
            "\t\t\t\t\t1\n"
            "\t\t\t\t\t2\n"
            "\t\t\t\t3\n");
  }
  GIVEN("0 LET n = 1*2+3") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("0 LET n = 1*2+3"))) ==
            "0\n"
            "\tLET\n"
            "\t\t=\n"
            "\t\t\tn\n"
            "\t\t\t+\n"
            "\t\t\t\t*\n"
            "\t\t\t\t\t1\n"
            "\t\t\t\t\t2\n"
            "\t\t\t\t3\n");
  }

  GIVEN("0 LET n = 1+2*3") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("0 LET n = 1+2*3"))) ==
            "0\n"
            "\tLET\n"
            "\t\t=\n"
            "\t\t\tn\n"
            "\t\t\t+\n"
            "\t\t\t\t1\n"
            "\t\t\t\t*\n"
            "\t\t\t\t\t2\n"
            "\t\t\t\t\t3\n");
  }
  GIVEN("0 LET n = (1+2)*3") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("0 LET n = (1+2)*3"))) ==
            "0\n"
            "\tLET\n"
            "\t\t=\n"
            "\t\t\tn\n"
            "\t\t\t*\n"
            "\t\t\t\t+\n"
            "\t\t\t\t\t1\n"
            "\t\t\t\t\t2\n"
            "\t\t\t\t3\n");
  }
  GIVEN("0 LET n = 1**2**3") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("0 LET n = 1**2**3"))) ==
            "0\n"
            "\tLET\n"
            "\t\t=\n"
            "\t\t\tn\n"
            "\t\t\t**\n"
            "\t\t\t\t1\n"
            "\t\t\t\t**\n"
            "\t\t\t\t\t2\n"
            "\t\t\t\t\t3\n");
  }
  GIVEN("0 LET n = --++1**2**3") {
    REQUIRE(parser_result_into_str(
                parser.parse(tokenizer.lex("0 LET n = --++1**2**3"))) ==
            "0\n"
            "\tLET\n"
            "\t\t=\n"
            "\t\t\tn\n"
            "\t\t\t-\n"
            "\t\t\t\t-\n"
            "\t\t\t\t\t+\n"
            "\t\t\t\t\t\t+\n"
            "\t\t\t\t\t\t\t**\n"
            "\t\t\t\t\t\t\t\t1\n"
            "\t\t\t\t\t\t\t\t**\n"
            "\t\t\t\t\t\t\t\t\t2\n"
            "\t\t\t\t\t\t\t\t\t3\n");
  }
}