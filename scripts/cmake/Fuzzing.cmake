# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run") # Default of 10 seconds

function(oa_define_fuzzer name source libraries)
  if (NOT PHI_COMPILER_CLANG)
    return()
  endif()

  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE Phi::ProjectOptions -coverage -fsanitize=fuzzer,undefined,address -fno-omit-frame-pointer -fno-common -g ${libraries})
  target_compile_options(${name} PRIVATE -fsanitize=fuzzer,undefined,address)
  target_include_directories(${name} PRIVATE "include")

  add_test(NAME ${name}_run COMMAND ${name} -max_total_time=${FUZZ_RUNTIME})
endfunction()
