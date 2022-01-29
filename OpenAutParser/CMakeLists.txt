project(OpenAutParser LANGUAGES CXX)

file(
  GLOB
  OPENAUTPARSER_SOURCES
  "src/Dummy.cpp")

file(
  GLOB
  OPENAUTPARSER_HEADERS)

add_library(${PROJECT_NAME} STATIC ${OPENAUTPARSER_SOURCES} ${OPENAUTPARSER_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Phi::Core
  PRIVATE Phi::ProjectOptions)
phi_enable_static_analyzers(${PROJECT_NAME})

add_subdirectory("tests")