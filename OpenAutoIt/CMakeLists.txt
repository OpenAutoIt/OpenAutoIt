project(OpenAutoIt LANGUAGES CXX)

file(GLOB_RECURSE OPENAUTOIT_HEADERS CONFIGURE_DEPENDS "include/*.hpp")

file(GLOB_RECURSE OPENAUTOIT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${PROJECT_NAME} ${OPENAUTOIT_HEADERS} ${OPENAUTOIT_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC OpenAutoIt::Parser OpenAutoIt::Runtime)
target_include_directories(${PROJECT_NAME} PUBLIC "include")
