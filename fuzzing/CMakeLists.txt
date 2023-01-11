# Project
project("OpenAutoItFuzzers" CXX)

# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run") # Default of 10 seconds

# Get all regression sample files
file(
  GLOB_RECURSE fuzz_sample_files
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/fuzzing/samples/*")

file(
  GLOB_RECURSE fuzz_dictionaries
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/scripts/dictionary/*")

function(oa_define_fuzzer name source libraries)
  if(NOT PHI_COMPILER_CLANG OR ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    return()
  endif()

  add_executable(${name} ${source})
  target_link_libraries(${name} PRIVATE -fsanitize=fuzzer ${libraries})
  target_compile_options(${name} PRIVATE -fsanitize=fuzzer)
  target_include_directories(${name} PRIVATE "include")

  add_test(
    NAME "${name}_run"
    CONFIGURATIONS "Release"
    COMMAND ${name} -max_total_time=${FUZZ_RUNTIME})
  add_test(
    NAME "${name}_run_only_ascii"
    CONFIGURATIONS "Release"
    COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -only_ascii=1)

  # Regression testing
  add_test(
    NAME "${name}_regression"
    CONFIGURATIONS "Release"
    COMMAND "${name}" ${fuzz_sample_files})

  # Test with
  foreach(dictionary ${fuzz_dictionaries})
    # Get just the name without the extension
    cmake_path(GET dictionary STEM file_name)

    add_test(
      NAME "${name}_run_with_${file_name}_dict"
      CONFIGURATIONS "Release"
      COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -dict=${dictionary})

    add_test(
      NAME "${name}_run_with_${file_name}_dict_only_ascii"
      CONFIGURATIONS "Release"
      COMMAND ${name} -max_total_time=${FUZZ_RUNTIME} -dict=${dictionary} -only_ascii=1)
  endforeach()
endfunction()

oa_define_fuzzer("OpenAutoItFuzzer_Lexer" "src/Lexer.fuzz.cpp" "OpenAutoItParser")
oa_define_fuzzer("OpenAutoItFuzzer_Parser" "src/Parser.fuzz.cpp" "OpenAutoItParser")
oa_define_fuzzer("OpenAutoItFuzzer_Interpreter" "src/Interpreter.fuzz.cpp" "OpenAutoItRuntime")
