cmake_minimum_required(VERSION 3.10)

project(libtorrent)

set(SRC
    simple_client
    dump_torrent
    make_torrent)

if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
	add_compile_options(-Wno-implicit-int-float-conversion)
endif()

foreach(CURRENT ${SRC})
    add_executable(${CURRENT} "${CURRENT}.cpp")
    target_link_libraries(${CURRENT} PRIVATE torrent-rasterbar)
endforeach(CURRENT)
