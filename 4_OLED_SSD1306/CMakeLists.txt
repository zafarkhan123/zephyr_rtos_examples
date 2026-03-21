# Minimum required version of CMake
cmake_minimum_required(VERSION 3.20.0)

# Find the Zephyr package to pull in the build system
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Define your project name
project(oled_hello)

# Add your source files (typically in a src/ directory)
target_sources(app PRIVATE src/main.c)





