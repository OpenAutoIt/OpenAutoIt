# Phi
add_subdirectory(Phi)

# fmt
add_subdirectory(fmt)

# Ensure exceptions are disable for fmtlib
target_compile_definitions(fmt PUBLIC FMT_EXCEPTIONS=0)
