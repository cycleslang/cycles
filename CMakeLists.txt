cmake_minimum_required(VERSION 3.10)

# Project Name
project(LexerProject C)

# Set compiler flags (Wall and g)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")

# Include the directory where libcycles/lexer.h is located
# Replace "." with the actual path if the 'libcycles' folder is elsewhere
include_directories(.)

# Define the executable and its source files
add_executable(lexer_app main.c)