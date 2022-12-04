#include "tokenizer.h"

namespace {

bool is_whitespace(char c) { return c == ' ' || c == '\t' || c == '\n'; }
bool is_digit(char c) { return '0' <= c && c <= '9'; }
bool is_letter(char c) { return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z'; }

}  // namespace
namespace tokenizer {

Vec<Rc<Token>> Tokenizer::lex(const Str &source) {
  source_ = source;
  words_ = Vec<Rc<Token>>();
  begin_ = 0;
  current_ = 0;

  while (peek()) {
    // TODO REM Status

    switch (status_) {
      case Status::Normal: {
        lex_normal();
      } break;
      case Status::Rem: {
        // TODO
        //        lex_rem();
      } break;
    }
  }

  return std::move(words_);
}

void Tokenizer::lex_normal() {
  while (peek() && status_ == Status::Normal) {
    char c = peek();

    if (is_digit(c)) {
      // TODO
      //      lex_number();
      continue;
    }

    if (is_letter(c)) {
      // TODO
      //      lex_word();
      continue;
    }

    if (c == '+') {
      words_.emplace_back(std::make_shared<token::Plus>());
      eat();
      continue;
    }

    if (c == '-') {
      words_.emplace_back(std::make_shared<token::Minus>());
      eat();
      continue;
    }

    if (c == '*') {
      // TODO
      //      lex_star();
      continue;
    }

    if (c == '/') {
      words_.emplace_back(std::make_shared<token::Divide>());
      eat();
      continue;
    }

    if (c == '>') {
      words_.emplace_back(std::make_shared<token::Greater>());
      eat();
      continue;
    }

    if (c == '=') {
      words_.emplace_back(std::make_shared<token::Equal>());
      eat();
      continue;
    }

    if (c == '<') {
      words_.emplace_back(std::make_shared<token::Less>());
      eat();
      continue;
    }

    if (c == '(') {
      words_.emplace_back(std::make_shared<token::LeftParenthesis>());
      eat();
      continue;
    }
    if (c == ')') {
      words_.emplace_back(std::make_shared<token::RightParenthesis>());
      eat();
      continue;
    }
  }
}

char Tokenizer::peek() const {
  if (current_ < source_.size()) {
    return source_[current_];
  } else {
    return 0;
  }
}
char Tokenizer::eat() {
  if (current_ < source_.size()) {
    return source_[current_++];
  } else {
    return 0;
  }
}
Str Tokenizer::get_word() {
  auto begin = begin_;
  begin_ = current_;
  return source_.substr(begin, current_ - begin_);
}

}  // namespace tokenizer
