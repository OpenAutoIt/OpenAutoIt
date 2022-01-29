# Project
project("OpenAutParserTests" CXX)

# Files
file(GLOB OPENAUTPARSER_TEST_SOURCES
     "src/Dummy.cpp")
file(GLOB OPENAUTPARSER_TEST_HEADERS)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${OPENAUTPARSER_TEST_SOURCES} ${OPENAUTPARSER_TEST_HEADERS})

add_executable(${PROJECT_NAME} ${OPENAUTPARSER_TEST_SOURCES} ${OPENAUTPARSER_TEST_HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenAutParser Catch2::Catch2WithMain Phi::ProjectOptions)

target_include_directories(${PROJECT_NAME} PRIVATE "include")

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tests")

phi_add_test(TARGET ${PROJECT_NAME})
