cmake_minimum_required(VERSION 3.0)
project(widberg_defs LANGUAGES NONE)

add_library(widberg_defs INTERFACE include/defs.h)
target_include_directories(widberg_defs INTERFACE include)
