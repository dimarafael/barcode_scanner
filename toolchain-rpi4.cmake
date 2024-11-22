# Specify the target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set the compiler paths
# sudo apt install gcc-12-aarch64-linux-gnu g++-12-aarch64-linux-gnu
set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc-12)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++-12)

# Set the sysroot path
set(CMAKE_SYSROOT /home/user/rpi-sysroot)

# Specify paths for includes and libraries
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
