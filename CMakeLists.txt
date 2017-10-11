cmake_minimum_required  (VERSION 2.6)
project (multi_redirect)
find_package (Threads)
add_definitions( -D_GNU_SOURCE )

file(GLOB_RECURSE MY_SOURCES "src/*.c")
file(GLOB_RECURSE MY_HEADERS "src/*.h")

set (MY_INCLUDE_DIRS "src")


# a direct quick way
foreach (_headerFile ${MY_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND MY_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES MY_INCLUDE_DIRS)
include_directories(${MY_INCLUDE_DIRS})

add_executable (multi_redirect ${MY_SOURCES})
target_link_libraries (multi_redirect ${CMAKE_THREAD_LIBS_INIT})
