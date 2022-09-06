#!/bin/bash

# Sanitizer options
export ASAN_OPTIONS="quarantine_size_mb=2048:redzone=512:strict_string_checks=1:detect_odr_violation=2:detect_stack_use_after_return=1:check_initialization_order=1:strict_init_order=1:detect_invalid_pointer_pairs=3:detect_leaks=1:print_stats=1:use_odr_indicator=1:strict_memcmp=1:print_scariness=1:handle_abort=1:symbolize=1:handle_segv=1:handle_sigill=1:allocator_release_to_os_interval_ms=500:handle_sigfpe=1:handle_sigbus=1:alloc_dealloc_mismatch=1:check_malloc_usable_size=1:abort_on_error=1"
export LSAN_OPTIONS="max_leaks=0:print_suppressions=1:report_objects=0:use_unaligned=1"
export UBSAN_OPTIONS="print_stacktrace=1:report_error_type=1:halt_on_error=1"

# Other options
jobs=$(($(nproc) / 2))
fuzzer=bin/RelWithDebInfo/OpenAutoItFuzzer_Parser
samples_dir="../OpenAutoItParser/tests/integration/samples ../OpenAutoItParser/fuzzers/samples"
timeout=1

# Preparations
mkdir build-fuzz -p

cd build-fuzz || exit

# Configure
cmake .. -DCMAKE_CXX_COMPILER=clang++ -DOA_SANATIZERS=ON -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" -DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-common -fno-inline-functions -march=native -mtune=native -DPHI_DEBUG -DOA_FUZZ_BUILD"

# Build
cmake --build . --config RelWithDebInfo

# Test
ctest . -C RelWithDebInfo

# Clean
find . -iname "*.gcno" -delete && find . -iname "*.gcda" -delete

# Create corpus
mkdir -p corpus

# Minimize corpus
mv corpus old_corpus
mkdir -p corpus
$fuzzer corpus old_corpus "$samples_dir" -merge=1 -close_fd_mask=1 -rss_limit_mb=0
rm -rf old_corpus

# Intial range fuzz
for i in {1,2,4,8,16,32,64}; do
    $fuzzer corpus "$samples_dir" -use_value_profile=1 -close_fd_mask=1 -timeout="$timeout" -jobs="$jobs" -workers="$jobs" -max_len="$i" -runs=1000000 -rss_limit_mb=4096 -dict=../OpenAutoItParser/fuzzers/dictionary/tokens.dict -dict=../OpenAutoItParser/fuzzers/dictionary/code.dict
done

# Indefinite fuzz
$fuzzer corpus "$samples_dir" -use_value_profile=1 -close_fd_mask=1 -timeout="$timeout" -jobs="$jobs" -workers="$jobs" -max_len=128 -rss_limit_mb=4096 -dict=../OpenAutoItParser/fuzzers/dictionary/tokens.dict -dict=../OpenAutoItParser/fuzzers/dictionary/code.dict

# Minimize all crashes
find . -maxdepth 1 -iname "crash-*" -exec bash -c '$fuzzer ${0} -minimize_crash=1 -timeout="$timeout" -close_fd_mask=1 -rss_limit_mb=0 -runs=1000000' {} \; && find . -iname "minimized-from-*" -exec bash -c 'mv ${0} crash-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Minimize all timeouts
find . -maxdepth 1 -iname "timeout-*" -exec bash -c '$fuzzer ${0} -minimize_crash=1 -timeout="$timeout" -close_fd_mask=1 -rss_limit_mb=0 -runs=1000000' {} \; && find . -iname "minimized-from-*" -exec bash -c 'mv ${0} timeout-$(sha1sum "${0}" | cut -f1 -d" ")' {} \;

# Print crash summary
find . -maxdepth 1 -type f \( -iname "crash-*" -or -iname "timeout-*" \) -printf "%s  %f\n" | sort -k1,1nr -k2,2
