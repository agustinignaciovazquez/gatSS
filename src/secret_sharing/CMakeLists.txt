add_library(secret_sharing distribute.c recover.c utils.c include/utils.h mod_256/integer_arithmetic.c mod_256/matrix_arithmetic.c mod_256/matrix_operations.c)

# Library dependencies
target_link_libraries(secret_sharing algorithms)
target_link_libraries(secret_sharing bmp)
target_link_libraries(secret_sharing rand)
target_link_libraries(secret_sharing steg)