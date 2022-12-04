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

  while (peek() != -1) {
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
  while (peek() != -1 && status_ == Status::Normal) {
    char c = static_cast<char>(peek());

    if (is_digit(c)) {
      lex_number();
      continue;
    }

    if (is_letter(c)) {
      lex_word();
      continue;
    }

    if (c == '*') {
      lex_star();
      continue;
    }

    if (c == '+') {
      words_.emplace_back(std::make_shared<token::Plus>());
      eat();
      align_begin();
      continue;
    }

    if (c == '-') {
      words_.emplace_back(std::make_shared<token::Minus>());
      eat();
      align_begin();
      continue;
    }

    if (c == '/') {
      words_.emplace_back(std::make_shared<token::Divide>());
      eat();
      align_begin();
      continue;
    }

    if (c == '>') {
      words_.emplace_back(std::make_shared<token::Greater>());
      eat();
      align_begin();
      continue;
    }

    if (c == '=') {
      words_.emplace_back(std::make_shared<token::Equal>());
      eat();
      align_begin();
      continue;
    }

    if (c == '<') {
      words_.emplace_back(std::make_shared<token::Less>());
      eat();
      align_begin();
      continue;
    }

    if (c == '(') {
      words_.emplace_back(std::make_shared<token::LeftParenthesis>());
      eat();
      align_begin();
      continue;
    }
    if (c == ')') {
      words_.emplace_back(std::make_shared<token::RightParenthesis>());
      eat();
      align_begin();
      continue;
    }
    if (is_whitespace(c)) {
      eat();
      align_begin();
      continue;
    }
  }
}

void Tokenizer::lex_star() {
  eat();
  if (peek() == '*') {
    eat();
    words_.emplace_back(std::make_shared<token::Power>());
    align_begin();
  } else {
    words_.emplace_back(std::make_shared<token::Multiply>());
    align_begin();
  }
}
void Tokenizer::lex_number() {
  while (peek() != -1 && is_digit(static_cast<char>(peek()))) {
    eat();
  }
  auto word = get_word();
  words_.push_back(std::make_shared<token::Number>(std::stoll(word)));
}

int32_t Tokenizer::peek() const {
  if (current_ < source_.size()) {
    return source_[current_];
  } else {
    return -1;
  }
}
char Tokenizer::eat() {
  if (current_ < source_.size()) {
    return source_[current_++];
  } else {
    return 0;
  }
}
void Tokenizer::align_begin() { begin_ = current_; }
Str Tokenizer::get_word() {
  auto begin = begin_;
  align_begin();
  return source_.substr(begin, current_ - begin);
}
void Tokenizer::lex_word() {
  while (peek() != -1 && (is_digit(static_cast<char>(peek())) ||
                          is_letter(static_cast<char>(peek())))) {
    eat();
  }
  auto word = get_word();
  align_begin();

  // Keyword

  if (word == "REM") {
    words_.emplace_back(std::make_shared<token::Rem>());
    return;
  }
  if (word == "LET") {
    words_.emplace_back(std::make_shared<token::Let>());
    return;
  }
  if (word == "PRINT") {
    words_.emplace_back(std::make_shared<token::Print>());
    return;
  }
  if (word == "INPUT") {
    words_.emplace_back(std::make_shared<token::Input>());
    return;
  }
  if (word == "GOTO") {
    words_.emplace_back(std::make_shared<token::Goto>());
    return;
  }
  if (word == "IF") {
    words_.emplace_back(std::make_shared<token::If>());
    return;
  }
  if (word == "THEN") {
    words_.emplace_back(std::make_shared<token::Then>());
    return;
  }
  if (word == "END") {
    words_.emplace_back(std::make_shared<token::End>());
    return;
  }

  // Command

  if (word == "RUN") {
    words_.emplace_back(std::make_shared<token::Run>());
    return;
  }
  if (word == "LOAD") {
    words_.emplace_back(std::make_shared<token::Load>());
    return;
  }
  if (word == "LIST") {
    words_.emplace_back(std::make_shared<token::List>());
    return;
  }
  if (word == "CLEAR") {
    words_.emplace_back(std::make_shared<token::Clear>());
    return;
  }
  if (word == "HELP") {
    words_.emplace_back(std::make_shared<token::Help>());
    return;
  }
  if (word == "QUIT") {
    words_.emplace_back(std::make_shared<token::Quit>());
    return;
  }

  // Variant

  words_.emplace_back(std::make_shared<token::Variant>(word));
}

}  // namespace tokenizer
