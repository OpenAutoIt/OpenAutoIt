# Project
project("OpenAutoItParserUnittests" CXX)

# Files
file(GLOB OPENAUTOITPARSER_TEST_SOURCES
     "src/Lexer.test.cpp"
     "src/Parser.test.cpp")
file(GLOB OPENAUTOITPARSER_TEST_HEADERS)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${OPENAUTOITPARSER_TEST_SOURCES} ${OPENAUTOITPARSER_TEST_HEADERS})

add_executable(${PROJECT_NAME} ${OPENAUTOITPARSER_TEST_SOURCES} ${OPENAUTOITPARSER_TEST_HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenAutoItParser Catch2::Catch2WithMain)

target_include_directories(${PROJECT_NAME} PRIVATE "include")

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tests")

phi_add_test(TARGET ${PROJECT_NAME})
