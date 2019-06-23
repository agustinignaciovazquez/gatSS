# Include dirs
include_directories(secret_sharing/include/)
include_directories(secret_sharing/bmp/include/)
include_directories(secret_sharing/utils/include/)
include_directories(secret_sharing/mod_256/include/)
include_directories(secret_sharing/stegano/include/)
include_directories(parser/include/)

# Executable
add_executable(gatSS main.c parser/parser.c secret_sharing/stegano/stegano.c secret_sharing/distribute.c secret_sharing/recover.c secret_sharing/bmp/bmp.c secret_sharing/utils/utils.c secret_sharing/mod_256/matrix_operations.c secret_sharing/mod_256/matrix_arithmetic.c secret_sharing/mod_256/integer_arithmetic.c)

set_target_properties(gatSS PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../target)