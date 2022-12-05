#pragma once
#include <stack>
#include <utility>

#include "tokenizer.h"
#include "type.h"

namespace {
inline void dump_indent(uint32_t indent, std::ostream& ostream) {
  for (uint32_t i{}; i < indent; ++i) {
    ostream << '\t';
  }
}
inline void dump_end_line(std::ostream& ostream) { ostream << std::endl; }

inline void dump_token(uint32_t indent, const Rc<tokenizer::Token>& token,
                       std::ostream& ostream) {
  dump_indent(indent, ostream);
  token->dump(ostream);
  dump_end_line(ostream);
}

}  // namespace

namespace parser {

class AstNode {
 public:
  virtual void dump(uint32_t indent, std::ostream& ostream) const = 0;
  virtual ~AstNode() = default;
};

namespace ast_node {

// Basic statement of basic
class Stmt : public AstNode {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override = 0;
  ~Stmt() override = default;
};

// Command
class Command : public AstNode {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override = 0;
  ~Command() override = default;
};

// Expression
class Expr : public AstNode {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override = 0;
  ~Expr() override = default;
};

// Nop for empty line
class Nop : public AstNode {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {}
};

// Error handler
class Invalid : public AstNode {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_indent(indent, ostream);
    ostream << "INVALID";
    dump_end_line(ostream);

    dump_indent(indent + 1, ostream);
    ostream << error_message_;
    dump_end_line(ostream);
  }

  explicit Invalid(Str error_message)
      : error_message_(std::move(error_message)) {}

 private:
  Str error_message_;
};

// For stack in parser
class Token : public AstNode {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, token_, ostream);
  }

  [[nodiscard]] Rc<tokenizer::Token> token() const { return token_; }

  explicit Token(Rc<tokenizer::Token> token) : token_(std::move(token)) {}

 private:
  Rc<tokenizer::Token> token_;
};

class LineNoStmt : public AstNode {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, number_, ostream);
    stmt_->dump(indent + 1, ostream);
  }

  LineNoStmt(const Rc<AstNode>& token, const Rc<AstNode>& stmt_)
      : number_(std::static_pointer_cast<tokenizer::token::Integer>(
            std::static_pointer_cast<ast_node::Token>(token)->token())),
        stmt_(std::static_pointer_cast<Stmt>(stmt_)) {}

 private:
  Rc<tokenizer::token::Integer> number_;
  Rc<Stmt> stmt_;
};

// Comment
class Rem : public Stmt {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, rem_, ostream);
    dump_token(indent + 1, rem_string_, ostream);
  }

  Rem(const Rc<AstNode>& rem, const Rc<AstNode>& rem_string)
      : rem_(std::static_pointer_cast<tokenizer::token::Rem>(
            std::static_pointer_cast<Token>(rem)->token())),
        rem_string_(std::static_pointer_cast<tokenizer::token::RemString>(
            std::static_pointer_cast<Token>(rem_string)->token())) {}

 private:
  Rc<tokenizer::token::Rem> rem_;
  Rc<tokenizer::token::RemString> rem_string_;
};

// Assignment
class Let : public Stmt {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, let_, ostream);
    dump_token(indent + 1, equal_, ostream);
    dump_token(indent + 2, variant_, ostream);
    expr_->dump(indent + 2, ostream);
  }

  Let(const Rc<AstNode>& let, const Rc<AstNode>& variant,
      const Rc<AstNode>& equal, const Rc<AstNode>& expr)
      : let_(std::static_pointer_cast<tokenizer::token::Let>(
            std::static_pointer_cast<Token>(let)->token())),
        variant_(std::static_pointer_cast<tokenizer::token::Variant>(
            std::static_pointer_cast<Token>(variant)->token())),
        equal_(std::static_pointer_cast<tokenizer::token::Equal>(
            std::static_pointer_cast<Token>(equal)->token())),
        expr_(std::static_pointer_cast<ast_node::Expr>(expr)) {}

 private:
  Rc<tokenizer::token::Let> let_;
  Rc<tokenizer::token::Variant> variant_;
  Rc<tokenizer::token::Equal> equal_;
  Rc<Expr> expr_;
};

// Output
class Print : public Stmt {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, print_, ostream);
    expr_->dump(indent + 1, ostream);
  }
  Print(const Rc<AstNode>& print, const Rc<AstNode>& expr)
      : print_(std::static_pointer_cast<tokenizer::token::Print>(
            std::static_pointer_cast<Token>(print)->token())),
        expr_(std::static_pointer_cast<ast_node::Expr>(expr)) {}

 private:
  Rc<tokenizer::token::Print> print_;
  Rc<Expr> expr_;
};

// Input
class Input : public Stmt {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, input_, ostream);
    dump_token(indent + 1, variant_, ostream);
  }

  Input(const Rc<AstNode>& input, const Rc<AstNode>& variant)
      : input_(std::static_pointer_cast<tokenizer::token::Input>(
            std::static_pointer_cast<Token>(input)->token())),
        variant_(std::static_pointer_cast<tokenizer::token::Variant>(
            std::static_pointer_cast<Token>(variant)->token())) {}

 private:
  Rc<tokenizer::token::Input> input_;
  Rc<tokenizer::token::Variant> variant_;
};

// Goto
class Goto : public Stmt {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, goto_, ostream);
    dump_token(indent + 1, number_, ostream);
  }

  Goto(const Rc<AstNode>& go_to, const Rc<AstNode>& number)
      : goto_(std::static_pointer_cast<tokenizer::token::Goto>(
            std::static_pointer_cast<Token>(go_to)->token())),
        number_(std::static_pointer_cast<tokenizer::token::Integer>(
            std::static_pointer_cast<Token>(number)->token())) {}

 private:
  Rc<tokenizer::token::Goto> goto_;
  Rc<tokenizer::token::Integer> number_;
};

// Control flow
class If : public Stmt {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, if_, ostream);
    expr_->dump(indent + 1, ostream);
    dump_token(indent, then_, ostream);
    dump_token(indent + 1, number_, ostream);
  }

  If(const Rc<AstNode>& _if, const Rc<AstNode>& expr, const Rc<AstNode>& then,
     const Rc<AstNode>& number)
      : if_(std::static_pointer_cast<tokenizer::token::If>(
            std::static_pointer_cast<Token>(_if)->token())),
        expr_(std::static_pointer_cast<ast_node::Expr>(expr)),
        then_(std::static_pointer_cast<tokenizer::token::Then>(
            std::static_pointer_cast<Token>(then)->token())),
        number_(std::static_pointer_cast<tokenizer::token::Integer>(
            std::static_pointer_cast<Token>(number)->token())) {}

 private:
  Rc<tokenizer::token::If> if_;
  Rc<Expr> expr_;
  Rc<tokenizer::token::Then> then_;
  Rc<tokenizer::token::Integer> number_;
};

// End
class End : public Stmt {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_indent(indent, ostream);
    end_->dump(ostream);
    dump_end_line(ostream);
  }

  explicit End(const Rc<AstNode>& end)
      : end_(std::static_pointer_cast<tokenizer::token::End>(
            std::static_pointer_cast<Token>(end)->token())) {}

 private:
  Rc<tokenizer::Token> end_;
};

// Expression

class VariantExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, variant_, ostream);
  }
  explicit VariantExpr(const Rc<AstNode>& variant)
      : variant_(std::static_pointer_cast<tokenizer::token::Variant>(
            std::static_pointer_cast<Token>(variant)->token())) {}

 private:
  Rc<tokenizer::token::Variant> variant_;
};
class IntegerExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, integer_, ostream);
  }
  explicit IntegerExpr(const Rc<AstNode>& variant)
      : integer_(std::static_pointer_cast<tokenizer::token::Integer>(
            std::static_pointer_cast<Token>(variant)->token())) {}

 private:
  Rc<tokenizer::token::Integer> integer_;
};

class NegExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, neg_, ostream);
    expr_->dump(indent + 1, ostream);
  }
  NegExpr(const Rc<AstNode>& neg, const Rc<AstNode>& expr)
      : neg_(std::static_pointer_cast<tokenizer::token::Minus>(
            std::static_pointer_cast<Token>(neg)->token())),
        expr_(std::static_pointer_cast<Expr>(expr)) {}

 private:
  Rc<tokenizer::token::Minus> neg_;
  Rc<Expr> expr_;
};
class PosExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, positive_, ostream);
    expr_->dump(indent + 1, ostream);
  }
  PosExpr(const Rc<AstNode>& positive, const Rc<AstNode>& expr)
      : positive_(std::static_pointer_cast<tokenizer::token::Plus>(
            std::static_pointer_cast<Token>(positive)->token())),
        expr_(std::static_pointer_cast<Expr>(expr)) {}

 private:
  Rc<tokenizer::token::Plus> positive_;
  Rc<Expr> expr_;
};

class GreaterExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  GreaterExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
              const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Greater>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Greater> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};
class EqualExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  EqualExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
            const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Equal>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Equal> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};
class LessExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  LessExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
           const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Less>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Less> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};

class PlusExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  PlusExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
           const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Plus>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Plus> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};
class MinusExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  MinusExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
            const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Minus>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Minus> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};

class MultiplyExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  MultiplyExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
               const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Multiply>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Multiply> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};
class DivideExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  DivideExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
             const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Divide>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Divide> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};

class PowerExpr : public Expr {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, op_, ostream);
    left_->dump(indent + 1, ostream);
    right_->dump(indent + 1, ostream);
  }
  PowerExpr(const Rc<AstNode>& left, const Rc<AstNode>& op,
            const Rc<AstNode>& right)
      : left_(std::static_pointer_cast<Expr>(left)),
        op_(std::static_pointer_cast<tokenizer::token::Power>(
            std::static_pointer_cast<Token>(op)->token())),
        right_(std::static_pointer_cast<Expr>(right)) {}

 private:
  Rc<tokenizer::token::Power> op_;
  Rc<Expr> left_;
  Rc<Expr> right_;
};

// User defined command

class Run : public Command {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, run_, ostream);
  }
  explicit Run(const Rc<AstNode>& run)
      : run_(std::static_pointer_cast<tokenizer::token::Run>(
            std::static_pointer_cast<Token>(run)->token())) {}

 private:
  Rc<tokenizer::token::Run> run_;
};
class Load : public Command {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, load_, ostream);
  }

  explicit Load(const Rc<AstNode>& load)
      : load_(std::static_pointer_cast<tokenizer::token::Load>(
            std::static_pointer_cast<Token>(load)->token())) {}

 private:
  Rc<tokenizer::token::Load> load_;
};
class List : public Command {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, list_, ostream);
  }

  explicit List(const Rc<AstNode>& list)
      : list_(std::static_pointer_cast<tokenizer::token::List>(
            std::static_pointer_cast<Token>(list)->token())) {}

 private:
  Rc<tokenizer::token::List> list_;
};
class Clear : public Command {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, clear_, ostream);
  }

  explicit Clear(const Rc<AstNode>& clear)
      : clear_(std::static_pointer_cast<tokenizer::token::Clear>(
            std::static_pointer_cast<Token>(clear)->token())) {}

 private:
  Rc<tokenizer::token::Clear> clear_;
};
class Help : public Command {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, help_, ostream);
  }

  explicit Help(const Rc<AstNode>& help)
      : help_(std::static_pointer_cast<tokenizer::token::Help>(
            std::static_pointer_cast<Token>(help)->token())) {}

 private:
  Rc<tokenizer::token::Help> help_;
};
class Quit : public Command {
 public:
  void dump(uint32_t indent, std::ostream& ostream) const override {
    dump_token(indent, quit_, ostream);
  }

  explicit Quit(const Rc<AstNode>& quit)
      : quit_(std::static_pointer_cast<tokenizer::token::Quit>(
            std::static_pointer_cast<Token>(quit)->token())) {}

 private:
  Rc<tokenizer::token::Quit> quit_;
};

}  // namespace ast_node

class Parser {
 public:
  Rc<AstNode> parse(const Vec<Rc<tokenizer::Token>>& tokens);

 private:
  bool ok_;
  Rc<ast_node::Invalid> error_msg_;

  uint32_t cursor_;

  Vec<Rc<tokenizer::Token>> tokens_;
  std::stack<Rc<AstNode>> stack_;

  void parse_stmt();
  void parse_cmd();

  void parse_rem();
  void parse_let();
  void parse_print();
  void parse_input();
  void parse_goto();
  void parse_if();
  void parse_end();

  void parse_expr();

  void parse_variant_expr();
  void parse_integer_expr();
  void parse_parenthesis_expr();

  void parse_unary_op_expr();
  void parse_compare_expr();
  void parse_plus_or_minus_expr();
  void parse_multiply_or_divide_expr();
  void parse_power_expr();

  void shift();

  Rc<AstNode> get_and_pop();
};

}  // namespace parser