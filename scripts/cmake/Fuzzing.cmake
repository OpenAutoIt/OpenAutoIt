# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run") # Default of 10 seconds

# Get all regression sample files
file(
  GLOB_RECURSE fuzz_sample_files
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/parser/fuzzing/samples/*
  ${CMAKE_SOURCE_DIR}/runtime/fuzzing/samples/*)

function(oa_define_fuzzer name source libraries)
  if(NOT PHI_COMPILER_CLANG OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    return()
  endif()

  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE -fsanitize=fuzzer ${libraries})
  target_compile_options(${name} PRIVATE -fsanitize=fuzzer)
  target_include_directories(${name} PRIVATE "include")

  add_test(NAME ${name}_run COMMAND ${name} -max_total_time=${FUZZ_RUNTIME})
  add_test(NAME ${name}_run_only_ascii COMMAND ${name} -max_total_time=${FUZZ_RUNTIME}
                                               -only_ascii=1)

  add_test(NAME ${name}_run_with_tokens_dict
           COMMAND ${name} -max_total_time=${FUZZ_RUNTIME}
                   -dict=${CMAKE_SOURCE_DIR}/scripts/dictionary/tokens.dict)
  add_test(NAME ${name}_run_with_tokens_dict_only_ascii
           COMMAND ${name} -max_total_time=${FUZZ_RUNTIME}
                   -dict=${CMAKE_SOURCE_DIR}/scripts/dictionary/tokens.dict -only_ascii=1)

  add_test(NAME ${name}_run_with_code_dict
           COMMAND ${name} -max_total_time=${FUZZ_RUNTIME}
                   -dict=${CMAKE_SOURCE_DIR}/scripts/dictionary/code.dict)
  add_test(NAME ${name}_run_with_code_dict_only_ascii
           COMMAND ${name} -max_total_time=${FUZZ_RUNTIME}
                   -dict=${CMAKE_SOURCE_DIR}/scripts/dictionary/code.dict -only_ascii=1)

  # Regression testing
  add_test(NAME ${name}_regression COMMAND ${name} ${fuzz_sample_files})
endfunction()
