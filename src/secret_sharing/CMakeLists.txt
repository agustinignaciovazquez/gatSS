add_library(secret_sharing distribute.c recover.c utils.c utils.h)

# Library dependencies
target_link_libraries(secret_sharing algorithms)
target_link_libraries(secret_sharing bmp)
target_link_libraries(secret_sharing rand)
target_link_libraries(secret_sharing steg)