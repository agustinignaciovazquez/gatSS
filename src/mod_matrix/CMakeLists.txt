add_library(mod_matrix mod_matrix.c)

# Library dependencies
target_link_libraries(mod_matrix mod)
target_link_libraries(mod_matrix logger)
