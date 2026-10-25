cmake_minimum_required(VERSION 3.16)

project(media-center
  VERSION 0.1
  LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON )
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET
  gstreamer-1.0
)


## Server
add_executable(media-srv.o
  src/main.c
  src/media.c
  src/media-srv.c
)
target_include_directories(media-srv PRIVATE
  include
)
target_link_libraries(media-srv PRIVATE
  PkgConfig::GSTREAMER
)

## Client

add_executable(media-cli.o
  src/media-cli.c
)

target_include_directories(media-cli PRIVATE
  include
)
