cmake_minimum_required(VERSION 3.0)
project(widberg-defs LANGUAGES NONE)

add_library(widberg-defs INTERFACE include/defs.h)
target_include_directories(widberg-defs INTERFACE include)
