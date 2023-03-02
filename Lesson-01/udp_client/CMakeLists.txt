cmake_minimum_required(VERSION 3.10)

project(udp-client C CXX)
set(CMAKE_CXX_STANDARD 14)

file(GLOB ${PROJECT_NAME}_SRC
        *.h
        *.cpp
        )

source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")
target_link_libraries("${PROJECT_NAME}" socket-wrapper)

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
endif()

