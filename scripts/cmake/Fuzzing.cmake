# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run") # Default of 10 seconds

# Get all regression sample files
file(GLOB_RECURSE fuzz_sample_files LIST_DIRECTORIES false CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/OpenAutoItParser/fuzzers/samples/*)

function(oa_define_fuzzer name source libraries)
  if (NOT PHI_COMPILER_CLANG)
    return()
  endif()

  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE -fsanitize=fuzzer ${libraries})
  target_compile_options(${name} PRIVATE -fsanitize=fuzzer)
  target_include_directories(${name} PRIVATE "include")
  # Don't inline functions defined as always inline
  target_compile_definitions(${name} PRIVATE PHI_CONFIG_ALWAYS_INLINE_OVERWRITE=PHI_NEVER_INLINE)

  phi_target_enable_sanitizer(TARGET ${name} SANITIZERS "address;undefined")

  add_test(NAME ${name}_run            COMMAND ${name} -max_total_time=${FUZZ_RUNTIME})
  add_test(NAME ${name}_run_only_ascii COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -only_ascii=1)

  add_test(NAME ${name}_run_with_tokens_dict            COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -dict=${CMAKE_SOURCE_DIR}/OpenAutoItParser/fuzzers/dictionary/tokens.dict)
  add_test(NAME ${name}_run_with_tokens_dict_only_ascii COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -dict=${CMAKE_SOURCE_DIR}/OpenAutoItParser/fuzzers/dictionary/tokens.dict -only_ascii=1)

  add_test(NAME ${name}_run_with_code_dict            COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -dict=${CMAKE_SOURCE_DIR}/OpenAutoItParser/fuzzers/dictionary/code.dict)
  add_test(NAME ${name}_run_with_code_dict_only_ascii COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -dict=${CMAKE_SOURCE_DIR}/OpenAutoItParser/fuzzers/dictionary/code.dict -only_ascii=1)

  # Regression testing
  add_test(NAME ${name}_regression COMMAND ${name} ${fuzz_sample_files})
endfunction()
