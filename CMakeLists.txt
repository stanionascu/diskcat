cmake_minimum_required(VERSION 3.10)
project(diskcat)
set(CMAKE_CXX_STANDARD 20)

option(USE_BUNDLED_ABSL_LIB "Use the absl submodule instead of the system package" TRUE)

find_package(PkgConfig REQUIRED)
pkg_check_modules(UDFREAD REQUIRED IMPORTED_TARGET "libudfread")
pkg_check_modules(DVDNAV REQUIRED IMPORTED_TARGET "dvdnav")

if(USE_BUNDLED_ABSL_LIB)
    add_subdirectory(abseil-cpp)
endif()

set(SOURCES src/diskcat.cc src/dvdparser.cc src/probe.cc)
add_executable(diskcat ${SOURCES})
target_link_libraries(diskcat
    PkgConfig::UDFREAD
    PkgConfig::DVDNAV
    absl::flags_parse
    absl::log_initialize
    absl::log)
