#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace mana {

// Base class for all expression nodes
class Expr_AST {
public:
  virtual ~Expr_AST() = default;
};

// numeric literals i.e. 1.0
class Number_Expr_AST : public Expr_AST {
  double val;

public:
  Number_Expr_AST(double val) : val(val) {}
};

// variables
class Variable_Expr_AST : public Expr_AST {
  std::string name;

public:
  Variable_Expr_AST(const std::string &name) : name(name) {}
};

class Binary_Expr_AST : public Expr_AST {
  char Op;
  std::unique_ptr<Expr_AST> LHS, RHS;

public:
  Binary_Expr_AST(char Op, std::unique_ptr<Expr_AST> LHS,
                  std::unique_ptr<Expr_AST> RHS)
      : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

// function calls
class Call_Expr_AST : public Expr_AST {
  std::string callee;
  std::vector<std::unique_ptr<Expr_AST>> args;

public:
  Call_Expr_AST(const std::string &callee,
                std::vector<std::unique_ptr<Expr_AST>> args)
      : callee(callee), args(std::move(args)) {}
};

// function prototypes
class Prototype_AST {
  std::string name;
  std::vector<std::string> args;

public:
  Prototype_AST(const std::string &name, std::vector<std::string> args)
      : name(name), args(std::move(args)) {}

  const std::string &getName() const { return name; }
};

// function definitions
class Function_AST {
  std::unique_ptr<Prototype_AST> proto;
  std::unique_ptr<Expr_AST> body;

public:
  Function_AST(std::unique_ptr<Prototype_AST> proto,
               std::unique_ptr<Expr_AST> body)
      : proto(std::move(proto)), body(std::move(body)) {}
};

} // namespace mana

#endif