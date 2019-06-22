add_library(mod mod.c)
# Tests
add_executable(mod_test test.c)
target_link_libraries(mod_test mod)
set_target_properties(mod_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../../target/test)