cmake_minimum_required(VERSION 3.5.0)

project(manalang LANGUAGES CXX)

add_executable(mana 
  "src/ast.h" 
  "src/lexer.h"
  "src/parser.h"
  "src/main.cpp"
)