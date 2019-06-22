# Subdirectories
add_subdirectory(mod)
add_subdirectory(secret_sharing)
add_subdirectory(mod_matrix)
add_subdirectory(algorithms)
add_subdirectory(bmp)
add_subdirectory(rand)
add_subdirectory(steg)
add_subdirectory(args)
add_subdirectory(logger)

# Executable
add_executable(gatSS main.c)

target_link_libraries(gatSS bmp)
target_link_libraries(gatSS mod_matrix)
target_link_libraries(gatSS mod)
target_link_libraries(gatSS algorithms)
target_link_libraries(gatSS steg)
target_link_libraries(gatSS rand)
target_link_libraries(gatSS args)
target_link_libraries(gatSS logger)
target_link_libraries(gatSS secret_sharing)
set_target_properties(gatSS PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../target)