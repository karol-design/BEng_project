# This is a top-level project CMakeLists.txt file.
# This is the primary file which CMake uses to define how to build the project. 
# It sets the project name and may set project-wide CMake variables.
# It includes the file "project.cmake" which implements the rest of the build system.

cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(template)
