#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

#include <ctype.h>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace mana {

static int Current_Token;
static int get_next_token() { return Current_Token = get_token(); }

static std::map<char, int> Binop_Precedence;

static int get_token_precedence() {
  if (!isascii(Current_Token)) {
    return -1;
  }

  // check if operator precedence has been set
  int token_precedence = Binop_Precedence[Current_Token];
  if (token_precedence <= 0) {
    return -1;
  }
  return token_precedence;
}

/**
 * @brief
 *
 * @param str
 * @return std::unique_ptr<Expr_AST>
 */
static std::unique_ptr<Expr_AST> log_error(const char *str) {
  fprintf(stderr, "Error: %s\n", str);
  return nullptr;
}

/**
 * @brief
 *
 * @param str
 * @return std::unique_ptr<Prototype_AST>
 */
static std::unique_ptr<Prototype_AST> log_error_p(const char *str) {
  log_error(str);
  return nullptr;
}

static std::unique_ptr<Expr_AST> parse_expression();

/**
 * @brief parse the number expression
 *
 * @return std::unique_ptr<Expr_AST>
 */
static std::unique_ptr<Expr_AST> parse_number() {
  auto result = std::make_unique<Number_Expr_AST>(Num_Val);
  get_next_token();
  return std::move(result);
}

/**
 * @brief parse inside the parenthesis for an expression
 *
 * @return std::unique_ptr<Expr_AST> expression that was parsed
 *         inside the parenthesis, nullptr if no expression,
 */
static std::unique_ptr<Expr_AST> parse_parenthesis() {
  get_next_token();
  auto expression_node = parse_expression();
  if (!expression_node) {
    return nullptr;
  }

  if (Current_Token != ')') {
    return log_error("expected ')'");
  }

  get_next_token();
  return expression_node;
}

static std::unique_ptr<Expr_AST> parse_identifier() {
  std::string id_name = Identifier_Str;

  get_next_token();

  if (Current_Token != '(') { // variable reference
    return std::make_unique<Variable_Expr_AST>(id_name);
  }

  get_next_token();
  std::vector<std::unique_ptr<Expr_AST>> args;
  if (Current_Token != ')') {
    while (true) {
      if (auto arg = parse_expression()) {
        args.push_back(std::move(arg));
      } else {
        return nullptr;
      }

      if (Current_Token == ')') {
        break;
      }

      if (Current_Token != ',') {
        return log_error("Expected ')' or ',' in argument list");
      }

      get_next_token();
    }
  }

  get_next_token();
  return std::make_unique<Call_Expr_AST>(id_name, std::move(args));
}

static std::unique_ptr<Expr_AST> parse_primary() {
  switch (Current_Token) {
  default:
    return log_error("unknown token when expecting an expression");
  case tok_identifier:
    return parse_identifier();
  case tok_number:
    return parse_number();
  case '(':
    return parse_parenthesis();
  }
}

static std::unique_ptr<Expr_AST>
parse_binary_op_rhs(int expr_precedence, std::unique_ptr<Expr_AST> LHS) {
  while (true) {
    int token_precedence = get_token_precedence();

    if (token_precedence < expr_precedence) {
      return LHS;
    }

    int binary_operator = Current_Token;
    get_next_token();

    auto RHS = parse_primary();
    if (!RHS) {
      return nullptr;
    }

    int next_precedence = get_token_precedence();
    if (token_precedence < next_precedence) {
      RHS = parse_binary_op_rhs(token_precedence + 1, std::move(RHS));
      if (!RHS) {
        return nullptr;
      }
    }

    // merge lefthand and righthand
    LHS = std::make_unique<Binary_Expr_AST>(binary_operator, std::move(LHS),
                                            std::move(RHS));
  }
}

static std::unique_ptr<Expr_AST> parse_expression() {
  auto LHS = parse_primary();
  if (!LHS) {
    return nullptr;
  }

  return parse_binary_op_rhs(0, std::move(LHS));
}

static std::unique_ptr<Prototype_AST> parse_prototype() {
  if (Current_Token != tok_identifier) {
    return log_error_p("Expected function name in prototype");
  }

  std::string function_name = Identifier_Str;
  get_next_token();

  if (Current_Token != '(') {
    return log_error_p("Expected '(' in prototype");
  }

  // read in arguments
  std::vector<std::string> arg_names;
  while (get_next_token() == tok_identifier) {
    arg_names.push_back(Identifier_Str);
  }
  if (Current_Token != ')') {
    return log_error_p("Expected ')' in prototype");
  }

  get_next_token();

  return std::make_unique<Prototype_AST>(function_name, std::move(arg_names));
}

static std::unique_ptr<Function_AST> parse_definition() {
  get_next_token();
  auto prototype = parse_prototype();
  if (!prototype) {
    return nullptr;
  }

  if (auto expression = parse_expression()) {
    return std::make_unique<Function_AST>(std::move(prototype),
                                          std::move(expression));
  }

  return nullptr;
}

static std::unique_ptr<Function_AST> parse_top_level_expression() {
  if (auto expression = parse_expression()) {
    auto prototype =
        std::make_unique<Prototype_AST>("__anon_expr", std::vector<std::string>());
    return std::make_unique<Function_AST>(std::move(prototype),
                                          std::move(expression));
  }
  return nullptr;
}

static std::unique_ptr<Prototype_AST> parse_external() {
  get_next_token();
  return parse_prototype();
}

} // namespace mana

#endif