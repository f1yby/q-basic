#pragma once
#include <algorithm>
#include <sstream>
#include <utility>

#include "type.h"

namespace tokenizer {
class Token {
 public:
  virtual void dump(std::ostream &ostream) const = 0;
  ~Token() = default;
};

namespace token {

// Numbers

class Number : public Token {
 public:
  [[nodiscard]] int64_t value() const { return value_; }

  void dump(std::ostream &ostream) const override { ostream << value_; }

 private:
  int64_t value_;
};

// Variant

class Variant : public Token {
 public:
  [[nodiscard]] Str value() const { return value_; }

  void dump(std::ostream &ostream) const override { ostream << value_; }

 private:
  Str value_;
};

// Operator

class Plus : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '+'; }
};

class Minus : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '-'; }
};

class Multiply : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '*'; }
};

class Divide : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '/'; }
};

class Power : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "**"; }
};

// Comparator

class Greater : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '>'; }
};

class Less : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '<'; }
};

class Equal : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '='; }
};

// Parenthesis

class LeftParenthesis : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '('; }
};

class RightParenthesis : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << ')'; }
};

// Keyword

class Rem : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "REM"; }
};

class Let : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "LET"; }
};

class Print : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "PRINT"; }
};

class Input : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "INPUT"; }
};

class Goto : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "GOTO"; }
};

class If : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "IF"; }
};

class Then : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "THEN"; }
};

class End : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "END"; }
};

// String inside REM

class RemString : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << value_; }

  explicit RemString(Str value) : value_(std::move(value)) {}

 private:
  Str value_;
};

// Command

class Run : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "RUN"; }
};

class Load : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "LOAD"; }
};

class List : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "LIST"; }
};

class Clear : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "CLEAR"; }
};

class Help : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "HELP"; }
};

class Quit : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << "QUIT"; }
};

// Space

class Space : public Token {
 public:
  void dump(std::ostream &ostream) const override {}
};

// End of line

class EoL : public Token {
 public:
  void dump(std::ostream &ostream) const override { ostream << '\n'; }
};

// Invalid Token (for error handling)

class Invalid : public Token {
 public:
  void dump(std::ostream &ostream) const override {
    ostream << "INVALID LINE: " << code_;
  }

  explicit Invalid(Str code) : code_(std::move(code)) {}

 private:
  Str code_;
};
}  // namespace token

class Tokenizer {
 public:
  Vec<Rc<Token>> lex(const Str &source);

 private:
  enum class Status {
    Normal,
    Rem,
  };

  // Input
  Str source_;

  // Output
  Vec<Rc<Token>> words_;

  // Position of source
  uint32_t begin_;
  uint32_t current_;

  Status status_;

  [[nodiscard]] int32_t peek() const;
  char eat();

  void align_begin();
  Str get_word();

  void lex_normal();
  void lex_rem();


  void lex_star();
  void lex_number();
  void lex_word();
};

}  // namespace tokenizer