# Project
project("OpenAutoItParserUnittests" CXX)

# Files
file(GLOB_RECURSE OPENAUTOITPARSER_TEST_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE OPENAUTOITPARSER_TEST_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${OPENAUTOITPARSER_TEST_SOURCES}
                                                    ${OPENAUTOITPARSER_TEST_HEADERS})

add_executable(${PROJECT_NAME} ${OPENAUTOITPARSER_TEST_SOURCES} ${OPENAUTOITPARSER_TEST_HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenAutoIt::Parser Phi::Test)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tests")

phi_add_test(TARGET ${PROJECT_NAME})
