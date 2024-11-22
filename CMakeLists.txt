cmake_minimum_required(VERSION 3.5.0)
project(barcode VERSION 0.1.0 LANGUAGES CXX)

# Specify the C++ standard
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cross-compilation settings
if(CMAKE_SYSTEM_NAME STREQUAL "")
    message(FATAL_ERROR "Please invoke CMake with a toolchain file for cross-compilation.")
endif()

# Find the curl package
find_package(CURL REQUIRED)



# Add source files
set(SOURCE_FILES
    main.cpp # Replace with your actual source files
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include directories if needed
include_directories(${CMAKE_SYSROOT}/usr/include)

# Link libraries
target_link_libraries(${PROJECT_NAME} CURL::libcurl) # Add more libraries if needed