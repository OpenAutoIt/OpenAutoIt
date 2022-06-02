project(OpenAutoItASTDumper LANGUAGES CXX)

file(
  GLOB
  OPENAUTOITASTDUMPER_HEADERS)

file(
  GLOB
  OPENAUTOITASTDUMPER_SOURCES
  "src/main.cpp")

add_executable(${PROJECT_NAME} ${OPENAUTOITASTDUMPER_SOURCES} ${OPENAUTOITASTDUMPER_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC OpenAutoItParser)
