project(OpenAutoItParser LANGUAGES CXX)

file(
  GLOB
  OPENAUTOITPARSER_HEADERS
  "include/OpenAutoIt/AST/ASTDocument.hpp"
  "include/OpenAutoIt/AST/ASTNode.hpp"
  "include/OpenAutoIt/AST/ASTVariableDeclaration.hpp"
  "include/OpenAutoIt/Lexer.hpp"
  "include/OpenAutoIt/Parser.hpp"
  "include/OpenAutoIt/Token.hpp"
  "include/OpenAutoIt/TokenKind.hpp"
  "include/OpenAutoIt/TokenStream.hpp")

file(
  GLOB
  OPENAUTOITPARSER_SOURCES
  "src/Lexer.cpp"
  "src/Parser.cpp"
  "src/TokenStream.cpp")

add_library(${PROJECT_NAME} STATIC ${OPENAUTOITPARSER_SOURCES} ${OPENAUTOITPARSER_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Phi::Core fmt::fmt magic_enum::magic_enum)

add_subdirectory("tests")

add_subdirectory("fuzzers")
