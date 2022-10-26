#!/bin/bash

# Sanitizer options
export ASAN_OPTIONS="quarantine_size_mb=2048:redzone=512:strict_string_checks=1:detect_odr_violation=2:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:detect_leaks=1:print_stats=1:use_odr_indicator=1:strict_memcmp=1:print_scariness=1:handle_abort=1:symbolize=1:handle_segv=1:handle_sigill=1:allocator_release_to_os_interval_ms=500:handle_sigfpe=1:handle_sigbus=1:alloc_dealloc_mismatch=1:check_malloc_usable_size=1:abort_on_error=1"
export LSAN_OPTIONS="max_leaks=0:print_suppressions=1:report_objects=1:use_unaligned=1"
export UBSAN_OPTIONS="print_stacktrace=1:report_error_type=1:halt_on_error=1"

# Other options
jobs=$(($(nproc) / 2))
fuzzer=bin/OpenAutoItFuzzer_Interpreter
samples_dir="../samples ../parser/fuzzing/samples ../runtime/fuzzing/samples"
dictionaries="-dict=../scripts/dictionary/tokens.dict -dict=../scripts/dictionary/code.dict"
config="Release"
timeout=10

# Preparations
mkdir build-fuzz-interpreter -p
cd build-fuzz-interpreter || exit

# Configure
cmake .. -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DOA_BUILD_TOOLS:BOOL=OFF -DOA_ENABLE_SANITIZER_ADDRESS:BOOL=ON -DOA_ENABLE_SANITIZER_UNDEFINED:BOOL=ON -DCMAKE_BUILD_TYPE="$config" -DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-common -march=native -mtune=native -DPHI_DEBUG -DOA_FUZZ_BUILD" || exit

# Build
cmake --build . --config "$config" --target OpenAutoItFuzzer_Interpreter || exit

# Clean
find . -iname "*.gcno" -delete && find . -iname "*.gcda" -delete

# Create corpus
mkdir -p corpus

# Minimize corpus
mv corpus old_corpus
mkdir -p corpus
$fuzzer corpus old_corpus $samples_dir -merge=1 -use_value_profile=1 -close_fd_mask=1 -rss_limit_mb=0 -only_ascii=1
rm -rf old_corpus

# Initial range fuzz
for i in {1,2,4,8,16,32,64,128}; do
    $fuzzer corpus $samples_dir -use_value_profile=1 -close_fd_mask=1 -timeout=$timeout -jobs="$jobs" -workers="$jobs" -max_len="$i" -runs=1000000 -rss_limit_mb=4096 $dictionaries -only_ascii=1
done

echo "- Starting indefinite fuzzing"

# Indefinite fuzz
$fuzzer corpus $samples_dir -use_value_profile=1 -close_fd_mask=1 -timeout=$timeout -jobs="$jobs" -workers="$jobs" -max_len=256 -rss_limit_mb=4096 $dictionaries -only_ascii=1

# Minimize all crashes
find . -maxdepth 1 -iname "crash-*" -exec bash -c '$fuzzer ${0} -minimize_crash=1 -timeout=$timeout -close_fd_mask=1 -rss_limit_mb=0 -runs=1000000' {} \; && find . -iname "minimized-from-*" -exec bash -c 'mv ${0} crash-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Minimize all timeouts
find . -maxdepth 1 -iname "timeout-*" -exec bash -c '$fuzzer ${0} -minimize_crash=1 -timeout=$timeout -close_fd_mask=1 -rss_limit_mb=0 -runs=1000000' {} \; && find . -iname "minimized-from-*" -exec bash -c 'mv ${0} timeout-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Minimize all leaks
find . -maxdepth 1 -iname "leak-*" -exec bash -c '$fuzzer ${0} -minimize_crash=1 -timeout=$timeout -close_fd_mask=1 -rss_limit_mb=0 -runs=1000000' {} \; && find . -iname "minimized-from-*" -exec bash -c 'mv ${0} leak-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Cleanse all crashes
find . -maxdepth 1 -iname "crash-*" -exec bash -c '$fuzzer ${0} -cleanse_crash=1 -timeout=$timeout -close_fd_mask=1 -rss_limit_mb=0 -exact_artifact_path=cleansed_crash && mv cleansed_crash crash-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Cleanse all timeouts
find . -maxdepth 1 -iname "timeout-*" -exec bash -c '$fuzzer ${0} -cleanse_crash=1 -timeout=$timeout -close_fd_mask=1 -rss_limit_mb=0 -exact_artifact_path=cleansed_timeout && mv cleansed_timeout timeout-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Cleanse all leaks
find . -maxdepth 1 -iname "leak-*" -exec bash -c '$fuzzer ${0} -cleanse_crash=1 -timeout=$timeout -close_fd_mask=1 -rss_limit_mb=0 -exact_artifact_path=cleansed_leak && mv cleansed_leak leak-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Print crash summary
find . -maxdepth 1 -type f \( -iname "crash-*" -or -iname "timeout-*" -or -iname "leak-*" \) -printf "%s  %f\n" | sort -k1,1nr -k2,2
