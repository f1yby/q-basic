#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#include "parser.h"

namespace parser {
Rc<AstNode> Parser::parse(const Vec<Rc<tokenizer::Token>>& tokens) {
  for (const auto& token : tokens) {
    if (typeid(*token) == typeid(tokenizer::token::Invalid)) {
      return std::make_shared<ast_node::Invalid>("unknown token");
    }
  }
  ok_ = true;
  cursor_ = 0;
  tokens_ = tokens;
  stack_ = std::stack<Rc<AstNode>>();

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Integer)) {
    parse_stmt();

  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Run) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Load) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::List) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Clear) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Help) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Quit)) {
    parse_cmd();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::EoL)) {
    return std::make_shared<ast_node::Nop>();
  } else {
    return std::make_shared<ast_node::Invalid>(
        "unexpected token at begin of the line");
  }

  if (ok_) {
    return stack_.top();
  } else {
    return error_msg_;
  }
}

void Parser::shift() {
  stack_.push(std::make_shared<ast_node::Token>(tokens_[cursor_]));
  ++cursor_;
}
Rc<AstNode> Parser::get_and_pop() {
  auto ret = stack_.top();
  stack_.pop();
  return ret;
}

void Parser::parse_stmt() {
  shift();
  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Rem)) {
    parse_rem();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Let)) {
    parse_let();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Print)) {
    parse_print();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Input)) {
    parse_input();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Goto)) {
    parse_goto();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::If)) {
    parse_if();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::End)) {
    parse_end();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::EoL)) {
    shift();
    get_and_pop();
    auto lineno = get_and_pop();
    stack_.push(std::make_shared<ast_node::ClearLine>(lineno));
    return ;
  } else

  {
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token in stmt");
    ok_ = false;
  }
  if (!ok_) {
    return;
  }

  auto stmt = get_and_pop();
  auto line_no = get_and_pop();
  stack_.push(std::make_shared<ast_node::LineNoStmt>(line_no, stmt));
}
void Parser::parse_cmd() {
  shift();
  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after command");
    return;
  }

  auto cmd = get_and_pop();

  auto token = std::static_pointer_cast<ast_node::Token>(cmd)->token();

  if (typeid(*(token)) == typeid(tokenizer::token::Run)) {
    stack_.push(std::make_shared<ast_node::Run>(cmd));
  } else if (typeid(*token) == typeid(tokenizer::token::Load)) {
    stack_.push(std::make_shared<ast_node::Load>(cmd));
  } else if (typeid(*token) == typeid(tokenizer::token::List)) {
    stack_.push(std::make_shared<ast_node::List>(cmd));
  } else if (typeid(*token) == typeid(tokenizer::token::Clear)) {
    stack_.push(std::make_shared<ast_node::Clear>(cmd));
  } else if (typeid(*token) == typeid(tokenizer::token::Help)) {
    stack_.push(std::make_shared<ast_node::Help>(cmd));
  } else if (typeid(*token) == typeid(tokenizer::token::Quit)) {
    stack_.push(std::make_shared<ast_node::Quit>(cmd));
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("unknown command");
    return;
  }
}

void Parser::parse_rem() {
  shift();

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::EoL)) {
    shift();
    get_and_pop();
    auto rem = get_and_pop();
    stack_.push(std::make_shared<ast_node::Rem>(
        rem, std::make_shared<ast_node::Token>(
                 std::make_shared<tokenizer::token::RemString>(""))));
    return;
  }

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::RemString)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after rem");
    return;
  }
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token after rem string");
    return;
  }
  shift();

  get_and_pop();
  auto rem_string = get_and_pop();
  auto rem = get_and_pop();

  stack_.push(std::make_shared<ast_node::Rem>(rem, rem_string));
}
void Parser::parse_let() {
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Variant)) {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("let requires variant");
    return;
  }
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Equal)) {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("let requires \'=\'");
    return;
  }
  shift();

  parse_expr();
  if (!ok_) {
    return;
  }

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token after expression");
    return;
  }
  shift();

  get_and_pop();
  auto expr = get_and_pop();
  auto equal = get_and_pop();
  auto variant = get_and_pop();
  auto let = get_and_pop();

  stack_.push(std::make_shared<ast_node::Let>(let, variant, equal, expr));
}
void Parser::parse_print() {
  shift();

  parse_expr();
  if (!ok_) {
    return;
  }

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token after expression");
    return;
  }
  shift();

  get_and_pop();
  auto expr = get_and_pop();
  auto print = get_and_pop();

  stack_.push(std::make_shared<ast_node::Print>(print, expr));
}
void Parser::parse_input() {
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Variant)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after input");
    return;
  }
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after variant");
    return;
  }
  shift();

  get_and_pop();
  auto variant = get_and_pop();
  auto input = get_and_pop();

  stack_.push(std::make_shared<ast_node::Input>(input, variant));
}
void Parser::parse_goto() {
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Integer)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after goto");
    return;
  }
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after number");
    return;
  }
  shift();

  get_and_pop();
  auto number = get_and_pop();
  auto _goto = get_and_pop();

  stack_.push(std::make_shared<ast_node::Goto>(_goto, number));
}
void Parser::parse_if() {
  shift();

  parse_expr();
  if (!ok_) {
    return;
  }

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Then)) {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token after expression");
    return;
  }
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Integer)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after then");

    return;
  }
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after number");

    return;
  }
  shift();

  get_and_pop();
  auto number = get_and_pop();
  auto then = get_and_pop();
  auto expr = get_and_pop();
  auto _if = get_and_pop();

  stack_.push(std::make_shared<ast_node::If>(_if, expr, then, number));
}
void Parser::parse_end() {
  shift();

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::EoL)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token after end");
    return;
  }
  shift();

  get_and_pop();
  auto end = get_and_pop();

  stack_.push(std::make_shared<ast_node::End>(end));
}
void Parser::parse_expr() {
  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::LeftParenthesis)) {
    parse_parenthesis_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Variant)) {
    parse_variant_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Integer)) {
    parse_integer_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_unary_op_expr();
  } else {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unexpected token in expression");
  }

  if (!ok_) {
    return;
  }

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Greater) ||
      typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Equal) ||
      typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Less)) {
    parse_compare_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_plus_or_minus_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Multiply) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Divide)) {
    parse_multiply_or_divide_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Power)) {
    parse_power_expr();
  } else {
    return;
  }
}

void Parser::parse_variant_expr() {
  shift();
  auto variant = get_and_pop();
  stack_.push(std::make_shared<ast_node::VariantExpr>(variant));
}
void Parser::parse_integer_expr() {
  shift();
  auto variant = get_and_pop();
  stack_.push(std::make_shared<ast_node::IntegerExpr>(variant));
}
void Parser::parse_parenthesis_expr() {
  shift();

  parse_expr();
  if (!ok_) {
    return;
  }

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::RightParenthesis)) {
    ok_ = false;
    error_msg_ =
        std::make_shared<ast_node::Invalid>("unmatched left parenthesis");
    return;
  }
  shift();

  get_and_pop();
  auto expr = get_and_pop();
  get_and_pop();
  stack_.push(expr);
}
void Parser::parse_unary_op_expr() {
  shift();
  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::LeftParenthesis)) {
    parse_parenthesis_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Variant)) {
    parse_variant_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Integer)) {
    parse_integer_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_unary_op_expr();
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token in unary expression");
  }
  if (!ok_) {
    return;
  }

  // S: * / **
  // R: + - < = > $

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Multiply) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Divide) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Power)) {
    auto expr = get_and_pop();
    auto op = get_and_pop();
    if (typeid(*(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
        typeid(tokenizer::token::Minus)) {
      stack_.push(std::make_shared<ast_node::NegExpr>(op, expr));
    } else if (typeid(
                   *(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
               typeid(tokenizer::token::Plus)) {
      stack_.push(std::make_shared<ast_node::PosExpr>(op, expr));
    } else {
      ok_ = false;
      error_msg_ =
          std::make_shared<ast_node::Invalid>("unknown unary operator");
      return;
    }

    if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
        typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
      parse_plus_or_minus_expr();
    } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Greater) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Equal) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Less)) {
      parse_compare_expr();
    }
    return;
  }

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Multiply) ||
      typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Divide)) {
    parse_multiply_or_divide_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Power)) {
    parse_power_expr();
  }
  if (!ok_) {
    return;
  }

  auto expr = get_and_pop();
  auto op = get_and_pop();
  if (typeid(*(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
      typeid(tokenizer::token::Minus)) {
    stack_.push(std::make_shared<ast_node::NegExpr>(op, expr));
  } else if (typeid(
                 *(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
             typeid(tokenizer::token::Plus)) {
    stack_.push(std::make_shared<ast_node::PosExpr>(op, expr));
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("unknown unary operator");
    return;
  }
}

void Parser::parse_compare_expr() {
  shift();
  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::LeftParenthesis)) {
    parse_parenthesis_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Variant)) {
    parse_variant_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Integer)) {
    parse_integer_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_unary_op_expr();
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token in binary expression");
  }
  if (!ok_) {
    return;
  }

  // S: + - * / **
  // R: < = > $

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Plus) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Minus) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Multiply) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Divide) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Power)) {
    auto right = get_and_pop();
    auto op = std::static_pointer_cast<ast_node::Token>(get_and_pop());
    auto left = get_and_pop();
    if (typeid(*(op->token())) == typeid(tokenizer::token::Greater)) {
      stack_.push(std::make_shared<ast_node::GreaterExpr>(left, op, right));
    } else if (typeid(*(op->token())) == typeid(tokenizer::token::Equal)) {
      stack_.push(std::make_shared<ast_node::EqualExpr>(left, op, right));
    } else if (typeid(*(op->token())) == typeid(tokenizer::token::Less)) {
      stack_.push(std::make_shared<ast_node::LessExpr>(left, op, right));
    } else {
      ok_ = false;
      error_msg_ =
          std::make_shared<ast_node::Invalid>("unknown binary operator");
      return;
    }

    return;
  }

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
      typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_plus_or_minus_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Multiply) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Divide)) {
    parse_multiply_or_divide_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Power)) {
    parse_power_expr();
  }
  if (!ok_) {
    return;
  }

  auto right = get_and_pop();
  auto op = std::static_pointer_cast<ast_node::Token>(get_and_pop());
  auto left = get_and_pop();
  if (typeid(*(op->token())) == typeid(tokenizer::token::Greater)) {
    stack_.push(std::make_shared<ast_node::GreaterExpr>(left, op, right));
  } else if (typeid(*(op->token())) == typeid(tokenizer::token::Equal)) {
    stack_.push(std::make_shared<ast_node::EqualExpr>(left, op, right));
  } else if (typeid(*(op->token())) == typeid(tokenizer::token::Less)) {
    stack_.push(std::make_shared<ast_node::LessExpr>(left, op, right));
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("unknown binary operator");
    return;
  }
}
void Parser::parse_plus_or_minus_expr() {
  shift();
  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::LeftParenthesis)) {
    parse_parenthesis_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Variant)) {
    parse_variant_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Integer)) {
    parse_integer_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_unary_op_expr();
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token in binary expression");
  }
  if (!ok_) {
    return;
  }

  // S: * / **
  // R: + - < = > $

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Multiply) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Divide) &&
      typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Power)) {
    auto right = get_and_pop();
    auto op = get_and_pop();
    auto left = get_and_pop();
    if (typeid(*(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
        typeid(tokenizer::token::Minus)) {
      stack_.push(std::make_shared<ast_node::MinusExpr>(left, op, right));
    } else if (typeid(
                   *(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
               typeid(tokenizer::token::Plus)) {
      stack_.push(std::make_shared<ast_node::PlusExpr>(left, op, right));
    } else {
      ok_ = false;
      error_msg_ =
          std::make_shared<ast_node::Invalid>("unknown unary operator");
      return;
    }

    if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
        typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
      parse_plus_or_minus_expr();
    } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Greater) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Equal) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Less)) {
      parse_compare_expr();
    }
    return;
  }

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Multiply) ||
      typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Divide)) {
    parse_multiply_or_divide_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Power)) {
    parse_power_expr();
  }
  if (!ok_) {
    return;
  }

  auto right = get_and_pop();
  auto op = get_and_pop();
  auto left = get_and_pop();
  if (typeid(*(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
      typeid(tokenizer::token::Minus)) {
    stack_.push(std::make_shared<ast_node::MinusExpr>(left, op, right));
  } else if (typeid(
                 *(std::static_pointer_cast<ast_node::Token>(op)->token())) ==
             typeid(tokenizer::token::Plus)) {
    stack_.push(std::make_shared<ast_node::PlusExpr>(left, op, right));
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("unknown binary operator");
    return;
  }
}
void Parser::parse_multiply_or_divide_expr() {
  shift();
  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::LeftParenthesis)) {
    parse_parenthesis_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Variant)) {
    parse_variant_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Integer)) {
    parse_integer_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_unary_op_expr();
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token in binary expression");
  }
  if (!ok_) {
    return;
  }

  // S: **
  // R: + - * / < = > $

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Power)) {
    auto right = get_and_pop();
    auto op = std::static_pointer_cast<ast_node::Token>(get_and_pop());
    auto left = get_and_pop();
    if (typeid(*(op->token())) == typeid(tokenizer::token::Multiply)) {
      stack_.push(std::make_shared<ast_node::MultiplyExpr>(left, op, right));
    } else if (typeid(*(op->token())) == typeid(tokenizer::token::Divide)) {
      stack_.push(std::make_shared<ast_node::DivideExpr>(left, op, right));
    } else {
      ok_ = false;
      error_msg_ =
          std::make_shared<ast_node::Invalid>("unknown binary operator");
      return;
    }

    if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
        typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
      parse_plus_or_minus_expr();
    } else if (typeid(*tokens_[cursor_]) ==
                   typeid(tokenizer::token::Multiply) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Divide)) {
      parse_multiply_or_divide_expr();
    } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Greater) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Equal) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Less)) {
      parse_compare_expr();
    }
    return;
  }

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Power)) {
    parse_power_expr();
  }
  if (!ok_) {
    return;
  }

  auto right = get_and_pop();
  auto op = std::static_pointer_cast<ast_node::Token>(get_and_pop());
  auto left = get_and_pop();
  if (typeid(*(op->token())) == typeid(tokenizer::token::Multiply)) {
    stack_.push(std::make_shared<ast_node::MultiplyExpr>(left, op, right));
  } else if (typeid(*(op->token())) == typeid(tokenizer::token::Divide)) {
    stack_.push(std::make_shared<ast_node::DivideExpr>(left, op, right));
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("unknown binary operator");
    return;
  }
}
void Parser::parse_power_expr() {
  shift();
  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::LeftParenthesis)) {
    parse_parenthesis_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Variant)) {
    parse_variant_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Integer)) {
    parse_integer_expr();
  } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
             typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
    parse_unary_op_expr();
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>(
        "unexpected token in binary expression");
  }
  if (!ok_) {
    return;
  }

  // S: **
  // R: + - * / < = > $

  if (typeid(*tokens_[cursor_]) != typeid(tokenizer::token::Power)) {
    auto right = get_and_pop();
    auto op = std::static_pointer_cast<ast_node::Token>(get_and_pop());
    auto left = get_and_pop();
    if (typeid(*(op->token())) == typeid(tokenizer::token::Power)) {
      stack_.push(std::make_shared<ast_node::PowerExpr>(left, op, right));
    } else {
      ok_ = false;
      error_msg_ =
          std::make_shared<ast_node::Invalid>("unknown binary operator");
      return;
    }

    if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Plus) ||
        typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Minus)) {
      parse_plus_or_minus_expr();
    } else if (typeid(*tokens_[cursor_]) ==
                   typeid(tokenizer::token::Multiply) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Divide)) {
      parse_multiply_or_divide_expr();
    } else if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Greater) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Equal) ||
               typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Less)) {
      parse_compare_expr();
    }
    return;
  }

  if (typeid(*tokens_[cursor_]) == typeid(tokenizer::token::Power)) {
    parse_power_expr();
  }
  if (!ok_) {
    return;
  }

  auto right = get_and_pop();
  auto op = std::static_pointer_cast<ast_node::Token>(get_and_pop());
  auto left = get_and_pop();
  if (typeid(*(op->token())) == typeid(tokenizer::token::Power)) {
    stack_.push(std::make_shared<ast_node::PowerExpr>(left, op, right));
  } else {
    ok_ = false;
    error_msg_ = std::make_shared<ast_node::Invalid>("unknown binary operator");
    return;
  }
}
void Parser::parse_clear_line() { shift(); }

}  // namespace parser

#pragma clang diagnostic pop