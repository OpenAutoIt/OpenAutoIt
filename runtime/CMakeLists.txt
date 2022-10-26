project(OpenAutoItRuntime LANGUAGES CXX)

file(GLOB_RECURSE OPENAUTOIT_RUNTIME_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

file(GLOB_RECURSE OPENAUTOIT_RUNTIME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${OPENAUTOIT_RUNTIME_HEADERS} ${OPENAUTOIT_RUNTIME_SOURCES})
add_library(OpenAutoIt::Runtime ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} PUBLIC OpenAutoIt::Parser)

add_subdirectory("tests")

add_subdirectory("fuzzing")
