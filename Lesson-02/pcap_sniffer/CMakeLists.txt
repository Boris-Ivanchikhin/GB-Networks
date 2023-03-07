cmake_minimum_required(VERSION 3.10)

project(pcap-sniffer C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

if (NOT WIN32)
    find_package(PCAP REQUIRED)
else()
    # Locally-installed.
    set(PCAP_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk-1.13/Include")
    if(CMAKE_CL_64)
        link_directories("${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk-1.13/Lib/x64")
    else()
        link_directories("${CMAKE_CURRENT_SOURCE_DIR}/npcap-sdk-1.13/Lib")
    endif()
endif()

#set(${PROJECT_NAME}_SRC main.cpp packet_printer.cpp)
file(GLOB ${PROJECT_NAME}_SRC
        *.h
        *.cpp
        *.c
        )

configure_file(config.h.in config.h @ONLY)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")

target_include_directories("${PROJECT_NAME}" SYSTEM PRIVATE ${PCAP_INCLUDE_DIRS})
target_include_directories("${PROJECT_NAME}" PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (NOT WIN32)
    target_link_libraries("${PROJECT_NAME}" pcap::pcap)
else()
    target_link_libraries("${PROJECT_NAME}" wpcap)
    target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
endif()
