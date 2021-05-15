cmake_minimum_required(VERSION 3.19)
project(host)

set(CMAKE_CXX_STANDARD 14)

add_executable(host main.cpp Firmware.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(host Threads::Threads)