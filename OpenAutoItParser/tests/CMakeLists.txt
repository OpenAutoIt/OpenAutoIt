# Integration tests not work with emscripten since we can't access the filesystem
if(NOT DEFINED PHI_PLATFORM_EMSCRIPTEN)
  add_subdirectory(integration)
endif()

add_subdirectory(unittests)
