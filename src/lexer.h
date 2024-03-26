#ifndef LEXER_H
#define LEXER_H

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace mana {

// returns tokens [0-255] if it is an unknown character
enum Token {
  tok_eof = -1,

  // commands
  tok_def = -2,
  tok_extern = -3,

  // primary
  tok_identifier = -4,
  tok_number = -5,
};

static std::string Identifier_Str; // tok_identifier
static double Num_Val; // tok_number

/**
 * @brief Lexer that returns the next token
 * 
 * @return <int> the next token from standard input
 */
static int get_token() {
  static int Last_Char = ' ';

  // skip whitespace
  while (std::isspace(Last_Char)) {
    Last_Char = getchar();
  }

  if (std::isalpha(Last_Char)) { // [a-zA-Z]
    Identifier_Str = Last_Char;

    while (std::isalnum((Last_Char = getchar()))) {
      Identifier_Str += Last_Char;
    }

    if (Identifier_Str == "def") {
      return tok_def;
    }
    if (Identifier_Str == "extern") {
      return tok_extern;
    }
    return tok_identifier;
  }

  if (std::isdigit(Last_Char) || Last_Char == '.') { // [0-9.]
    std::string num_str;

    do {
      num_str += Last_Char;
      Last_Char = getchar();
    } while (std::isdigit(Last_Char) || Last_Char == '.');

    Num_Val = strtod(num_str.c_str(), nullptr);
    return tok_number;
  }

  if (Last_Char == '#') { // comment
    do {
      Last_Char = getchar();
    } while (Last_Char != EOF && Last_Char != '\n' && Last_Char != '\r');

    if (Last_Char != EOF) {
      return get_token();
    }
  }

  // check for end of file
  if (Last_Char == EOF) {
    return tok_eof;
  }

  // return last character's ascii value
  int this_char = Last_Char;
  Last_Char = getchar();
  return this_char;
}

} // namespace mana

#endif