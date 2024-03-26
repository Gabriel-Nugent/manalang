#include "lexer.h"
#include "parser.h"


#include <cstdio>

namespace mana {

static void handle_definition() {
  if (parse_definition()) {
    fprintf(stderr, "Parsed a function definition.\n");
  } else {
    get_next_token();
  }
}

static void handle_external() {
  if (parse_external()) {
    fprintf(stderr, "Parsed an extern\n");
  } else {
    get_next_token();
  }
}

static void handle_top_level_expression() {
  if (parse_top_level_expression()) {
    fprintf(stderr, "Parsed a top-level expression\n");
  } else {
    get_next_token();
  }
}

static void main_loop() {
  while (true) {
    fprintf(stderr, "ready> ");
    switch (Current_Token) {
    case tok_eof:
      return;
    case ';': // ignore top-level semicolons.
      get_next_token();
      break;
    case tok_def:
      handle_definition();
      break;
    case tok_extern:
      handle_external();
      break;
    default:
      handle_top_level_expression();
      break;
    }
  }
}

} // namespace mana

int main() {
  // set operator precedences
  mana::Binop_Precedence['<'] = 10;
  mana::Binop_Precedence['+'] = 20;
  mana::Binop_Precedence['-'] = 20;
  mana::Binop_Precedence['*'] = 40;

  fprintf(stderr, "ready> ");
  mana::get_next_token();

  mana::main_loop();

  return 0;
}