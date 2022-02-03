project(OpenAutoItParser LANGUAGES CXX)

file(
  GLOB
  OPENAUTOITPARSER_HEADERS
  "include/OpenAutoIt/Token.hpp"
  "include/OpenAutoIt/Tokenizer.hpp")

file(
  GLOB
  OPENAUTOITPARSER_SOURCES
  "src/Tokenizer.cpp")

add_library(${PROJECT_NAME} STATIC ${OPENAUTOITPARSER_SOURCES} ${OPENAUTOITPARSER_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Phi::Core fmt::fmt magic_enum::magic_enum
  PRIVATE Phi::ProjectOptions)
phi_enable_static_analyzers(${PROJECT_NAME})

add_subdirectory("tests")

add_subdirectory("fuzzers")
