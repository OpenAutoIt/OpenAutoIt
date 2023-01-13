#!/bin/bash -eu

# Create build directory
mkdir -p build
cd build || exit 1

# Configure cmake
cmake .. -G Ninja \
    -DCMAKE_C_COMPILER="${CC}" -DCMAKE_CXX_COMPILER="${CXX}" \
    -DCMAKE_C_FLAGS="${CFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS" -DCMAKE_CXX_FLAGS="${CXXFLAGS} -DPHI_CONFIG_ENABLE_ASSERTIONS" \
    -DPHI_FUZZING_LIBRARY:STRING="${LIB_FUZZING_ENGINE}"

# Build fuzzers
ninja OpenAutoItFuzzer_Lexer OpenAutoItFuzzer_Parser OpenAutoItFuzzer_Interpreter

# Copy fuzzers to out
cp bin/* "${OUT}/"

# Build dictionary
cd ..

cat scripts/dictionary/code.dict >"${OUT}/OpenAutoItFuzzer_Lexer.dict"
cat scripts/dictionary/tokens.dict >>"${OUT}/OpenAutoItFuzzer_Lexer.dict"

cp "${OUT}/OpenAutoItFuzzer_Lexer.dict" "${OUT}/OpenAutoItFuzzer_Parser.dict"
cp "${OUT}/OpenAutoItFuzzer_Lexer.dict" "${OUT}/OpenAutoItFuzzer_Interpreter.dict"

# Build seed corpus
mkdir -p "OpenAutoItFuzzer_Lexer_seed_corpus"

cp -a "fuzzing/samples/." "OpenAutoItFuzzer_Lexer_seed_corpus"
find samples -type f -exec bash -c 'cp ${0} OpenAutoItFuzzer_Lexer_seed_corpus' {} \;

zip -r "${OUT}/OpenAutoItFuzzer_Lexer_seed_corpus.zip" "OpenAutoItFuzzer_Lexer_seed_corpus"
cp "${OUT}/OpenAutoItFuzzer_Lexer_seed_corpus.zip" "${OUT}/OpenAutoItFuzzer_Parser_seed_corpus.zip"
cp "${OUT}/OpenAutoItFuzzer_Lexer_seed_corpus.zip" "${OUT}/OpenAutoItFuzzer_Interpreter_seed_corpus.zip"
