# Project
project("OpenAutoItParserFuzzers" CXX)

include(Fuzzing)

oa_define_fuzzer("OpenAutoItFuzzer_Tokenizer" "src/Tokenizer.fuzz.cpp" "OpenAutoItParser")
