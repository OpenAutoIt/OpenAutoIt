project(OpenAutoItParser LANGUAGES CXX)

file(GLOB_RECURSE OPENAUTOITPARSER_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

file(GLOB_RECURSE OPENAUTOITPARSER_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${OPENAUTOITPARSER_SOURCES} ${OPENAUTOITPARSER_HEADERS})
add_library(OpenAutoIt::Parser ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} PUBLIC Phi::Core fmt::fmt)

add_subdirectory("tests")
