project(OpenAutoItParser LANGUAGES CXX)

file(GLOB_RECURSE OPENAUTOITPARSER_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

file(GLOB_RECURSE OPENAUTOITPARSER_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${OPENAUTOITPARSER_SOURCES} ${OPENAUTOITPARSER_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} PUBLIC Phi::Core fmt::fmt magic_enum::magic_enum)

add_subdirectory("tests")

add_subdirectory("fuzzers")
