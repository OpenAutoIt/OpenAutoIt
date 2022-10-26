# Project
project("OpenAutoItRuntimeFuzzers" CXX)

include(Fuzzing)

oa_define_fuzzer("OpenAutoItFuzzer_Interpreter" "src/Interpreter.fuzz.cpp" "OpenAutoItRuntime")
