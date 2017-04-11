cmake_minimum_required(VERSION 3.7)
project(nessi_guidestar)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/readnessi.c)

add_executable(nessi_guidestar ${SOURCE_FILES})

include_directories(/usr/local/include)


link_directories(/usr/local/lib)

target_link_libraries(nessi_guidestar cfitsio m)
