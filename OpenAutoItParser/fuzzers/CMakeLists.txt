# Project
project("OpenAutoItParserFuzzers" CXX)

include(Fuzzing)

oa_define_fuzzer("OpenAutoItFuzzer_Lexer" "src/Lexer.fuzz.cpp" "OpenAutoItParser")
#oa_define_fuzzer("OpenAutoItFuzzer_Parser" "src/Parser.fuzz.cpp" "OpenAutoItParser")
