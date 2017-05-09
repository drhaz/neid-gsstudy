cmake_minimum_required(VERSION 3.7)
project(nessi_guidestar)

set(CMAKE_C_STANDARD 11)

set(EXTREME_SOURCE1
        src/momentCentroid.c
        src/load_pixels.c
        src/load_pixels.h
        src/nessi-cent.c
        src/utils.h
        src/utils.c
   )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")


message("CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE is ${CMAKE_C_FLAGS_RELEASE}")
message("CMAKE_C_FLAGS is ${CMAKE_C_FLAGS}")

add_executable(extreme-nessi-cent ${EXTREME_SOURCE1})

include_directories(/usr/local/include /usr/include/libxml2)

link_directories(/usr/local/lib /usr/lib/x86_64-linux-gnu/)

target_link_libraries(extreme-nessi-cent cfitsio m efence)
