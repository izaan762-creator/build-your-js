cmake_minimum_required(VERSION 3.14)
project(myjs VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD   11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED   ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_RELEASE   "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(QJS_DIR ${CMAKE_SOURCE_DIR}/quickjs)

set(QJS_SOURCES
    ${QJS_DIR}/quickjs.c
    ${QJS_DIR}/cutils.c
    ${QJS_DIR}/dtoa.c
    ${QJS_DIR}/libregexp.c
    ${QJS_DIR}/libunicode.c
)

add_library(quickjs_lib STATIC ${QJS_SOURCES})
target_include_directories(quickjs_lib PUBLIC ${QJS_DIR})
target_compile_definitions(quickjs_lib PRIVATE
    _GNU_SOURCE
    CONFIG_VERSION="2024-01-13"
    CONFIG_BIGNUM
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(quickjs_lib PRIVATE
      -Wno-unused-variable -Wno-sign-compare
      -Wno-implicit-fallthrough -Wno-missing-field-initializers
      -Wno-array-bounds
  )
endif()

add_executable(myjs src/main.cpp)
target_link_libraries(myjs PRIVATE quickjs_lib m)
target_include_directories(myjs PRIVATE ${QJS_DIR})
target_compile_definitions(myjs PRIVATE _GNU_SOURCE CONFIG_VERSION="2024-01-13")

if(UNIX AND NOT APPLE)
  target_link_libraries(myjs PRIVATE dl)
endif()

install(TARGETS myjs DESTINATION bin)
message(STATUS "Build: ${CMAKE_BUILD_TYPE} | CC: ${CMAKE_C_COMPILER}")
