cmake_minimum_required(VERSION 3.7)
project(nessi_guidestar)

set(CMAKE_CXX_STANDARD 11)

set(EXTREME_SOURCE1
        src/momentCentroid.c
        src/load_pixels.c
        src/load_pixels.h
        src/nessi-cent.c
   )

add_executable(extreme-nessi-cent ${EXTREME_SOURCE1})

include_directories(/usr/local/include /usr/include/libxml2)

link_directories(/usr/local/lib /usr/lib/x86_64-linux-gnu/)

target_link_libraries(extreme-nessi-cent cfitsio m)
