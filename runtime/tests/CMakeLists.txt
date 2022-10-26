# Project
project("OpenAutoItRuntimeUnittests" CXX)

# Files
file(GLOB OPENAUTOITRUNTIME_TEST_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB OPENAUTOITRUNTIME_TEST_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${OPENAUTOITRUNTIME_TEST_SOURCES}
                                                    ${OPENAUTOITRUNTIME_TEST_HEADERS})

add_executable(${PROJECT_NAME} ${OPENAUTOITRUNTIME_TEST_SOURCES} ${OPENAUTOITRUNTIME_TEST_HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE OpenAutoIt::Runtime Phi::Test)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tests")

phi_add_test(TARGET ${PROJECT_NAME})
