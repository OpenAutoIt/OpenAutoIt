# Project
project("OpenAutoItParserIntegrationTests" CXX)

# Files
file(GLOB OPENAUTOITPARSER_TEST_SOURCES "src/main.cpp")
file(GLOB OPENAUTOITPARSER_TEST_HEADERS)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${OPENAUTOITPARSER_TEST_SOURCES}
                                                    ${OPENAUTOITPARSER_TEST_HEADERS})

add_executable(${PROJECT_NAME} ${OPENAUTOITPARSER_TEST_SOURCES} ${OPENAUTOITPARSER_TEST_HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenAutoItParser)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tests")

add_test(NAME ${PROJECT_NAME}
         COMMAND $<TARGET_FILE:${PROJECT_NAME}>
                 "${CMAKE_SOURCE_DIR}/OpenAutoItParser/tests/integration/samples")
